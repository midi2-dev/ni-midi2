# Extended Data Messages

Code examples can be found in [`extended_data_message.examples.cpp`](extended_data_message.examples.cpp).

## Message Creation and Filtering

Extended Data Messages are represented by a

    struct extended_data_message : universal_packet
    {
        explicit extended_data_message(status_t);
    };

Usually one will not use this class directly, but instead

    struct sysex8_packet : extended_data_message
    {
        sysex8_packet(status_t, uint8_t stream_id, group_t);

        packet_format format() const;

        uint8_t stream_id() const;
        void    set_stream_id(uint8_t);

        uint8_t payload_byte(size_t b) const;
        void    set_payload_byte(size_t, uint8_t);

        size_t payload_size() const;
        void   set_payload_size(size_t);

        void add_payload_byte(uint8_t);
    };

A `sysex8_packet` can hold a payload of up to 13 bytes and provides APIs to add or read the payload bytes.
System Exclusive 8 packets payload is allowed to be 8 bit, different to traditional MIDI System Exclusive (7 bit).

In MIDI 2 is allowed to have multiple parallel System Exclusive 8 streams running in parallel. Therefore every  `sysex8_packet` needs to have a `stream_id`. A UMP Endpoint communicates the maximum number of parallel UMP streams allowed using [Stream Messages](stream_message.md).

Instead of using sysex8_packet constructors one can create messages using factory functions:

    sysex8_packet make_sysex8_complete_packet(uint8_t stream_id, group_t);
    sysex8_packet make_sysex8_start_packet(uint8_t stream_id, group_t);
    sysex8_packet make_sysex8_continue_packet(uint8_t stream_id, group_t);
    sysex8_packet make_sysex8_end_packet(uint8_t stream_id, group_t);

Filtering of Extended Data Messages can be done checking `universal_packet::type()` against
`packet_type::extended_data` or use

    bool is_extended_data_message(const universal_packet&);
    bool is_sysex8_packet(const universal_packet&);

### SysEx8 Packet View

Once validated being a `sysex8` packet one can create a `sysex8_packet_view`
for a `universal_packet`:

    struct sysex8_packet_view
    {
        explicit sysex8_packet_view(const universal_packet&);

        group_t       group() const;
        packet_format format() const;
        uint8_t       stream_id() const;
        size_t        payload_size() const;
        uint8_t       payload_byte(size_t b) const;
    };

View members provide accessors to the properties of the message.

The additional

    std::optional<sysex8_packet_view> as_sysex8_packet_view(const universal_packet&);

helper allows to combine packet type check and view creation in a single statement.
