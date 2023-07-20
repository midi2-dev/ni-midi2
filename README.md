# ni-midi2

`ni-midi2` is a C++ library for working with MIDI 2 Universal MIDI Packets (UMP) and MIDI-CI 1.2 messages.

## Library Design

The library provides the basic functionality of UMP 1.1 and MIDI-CI 1.2 by providing base classes for all UMP 1.1 packet types, (Universal) System Exclusive messages and MIDI-CI messages.

    struct universal_packet;

    struct utility_message;
    struct system_message;
    struct midi1_channel_voice_message;
    struct midi2_channel_voice_message;
    struct data_message;
    struct extended_data_message;
    struct flex_data_message;
    struct stream_message;

    struct sysex;
    struct sysex7;
    struct sysex8;

    struct universal_sysex::message;
    struct midi::ci::message;

There are concrete types for controllers, velocity and pitch, plus type aliases for common message field types.

    struct velocity;

    struct pitch_bend;
    struct pitch_bend_sensitivity;

    struct pitch_7_9;
    struct pitch_7_25;
    struct pitch_increment;

    struct controller_value;
    struct controller_increment;

    using group_t      = uint4_t;
    using status_t     = uint8_t;
    using channel_t    = uint4_t;
    using controller_t = uint7_t;
    ...

Mathematical operators allow to do integer / fixed point math on pitches and controllers, type constructors allow initialization with values of different resolution.

    pitch_with_pb = a_pitch_7_25 + a_pitch_bend_value * cur_pitch_bend_sensitivity;
    a_controller_value += a_controller_increment;

    auto from7bit = controller_value{ uint7_t{ 44 } };
    auto from32bit = controller_value{ uint32_t{ 0x45883312 } };
    auto fromFloat = pitch_7_25 { 66.1f };

Conrete instances of packets or messages are created using factory functions.

Incoming packets and messages are inspected using data views.

The library is completed by a number of helper functionalities dealing with conversion from / to MIDI 1 byte stream data format, collecting sysex messages and more.

## Factory Functions

UMPs and SysEx messages are created using free factory functions, there are no classes / constructors available to create specific packets or MIDI-CI messages.

Examples for UMP factory functions:

    system_message make_system_message(group_t, status_t, uint7_t data1 = 0, uint7_t data2 = 0);
    system_message make_song_position_message(group_t, uint14_t position);

    midi1_channel_voice_message make_midi1_note_on_message(group_t, channel_t, note_nr_t, velocity);
    midi1_channel_voice_message make_midi1_program_change_message(group_t, channel_t, program_t);
    midi1_channel_voice_message make_midi1_pitch_bend_message(group_t, channel_t, pitch_bend);
    ...

    midi2_channel_voice_message make_midi2_note_on_message(group_t, status_t, note_nr_t, velocity);
    midi2_channel_voice_message make_midi2_note_on_message(group_t, status_t, note_nr_t, velocity, pitch_7_9);
    midi2_channel_voice_message make_registered_controller_message(
        group_t, channel_t, uint7_t bank, uint7_t index, controller_value);
    ...

    data_message make_sysex7_complete_packet(group_t = 0);
    ...

    flex_data_message make_set_tempo_message(group_t, uint32_t ten_ns_per_quarter_note);
    ...

    stream_message make_endpoint_discovery_message(
        uint8_t filter, uint8_t ump_version_major = 1, uint8_t ump_version_minor = 1);
    ...

Examples for MIDI-CI factory functions:

    ci::message make_discovery_inquiry(muid_t            source_muid,
                              const identity_t& identity,
                              uint7_t           categories,
                              uint28_t          max_message_size,
                              uint7_t           output_path_id = 0);
    ...

    ci::message make_midi_message_report_reply(
        muid_t source_muid, uint7_t system_messages, uint7_t channel_controller_messages,
        uint7_t note_data_messages, uint7_t device_id = 0x7F);


## Data Views

One can inspect incoming UMPs using packet data views:

    if (packet.type() == packet_type::system)
    {
        auto m = system_message_view{ packet };

        // access message data
        if (m.status() == system_status::song_select)
        {
            auto song = m.data_byte_1();
        }
    }

