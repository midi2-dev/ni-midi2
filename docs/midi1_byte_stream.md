# MIDI 1 Byte Stream Helpers

_WORK IN PROGRESS_

Code examples can be found in [`midi1_byte_stream.examples.cpp`](midi1_byte_stream.examples.cpp).

## MIDI 1 Byte Stream Parser

    class midi1_byte_stream_parser
    {
    public:
        using packet_callback = std::function<void(universal_packet)>;
        using sysex_callback  = std::function<void(const midi::sysex7&)>;

        explicit midi1_byte_stream_parser(packet_callback, sysex_callback = {}, bool enable_callbacks = true);
        midi1_byte_stream_parser(group_t, packet_callback, sysex_callback = {}, bool enable_callbacks = true);

        bool callbacks_enabled() const;
        void enable_callbacks(bool);

        group_t group() const;
        void    set_group(group_t);

        void feed(uint8_t);
        void feed(const uint8_t* data, size_t num_bytes);
        void feed(const uint8_t* begin, const uint8_t* end);

        void reset();
    };

## MIDI 1 Byte Stream Conversion

    universal_packet from_midi1_byte_stream(uint8_t status, uint7_t d1, uint7_t d2);

    size_t midi1_byte_stream_size(const universal_packet&);

    size_t to_midi1_byte_stream(const universal_packet&, uint8_t bytes[8]);
