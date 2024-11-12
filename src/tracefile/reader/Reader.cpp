//
// Copyright (c) 2024, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// SPDX-License-Identifier: MPL-2.0
//

#include "osi-utilities/tracefile/Reader.h"

#include "osi-utilities/tracefile/reader/NativeBinaryTraceFileReader.h"
#include "osi-utilities/tracefile/reader/TXTHTraceFileReader.h"
#include "osi-utilities/tracefile/reader/MCAPTraceFileReader.h"

std::unique_ptr<osi3::TraceFileReader> createTraceFileReader(const std::string& format) {
    if (format == "mcap") {
        return std::make_unique<osi3::MCAPTraceFileReader>();
    }
    if (format == "osi") {
        return std::make_unique<osi3::NativeBinaryTraceFileReader>();
    }
    if (format == "txth") {
        return std::make_unique<osi3::TXTHTraceFileReader>();
    }
    throw std::invalid_argument("Unsupported format: " + format);
}