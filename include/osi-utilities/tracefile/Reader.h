//
// Copyright (c) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// SPDX-License-Identifier: MPL-2.0
//

#ifndef OSIUTILITIES_TRACEFILE_READER_H_
#define OSIUTILITIES_TRACEFILE_READER_H_

#include <google/protobuf/message.h>

#include <memory>
#include <optional>
#include <string>

namespace osi3 {
/**
 * @brief Enumeration of supported top-level message types in trace files
 */
enum class ReaderTopLevelMessage : u_int8_t {
    kUnknown = 0,             /**< Unknown message type */
    kGroundTruth,             /**< OSI3::GroundTruth data */
    kSensorData,              /**< OSI3::SensorSata */
    kSensorView,              /**< OSI3::SensorView */
    kSensorViewConfiguration, /**< OSI3::SensorViewConfiguration */
    kHostVehicleData,         /**< OSI3::HostVehicleData */
    kTrafficCommand,          /**< OSI3::TrafficCommand */
    kTrafficCommandUpdate,    /**< OSI3::TrafficCommandUpdate */
    kTrafficUpdate,           /**< OSI3::TrafficUpdate */
    kMotionRequest,           /**< OSI3::MotionRequest */
    kStreamingUpdate,         /**< OSI3::StreamingUpdate */
};

/**
 * @brief Map of trace file names to their corresponding message type
 */
const std::unordered_map<std::string, osi3::ReaderTopLevelMessage> kFileNameMessageTypeMap = {{"_gt_", osi3::ReaderTopLevelMessage::kGroundTruth},
                                                                                      {"_sd_", osi3::ReaderTopLevelMessage::kSensorData},
                                                                                      {"_sv_", osi3::ReaderTopLevelMessage::kSensorView},
                                                                                      {"_svc_", osi3::ReaderTopLevelMessage::kSensorViewConfiguration},
                                                                                      {"_hvd_", osi3::ReaderTopLevelMessage::kHostVehicleData},
                                                                                      {"_tc_", osi3::ReaderTopLevelMessage::kTrafficCommand},
                                                                                      {"_tcu_", osi3::ReaderTopLevelMessage::kTrafficCommandUpdate},
                                                                                      {"_tu_", osi3::ReaderTopLevelMessage::kTrafficUpdate},
                                                                                      {"_mr_", osi3::ReaderTopLevelMessage::kMotionRequest},
                                                                                      {"_su_", osi3::ReaderTopLevelMessage::kStreamingUpdate}};

/**
 * @brief Structure containing the result of a read operation
 */
struct ReadResult {
    std::unique_ptr<google::protobuf::Message> message;                   /**< The parsed protobuf message */
    ReaderTopLevelMessage message_type = ReaderTopLevelMessage::kUnknown; /**< Type of the message */
    std::string channel_name;                                                  /**< Channel name (only for MCAP format) */
};

/**
 * @brief Abstract base class for reading trace files in various formats
 */
class TraceFileReader {
   public:
    /** @brief Default constructor */
    TraceFileReader() = default;

    /** @brief Virtual destructor */
    virtual ~TraceFileReader() = default;

    /** @brief Deleted copy constructor */
    TraceFileReader(const TraceFileReader&) = delete;

    /** @brief Deleted copy assignment operator */
    TraceFileReader& operator=(const TraceFileReader&) = delete;

    /** @brief Deleted move constructor */
    TraceFileReader(TraceFileReader&&) = delete;

    /** @brief Deleted move assignment operator */
    TraceFileReader& operator=(TraceFileReader&&) = delete;

    /**
     * @brief Opens a trace file for reading
     * @param file_path Path to the file to be opened
     * @return true if successful, false otherwise
     */
    virtual bool Open(const std::string& file_path) = 0;

    /**
     * @brief Reads the next message from the trace file
     * @return Optional ReadResult containing the message if available
     */
    virtual std::optional<ReadResult> ReadMessage() = 0;

    /**
     * @brief Closes the trace file
     */
    virtual void Close() = 0;

    /**
     * @brief Checks if more messages are available, should be used before calling ReadMessage()
     * @return true if there are more messages to read, false otherwise
     */
    virtual bool HasNext() = 0;
};

// TODO change to function which guesses on the filename endings
/**
 * @brief Factory function to create trace file readers based on the input file format
 * @param  The format of the input file (e.g., "mcap")
 * @return Unique pointer to a TraceFileReader implementation
 */
std::unique_ptr<TraceFileReader> createTraceFileReader(const std::string& format);
}  // namespace osi3
#endif
