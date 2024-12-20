//
// Copyright (c) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// SPDX-License-Identifier: MPL-2.0
//

#ifndef OSIUTILITIES_TRACEFILE_WRITER_SINGLECHANNELBINARYTRACEFILEWRITER_H_
#define OSIUTILITIES_TRACEFILE_WRITER_SINGLECHANNELBINARYTRACEFILEWRITER_H_

#include <fstream>

#include "osi-utilities/tracefile/Writer.h"

namespace osi3 {

/**
 * @brief Implementation of TraceFileWriter for binary format files containing OSI messages
 *
 * This class provides functionality to write OSI messages in the single binary channel format.
 * It stores messages in their serialized protobuf binary representation in a single channel.
 * Messages are separated by a length specification before each message.
 * The length is represented by a four-byte, little-endian, unsigned integer. T
 */
class SingleChannelBinaryTraceFileWriter final : public TraceFileWriter {
   public:
    bool Open(const std::filesystem::path& file_path) override;
    void Close() override;

    /**
     * @brief Writes a protobuf message to the file
     * @tparam T Type of the protobuf message
     * @param top_level_message The message to write
     * @return true if successful, false otherwise
     */
    template <typename T>
    bool WriteMessage(const T& top_level_message);

   private:
    std::ofstream trace_file_;
    bool file_open_ = false;
};

}  // namespace osi3
#endif  // OSIUTILITIES_TRACEFILE_WRITER_SINGLECHANNELBINARYTRACEFILEWRITER_H_