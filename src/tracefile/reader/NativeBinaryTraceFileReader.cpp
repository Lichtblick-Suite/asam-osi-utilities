//
// Copyright (c) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// SPDX-License-Identifier: MPL-2.0
//

#include "osi-utilities/tracefile/reader/NativeBinaryTraceFileReader.h"

#include <filesystem>
#include <sstream>

namespace osi3 {

bool NativeBinaryTraceFileReader::Open(const std::string& filename, const ReaderTopLevelMessage message_type) {
    message_type_ = message_type;
    return this->Open(filename);
}

bool NativeBinaryTraceFileReader::Open(const std::string& file_path) {
    // check if at least .osi ending is present
    if (file_path.find(".osi") == std::string::npos) {
        std::cerr << "ERROR: The trace file '" << file_path << "' must have a '.osi' extension." << std::endl;
        return false;
    }

    // check if file exists
    if (!std::filesystem::exists(file_path)) {
        std::cerr << "ERROR: The trace file '" << file_path << "' does not exist." << std::endl;
        return false;
    }

    // Determine message type based on filename if not specified in advance
    if (message_type_ == ReaderTopLevelMessage::kUnknown) {
        for (const auto& [key, value] : kFileNameMessageTypeMap) {
            if (file_path.find(key) != std::string::npos) {
                message_type_ = value;
                break;
            }
        }
    }
    // if message_type_ is still unknown, return false
    if (message_type_ == ReaderTopLevelMessage::kUnknown) {
        std::cerr << "ERROR: Unable to determine message type from the filename '" << file_path
                  << "'. Please ensure the filename follows the recommended OSI naming conventions as specified in the documentation or specify the message type manually."
                  << std::endl;
        return false;
    }

    parser_ = kParserMap_.at(message_type_);

    trace_file_ = std::ifstream(file_path, std::ios::binary);
    if (!trace_file_) {
        std::cerr << "ERROR: Failed to open trace file: " << file_path << std::endl;
        return false;
    }
    return true;
}

void NativeBinaryTraceFileReader::Close() { trace_file_.close(); }

bool NativeBinaryTraceFileReader::HasNext() { return (trace_file_ && trace_file_.is_open() && trace_file_.peek() != EOF); }

std::optional<ReadResult> NativeBinaryTraceFileReader::ReadMessage() {
    // check if ready and if there are messages left
    if (!this->HasNext()) {
        std::cerr << "Unable to read message: No more messages available in trace file or file not opened." << std::endl;
        return std::nullopt;
    }

    const auto serialized_msg = ReadNextMessageFromFile();

    if (serialized_msg.empty()) {
        throw std::runtime_error("Failed to read message");
    }

    ReadResult result;
    result.message = parser_(serialized_msg);
    result.message_type = message_type_;

    return result;
}

std::vector<char> NativeBinaryTraceFileReader::ReadNextMessageFromFile() {
    uint32_t message_size = 0;

    if (!trace_file_.read(reinterpret_cast<char*>(&message_size), sizeof(message_size))) {
        throw std::runtime_error("ERROR: Failed to read message size from file.");
    }
    std::vector<char> serialized_msg(message_size);
    if (!trace_file_.read(serialized_msg.data(), message_size)) {
        throw std::runtime_error("ERROR: Failed to read message from file");
    }
    return serialized_msg;
}

}  // namespace osi3