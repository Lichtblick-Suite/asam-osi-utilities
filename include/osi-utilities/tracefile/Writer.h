//
// Copyright (c) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// SPDX-License-Identifier: MPL-2.0
//

#ifndef OSIUTILITIES_TRACEFILE_WRITER_H_
#define OSIUTILITIES_TRACEFILE_WRITER_H_

#include <google/protobuf/message.h>

#include <filesystem>
#include <memory>
#include <string>

namespace osi3 {

/**
 * @brief Abstract base class for writing trace files in various formats
 *
 * This class provides an interface for writing protobuf messages to trace files.
 * Different implementations can support various file formats like MCAP.
 */
class TraceFileWriter {
   public:
    /** @brief Virtual destructor */
    virtual ~TraceFileWriter() = default;

    /** @brief Default constructor */
    TraceFileWriter() = default;

    /** @brief Deleted copy constructor */
    TraceFileWriter(const TraceFileWriter&) = delete;

    /** @brief Deleted copy assignment operator */
    TraceFileWriter& operator=(const TraceFileWriter&) = delete;

    /** @brief Deleted move constructor */
    TraceFileWriter(TraceFileWriter&&) = delete;

    /** @brief Deleted move assignment operator */
    TraceFileWriter& operator=(TraceFileWriter&&) = delete;

    /**
     * @brief Opens a file for writing
     * @param file_path Path to the file to be created/opened
     * @return true if successful, false otherwise
     */
    virtual bool Open(const std::string& file_path) = 0;

    /**
     * @brief Writes a protobuf message to the file
     * @tparam T Type of the protobuf message
     * @param top_level_message The message to write
     * @return true if successful, false otherwise
     */
    template <typename T>
    bool WriteMessage(T top_level_message) = delete;

    /**
     * @brief Writes a protobuf message to the file
     * @tparam T Type of the protobuf message
     * @param top_level_message The message to write
     * @param topic Optional topic name for the message
     * @return true if successful, false otherwise
     */
    template <typename T>
    bool WriteMessage(T top_level_message, const std::string& topic = "") = delete;

    /**
     * @brief Sets metadata for the trace file
     * @param name Name of the metadata entry
     * @param metadata_entries Key-value pairs of metadata
     * @return true if successful, false otherwise
     */
    virtual bool SetMetadata(const std::string& name, const std::unordered_map<std::string, std::string>& metadata_entries) = 0;

    /**
     * @brief Closes the trace file
     */
    virtual void Close() = 0;
};

/**
 * @brief Factory function to create trace file writers
 * @param format The desired output format (e.g., "mcap")
 * @return Unique pointer to a TraceFileWriter implementation
 */
std::unique_ptr<TraceFileWriter> CreateTraceFileWriter(const std::string& format);

}  // namespace osi3
#endif