# MIDI 1 Channel Voice Messages

Code examples can be found in [`midi1_channel_voice_message.examples.cpp`](midi1_channel_voice_message.examples.cpp).

## Message Creation and Filtering

MIDI 1 Channel Voice Messages are represented by a

```cpp
struct midi1_channel_voice_message : universal_packet
{
    midi1_channel_voice_message();
    midi1_channel_voice_message(group_t, status_t, uint7_t data1 = 0, uint7_t data2 = 0);
    midi1_channel_voice_message(const universal_packet&);
};
```

Instead of using `midi1_channel_voice_message` constructors one can create messages using factory functions:

```cpp
midi1_channel_voice_message
make_midi1_channel_voice_message(group_t, status_t, channel_t, uint7_t data1, uint7_t data2 = 0);

midi1_channel_voice_message
make_midi1_note_off_message(group_t, channel_t, note_nr_t, velocity);

midi1_channel_voice_message
make_midi1_note_on_message(group_t, channel_t, note_nr_t, velocity);

midi1_channel_voice_message
make_midi1_poly_pressure_message(group_t, channel_t, note_nr_t, controller_value);

midi1_channel_voice_message
make_midi1_control_change_message(group_t, channel_t, controller_t, controller_value);

midi1_channel_voice_message
make_midi1_program_change_message(group_t, channel_t, program_t);

midi1_channel_voice_message
make_midi1_channel_pressure_message(group_t, channel_t, controller_value);

midi1_channel_voice_message
make_midi1_pitch_bend_message(group_t, channel_t, pitch_bend);
```

Filtering of MIDI 1 Channel Voice Messages can be done checking `universal_packet::type()` against
`packet_type::midi1_channel_voice` or use

```cpp
bool is_midi1_channel_voice_message(const universal_packet&);
```

## Message View

Once validated against `packet_type::midi1_channel_voice` one can create a `midi1_channel_voice_message_view`
for a `universal_packet`:

```cpp
struct midi1_channel_voice_message_view
{
    midi1_channel_voice_message_view(const universal_packet&);

    group_t   group() const;
    status_t  status() const;
    channel_t channel() const;
    uint7_t   data_byte_1() const;
    uint7_t   data_byte_2() const;

    uint14_t get_14bit_value() const;
};
```

View members provide accessors to the properties of the message.

The additional

```cpp
std::optional<midi1_channel_voice_message_view> as_midi1_channel_voice_message_view(const universal_packet&);
```

helper allows to combine packet type check and view creation in a single statement.
