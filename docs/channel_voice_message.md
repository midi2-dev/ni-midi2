# Protocol Agnostic Channel Voice Message Helpers

Protocol agnostic Channel Voice Message helpers allow you to handle MIDI 1 (low resolution)
and MIDI 2 (high resolution) Channel Voice Messages in a single branch of code. By using these helpers
it does not matter of what packet type incoming Channel Voice Message are.

In a first step one filters incoming packets by Channel Voice Message type, and then extract
the appropriate properties from the packet.

Code examples can be found in [`channel_voice_message.examples.cpp`](channel_voice_message.examples.cpp).

## Channel Voice Message Filters (`is_XXX_message`)

Available filter functions are:

    bool is_channel_voice_message_with_status(const universal_packet&, status_t);

    bool is_note_on_message(const universal_packet&);
    bool is_note_off_message(const universal_packet&);
    bool is_poly_pressure_message(const universal_packet&);
    bool is_control_change_message(const universal_packet&);
    bool is_program_change_message(const universal_packet&);
    bool is_channel_pressure_message(const universal_packet&);
    bool is_channel_pitch_bend_message(const universal_packet&);

Additionally, one may use

    bool is_note_on_with_attribute(const universal_packet&, uint8_t);
    bool is_note_off_with_attribute(const universal_packet&, uint8_t);
    bool is_note_on_with_pitch_7_9(const universal_packet&);

to check for MIDI 2 note messages with attributes.

## Note Message Properties

The following functions are available to extract properties from Note messages:

    note_nr_t get_note_nr(const universal_packet&);
    pitch_7_9 get_note_pitch(const universal_packet&);
    velocity  get_note_velocity(const universal_packet&);

`get_note_nr` is applicable to _Poly Pressure_ and _Per-Note Controller_ messages, too.

Additionally, one may use

    uint8_t  get_midi2_note_attribute(const universal_packet&);
    uint16_t get_midi2_note_attribute_data(const universal_packet&);

to retrieve Per Note Attribute type and data.

## Controller Message Properties

Use

    controller_t     get_controller_nr(const universal_packet&);
    controller_value get_controller_value(const universal_packet&);

to extract _Control Change_ message properties.

`get_controller_value` is applicable to _Poly Pressure_, _Registered/Assignable Controller_ and _Per-Note Controller_ messages, too.

Use these functions to retrieve properties of specific messages:

    controller_value get_poly_pressure_value(const universal_packet&);
    uint7_t get_program_value(const universal_packet&);
    controller_value get_channel_pressure_value(const universal_packet&);
    pitch_bend get_channel_pitch_bend_value(const universal_packet&);

## Conversion between Protocols

One may use

    std::optional<midi1_channel_voice_message>
    as_midi1_channel_voice_message(const midi2_channel_voice_message_view&);

to convert a MIDI 2 Channel Voice Message to its MIDI 1 counterpart and

    std::optional<midi2_channel_voice_message>
    as_midi2_channel_voice_message(const midi1_channel_voice_message_view&);

to convert a MIDI 1 Channel Voice Message to its MIDI 2 counterpart.

**Note:** MIDI 1 <-> MIDI 2 translation rules do require some sequences of MIDI 1 messages to be translated to single MIDI 2 messages and vice versa. The above mentioned functions do not follow these rules. Actually they filter some _Control Change_ messages (mostly related to (N)RPNs) and cannot handle MIDI 2 _Program Change_ messages with  `bank` data.