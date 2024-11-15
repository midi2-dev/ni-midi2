# High Resolution (MIDI 2) Channel Voice Messages

Code examples can be found in [`midi2_channel_voice_message.examples.cpp`](midi2_channel_voice_message.examples.cpp).

## Message Creation and Filtering

MIDI 2 Channel Voice Messages are represented by a

    struct midi2_channel_voice_message : universal_packet
    {
        midi2_channel_voice_message();
        midi2_channel_voice_message(group_t, status_t, channel_t, uint8_t byte3, uint8_t byte4, uint32_t data);
        midi2_channel_voice_message(const universal_packet&);
    };

Instead of using `midi2_channel_voice_message` constructors one can create messages using factory functions:

    midi2_channel_voice_message
    make_midi2_channel_voice_message(group_t, status_t, channel_t, uint7_t index1, uint7_t index2, uint32_t data);

    midi2_channel_voice_message
    make_midi2_note_off_message(group_t, channel_t, note_nr_t, velocity, uint8_t attribute = 0, uint16_t attribute_data = 0);
    midi2_channel_voice_message
    make_midi2_note_on_message(group_t, channel_t, note_nr_t, velocity);
    midi2_channel_voice_message
    make_midi2_note_on_message(group_t, channel_t, note_nr_t, velocity, pitch_7_9);
    midi2_channel_voice_message
    make_midi2_note_on_message(group_t, channel_t, note_nr_t, velocity, uint8_t attribute, uint16_t attribute_data);

    midi2_channel_voice_message
    make_midi2_poly_pressure_message(group_t, channel_t, note_nr_t, controller_value);
    midi2_channel_voice_message
    make_registered_per_note_controller_message(group_t, channel_t, note_nr_t, uint8_t controller, controller_value);
    midi2_channel_voice_message
    make_assignable_per_note_controller_message(group_t, channel_t, note_nr_t, uint8_t controller, controller_value);
    midi2_channel_voice_message
    make_per_note_management_message(group_t, channel_t, note_nr_t, note_management_flags);

    midi2_channel_voice_message
    make_midi2_control_change_message(group_t, channel_t, uint7_t controller, controller_value);
    midi2_channel_voice_message
    make_registered_controller_message(group_t, channel_t, uint7_t bank, uint7_t index, controller_value);
    midi2_channel_voice_message
    make_assignable_controller_message(group_t, channel_t, uint7_t bank, uint7_t index, controller_value);
    midi2_channel_voice_message
    make_relative_registered_controller_message(group_t, channel_t, uint7_t bank, uint7_t index, controller_increment);
    midi2_channel_voice_message
    make_relative_assignable_controller_message(group_t, channel_t, uint7_t bank, uint7_t index, controller_increment);

    midi2_channel_voice_message
    make_midi2_program_change_message(group_t, channel_t, program_t);
    midi2_channel_voice_message
    make_midi2_program_change_message(group_t, channel_t, program_t, uint14_t bank);

    midi2_channel_voice_message
    make_midi2_channel_pressure_message(group_t, channel_t, controller_value);

    midi2_channel_voice_message
    make_midi2_pitch_bend_message(group_t, channel_t, pitch_bend);
    midi2_channel_voice_message
    make_per_note_pitch_bend_message(group_t, channel_t, note_nr_t, pitch_bend);

Filtering of MIDI 2 Channel Voice Messages can be done checking `universal_packet::type()` against
`packet_type::midi2_channel_voice` or use

    bool is_midi2_channel_voice_message(const universal_packet&);
    bool is_registered_controller_message(const universal_packet&);
    bool is_assignable_controller_message(const universal_packet&);
    bool is_registered_per_note_controller_message(const universal_packet&);
    bool is_registered_per_note_controller_pitch_message(const universal_packet&);
    bool is_assignable_per_note_controller_message(const universal_packet&);
    bool is_per_note_pitch_bend_message(const universal_packet&);

    bool is_note_on_with_attribute(const universal_packet&, uint8_t);
    bool is_note_off_with_attribute(const universal_packet&, uint8_t);
    bool is_note_on_with_pitch_7_9(const universal_packet&);

    bool is_pitch_bend_sensitivity_message(const universal_packet&);
    bool is_per_note_pitch_bend_sensitivity_message(const universal_packet&);

## Message View and Properties 

Once validated against `packet_type::midi2_channel_voice` one can create a `midi2_channel_voice_message_view`
for a `universal_packet`:

    struct midi2_channel_voice_message_view
    {
        midi2_channel_voice_message_view(const universal_packet&);

        group_t   group() const;
        status_t  status() const;
        channel_t channel() const;
        uint7_t   byte3() const;
        uint7_t   byte4() const;
        uint32_t  data() const;
    };

View members provide accessors to the properties of the message.

The additional

    std::optional<midi2_channel_voice_message_view> as_midi2_channel_voice_message_view(const universal_packet&);

helper allows to combine packet type check and view creation in a single statement.

There are free functions available to retrieve properties of the specific messages:

    uint8_t  get_midi2_note_attribute(const universal_packet&);
    uint16_t get_midi2_note_attribute_data(const universal_packet&);

    uint8_t get_per_note_controller_index(const universal_packet&);

    pitch_bend_sensitivity get_pitch_bend_sensitivity_value(const universal_packet&);
    pitch_bend_sensitivity get_per_note_pitch_bend_sensitivity_value(const universal_packet&);

    pitch_bend get_per_note_pitch_bend_value(const universal_packet&);