Alternatively one can use `std::optional` to check and cast in a single statement:

    if (auto m = as_system_message_view(packet))
    {
        // access message data
        if (m.status() == system_status::song_position)
        {
            auto pos = m.get_song_position();
        }
    }

There are packet data views available for the majority of UMP message types.

    struct utility_message_view;
    struct system_message_view;
    struct midi1_channel_voice_message_view;
    struct midi2_channel_voice_message_view;

    struct data_message_view;
    struct sysex7_packet_view;

    struct flex_data_message;

    struct stream_message;

    struct endpoint_discovery_view;
    struct endpoint_info_view;
    struct device_identity_view;
    struct endpoint_name_view;
    struct product_instance_id_view;
    struct stream_configuration_view;

    struct function_block_discovery_view;
    struct function_block_info_view;
    struct function_block_name_view;

The same is true for Universal SysEx and MIDI-CI messages.

    struct universal_sysex::message_view;
    universal_sysex::identity_reply_view;

    struct capability_inquiry_view;
    struct ci::discovery_inquiry_view;
    struct ci::discovery_reply_view;
    ...
    struct ci::profile_inquiry_reply_view;
    ...
    struct ci::property_data_message_view;
    ...

As the content / size of such messages may not comply to the specification, it is recommended to either use the `std::optional` helpers like `ci::as<profile_specific_data_view>()` or `as_universal_sysex_view()` or the `validate()` helpers of the message view structs.

## Additional Functionalities

### MIDI 1 byte stream data helpers

The library provides a `midi1_byte_stream_parser` class and also free helper functions to convert `from_midi1_byte_stream()` and `to_midi1_byte_stream()`.

The `midi1_byte_stream_parser` can be configured to automatically parse and collect Sysex7 messages.

### Sysex7 collector

The `sysex7_collector` class allows to easily collect System Exclusive messages (like MIDI-CI 1.2).

    sysex7_collector c {
        [](const sysex7 &s)
        {
            // do something with message
            ...
        }
    };

    while(b = readMIDI1Byte())
        c.feed(b);

### Jitter Reduction Timestamps

There are experimental implementations for a Jitter Reduction Timestamps clock generator and follower available.

    struct jr_timestamp_t;
    struct jr_clock_message;
    struct jr_timestamp_message;

    class jr_clock;
    class jr_clock_follower;

Please be aware that these classes only demonstrate the general concept of Jitter Reduction Timestamps, they are not intended as a ready-to-use production solution.

## Building and Testing

The library uses `cmake` as its build system. Simply call `cmake` on the top level source folder to generate a build recipe for your preferred compiler and IDE / `make`.

There are some `cmake options` that allow customization of what is build and how:

    option( NIMIDI2_TREAT_WARNINGS_AS_ERRORS "Treat compile warnings as errors"   OFF )
    option( NIMIDI2_UNITY_BUILDS             "Build ni-midi2 with unity builds"   ON  )
    option( NIMIDI2_TESTS                    "Build ni-midi2 Tests"     ${IS_NIMIDI2} )

If you do not need to build unit tests, specify `-DNIMIDI2_TESTS=OFF` on the `cmake` command line. This is the default if this project is included via `add_subdirectory` into your project.

If you want to build the unit tests, the `CMakeLists.txt` file requires to `find_package(GTest "1.11.0")` through standard `cmake` mechanisms. The library itself only depends on the C++17 standard library.

By default, this project enables cmake unity builds on its targets, you may turn them off by passing `-DNIMIDI2_UNITY_BUILDS=OFF` on the `cmake` command line.

In case you plan to contribute please pass `-DNIMIDI2_TREAT_WARNINGS_AS_ERRORS=ON` on the `cmake` command line, this may help with keeping the code free of warning messages.

## TODOs

* sysex7 data encoding
* `extended_data_message_view`
* `sysex8_view`
* `sysex8_collector`
* support for mixed dataset
* specific flex data message data views
