cmake_minimum_required(VERSION 3.16)
project(osi-utilities)

# Som features (e.g. mcap) require at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Generate compile_commands.json for clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)

# Add the include folder for public headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# option for specifying the path to the open_simulation_interface.
option(USE_EXTERNAL_OSI "Use an external version of open_simulation_interface" OFF)
option(LINK_WITH_SHARED_OSI "Link utils with shared OSI library instead of statically linking" OFF)

# Locate open_simulation_interface using find_package or custom path.
if (USE_EXTERNAL_OSI)
    message(FATAL_ERROR "Not implemented yet")
else ()
    # Download and use our own open_simulation_interface package.
    include(FetchContent)
    FetchContent_Declare(
            open_simulation_interface
            GIT_REPOSITORY https://github.com/OpenSimulationInterface/open-simulation-interface.git
            GIT_TAG v3.7.0
    )
    FetchContent_MakeAvailable(open_simulation_interface)
    set(OSI_INCLUDE_DIR ${open_simulation_interface_BINARY_DIR})
endif ()

# option to enable coverage reporting
option(CODE_COVERAGE "Enable coverage reporting" OFF)
if (CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
    message(WARNING "Added coverage options")
endif ()

# build doxygen documentation
find_package(Doxygen)
if (DOXYGEN_FOUND AND PROJECT_IS_TOP_LEVEL)
    message(STATUS "Doxygen found, generating API documentation")
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile)
    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    # note the option ALL which allows to build the docs together with the application
    add_custom_target(
            library_api_doc
            ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
    )
endif (DOXYGEN_FOUND AND PROJECT_IS_TOP_LEVEL)

add_subdirectory(src)
if (PROJECT_IS_TOP_LEVEL)
    enable_testing()
    add_subdirectory(tests)
endif ()
add_subdirectory(examples)
