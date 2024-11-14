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
 *
 * @Note The WriteMessage() function is intentionally omitted from this base class since it is format-specific.
 * Users should dynamically cast to the concrete implementation class to access the appropriate WriteMessage() function.
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
     * @brief Closes the trace file
     */
    virtual void Close() = 0;
};


}  // namespace osi3
#endif