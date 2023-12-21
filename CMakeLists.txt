cmake_minimum_required(VERSION 3.18)

include_guard(GLOBAL)

project( ni-midi2 LANGUAGES CXX )

if( CMAKE_PROJECT_NAME STREQUAL "ni-midi2" )
    set( IS_NIMIDI2 TRUE )
else()
    set( IS_NIMIDI2 FALSE )
endif()

option( NIMIDI2_TREAT_WARNINGS_AS_ERRORS "Treat compile warnings as errors"   OFF )
option( NIMIDI2_UNITY_BUILDS             "Build ni-midi2 with unity builds"   ON  )
option( NIMIDI2_TESTS                    "Build ni-midi2 Tests"     ${IS_NIMIDI2} )

option( NIMIDI2_CUSTOM_SYSEX_DATA_ALLOCATOR "Build with custom sysex data allocator" OFF )
option( NIMIDI2_PMR_SYSEX_DATA              "Build with sysex data use pmr"          OFF )

if (NIMIDI2_PMR_SYSEX_DATA AND NIMIDI2_CUSTOM_SYSEX_DATA_ALLOCATOR)
    message(FATAL_ERROR "NIMIDI2_PMR_SYSEX_DATA and NIMIDI2_CUSTOM_SYSEX_DATA_ALLOCATOR are mutually exclusibe")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(code-coverage)
include(nimidi2_add_test)
include(nimidi2_treat_warnings_as_errors)

if(NOT CMAKE_CXX_STANDARD)
    if (NOT NIMIDI2_PMR_SYSEX_DATA)
        set(CMAKE_CXX_STANDARD 17)
    else()
        set(CMAKE_CXX_STANDARD 20) # pmr requires at least c++20
    endif()
endif()

set(LibSources
    inc/midi/types.h
    inc/midi/manufacturer.h
    inc/midi/universal_packet.h src/universal_packet.cpp
    inc/midi/utility_message.h
    inc/midi/system_message.h
    inc/midi/channel_voice_message.h
    inc/midi/midi1_byte_stream.h src/midi1_byte_stream.cpp
    inc/midi/midi1_channel_voice_message.h
    inc/midi/midi2_channel_voice_message.h
    inc/midi/data_message.h
    inc/midi/extended_data_message.h
    inc/midi/flex_data_message.h
    inc/midi/stream_message.h
    inc/midi/sysex.h src/sysex.cpp
    inc/midi/sysex_collector.h src/sysex_collector.cpp
    inc/midi/universal_sysex.h src/universal_sysex.cpp
    inc/midi/capability_inquiry.h src/capability_inquiry.cpp
    inc/midi/jitter_reduction_timestamps.h src/jitter_reduction_timestamps.cpp
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${LibSources})

add_library(ni-midi2 EXCLUDE_FROM_ALL ${LibSources})
target_include_directories(ni-midi2 PUBLIC inc)
add_library(ni::midi2 ALIAS ni-midi2)

if (NIMIDI2_TREAT_WARNINGS_AS_ERRORS)
    nimidi2_treat_warnings_as_errors(ni-midi2)
endif()
if( NIMIDI2_UNITY_BUILDS )
    set_target_properties(ni-midi2 PROPERTIES
        UNITY_BUILD ON
        UNITY_BUILD_BATCH_SIZE 32)
endif()

if ( NIMIDI2_CUSTOM_SYSEX_DATA_ALLOCATOR )
    target_compile_definitions(ni-midi2 PUBLIC NIMIDI2_CUSTOM_SYSEX_DATA_ALLOCATOR=1)
endif()

if ( NIMIDI2_PMR_SYSEX_DATA )
    target_compile_definitions(ni-midi2 PUBLIC NIMIDI2_PMR_SYSEX_DATA=1)
endif()

if( NIMIDI2_TESTS )
    enable_testing()

    set(TestSources
        tests/tests.cpp
        tests/type_tests.cpp
        tests/value_translation_tests.cpp
        tests/universal_packet_tests.cpp
        tests/data_message_tests.cpp
        tests/extended_data_message_tests.cpp
        tests/flex_data_message_tests.cpp
        tests/stream_message_tests.cpp
        tests/system_message_tests.cpp
        tests/utility_message_tests.cpp
        tests/midi1_channel_voice_message_tests.cpp
        tests/midi2_channel_voice_message_tests.cpp
        tests/channel_voice_message_tests.cpp
        tests/sysex_tests.cpp tests/sysex_tests.h
        tests/sysex7_collector_tests.cpp
        tests/sysex7_test_data.cpp tests/sysex7_test_data.h
        tests/sysex8_collector_tests.cpp
        tests/sysex8_test_data.cpp tests/sysex8_test_data.h
        tests/universal_sysex_tests.cpp
        tests/capability_inquiry_tests.cpp
        tests/ci_profile_configuration_tests.cpp
        tests/ci_property_exchange_tests.cpp
        tests/ci_process_inquiry_tests.cpp
        tests/jitter_reduction_timestamps_tests.cpp
        tests/midi1_byte_stream_tests.cpp
    )

    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${TestSources})

    add_executable(ni-midi2-tests ${TestSources})
    target_link_libraries(ni-midi2-tests PRIVATE ni::midi2)

    if (NIMIDI2_TREAT_WARNINGS_AS_ERRORS)
        nimidi2_treat_warnings_as_errors(ni-midi2-tests)
    endif()
    if(NIMIDI2_UNITY_BUILDS)
        set_target_properties(ni-midi2-tests PROPERTIES UNITY_BUILD ON)
    endif()

    find_package(GTest REQUIRED)
    target_link_libraries(ni-midi2-tests PRIVATE GTest::GTest GTest::gmock_main)

    nimidi2_add_test(ni-midi2-tests GTEST)
endif(NIMIDI2_TESTS)
