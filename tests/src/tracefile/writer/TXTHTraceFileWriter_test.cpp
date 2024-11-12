//
// Copyright (c) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// SPDX-License-Identifier: MPL-2.0
//

#include <gtest/gtest.h>
#include "osi-utilities/tracefile/writer/TXTHTraceFileWriter.h"
#include "osi_groundtruth.pb.h"
#include "osi_sensorview.pb.h"

#include <filesystem>
#include <fstream>

class TxthTraceFileWriterTest : public ::testing::Test {
protected:
    osi3::TXTHTraceFileWriter writer_;
    const std::string test_file_gt_ = "test_output_gt.txth";
    const std::string test_file_sv_ = "test_output_sv.txth";

    void TearDown() override {
        writer_.Close();
        std::filesystem::remove(test_file_gt_);
        std::filesystem::remove(test_file_sv_);
    }
};

TEST_F(TxthTraceFileWriterTest, OpenWithValidExtension) {
    EXPECT_TRUE(writer_.Open(test_file_gt_));
}

TEST_F(TxthTraceFileWriterTest, OpenWithInvalidExtension) {
    EXPECT_FALSE(writer_.Open("test.invalid"));
}

TEST_F(TxthTraceFileWriterTest, WriteGroundTruthMessage) {
    ASSERT_TRUE(writer_.Open(test_file_gt_));

    osi3::GroundTruth ground_truth;

    ground_truth.mutable_timestamp()->set_seconds(123);
    ground_truth.mutable_timestamp()->set_nanos(456);

    EXPECT_TRUE(writer_.WriteMessage(ground_truth));

    writer_.Close();

    // Verify file content
    std::ifstream file(test_file_gt_);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    EXPECT_TRUE(content.find("seconds: 123") != std::string::npos);
    EXPECT_TRUE(content.find("nanos: 456") != std::string::npos);
}

TEST_F(TxthTraceFileWriterTest, WriteSensorViewMessage) {
    ASSERT_TRUE(writer_.Open(test_file_sv_));

    osi3::SensorView sensor_view;
    sensor_view.mutable_timestamp()->set_seconds(789);
    sensor_view.mutable_timestamp()->set_nanos(101);

    EXPECT_TRUE(writer_.WriteMessage(sensor_view));

    writer_.Close();

    std::ifstream file(test_file_sv_);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    EXPECT_TRUE(content.find("seconds: 789") != std::string::npos);
    EXPECT_TRUE(content.find("nanos: 101") != std::string::npos);
}

TEST_F(TxthTraceFileWriterTest, WriteMessageToClosedFile) {
    osi3::GroundTruth ground_truth;
    EXPECT_FALSE(writer_.WriteMessage(ground_truth));
}

TEST_F(TxthTraceFileWriterTest, MultipleMessages) {
    ASSERT_TRUE(writer_.Open(test_file_gt_));

    osi3::GroundTruth ground_truth_1;
    osi3::GroundTruth ground_truth_2;
    ground_truth_1.mutable_timestamp()->set_seconds(111);
    ground_truth_2.mutable_timestamp()->set_seconds(222);

    EXPECT_TRUE(writer_.WriteMessage(ground_truth_1));
    EXPECT_TRUE(writer_.WriteMessage(ground_truth_2));

    writer_.Close();

    std::ifstream file(test_file_gt_);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    EXPECT_TRUE(content.find("seconds: 111") != std::string::npos);
    EXPECT_TRUE(content.find("seconds: 222") != std::string::npos);
}

TEST_F(TxthTraceFileWriterTest, CloseAndReopenFile) {
    ASSERT_TRUE(writer_.Open(test_file_gt_));
    writer_.Close();
    EXPECT_TRUE(writer_.Open(test_file_gt_));
}

TEST_F(TxthTraceFileWriterTest, WriteToReadOnlyLocation) {
    const std::string readonly_path = "/root/test.txth";
    EXPECT_FALSE(writer_.Open(readonly_path));
}
