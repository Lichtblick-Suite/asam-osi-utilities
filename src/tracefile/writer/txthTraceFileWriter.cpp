//
// Copyright (c) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// SPDX-License-Identifier: MPL-2.0
//

#include "osi-utilities/tracefile/writer/txthTraceFileWriter.h"

#include "osi_groundtruth.pb.h"
#include "osi_hostvehicledata.pb.h"
#include "osi_motionrequest.pb.h"
#include "osi_sensordata.pb.h"
#include "osi_sensorview.pb.h"
#include "osi_streamingupdate.pb.h"
#include "osi_trafficcommand.pb.h"
#include "osi_trafficcommandupdate.pb.h"
#include "osi_trafficupdate.pb.h"

namespace osi3 {

bool txthTraceFileWriter::Open(const std::string& filename) {
    if (filename.substr(filename.length() - 5) != ".txth") {
        std::cerr << "Error: Filename must end with .txth extension\n";
        return false;
    }

    trace_file_.open(filename);
    if (trace_file_.is_open()) {
        file_open_ = true;
        return true;
    }
    return false;
}

void txthTraceFileWriter::Close() {
    if (file_open_) {
        trace_file_.close();
        file_open_ = false;
    }
}

template <typename T>
bool txthTraceFileWriter::WriteMessage(T top_level_message) {
    if (!file_open_) {
        std::cerr << "Error: Cannot write message, file is not open\n";
        return false;
    }

    std::string text_output;
    if (!google::protobuf::TextFormat::PrintToString(top_level_message, &text_output)) {
        std::cerr << "Error: Failed to convert message to text format\n";
        return false;
    }

    trace_file_ << text_output;
    return true;
}

// Template instantiations for allowed OSI top-level messages
template bool txthTraceFileWriter::WriteMessage<osi3::GroundTruth>(osi3::GroundTruth);
template bool txthTraceFileWriter::WriteMessage<osi3::SensorData>(osi3::SensorData);
template bool txthTraceFileWriter::WriteMessage<osi3::SensorView>(osi3::SensorView);
template bool txthTraceFileWriter::WriteMessage<osi3::HostVehicleData>(osi3::HostVehicleData);
template bool txthTraceFileWriter::WriteMessage<osi3::TrafficCommand>(osi3::TrafficCommand);
template bool txthTraceFileWriter::WriteMessage<osi3::TrafficCommandUpdate>(osi3::TrafficCommandUpdate);
template bool txthTraceFileWriter::WriteMessage<osi3::TrafficUpdate>(osi3::TrafficUpdate);
template bool txthTraceFileWriter::WriteMessage<osi3::MotionRequest>(osi3::MotionRequest);
template bool txthTraceFileWriter::WriteMessage<osi3::StreamingUpdate>(osi3::StreamingUpdate);

}  // namespace osi3