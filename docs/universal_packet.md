# Universal MIDI Packet (UMP)

## `universal_packet` Data Type

_Universal MIDI Packets_ (UMPs) are represented using the `universal_packet` type. Technically a UMP is a sequence of 1 to 4 32 bit words in native endianness represented by an array of up to four 32 bit unsigned values (`uint32_t`). One can easily convert UMPs from one representation to another by copying the up to four data words from one C++ data structure to another.

    struct universal_packet
    {
        uint32_t data[4]{ 0u, 0u, 0u, 0u };

        packet_type type() const;
        size_t      size() const;

        group_t  group() const { return ((data[0] >> 24u) & 0x0F); }
        status_t status() const { return byte2(); }

        uint8_t byte2() const { return ((data[0] >> 16u) & 0xFF); }
        uint8_t byte3() const { return ((data[0] >> 8u) & 0xFF); }
        uint8_t byte4() const { return (data[0] & 0xFF); }

        uint8_t get_byte(size_t b) const;
        uint7_t get_byte_7bit(size_t b) const { return get_byte(b) & 0x7F; }

        bool has_channel() const;
        channel_t channel() const;

        void set_type(packet_type);
        void set_group(group_t);
        void set_byte(size_t, uint8_t);
        void set_byte_7bit(size_t b, uint8_t v) { set_byte(b, v & 0x7F); }

        universal_packet(uint32_t w) { data[0] = w; }
        universal_packet(uint32_t w1, uint32_t w2);
        universal_packet(uint32_t w1, uint32_t w2, uint32_t w3);
        universal_packet(uint32_t w1, uint32_t w2, uint32_t w3, uint32_t w4);

        bool is_utility_message() const;
        bool is_system_message() const;
        bool is_channel_voice_message() const;
        bool is_data_message() const;

        bool is_midi1_protocol_message() const;
    };

The `universal_packet` type provides accessors for `group`, `type`, `status`, `channel` (if applicable), individual data bytes and words. Please be aware that the `status` accessor in the `universal_packet` always provides the full status byte content, not masking the `channel` portion in Channel Voice Messages. Also be aware that some messages (like _Flex Data_) have additional status information in the packet. 

There are convienience helpers available to check the `universal_packet` against a specific type.

## Packet Types

UMP packet types are represented by the `enum class packet_type`:

    enum class packet_type : uint4_t {
        utility             = 0x0,
        system              = 0x1,
        midi1_channel_voice = 0x2,
        data                = 0x3,
        midi2_channel_voice = 0x4,
        extended_data       = 0x5,

        // introduced in UMP 1.1
        flex_data = 0xD,
        stream    = 0xF,
    };

## Status

The `universal_packet.h` header contains definitions for all currently known status bytes of the several message types. For status bytes with a channel component in the lower nibble the constants assume channel 0.

It is recommended to use the `status` members of the respective message data views to for comparisons. When using `universal_packet::status()` be aware to eventuelly mask the channel portion before comparing. 

### Utility Messages

`namespace utility_status` contains status definitions for all currently defined Utility messages.

    namespace utility_status {
        constexpr status_t noop         = 0x00;
        constexpr status_t jr_clock     = 0x10;
        constexpr status_t jr_timestamp = 0x20;
    } // namespace utility_status

### System Messages

`namespace system_status` contains status definitions for all currently defined System messages.

    namespace system_status {
        constexpr status_t mtc_quarter_frame = 0xF1;
        constexpr status_t song_position     = 0xF2;
        constexpr status_t song_select       = 0xF3;
        constexpr status_t tune_request      = 0xF6;
        constexpr status_t clock             = 0xF8;
        constexpr status_t start             = 0xFA;
        constexpr status_t cont              = 0xFB;
        constexpr status_t stop              = 0xFC;
        constexpr status_t active_sense      = 0xFE;
        constexpr status_t reset             = 0xFF;
    } // namespace system_status

Please refer to [system_message.md](system_message.md) for detailed information on these messages.

### MIDI 1 Channel Voice Messages

`namespace midi1_channel_voice_status` provides status definitions for the Channel Voice Messages already available in MIDI 1.

    namespace midi1_channel_voice_status {
        constexpr status_t note_off         = 0x80;
        constexpr status_t note_on          = 0x90;
        constexpr status_t poly_pressure    = 0xA0;
        constexpr status_t control_change   = 0xB0;
        constexpr status_t program_change   = 0xC0;
        constexpr status_t channel_pressure = 0xD0;
        constexpr status_t pitch_bend       = 0xE0;
    } // namespace midi1_channel_voice_status

Please refer to [midi1_channel_voice_message.md](midi1_channel_voice_message.md) for detailed information on these messages.

