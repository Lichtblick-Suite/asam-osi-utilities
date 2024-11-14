# Add subdirectories for organized modules
include_directories(tracefile)

# Option to build and run tests.
option(BUILD_TESTING "Build the testing suite" ON)

# specify library source files
set(OSIUtilities_SRCS
        tracefile/reader/Reader.cpp
        tracefile/MCAPImplementation.cpp
        tracefile/reader/NativeBinaryTraceFileReader.cpp
        tracefile/writer/NativeBinaryTraceFileWriter.cpp
        tracefile/reader/TXTHTraceFileReader.cpp
        tracefile/writer/TXTHTraceFileWriter.cpp
        tracefile/reader/MCAPTraceFileReader.cpp
        tracefile/writer/MCAPTraceFileWriter.cpp
)

# Create a library target for the entire library
add_library(OSIUtilities ${OSIUtilities_SRCS})

# Specify the public headers for the library for clean access
target_include_directories(OSIUtilities PUBLIC ${PROJECT_SOURCE_DIR}/include)


# Link against OSI made available by parent CMakeLists.txt
if(LINK_WITH_SHARED_OSI)
    target_link_libraries(OSIUtilities PRIVATE open_simulation_interface)
else()
    target_link_libraries(OSIUtilities PRIVATE open_simulation_interface_pic)
    include_directories(${OSI_INCLUDE_DIR})
endif()

# get mcap and its dependencies
# todo change logic and cleanup
find_package(PkgConfig REQUIRED)
pkg_check_modules(lz4 REQUIRED IMPORTED_TARGET liblz4)
pkg_check_modules(zstd REQUIRED libzstd)
target_link_libraries(OSIUtilities PRIVATE ${lz4_LIBRARIES} ${zstd_LIBRARIES})

target_include_directories(OSIUtilities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../lib/mcap/cpp/mcap/include)