//
// Copyright (c) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// SPDX-License-Identifier: MPL-2.0
//

#ifndef NATIVEBINARYTRACEFILEREADER_H
#define NATIVEBINARYTRACEFILEREADER_H

#include "Reader.h"
#include <fstream>

namespace osi3 {


/**
 * @brief Function type for parsing binary messages into protobuf objects
 */
using MessageParserFunc = std::function<std::unique_ptr<google::protobuf::Message>(const std::vector<char>&)>;

/**
 * @brief Reader implementation for native binary OSI trace files
 */
class NativeBinaryTraceFileReader final : public osi3::TraceFileReader {
    public:
    bool Open(const std::string& filename) override;

     /**
     * @brief Opens a trace file with specified message type
     * @param filename Path to the trace file
     * @param message_type Expected message type in the file
     * @return true if successful, false otherwise
     */
    bool Open(const std::string& filename, ReaderTopLevelMessage message_type);
    std::optional<ReadResult> ReadMessage() override;
    void Close() override;
    bool HasNext() override;

    /**
     * @brief Gets the current message type being read
     * @return The message type enum value
     */
    ReaderTopLevelMessage GetMessageType() const {return message_type_;};

private:
    std::ifstream trace_file_;                         /**< File stream for reading */
    MessageParserFunc parser_;                         /**< Message parsing function */
    ReaderTopLevelMessage message_type_ = ReaderTopLevelMessage::kUnknown;  /**< Current message type */

    /**
     * @brief Reads raw binary message data from file
     * @return Vector containing the raw message bytes
     */
    std::vector<char> ReadNextMessageFromFile();
};


} // namespace osi3

#endif //NATIVEBINARYTRACEFILEREADER_H
