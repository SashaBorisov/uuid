#
# Copyright © 2021, Alexander Borisov, https://github.com/SashaBorisov/uuid
#
cmake_minimum_required(VERSION 3.14)
project(uuid VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(uuid STATIC ./impl/rfc4122/uuid.cpp)
target_include_directories(uuid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/iface)

option(UUID_BUILD_TESTS OFF)

if(UUID_BUILD_TESTS)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(uuid_tests ./tests/uuid_tests.cpp)
target_include_directories(uuid_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/iface)
target_link_libraries(uuid_tests gtest_main)
target_link_libraries(uuid_tests uuid)

include(GoogleTest)
gtest_discover_tests(uuid_tests)

endif() # UUID_BUILD_TESTS
