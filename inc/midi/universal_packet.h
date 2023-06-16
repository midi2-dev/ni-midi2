//
// Copyright (c) 2023 Native Instruments
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

//--------------------------------------------------------------------------

#include <midi/types.h>

#include <cassert>
#include <iosfwd>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

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

//--------------------------------------------------------------------------

//! message format types
namespace packet_format {
    constexpr packet_format_t complete = 0x0; //!< complete message
    constexpr packet_format_t start    = 0x1; //!< start of message
    constexpr packet_format_t cont     = 0x2; //!< continued message
    constexpr packet_format_t end      = 0x3; //!< end of message
} // namespace packet_format

//--------------------------------------------------------------------------

namespace utility_status {
    constexpr status_t noop         = 0x00;
    constexpr status_t jr_clock     = 0x10;
    constexpr status_t jr_timestamp = 0x20;
} // namespace utility_status

//--------------------------------------------------------------------------

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

//--------------------------------------------------------------------------

namespace midi1_channel_voice_status {
    constexpr status_t note_off         = 0x80;
    constexpr status_t note_on          = 0x90;
    constexpr status_t poly_pressure    = 0xA0;
    constexpr status_t control_change   = 0xB0;
    constexpr status_t program_change   = 0xC0;
    constexpr status_t channel_pressure = 0xD0;
    constexpr status_t pitch_bend       = 0xE0;
} // namespace midi1_channel_voice_status

//--------------------------------------------------------------------------

namespace data_status {
    constexpr status_t sysex7_complete = (packet_format::complete << 4);
    constexpr status_t sysex7_start    = (packet_format::start << 4);
    constexpr status_t sysex7_continue = (packet_format::cont << 4);
    constexpr status_t sysex7_end      = (packet_format::end << 4);
} // namespace data_status

//--------------------------------------------------------------------------

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

//--------------------------------------------------------------------------

namespace extended_data_status {
    constexpr status_t sysex8_complete = (packet_format::complete << 4);
    constexpr status_t sysex8_start    = (packet_format::start << 4);
    constexpr status_t sysex8_continue = (packet_format::cont << 4);
    constexpr status_t sysex8_end      = (packet_format::end << 4);

    constexpr status_t mixed_data_set_header  = 0x80;
    constexpr status_t mixed_data_set_payload = 0x90;
} // namespace extended_data_status

//--------------------------------------------------------------------------
// introduced in UMP 1.1

namespace protocol {
    constexpr protocol_t midi1 = 0x1;
    constexpr protocol_t midi2 = 0x2;
} // namespace protocol

namespace extensions {
    constexpr protocol_t jitter_reduction_transmit = 0x1;
    constexpr protocol_t jitter_reduction_receive  = 0x2;
} // namespace extensions

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

//--------------------------------------------------------------------------

namespace control_change {
    constexpr controller_t bank_select_msb   = 0;
    constexpr controller_t modulation_wheel  = 1;
    constexpr controller_t breath_controller = 2;

    constexpr controller_t foot_controller = 4;
    constexpr controller_t portamento_time = 5;
    constexpr controller_t data_entry_msb  = 6;
    constexpr controller_t volume          = 7;
    constexpr controller_t balance         = 8;

    constexpr controller_t pan                   = 10;
    constexpr controller_t expression_controller = 11;
    constexpr controller_t effect_control_1      = 12;
    constexpr controller_t effect_control_2      = 13;
    constexpr controller_t general_purpose_1     = 16;
    constexpr controller_t general_purpose_2     = 17;
    constexpr controller_t general_purpose_3     = 18;
    constexpr controller_t general_purpose_4     = 19;
    constexpr controller_t bank_select           = 32;

    constexpr controller_t bank_select_lsb = 32;
    constexpr controller_t lsb             = 32; // add to access lsb of controllers 0..31

    constexpr controller_t data_entry_lsb = 38;

    constexpr controller_t damper_pedal      = 64;
    constexpr controller_t sustain           = 64;
    constexpr controller_t portamento_on_off = 65;
    constexpr controller_t sustenuto         = 66;
    constexpr controller_t soft_pedal        = 67;
    constexpr controller_t legato_footswitch = 68;
    constexpr controller_t hold_2            = 69;

    constexpr controller_t sound_controller_1  = 70;
    constexpr controller_t sound_variation     = 70;
    constexpr controller_t sound_controller_2  = 71;
    constexpr controller_t timbre              = 71;
    constexpr controller_t harmonic_intensity  = 71;
    constexpr controller_t sound_controller_3  = 72;
    constexpr controller_t release_time        = 72;
    constexpr controller_t sound_controller_4  = 73;
    constexpr controller_t attack_time         = 73;
    constexpr controller_t sound_controller_5  = 74;
    constexpr controller_t brightness          = 74;
    constexpr controller_t sound_controller_6  = 75;
    constexpr controller_t decay_time          = 75;
    constexpr controller_t sound_controller_7  = 76;
    constexpr controller_t vibrato_rate        = 76;
    constexpr controller_t sound_controller_8  = 77;
    constexpr controller_t vibrato_depth       = 77;
    constexpr controller_t sound_controller_9  = 78;
    constexpr controller_t vibrato_delay       = 78;
    constexpr controller_t sound_controller_10 = 79;

