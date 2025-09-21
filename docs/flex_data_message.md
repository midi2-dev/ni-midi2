# Flex Data Messages

Code examples can be found in [`flex_data_message.examples.cpp`](flex_data_message.examples.cpp).

## Message Creation and Filtering

Flex Data Messages are represented by a

```cpp
struct flex_data_message : universal_packet
{
    flex_data_message(
        group_t group, packet_format format, packet_address address,
        channel_t channel, status_t status_bank, status_t status,
        uint32_t data1, uint32_t data2, uint32_t data3);

    packet_format  format() const;
    packet_address address() const;
    status_t       status_bank() const;
    status_t       status() const;

    std::string        payload_as_string() const;
    static std::string payload_as_string(const universal_packet&);
};
```

`packet_adress` can be `channel (0b00)` or `group (0b01)`. Use the `channel` parameter to specify a channel in `channel` address mode (will be ignored in `group` address mode).
Instead of using `flex_data_message` constructors one can create messages using factory functions:

```cpp
flex_data_message make_flex_data_message(
    group_t, packet_format, packet_address,
    uint4_t channel, status_t status_bank, status_t status,
    uint32_t data1 = 0, uint32_t data2 = 0, uint32_t data3 = 0);

flex_data_message make_flex_data_text_message(
    group_t, packet_format, packet_address,
    uint4_t channel, status_t status_bank, status_t status,
    const std::string_view& text);

flex_data_message make_set_tempo_message(group_t, uint32_t ten_ns_per_quarter_note);

flex_data_message make_set_time_signature_message(
    group_t, uint8_t numerator, uint8_t denominator, uint8_t nr_32rd_notes);

flex_data_message make_set_metronome_message(
    group_t, uint8_t num_clocks_per_primary_click,
    uint8_t bar_accent_part1, uint8_t bar_accent_part2, uint8_t bar_accent_part3,
    uint8_t num_subdivision_clicks1, uint8_t num_subdivision_clicks2);

flex_data_message make_set_key_signature_message(
    group_t, packet_address, uint4_t channel, uint4_t sharps_or_flats, uint4_t tonic_note);

flex_data_message make_set_chord_message(
    group_t, packet_address, uint4_t channel, uint32_t data1, uint32_t data2, uint32_t data3);
```

Be aware that a single `flex_data_text_message` can hold only 12 characters, if the text is larger you have to send a sequence of packets
using the `packet_format` mechanism (`start`, `cont`, `end`).

Be also aware that the `denominator` in a _Set Time Signature_ message is expressed as a negative power of two, meaning that
`2` represents a quarter note, `3` and eights note and so on.

Filtering of _Flex Data Messages_ can be done checking `universal_packet::type()` against
`packet_type::flex_data` or use

```cpp
bool is_flex_data_message(const universal_packet&);
```

### Flex Data Message Data View:

Once validated being a `flex_data` packet one can create a `flex_data_message_view`
for a `universal_packet`:

```cpp
struct flex_data_message_view
{
    explicit flex_data_message_view(const universal_packet&);

    group_t        group() const;
    packet_format  format() const;
    packet_address address() const;
    channel_t      channel() const;
    status_t       status_bank() const;
    status_t       status() const;
    uint32_t       data1() const;
    uint32_t       data2() const;
    uint32_t       data3() const;
    uint8_t        data_byte1() const;
    uint8_t        data_byte2() const;
    uint8_t        data_byte2() const;
    uint8_t        data_byte3() const;
    uint8_t        data_byte2() const;
    uint8_t        data_byte4() const;
    uint8_t        data_byte5() const;
    uint8_t        data_byte6() const;
    uint8_t        data_byte7() const;
    uint8_t        data_byte8() const;

    const std::string payload_as_string() const;
};
```
View members provide accessors to the properties of the message.
In case of Flex Data Text Messages you can retrieve the message payload using `payload_as_string()`.

The additional

```cpp
std::optional<flex_data_message_view> as_flex_data_message_view(const universal_packet&);
```

helper allows to combine packet type check and view creation in a single statement.
