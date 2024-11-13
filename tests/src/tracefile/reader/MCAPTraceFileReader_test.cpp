//
// Copyright (c) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// SPDX-License-Identifier: MPL-2.0
//

#include <gtest/gtest.h>
#include "osi-utilities/tracefile/reader/MCAPTraceFileReader.h"
#include "osi-utilities/tracefile/writer/MCAPTraceFileWriter.h"
#include "osi_groundtruth.pb.h"
#include "osi_sensorview.pb.h"

#include <filesystem>

static const char* JSON_SCHEMA_TEXT = R"({
  "test_field1": "abc",
})";

class McapTraceFileReaderTest : public ::testing::Test {
protected:
    osi3::MCAPTraceFileReader reader_;
    osi3::MCAPTraceFileWriter writer_;
    const std::string test_file_ = "test.mcap";

    void SetUp() override {
        CreateTestMcapFile();
    }

    void TearDown() override {
        reader_.Close();
        std::filesystem::remove(test_file_);
    }

private:
    void CreateTestMcapFile() {
        ASSERT_TRUE(writer_.Open(test_file_));

        // Add required metadata
        std::unordered_map<std::string, std::string> metadata_entries;
        metadata_entries["timestamp"] = writer_.GetCurrentTimeAsString();
        metadata_entries["zero_time"] = writer_.GetCurrentTimeAsString();
        writer_.SetMetadata("asam_osi", metadata_entries);

        // Add channels for different message types
        writer_.AddChannel("gt", osi3::GroundTruth::descriptor());
        writer_.AddChannel("sv", osi3::SensorView::descriptor());

        // Write GroundTruth message
        auto gt = std::make_unique<osi3::GroundTruth>();
        gt->mutable_timestamp()->set_seconds(0);
        gt->mutable_timestamp()->set_nanos(456);
        ASSERT_TRUE(writer_.WriteMessage(*gt, "gt"));

        // Write SensorView message
        auto sv = std::make_unique<osi3::SensorView>();
        sv->mutable_timestamp()->set_seconds(1);
        sv->mutable_timestamp()->set_nanos(101);
        ASSERT_TRUE(writer_.WriteMessage(*sv, "sv"));

        // Add non-OSI JSON channel and message
        auto* mcap_writer = writer_.GetMcapWriter();

        auto json_schema =mcap::Schema("my_json_schema", "jsonschema", JSON_SCHEMA_TEXT);
        mcap_writer->addSchema(json_schema);

        mcap::Channel channel("json_topic", "json", json_schema.id);
        mcap_writer->addChannel(channel);

        std::string json_data = "{\"test_field1\": \"data\"}";
        mcap::Message msg;
        msg.channelId = channel.id;
        msg.data = reinterpret_cast<const std::byte*>(json_data.data());
        msg.dataSize = json_data.size();
        msg.logTime = 2;
        msg.publishTime = msg.logTime;
        auto status = mcap_writer->write(msg);
        std::cout << "Status: " << status.message << std::endl;
        ASSERT_EQ(status.code, mcap::StatusCode::Success);
        writer_.Close();
    }
};

TEST_F(McapTraceFileReaderTest, OpenValidFile) {
    EXPECT_TRUE(reader_.Open(test_file_));
}

TEST_F(McapTraceFileReaderTest, OpenNonexistentFile) {
    EXPECT_FALSE(reader_.Open("nonexistent.mcap"));
}

TEST_F(McapTraceFileReaderTest, ReadGroundTruthMessage) {
    ASSERT_TRUE(reader_.Open(test_file_));
    reader_.SetSkipNonOSIMsgs(true);
    EXPECT_TRUE(reader_.HasNext());

    const auto result = reader_.ReadMessage();
    ASSERT_TRUE(result.has_value());

    auto* gt = dynamic_cast<osi3::GroundTruth*>(result->message.get());
    ASSERT_NE(gt, nullptr);
    EXPECT_EQ(gt->timestamp().seconds(), 0);
    EXPECT_EQ(gt->timestamp().nanos(), 456);
    EXPECT_EQ(result->channel_name, "gt");
}