    constexpr controller_t general_purpose_5 = 80;
    constexpr controller_t general_purpose_6 = 81;
    constexpr controller_t general_purpose_7 = 82;
    constexpr controller_t general_purpose_8 = 83;

    constexpr controller_t portamento_control = 84;

    constexpr controller_t effects_1_depth   = 91;
    constexpr controller_t reverb_send_level = 91;
    constexpr controller_t effects_2_depth   = 92;
    constexpr controller_t effects_3_depth   = 93;
    constexpr controller_t chorus_send_level = 93;
    constexpr controller_t effects_4_depth   = 94;
    constexpr controller_t effects_5_depth   = 95;

    constexpr controller_t data_increment = 96;
    constexpr controller_t data_decrement = 97;
    constexpr controller_t nrpn_lsb       = 98;
    constexpr controller_t nrpn_msb       = 99;
    constexpr controller_t rpn_lsb        = 100;
    constexpr controller_t rpn_msb        = 101;

    constexpr controller_t all_sound_off         = 120;
    constexpr controller_t reset_all_controllers = 121;
    constexpr controller_t local_control         = 122;
    constexpr controller_t all_notes_off         = 123;
    constexpr controller_t omni_mode_off         = 124;
    constexpr controller_t omni_mode_on          = 125;
    constexpr controller_t mono_mode_on          = 126;
    constexpr controller_t poly_mode_off         = 127;
    constexpr controller_t mono_mode_off         = 127;
    constexpr controller_t poly_mode_on          = 127;
} // namespace control_change

//--------------------------------------------------------------------------

namespace registered_parameter_number {
    constexpr controller_t pitch_bend_sensitivity = 0;
    constexpr controller_t fine_tuning            = 1;
    constexpr controller_t coarse_tuning          = 2;
    constexpr controller_t tuning_program_select  = 3;
    constexpr controller_t tuning_bank_select     = 4;
} // namespace registered_parameter_number

//--------------------------------------------------------------------------

namespace registered_per_note_controller {
    constexpr controller_t modulation = 1;
    constexpr controller_t breath     = 2;
    constexpr controller_t pitch_7_25 = 3;

    constexpr controller_t volume  = 7;
    constexpr controller_t balance = 8;

    constexpr controller_t pan        = 10;
    constexpr controller_t expression = 11;

    constexpr controller_t sound_controller_1  = 70;
    constexpr controller_t sound_variation     = 70;
    constexpr controller_t sound_controller_2  = 71;
    constexpr controller_t timbre              = 71;
    constexpr controller_t harmonic_intensity  = 71;
    constexpr controller_t sound_controller_3  = 72;
    constexpr controller_t release_time        = 72;
    constexpr controller_t sound_controller_4  = 73;
    constexpr controller_t attack_time         = 73;
    constexpr controller_t sound_controller_5  = 74;
    constexpr controller_t brightness          = 74;
    constexpr controller_t sound_controller_6  = 75;
    constexpr controller_t decay_time          = 75;
    constexpr controller_t sound_controller_7  = 76;
    constexpr controller_t vibrato_rate        = 76;
    constexpr controller_t sound_controller_8  = 77;
    constexpr controller_t vibrato_depth       = 77;
    constexpr controller_t sound_controller_9  = 78;
    constexpr controller_t vibrato_delay       = 78;
    constexpr controller_t sound_controller_10 = 79;

    constexpr controller_t effects_1_depth   = 91;
    constexpr controller_t reverb_send_level = 91;
    constexpr controller_t effects_2_depth   = 92;
    constexpr controller_t effects_3_depth   = 93;
    constexpr controller_t chorus_send_level = 93;
    constexpr controller_t effects_4_depth   = 94;
    constexpr controller_t effects_5_depth   = 95;
} // namespace registered_per_note_controller

//--------------------------------------------------------------------------
//! Universal MIDI packet
struct universal_packet
{
    uint32_t data[4]{ 0u, 0u, 0u, 0u };

    constexpr packet_type type() const { return static_cast<packet_type>((data[0] >> 28u) & 0x0F); }
    constexpr size_t      size() const;

    constexpr group_t  group() const { return ((data[0] >> 24u) & 0x0F); }
    constexpr status_t status() const { return byte2(); }

    constexpr uint8_t byte2() const { return ((data[0] >> 16u) & 0xFF); }
    constexpr uint8_t byte3() const { return ((data[0] >> 8u) & 0xFF); }
    constexpr uint8_t byte4() const { return (data[0] & 0xFF); }

    constexpr uint8_t get_byte(size_t b) const;
    constexpr uint7_t get_byte_7bit(size_t b) const { return get_byte(b) & 0x7F; }

