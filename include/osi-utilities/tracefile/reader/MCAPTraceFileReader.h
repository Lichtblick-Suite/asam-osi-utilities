//
// Copyright (c) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// SPDX-License-Identifier: MPL-2.0
//

#ifndef OSIUTILITIES_TRACEFILE_READER_MCAPTRACEFILEREADER_H_
#define OSIUTILITIES_TRACEFILE_READER_MCAPTRACEFILEREADER_H_

#include <mcap/reader.hpp>

#include "osi-utilities/tracefile/Reader.h"
#include "osi_groundtruth.pb.h"
#include "osi_hostvehicledata.pb.h"
#include "osi_motionrequest.pb.h"
#include "osi_sensordata.pb.h"
#include "osi_sensorview.pb.h"
#include "osi_sensorviewconfiguration.pb.h"
#include "osi_streamingupdate.pb.h"
#include "osi_trafficcommand.pb.h"
#include "osi_trafficcommandupdate.pb.h"
#include "osi_trafficupdate.pb.h"

namespace osi3 {

/**
 * @brief Implementation of TraceFileReader for MCAP format files containing OSI messages
 *
 * This class provides functionality to read and deserialize OSI messages from MCAP files.
 * It supports various OSI message types including GroundTruth, SensorData, SensorView, etc.
 */
class MCAPTraceFileReader final : public TraceFileReader {
   public:
    bool Open(const std::string& file_path) override;

    /**
     * @brief Opens a trace file for reading
     *
     * This alternative opening method allows specifying additional options
     * like only requesting a certain topic
     *
     * @param file_path Path to the file to be opened
     * @param options Options for the MCAP writer
     * @return true if successful, false otherwise
     */
    bool Open(const std::string& file_path, const mcap::ReadMessageOptions& options);

    std::optional<ReadResult> ReadMessage() override;
    void Close() override;
    bool HasNext() override;

    /**
     * @brief Sets whether to skip non-OSI messages during reading
     * @param skip If true, non-OSI messages will be skipped during reading. If false, all messages will be processed
     *
     * If the file contains non-OSI messages and this option is not set to true, an exception will be thrown.
     */
    void SetSkipNonOSIMsgs(const bool skip) { skip_non_osi_msgs_ = skip; }

   private:
    mcap::McapReader mcap_reader_;
    std::unique_ptr<mcap::LinearMessageView> message_view_;  // Cannot copy or move LinearMessageView
    std::unique_ptr<mcap::LinearMessageView::Iterator> message_iterator_;

    bool skip_non_osi_msgs_ = false;
    mcap::ReadMessageOptions mcap_options_;

    /**
     * @brief Template function to deserialize MCAP messages into specific OSI message types
     * @tparam T The OSI message type to deserialize into
     * @param mcap_msg The MCAP message containing serialized data
     * @return Unique pointer to the deserialized OSI protobuf message
     * @throws std::runtime_error if deserialization fails
     */
    template <typename T>
    std::unique_ptr<google::protobuf::Message> Deserialize(const mcap::Message& mcap_msg) {
        auto output = std::make_unique<T>();
        if (!output->ParseFromArray(mcap_msg.data, mcap_msg.dataSize)) {
            throw std::runtime_error("Error: Failed to deserialize message.");
        }
        return output;
    }

    using DeserializeFunction = std::function<std::unique_ptr<google::protobuf::Message>(const mcap::Message& msg)>;
    /**
     * @brief Map containing message type specific deserializer functions and corresponding OSI message types
     *
     * Maps OSI message type strings to pairs of deserializer functions and ReaderTopLevelMessage enums
     * which are part of the ReadResult struct.Used for dynamic message deserialization
     * based on channel schema which is defined by the OSI specification.
     */
    const std::map<std::string, std::pair<DeserializeFunction, ReaderTopLevelMessage>> deserializer_map_ = {
        {"osi3.GroundTruth", {[this](const mcap::Message& msg) { return Deserialize<osi3::GroundTruth>(msg); }, ReaderTopLevelMessage::kGroundTruth}},
        {"osi3.SensorData", {[this](const mcap::Message& msg) { return Deserialize<osi3::SensorData>(msg); }, ReaderTopLevelMessage::kSensorData}},
        {"osi3.SensorView", {[this](const mcap::Message& msg) { return Deserialize<osi3::SensorView>(msg); }, ReaderTopLevelMessage::kSensorView}},
        {"osi3.SensorViewConfiguration",
         {[this](const mcap::Message& msg) { return Deserialize<osi3::SensorViewConfiguration>(msg); }, ReaderTopLevelMessage::kSensorViewConfiguration}},
        {"osi3.HostVehicleData", {[this](const mcap::Message& msg) { return Deserialize<osi3::HostVehicleData>(msg); }, ReaderTopLevelMessage::kHostVehicleData}},
        {"osi3.TrafficCommand", {[this](const mcap::Message& msg) { return Deserialize<osi3::TrafficCommand>(msg); }, ReaderTopLevelMessage::kTrafficCommand}},
        {"osi3.TrafficCommandUpdate", {[this](const mcap::Message& msg) { return Deserialize<osi3::TrafficCommandUpdate>(msg); }, ReaderTopLevelMessage::kTrafficCommandUpdate}},
        {"osi3.TrafficUpdate", {[this](const mcap::Message& msg) { return Deserialize<osi3::TrafficUpdate>(msg); }, ReaderTopLevelMessage::kTrafficUpdate}},
        {"osi3.MotionRequest", {[this](const mcap::Message& msg) { return Deserialize<osi3::MotionRequest>(msg); }, ReaderTopLevelMessage::kMotionRequest}},
        {"osi3.StreamingUpdate", {[this](const mcap::Message& msg) { return Deserialize<osi3::StreamingUpdate>(msg); }, ReaderTopLevelMessage::kStreamingUpdate}}};

    /**
     * @brief Internal callback function invoked by the MCAP reader when problems occur during reading operations
     * @param status The MCAP status object containing error information
     *
     * This is an internal method called by the underlying MCAP reader implementation
     * when it encounters issues during file reading operations.
     */
    static void OnProblem(const mcap::Status& status);
};

}  // namespace osi3
#endif