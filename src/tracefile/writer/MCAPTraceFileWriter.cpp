//
// Copyright (c) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// SPDX-License-Identifier: MPL-2.0
//

#include "osi-utilities/tracefile/writer/MCAPTraceFileWriter.h"

#include <google/protobuf/stubs/common.h>

#include "google/protobuf/descriptor.pb.h"
#include "osi_groundtruth.pb.h"
#include "osi_hostvehicledata.pb.h"
#include "osi_motionrequest.pb.h"
#include "osi_sensordata.pb.h"
#include "osi_sensorview.pb.h"
#include "osi_streamingupdate.pb.h"
#include "osi_trafficcommand.pb.h"
#include "osi_trafficcommandupdate.pb.h"
#include "osi_trafficupdate.pb.h"
#include "osi_version.pb.h"

namespace {
// helper functions from https://github.com/foxglove/mcap/blob/4ec37c5a5d0115bceaca428b1e8a0e3e5aae20cf/website/docs/guides/cpp/protobuf.md?plain=1#L198
// TODO: might want to change to the approach used here https://github.com/foxglove/mcap/blob/main/cpp/examples/protobuf/writer.cpp
// Recursively adds all `fd` dependencies to `fd_set`.
void fdSetInternal(google::protobuf::FileDescriptorSet& fd_set, std::unordered_set<std::string>& files, const google::protobuf::FileDescriptor* file_descriptor) {
    for (int i = 0; i < file_descriptor->dependency_count(); ++i) {
        const auto* dep = file_descriptor->dependency(i);
        if (auto [_, inserted] = files.insert(dep->name()); !inserted) {
            continue;
        }
        fdSetInternal(fd_set, files, file_descriptor->dependency(i));
    }
    file_descriptor->CopyTo(fd_set.add_file());
}

// Returns a serialized google::protobuf::FileDescriptorSet containing
// the necessary google::protobuf::FileDescriptor's to describe d.
std::string fdSet(const google::protobuf::Descriptor* descriptor) {
    std::unordered_set<std::string> files;
    google::protobuf::FileDescriptorSet fd_set;
    fdSetInternal(fd_set, files, descriptor->file());
    return fd_set.SerializeAsString();
}

}  // namespace