You may also look into [channel_voice_message.md](channel_voice_message.md) for MIDI protocol agnostic handling of Channel Voice Messages.

### Data Messages

Data Messages cover traditional MIDI 1 7 bit System Exclusive. UMP packet status definitions for these can be
found in `namespace data_status`.

    namespace data_status {
        constexpr status_t sysex7_complete = (status_t(packet_format::complete) << 4);
        constexpr status_t sysex7_start    = (status_t(packet_format::start) << 4);
        constexpr status_t sysex7_continue = (status_t(packet_format::cont) << 4);
        constexpr status_t sysex7_end      = (status_t(packet_format::end) << 4);
    } // namespace data_status

Please refer to [data_message.md](data_message.md) for detailed information on these messages.

Typically you may not deal with these messages directly, refer to [sysex_collector.md](sysex_collector.md) for more information.

### (MIDI 2) Channel Voice Messages

`namespace channel_voice_status` provides status definitions for all (MIDI 2) Channel Voice Messages including the High Resolution versions of Channel Voice Messages.

    namespace channel_voice_status {
        constexpr status_t registered_per_note_controller = 0x00;
        constexpr status_t assignable_per_note_controller = 0x10;

        constexpr status_t registered_controller = 0x20;
        constexpr status_t assignable_controller = 0x30;

        constexpr status_t relative_registered_controller = 0x40;
        constexpr status_t relative_assignable_controller = 0x50;

        constexpr status_t per_note_pitch_bend = 0x60;

        constexpr status_t note_off            = 0x80;
        constexpr status_t note_on             = 0x90;
        constexpr status_t poly_pressure       = 0xA0;
        constexpr status_t control_change      = 0xB0;
        constexpr status_t program_change      = 0xC0;
        constexpr status_t channel_pressure    = 0xD0;
        constexpr status_t pitch_bend          = 0xE0;
        constexpr status_t per_note_management = 0xF0;
    } // namespace channel_voice_status

Please refer to [midi2_channel_voice_message.md](midi2_channel_voice_message.md) for detailed information on these messages.

You may also look into [channel_voice_message.md](channel_voice_message.md) for MIDI protocol agnostic handling of Channel Voice Messages.

### Extended Data Messages

Extended Data Messages include the 8 bit version of System Exclusive and newly introduced Mixed Data Set Messages. UMP packet status definitions for these can be found in `namespace extended_data_status`.

    namespace extended_data_status {
        constexpr status_t sysex8_complete = (status_t(packet_format::complete) << 4);
        constexpr status_t sysex8_start    = (status_t(packet_format::start) << 4);
        constexpr status_t sysex8_continue = (status_t(packet_format::cont) << 4);
        constexpr status_t sysex8_end      = (status_t(packet_format::end) << 4);

        constexpr status_t mixed_data_set_header  = 0x80;
        constexpr status_t mixed_data_set_payload = 0x90;
    } // namespace extended_data_status

Please refer to [extended_data_message.md](extended_data_message.md) for detailed information on these messages.

Typically you may not deal with these messages directly, refer to [sysex_collector.md](sysex_collector.md) for more information.

### Flex Data Messages

Flex Data Messages come in four different formats, as defined in `packet_format`.

    enum class packet_format : uint2_t {
        complete = 0b00, //!< complete message
        start    = 0b01, //!< start of message
        cont     = 0b10, //!< continued message
        end      = 0b11  //!< end of message
    };

Also, Flex data messages have a _packet address_ field:

    enum class packet_address : uint2_t { channel = 0b00, group = 0b01 };

Please refer to [flex_data_message.md](flex_data_message.md) for detailed information on these messages.

### Stream Messages

`namespace stream_status` contains status definitions for all currently defined Stream messages.

    namespace stream_status {
        constexpr status_t endpoint_discovery           = 0x00;
        constexpr status_t endpoint_info                = 0x01;
        constexpr status_t device_identity              = 0x02;
        constexpr status_t endpoint_name                = 0x03;
        constexpr status_t product_instance_id          = 0x04;
        constexpr status_t stream_configuration_request = 0x05;
        constexpr status_t stream_configuration_notify  = 0x06;

        constexpr status_t function_block_discovery = 0x10;
        constexpr status_t function_block_info      = 0x11;
        constexpr status_t function_block_name      = 0x12;
    } // namespace stream_status

Some Stream messages have `protocol` and `extensions` fields:

    namespace protocol {
        constexpr protocol_t midi1 = 0x1;
        constexpr protocol_t midi2 = 0x2;
    } // namespace protocol

    namespace extensions {
        constexpr protocol_t jitter_reduction_transmit = 0x1;
        constexpr protocol_t jitter_reduction_receive  = 0x2;
    } // namespace extensions

Please refer to [stream_message.md](stream_message.md) for detailed information on these messages.
