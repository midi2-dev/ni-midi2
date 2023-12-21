###
### this file is private to the Native Instruments build system, please ignore
###

if (TARGET ni-midi2)
  return()
endif()

### set package path to current path if not set.
###
### we have to define and use a local function here so that CMAKE_CURRENT_FUNCTION_LIST_DIR is set.
function( pr_init_nimidi2_package_path )
  if( NOT DEFINED NI_NIMIDI2_PACKAGE_PATH )
      set( NI_NIMIDI2_PACKAGE_PATH ${CMAKE_CURRENT_FUNCTION_LIST_DIR} PARENT_SCOPE )
  endif()
endfunction()
pr_init_nimidi2_package_path()

set( ni_midi2_sources
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/types.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/types.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/manufacturer.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/universal_packet.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/src/universal_packet.cpp"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/utility_message.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/system_message.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/channel_voice_message.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/midi1_byte_stream.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/src/midi1_byte_stream.cpp"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/midi1_channel_voice_message.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/midi2_channel_voice_message.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/data_message.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/extended_data_message.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/flex_data_message.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/stream_message.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/sysex.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/src/sysex.cpp"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/sysex_collector.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/src/sysex_collector.cpp"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/universal_sysex.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/src/universal_sysex.cpp"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/capability_inquiry.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/src/capability_inquiry.cpp"
    "${NI_NIMIDI2_PACKAGE_PATH}/inc/midi/jitter_reduction_timestamps.h"
    "${NI_NIMIDI2_PACKAGE_PATH}/src/jitter_reduction_timestamps.cpp"
)

add_library( ni-midi2 EXCLUDE_FROM_ALL ${ni_midi2_sources} )
set_target_properties( ni-midi2 PROPERTIES
    FOLDER 3rdparty
    UNITY_BUILD ON
    UNITY_BUILD_BATCH_SIZE 32
)
target_include_directories( ni-midi2
    SYSTEM PUBLIC "${NI_NIMIDI2_PACKAGE_PATH}/inc"
)
add_library(ni::midi2 ALIAS ni-midi2)

option( NIMIDI2_PMR_SYSEX_DATA "Build with sysex data use pmr" OFF )

if (NIMIDI2_PMR_SYSEX_DATA)
    target_compile_definitions(ni-midi2 PUBLIC NIMIDI2_PMR_SYSEX_DATA)
endif()

###### Tests ######

option( NI_MIDI2_BUILD_TESTS "Build ni-midi tests" ${NI_3RDPARTY_BUILD_TESTS} )

if ( NI_MIDI2_BUILD_TESTS )
    enable_testing()

    ni_build_headers_in_isolation( ni-midi2 )

    set( ni_midi2_test_sources
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/type_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/value_translation_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/universal_packet_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/data_message_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/extended_data_message_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/flex_data_message_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/stream_message_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/system_message_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/utility_message_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/midi1_channel_voice_message_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/midi2_channel_voice_message_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/channel_voice_message_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/sysex_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/sysex7_collector_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/sysex7_test_data.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/sysex8_collector_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/sysex8_test_data.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/universal_sysex_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/capability_inquiry_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/ci_profile_configuration_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/ci_property_exchange_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/ci_process_inquiry_tests.cpp"
        "${NI_NIMIDI2_PACKAGE_PATH}/tests/midi1_byte_stream_tests.cpp"
    )

    add_executable( ni-midi2-tests ${ni_midi2_test_sources})
    target_link_libraries(ni-midi2-tests PRIVATE ni::midi2 gtest)

    set_target_properties(ni-midi2-tests PROPERTIES UNITY_BUILD ON)

    ni_add_test(ni-midi2-tests GTEST)
endif( NI_MIDI2_BUILD_TESTS )
