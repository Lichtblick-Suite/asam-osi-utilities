# Add subdirectories for organized modules
include_directories(tracefile)

# Option to build and run tests.
option(BUILD_TESTING "Build the testing suite" ON)

# specify library source files
set(OSIUtilities_SRCS
        tracefile/reader/Reader.cpp
        tracefile/MCAPImplementation.cpp
        tracefile/reader/SingleChannelBinaryTraceFileReader.cpp
        tracefile/writer/SingleChannelBinaryTraceFileWriter.cpp
        tracefile/reader/TXTHTraceFileReader.cpp
        tracefile/writer/TXTHTraceFileWriter.cpp
        tracefile/reader/MCAPTraceFileReader.cpp
        tracefile/writer/MCAPTraceFileWriter.cpp
)

# Create a library target for the entire library
add_library(OSIUtilities ${OSIUtilities_SRCS})

# add a compile definition to specify the version of the trace file specification.
# This is not the version of the linked OSI library, but the version of the trace file spec
# which is implemented (in functions, methods etc.) by this library (OSIUtilities).
# TODO: As of today (2nd of December 2024) mcap support is neither merged nor officially released yet.
# TODO: Change the following value once a new OSI version including mcap is officially released. Until then assume 3.7.1.
target_compile_definitions(OSIUtilities PRIVATE OSI_TRACE_FILE_SPEC_VERSION="3.7.1")


# Specify the public headers for the library for clean access
target_include_directories(OSIUtilities PUBLIC ${PROJECT_SOURCE_DIR}/include)


# Link against OSI made available by parent CMakeLists.txt
if (LINK_WITH_SHARED_OSI)
    target_link_libraries(OSIUtilities PRIVATE open_simulation_interface)
else ()
    target_link_libraries(OSIUtilities PRIVATE open_simulation_interface_pic)
    include_directories(${OSI_INCLUDE_DIR})
endif ()

# get mcap and its dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(lz4 REQUIRED IMPORTED_TARGET liblz4)
pkg_check_modules(zstd REQUIRED libzstd)
target_link_libraries(OSIUtilities PRIVATE ${lz4_LIBRARIES} ${zstd_LIBRARIES})
target_include_directories(OSIUtilities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../lib/mcap/cpp/mcap/include)