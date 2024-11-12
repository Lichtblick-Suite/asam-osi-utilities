//
// Copyright (c) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// SPDX-License-Identifier: MPL-2.0
//

#ifndef OSIUTILITIES_TRACEFILE_WRITER_MCAPTRACEFILEWRITER_H_
#define OSIUTILITIES_TRACEFILE_WRITER_MCAPTRACEFILEWRITER_H_

#include <mcap/mcap.hpp>

#include "../Writer.h"

namespace osi3 {
/**
 * @brief MCAP format implementation of the trace file writer
 *
 * Handles writing OSI messages to MCAP format files with support for
 * channels, schemas, and metadata.
 */
class MCAPTraceFileWriter final : public osi3::TraceFileWriter {
   public:
    bool Open(const std::string& filename) override;

    template <typename T>
    bool WriteMessage(T top_level_message, const std::string& topic = "");
    bool SetMetadata(const std::string& name, const std::unordered_map<std::string, std::string>& metadata_entries) override;

    /**
     * @brief Adds a new channel to the MCAP file
     * @param topic Name of the channel/topic
     * @param descriptor Protobuf descriptor for the message type
     * @param channel_metadata Additional metadata for the channel
     * @return Channel ID for the newly created channel
     */
    uint16_t AddChannel(const std::string& topic, const google::protobuf::Descriptor* descriptor, std::unordered_map<std::string, std::string> channel_metadata = {});

    void Close() override;

   /**
   * @brief Gets the underlying MCAP writer instance
   * @return Pointer to the internal McapWriter object
   *
   * This function can be useful for advanced operations
   * like adding non-OSI message which requires direct access to
   * underlying the MCAP writer.
   */
    mcap::McapWriter* GetMcapWriter() { return &mcap_writer_; }

   private:
    mcap::McapWriter mcap_writer_;                        /**< MCAP writer instance */
    mcap::McapWriterOptions mcap_options_{"protobuf"};    /**< MCAP writer configuration */
    bool file_open_ = false;                              /**< File open state */
    std::vector<mcap::Schema> schemas_;                   /**< Registrated schemas */
    std::map<std::string, uint16_t> topic_to_channel_id_; /**< Topic to channel ID mapping */

    /**
     * @brief Adds standard metadata to the MCAP file
     *
     * Includes OSI version information and file creation timestamp
     */
    void AddCommonMetadata();
};
}  // namespace osi3
#endif  // OSIUTILITIES_TRACEFILE_WRITER_MCAPTRACEFILEWRITER_H_