    constexpr bool has_channel() const
    {
        return type() == packet_type::midi1_channel_voice || type() == packet_type::midi2_channel_voice ||
               type() == packet_type::flex_data;
    }
    constexpr channel_t channel() const
    {
        assert(has_channel());
        return byte2() & 0x0F;
    }

    constexpr void set_type(packet_type);
    constexpr void set_group(group_t);
    constexpr void set_byte(size_t, uint8_t);
    constexpr void set_byte_7bit(size_t b, uint8_t v) { set_byte(b, v & 0x7F); }

    constexpr universal_packet() = default;
    constexpr explicit universal_packet(uint32_t w) { data[0] = w; }
    constexpr universal_packet(uint32_t w1, uint32_t w2)
    {
        data[0] = w1;
        data[1] = w2;
    }
    constexpr universal_packet(uint32_t w1, uint32_t w2, uint32_t w3)
    {
        data[0] = w1;
        data[1] = w2;
        data[2] = w3;
    }
    constexpr universal_packet(uint32_t w1, uint32_t w2, uint32_t w3, uint32_t w4)
    {
        data[0] = w1;
        data[1] = w2;
        data[2] = w3;
        data[3] = w4;
    }

    constexpr universal_packet(const universal_packet& other) = default;
    constexpr universal_packet(universal_packet&& other)      = default;

    constexpr universal_packet& operator=(const universal_packet&) = default;
    constexpr universal_packet& operator=(universal_packet&&)      = default;

    constexpr bool operator==(const universal_packet&) const;
    constexpr bool operator!=(const universal_packet& p) const { return !operator==(p); }

    constexpr void reset();

    constexpr bool is_utility_message() const { return type() == packet_type::utility; }
    constexpr bool is_system_message() const { return type() == packet_type::system; }
    constexpr bool is_channel_voice_message() const
    {
        return (type() == packet_type::midi1_channel_voice) || (type() == packet_type::midi2_channel_voice);
    }
    constexpr bool is_data_message() const
    {
        return (type() == packet_type::data) || (type() == packet_type::extended_data);
    }

    constexpr bool is_midi1_protocol_message() const;
};

//--------------------------------------------------------------------------

constexpr void universal_packet::set_type(packet_type type)
{
    const auto t = static_cast<unsigned>(type);
    assert(t < 16);

    data[0] = (data[0] & 0x0FFFFFFF) | (t << 28);
}

//--------------------------------------------------------------------------

constexpr void universal_packet::set_group(group_t c)
{
    assert(c < 16);

    data[0] = (data[0] & 0xF0FFFFFF) | (c << 24);
}

//--------------------------------------------------------------------------

constexpr size_t universal_packet::size() const
{
    constexpr size_t size_lookup[16] = { 1, 1, 1, 2, 2, 4, 1, 1, 2, 2, 2, 3, 3, 4, 4, 4 };

    return size_lookup[static_cast<unsigned>(type())];
}

//--------------------------------------------------------------------------

constexpr uint8_t universal_packet::get_byte(size_t b) const
{
    assert(b < 16); // invalid byte

    const auto word  = b / 4;
    const auto byte  = b % 4;
    const auto shift = (3 - byte) * 8;

    return (data[word] >> shift) & 0xFF;
}

//--------------------------------------------------------------------------

constexpr void universal_packet::set_byte(size_t b, uint8_t v)
{
    assert(b < 16); // invalid byte
    if (b < 16)
    {
        uint32_t& word = data[b / 4];
        switch (b % 4)
        {
        case 0:
            word = (word & 0x00FFFFFF) | (v << 24u);
            break;
        case 1:
            word = (word & 0xFF00FFFF) | (v << 16u);
            break;
        case 2:
            word = (word & 0xFFFF00FF) | (v << 8u);
            break;
        case 3:
            word = (word & 0xFFFFFF00) | v;
            break;
        }
    }
}

//--------------------------------------------------------------------------

constexpr bool universal_packet::operator==(const universal_packet& o) const
{
    if (&o != this)
    {
        const auto len = size();
        for (auto i = 0u; i < len; ++i)
        {
            if (data[i] != o.data[i])
                return false;
        }
    }

    return true;
}

//--------------------------------------------------------------------------

constexpr void universal_packet::reset()
{
    for (auto& d : data)
        d = 0;
}

//--------------------------------------------------------------------------

constexpr bool universal_packet::is_midi1_protocol_message() const
{
    switch (type())
    {
    case packet_type::system:
        switch (status())
        {
        case system_status::mtc_quarter_frame:
        case system_status::song_position:
        case system_status::song_select:
        case system_status::tune_request:
        case system_status::clock:
        case system_status::start:
        case system_status::cont:
        case system_status::stop:
        case system_status::active_sense:
        case system_status::reset:
            return true;
        default:
            return false;
        }
        break;
    case packet_type::midi1_channel_voice:
        return (status() >= 0x80 && (status() < 0xF0));
    default:
        break;
    }
    return false;
}

//--------------------------------------------------------------------------

std::ostream& operator<<(std::ostream&, const universal_packet&);
std::istream& operator>>(std::istream&, universal_packet&);

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
