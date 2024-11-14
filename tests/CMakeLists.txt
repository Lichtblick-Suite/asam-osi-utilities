include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)
include(GoogleTest)

file(GLOB_RECURSE test_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE src_sources ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp)

add_executable(
        unit_tests
        ${test_sources}
        ${src_sources}
)
target_link_libraries(unit_tests PRIVATE GTest::gtest_main)

# Link against OSI made available by parent CMakeLists.txt
if (LINK_WITH_SHARED_OSI)
    target_link_libraries(unit_tests PRIVATE open_simulation_interface)
else ()
    target_link_libraries(unit_tests PRIVATE open_simulation_interface_pic)
    include_directories(${OSI_INCLUDE_DIR})
endif ()

# get mcap and its dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(lz4 REQUIRED IMPORTED_TARGET liblz4)
pkg_check_modules(zstd REQUIRED libzstd)
target_link_libraries(unit_tests PUBLIC ${lz4_LIBRARIES} ${zstd_LIBRARIES})
target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lib/mcap/cpp/mcap/include)

# include public headers of the library
target_include_directories(unit_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
gtest_discover_tests(unit_tests)   # Register the tests to gtest