cmake_minimum_required(VERSION 3.16)
project(TRTCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(trt_core STATIC
    core/protocol/parser.cpp
    core/protocol/encoder.cpp
    core/dispatcher/command_registry.cpp
    core/dispatcher/command_dispatcher.cpp
    core/capabilities/capability_manager.cpp
    core/board/board_context.cpp
    core/board/board_info.cpp
    core/errors/error_codes.cpp
    core/logging/logger.cpp
    modules/module_manager.cpp
)
target_include_directories(trt_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(trt_simulator STATIC
    simulator/simulator_board.cpp
)
target_include_directories(trt_simulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(trt_simulator PUBLIC trt_core)

add_executable(trt_simulator_app simulator/main.cpp)
target_link_libraries(trt_simulator_app PRIVATE trt_simulator)

add_executable(trt_simulator_example examples/simulator/protocol_walkthrough.cpp)
target_link_libraries(trt_simulator_example PRIVATE trt_simulator)

add_executable(trt_tests tests/simulator_protocol_tests.cpp)
target_link_libraries(trt_tests PRIVATE trt_simulator)

enable_testing()
add_test(NAME trt_protocol_simulator_tests COMMAND trt_tests)