TEST_F(McapTraceFileReaderTest, ReadSensorViewMessage) {
    ASSERT_TRUE(reader_.Open(test_file_));
    reader_.SetSkipNonOSIMsgs(true);
    ASSERT_TRUE(reader_.HasNext());

    // Skip first message
    reader_.ReadMessage();

    const auto result = reader_.ReadMessage();
    ASSERT_TRUE(result.has_value());

    auto* sv = dynamic_cast<osi3::SensorView*>(result->message.get());
    ASSERT_NE(sv, nullptr);
    EXPECT_EQ(sv->timestamp().seconds(), 1);
    EXPECT_EQ(sv->timestamp().nanos(), 101);
    EXPECT_EQ(result->channel_name, "sv");
}

TEST_F(McapTraceFileReaderTest, PreventMultipleFileOpens) {
    // First open should succeed
    EXPECT_TRUE(reader_.Open(test_file_));

    // Second open should fail while first file is still open
    EXPECT_FALSE(reader_.Open("testdata/another.mcap"));

    // After closing, opening a new file should work
    reader_.Close();
    EXPECT_TRUE(reader_.Open(test_file_));
}



TEST_F(McapTraceFileReaderTest, HasNextReturnsFalseWhenEmpty) {
    ASSERT_TRUE(reader_.Open(test_file_));
    reader_.SetSkipNonOSIMsgs(true);
    ASSERT_TRUE(reader_.HasNext());

    reader_.ReadMessage();  // Read first message
    ASSERT_TRUE(reader_.HasNext());
    reader_.ReadMessage();  // Read second message
    reader_.ReadMessage();  // Read third non-osi message
    EXPECT_FALSE(reader_.HasNext());
}

TEST_F(McapTraceFileReaderTest, ReadMessageReturnsNulloptWhenEmpty) {
    ASSERT_TRUE(reader_.Open(test_file_));
    reader_.SetSkipNonOSIMsgs(true);

    reader_.ReadMessage();  // Read first message
    reader_.ReadMessage();  // Read second message
    reader_.ReadMessage();  // Read third non-osi message

    const auto result = reader_.ReadMessage();
    EXPECT_FALSE(result.has_value());
}

TEST_F(McapTraceFileReaderTest, HasNextReturnsFalseWhenNotOpened) {
    EXPECT_FALSE(reader_.HasNext());
}

TEST_F(McapTraceFileReaderTest, ReadInvalidMessageFormat) {
    const std::string invalid_file = "invalid.mcap";
    {
        std::ofstream file(invalid_file, std::ios::binary);
        file << "Invalid MCAP format";
    }

    EXPECT_FALSE(reader_.Open(invalid_file));
    std::filesystem::remove(invalid_file);
}

TEST_F(McapTraceFileReaderTest, CloseAndReopenFile) {
    ASSERT_TRUE(reader_.Open(test_file_));
    reader_.Close();
    EXPECT_TRUE(reader_.Open(test_file_));
    EXPECT_TRUE(reader_.HasNext());
}


TEST_F(McapTraceFileReaderTest, SkipNonOSIMessagesWhenEnabled) {
    ASSERT_TRUE(reader_.Open(test_file_));
    reader_.SetSkipNonOSIMsgs(true);

    // Read first OSI message (GroundTruth)
    auto result1 = reader_.ReadMessage();
    ASSERT_TRUE(result1.has_value());
    EXPECT_EQ(result1->channel_name, "gt");

    // Read second OSI message (SensorView)
    auto result2 = reader_.ReadMessage();
    ASSERT_TRUE(result2.has_value());
    EXPECT_EQ(result2->channel_name, "sv");

    // Third message (JSON) should be skipped automatically
    auto result3 = reader_.ReadMessage();
    EXPECT_FALSE(result3.has_value());
}

TEST_F(McapTraceFileReaderTest, ThrowExceptionForNonOSIMessagesWhenSkipDisabled) {
    ASSERT_TRUE(reader_.Open(test_file_));
    reader_.SetSkipNonOSIMsgs(false);

    // Read first OSI message (GroundTruth)
    auto result1 = reader_.ReadMessage();
    ASSERT_TRUE(result1.has_value());

    // Read second OSI message (SensorView)
    auto result2 = reader_.ReadMessage();
    ASSERT_TRUE(result2.has_value());

    // Third message (JSON) should throw an exception
    EXPECT_THROW({
        reader_.ReadMessage();
    }, std::runtime_error);
}