namespace osi3 {

bool MCAPTraceFileWriter::Open(const std::string& file_path) {
    const auto res = mcap_writer_.open(file_path, mcap_options_);
    if (res.ok()) {
        file_open_ = true;
        this->AddVersionMetadata();
    }
    return res.ok();
}

bool MCAPTraceFileWriter::Open(const std::string& file_path, const mcap::McapWriterOptions& options) {
    mcap_options_ = options;
    return this->Open(file_path);
}

template <typename T>
bool MCAPTraceFileWriter::WriteMessage(T top_level_message, const std::string& topic) {
    if (topic.empty()) {
        std::cerr << "MCAPTraceFileWriter: cannot write message, topic is empty\n";
        return false;
    }
    if (!file_open_) {
        std::cerr << "MCAPTraceFileWriter: cannot write message, file is not open\n";
        return false;
    }
    if (!required_metadata_added_) {
        std::cerr << "MCAPTraceFileWriter: cannot write message, required metadata (according to the OSI specification) was not set in advance\n";
        return false;
    }

    // get channel id from topic using topic_to_channel_id_
    const auto topic_channel_id = topic_to_channel_id_.find(topic);
    if (topic_channel_id == topic_to_channel_id_.end()) {
        std::cerr << "MCAPTraceFileWriter: cannot write message, topic " << topic << " not found\n";
        return false;
    }

    const std::string data = top_level_message.SerializeAsString();
    mcap::Message msg;
    msg.channelId = topic_channel_id->second;

    // msg.logTime should be now in nanoseconds
    msg.logTime = top_level_message.timestamp().seconds() * 1000000000 + top_level_message.timestamp().nanos();
    msg.publishTime = msg.logTime;
    msg.data = reinterpret_cast<const std::byte*>(data.data());
    msg.dataSize = data.size();
    if (const auto status = mcap_writer_.write(msg); status.code != mcap::StatusCode::Success) {
        std::cerr << "Error: Failed to write message " << status.message;
        return false;
    }
    std::cout << "Wrote message with timestamp: " << msg.logTime << std::endl;  // todo remove debug print
    return true;
}

bool MCAPTraceFileWriter::SetMetadata(const std::string& name, const std::unordered_map<std::string, std::string>& metadata_entries) {
    mcap::Metadata metadata;
    metadata.name = name;
    metadata.metadata = metadata_entries;

    // check if the provided metadata contains the required metadata by the OSI specification
    // to allow writing messages to the trace file
    if ((metadata.name == "asam_osi") && (metadata.metadata.find("timestamp") != metadata.metadata.end()) && (metadata.metadata.find("zero_time") != metadata.metadata.end())) {
        this->required_metadata_added_ = true;
    }

    // add metadata to file
    if (const auto status = mcap_writer_.write(metadata); status.code != mcap::StatusCode::Success) {
        std::cerr << "Error: Failed to write metadata with name " << name << "\n" << status.message;
        return false;
    }
    return true;
}

void MCAPTraceFileWriter::Close() {
    file_open_ = false;
    mcap_writer_.close();
}

void MCAPTraceFileWriter::AddVersionMetadata() {
    mcap::Metadata metadata_versions{"versions"};
    const auto osi_version = osi3::InterfaceVersion::descriptor()->file()->options().GetExtension(osi3::current_interface_version);
    metadata_versions.metadata["osi"] =
        std::to_string(osi_version.version_major()) + "." + std::to_string(osi_version.version_minor()) + "." + std::to_string(osi_version.version_patch());

    if (const auto status = mcap_writer_.write(metadata_versions); status.code != mcap::StatusCode::Success) {
        throw std::runtime_error("Error: Failed to write metadata versions. " + status.message);
    }
}

uint16_t MCAPTraceFileWriter::AddChannel(const std::string& topic, const google::protobuf::Descriptor* descriptor, std::unordered_map<std::string, std::string> channel_metadata) {
    // Check if the schema for this descriptor's full name already exists
    mcap::Schema path_schema;
    const auto it_schema = std::find_if(schemas_.begin(), schemas_.end(), [&](const mcap::Schema& schema) { return schema.name == descriptor->full_name(); });

    // Check if topic already exists
    if (topic_to_channel_id_.find(topic) != topic_to_channel_id_.end()) {
        // if it has the same schema, return the channel id
        if (it_schema != schemas_.end()) {
            std::cout << "WARNING: Topic already exists with the same message type, returning original channel id\n";
            return topic_to_channel_id_[topic];
        }
        throw std::runtime_error("Topic already exists with a different message type");
    }

    // for a new topic, check if the schema exists or must be added first
    if (it_schema == schemas_.end()) {
        // Schema doesn't exist, create a new one
        path_schema = mcap::Schema(descriptor->full_name(), "protobuf", fdSet(descriptor));
        mcap_writer_.addSchema(path_schema);
        schemas_.push_back(path_schema);
    } else {
        // Schema already exists, use the existing one
        path_schema = *it_schema;
    }

    // add osi version to channel metadata as required by spec.
    const auto osi_version = osi3::InterfaceVersion::descriptor()->file()->options().GetExtension(osi3::current_interface_version);
    channel_metadata["osi_version"] =
        std::to_string(osi_version.version_major()) + "." + std::to_string(osi_version.version_minor()) + "." + std::to_string(osi_version.version_patch());
    channel_metadata["protobuf_version"] = google::protobuf::internal::VersionString(GOOGLE_PROTOBUF_VERSION);

    // add the channel to the writer/mcap file
    mcap::Channel channel(topic, "protobuf", path_schema.id, channel_metadata);
    mcap_writer_.addChannel(channel);

    // store channel id for writing use
    topic_to_channel_id_[topic] = channel.id;

    return channel.id;
}

std::string MCAPTraceFileWriter::GetCurrentTimeAsString() {
    const auto now = std::chrono::system_clock::now();
    const auto now_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    const auto timer = std::chrono::system_clock::to_time_t(now);
    const std::tm utc_time_structure = *std::gmtime(&timer);  // Greenwich Mean Time (GMT) is in Coordinated Universal Time (UTC) zone
    std::ostringstream oss;
    oss << std::put_time(&utc_time_structure, "%Y%m%dT%H%M%S");
    oss << '.' << std::setfill('0') << std::setw(1) << (now_in_ms.count() / 100);
    oss << "Z";  // As GMT is used as a reference time zone, add Z to indicate UTC (+00:00)
    return oss.str();
}

// template instantiations of allowed OSI top-level messages
template bool MCAPTraceFileWriter::WriteMessage<osi3::GroundTruth>(osi3::GroundTruth, const std::string&);
template bool MCAPTraceFileWriter::WriteMessage<osi3::SensorData>(osi3::SensorData, const std::string&);
template bool MCAPTraceFileWriter::WriteMessage<osi3::SensorView>(osi3::SensorView, const std::string&);
template bool MCAPTraceFileWriter::WriteMessage<osi3::HostVehicleData>(osi3::HostVehicleData, const std::string&);
template bool MCAPTraceFileWriter::WriteMessage<osi3::TrafficCommand>(osi3::TrafficCommand, const std::string&);
template bool MCAPTraceFileWriter::WriteMessage<osi3::TrafficCommandUpdate>(osi3::TrafficCommandUpdate, const std::string&);
template bool MCAPTraceFileWriter::WriteMessage<osi3::TrafficUpdate>(osi3::TrafficUpdate, const std::string&);
template bool MCAPTraceFileWriter::WriteMessage<osi3::MotionRequest>(osi3::MotionRequest, const std::string&);
template bool MCAPTraceFileWriter::WriteMessage<osi3::StreamingUpdate>(osi3::StreamingUpdate, const std::string&);
}  // namespace osi3