# Data Messages

Code examples can be found in [`data_message.examples.cpp`](data_message.examples.cpp).

## Message Creation and Filtering

Data Messages are represented by a

    struct data_message : universal_packet
    {
        explicit data_message(status_t);
    };

Usually one will not use this class directly, but instead

    struct sysex7_packet : data_message
    {
        sysex7_packet(status_t, group_t);

        packet_format format() const;

        uint8_t payload_byte(size_t b) const;
        void    set_payload_byte(size_t, uint8_t);

        size_t payload_size() const;
        void   set_payload_size(size_t);

        void add_payload_byte(uint8_t);
    };

A `sysex7_packet` can hold a payload of up to six bytes and provides APIs to add or read the payload bytes. Be aware that the payload shall only be 7 bit data.

Instead of using sysex7_packet constructors one can create messages using factory functions:

    sysex7_packet make_sysex7_complete_packet(group_t);
    sysex7_packet make_sysex7_start_packet(group_t);
    sysex7_packet make_sysex7_continue_packet(group_t);
    sysex7_packet make_sysex7_end_packet(group_t);

Filtering of Data Messages can be done checking `universal_packet::type()` against
`packet_type::data` or use

    bool is_data_message(const universal_packet&);
    bool is_sysex7_packet(const universal_packet&);

### SysEx7 Packet View

Once validated being a `sysex7` packet one can create a `sysex7_packet_view`
for a `universal_packet`:

    struct sysex7_packet_view
    {
        explicit sysex7_packet_view(const universal_packet&);

        group_t       group() const;
        status_t      status() const;
        packet_format format() const;
        size_t        payload_size() const;
        uint8_t       payload_byte(size_t b) const;
    };

View members provide accessors to the properties of the message.

The additional

    std::optional<sysex7_packet_view> as_sysex7_packet_view(const universal_packet&);

helper allows to combine packet type check and view creation in a single statement.
