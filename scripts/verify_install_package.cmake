if(NOT DEFINED LIVIO_INSTALL_PREFIX)
    message(FATAL_ERROR "LIVIO_INSTALL_PREFIX is required")
endif()

get_filename_component(_repo_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
get_filename_component(_prefix "${LIVIO_INSTALL_PREFIX}" ABSOLUTE)
set(_smoke_root "${_repo_root}/build/install-smoke-ci")
set(_smoke_build "${_smoke_root}/out")

file(REMOVE_RECURSE "${_smoke_root}")
file(MAKE_DIRECTORY "${_smoke_root}")
file(WRITE "${_smoke_root}/CMakeLists.txt" [=[
cmake_minimum_required(VERSION 4.2)
project(LivioArkInstallSmoke LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(LivioArk CONFIG REQUIRED)
add_executable(smoke main.cpp)
target_link_libraries(smoke PRIVATE LivioArk::LivioArk)
]=])
file(WRITE "${_smoke_root}/main.cpp" [=[
#include "livioark/log/log.hpp"

int main()
{
    livio::ark::log::init();
    LOG_INFO("install smoke");
    return 0;
}
]=])

execute_process(
    COMMAND "${CMAKE_COMMAND}" -S "${_smoke_root}" -B "${_smoke_build}" -G Ninja "-DCMAKE_PREFIX_PATH=${_prefix}"
    RESULT_VARIABLE _configure_result
)
if(NOT _configure_result EQUAL 0)
    message(FATAL_ERROR "Install smoke configure failed with exit code ${_configure_result}")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" --build "${_smoke_build}"
    RESULT_VARIABLE _build_result
)
if(NOT _build_result EQUAL 0)
    message(FATAL_ERROR "Install smoke build failed with exit code ${_build_result}")
endif()

message(STATUS "Install package smoke passed: ${_prefix}")
