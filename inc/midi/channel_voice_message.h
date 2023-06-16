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

#include <midi/midi1_channel_voice_message.h>
#include <midi/midi2_channel_voice_message.h>
#include <midi/types.h>

#include <optional>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

constexpr bool is_channel_voice_message_with_status(const universal_packet&, status_t);

constexpr bool is_note_on_message(const universal_packet&);
constexpr bool is_note_off_message(const universal_packet&);

constexpr note_id_t get_note_id(const universal_packet&);
constexpr note_nr_t get_note_nr(const universal_packet&);

constexpr pitch_7_9 get_note_pitch(const universal_packet&);
constexpr velocity  get_note_velocity(const universal_packet&);

constexpr bool             is_poly_pressure_message(const universal_packet&);
constexpr controller_value get_poly_pressure_value(const universal_packet&);

constexpr bool             is_control_change_message(const universal_packet&);
constexpr controller_t     get_controller_nr(const universal_packet&);
constexpr controller_value get_controller_value(const universal_packet&);

constexpr bool    is_program_change_message(const universal_packet&);
constexpr uint7_t get_program_value(const universal_packet&);

constexpr bool             is_channel_pressure_message(const universal_packet&);
constexpr controller_value get_channel_pressure_value(const universal_packet&);

constexpr bool       is_channel_pitch_bend_message(const universal_packet&);
constexpr pitch_bend get_channel_pitch_bend_value(const universal_packet&);

constexpr std::optional<midi1_channel_voice_message> as_midi1_channel_voice_message(
  const midi2_channel_voice_message_view&);
constexpr std::optional<midi2_channel_voice_message> as_midi2_channel_voice_message(
  const midi1_channel_voice_message_view&);

//--------------------------------------------------------------------------
// constexpr implementations
//--------------------------------------------------------------------------

constexpr bool is_channel_voice_message_with_status(const universal_packet& p, status_t status)
{
    return ((p.status() & 0xF0) == status) && p.is_channel_voice_message();
}

//--------------------------------------------------------------------------

constexpr bool is_note_on_message(const universal_packet& p)
{
    if (is_channel_voice_message_with_status(p, 0x90))
    {
        // MIDI 1 Note On with velocity 0 is Note Off
        return (p.type() == packet_type::midi2_channel_voice) || (p.byte4() != 0);
    }

    return false;
}

//--------------------------------------------------------------------------

constexpr bool is_note_off_message(const universal_packet& p)
{
    if (is_channel_voice_message_with_status(p, 0x80))
        return true;

    // MIDI 1 Note On with velocity 0 is Note Off
    return (p.type() == packet_type::midi1_channel_voice) && ((p.status() & 0xF0) == 0x90) && (p.byte4() == 0);
}

//--------------------------------------------------------------------------

constexpr note_id_t get_note_id(const universal_packet& p)
{
    // assert(is_channel_voice_message_with_status(p, 0x80) ||
    //        is_channel_voice_message_with_status(p, 0x90) ||
    //        is_channel_voice_message_with_status(p, 0xA0));
    return p.byte3() & 0x7F;
}

//--------------------------------------------------------------------------

constexpr note_nr_t get_note_nr(const universal_packet& p)
{
    // assert(is_channel_voice_message_with_status(p, 0x80) ||
    //        is_channel_voice_message_with_status(p, 0x90) ||
    //        is_channel_voice_message_with_status(p, 0xA0));
    if ((p.type() == packet_type::midi2_channel_voice) && ((p.status() & 0xF0) == 0x90) && (p.byte4() == 0x03))
    {
        // Pitch 7.9
        return note_nr_t((p.data[1] >> 9) & 0x7F);
    }

    return note_nr_t(p.byte3() & 0x7F);
}

//--------------------------------------------------------------------------

constexpr pitch_7_9 get_note_pitch(const universal_packet& p)
{
    // assert(is_channel_voice_message_with_status(p, 0x90);
    if ((p.type() == packet_type::midi2_channel_voice) && ((p.status() & 0xF0) == 0x90) && (p.byte4() == 0x03))
    {
        // Pitch 7.9
        return pitch_7_9{ uint16_t(p.data[1] & 0xFFFF) };
    }

    return pitch_7_9{ note_nr_t(p.byte3() & 0x7F) };
}

//--------------------------------------------------------------------------

constexpr velocity get_note_velocity(const universal_packet& p)
{
    // assert(is_channel_voice_message_with_status(p, 0x80) ||
    //        is_channel_voice_message_with_status(p, 0x90));
    if (p.type() == packet_type::midi2_channel_voice)
        return velocity{ uint16_t(p.data[1] >> 16) };

    // MIDI 1 Note On with velocity 0 is Note Off with velocity 64
    if ((p.type() == packet_type::midi1_channel_voice) && ((p.status() & 0xF0) == 0x90) && (p.byte4() == 0))
        return velocity{ uint7_t{ 64 } };

    return velocity{ uint7_t(p.byte4() & 0x7F) };
}

//--------------------------------------------------------------------------

constexpr bool is_poly_pressure_message(const universal_packet& p)
{
    return is_channel_voice_message_with_status(p, 0xA0);
}

//--------------------------------------------------------------------------

constexpr controller_value get_poly_pressure_value(const universal_packet& p)
{
    return get_controller_value(p);
}

//--------------------------------------------------------------------------

constexpr bool is_control_change_message(const universal_packet& p)
{
    return is_channel_voice_message_with_status(p, 0xB0);
}

//--------------------------------------------------------------------------

constexpr controller_t get_controller_nr(const universal_packet& p)
{
    // assert(is_control_change_message(p))
    return controller_t(p.byte3() & 0x7F);
}

//--------------------------------------------------------------------------

constexpr controller_value get_controller_value(const universal_packet& p)
{
    // assert((type() == packet_type::midi2_channel_voice) ||
    //        is_poly_pressure_message(p) ||
    //        is_control_change_message(p));
    if (p.type() == packet_type::midi1_channel_voice)
    {
        return controller_value{ uint7_t(p.byte4() & 0x7F) };
    }
    return controller_value{ p.data[1] };
}

//--------------------------------------------------------------------------

constexpr bool is_program_change_message(const universal_packet& p)
{
    return is_channel_voice_message_with_status(p, 0xC0);
}

//--------------------------------------------------------------------------

constexpr uint7_t get_program_value(const universal_packet& p)
{
    // assert(is_program_change_message(p));
    switch (p.type())
    {
    case packet_type::midi1_channel_voice:
        return uint7_t(p.byte3() & 0x7F);
    case packet_type::midi2_channel_voice:
        return uint7_t((p.data[1] >> 24u) & 0x7F);
    default:
        break;
    }

    return 0xFF; // invalid
}

//--------------------------------------------------------------------------

constexpr bool is_channel_pressure_message(const universal_packet& p)
{
    return is_channel_voice_message_with_status(p, 0xD0);
}

//--------------------------------------------------------------------------

constexpr controller_value get_channel_pressure_value(const universal_packet& p)
{
    // assert(is_channel_pressure_message(p));
    if (p.type() == packet_type::midi1_channel_voice)
    {
        return controller_value{ uint7_t(p.byte3() & 0x7F) };
    }
    return controller_value{ p.data[1] };
}

//--------------------------------------------------------------------------

constexpr bool is_channel_pitch_bend_message(const universal_packet& p)
{
    return is_channel_voice_message_with_status(p, 0xE0);
}

//--------------------------------------------------------------------------

constexpr pitch_bend get_channel_pitch_bend_value(const universal_packet& p)
{
    // assert(is_channel_pitch_bend_message(p));
    if (p.type() == packet_type::midi1_channel_voice)
    {
        return pitch_bend{ uint14_t(p.byte3() | (p.byte4() << 7)) };
    }
    return pitch_bend{ p.data[1] };
}

//--------------------------------------------------------------------------
//! \note this is not a complete MIDI 2 -> 1 translator
constexpr std::optional<midi1_channel_voice_message> as_midi1_channel_voice_message(
  const midi2_channel_voice_message_view& m)
{
    switch (m.status())
    {
    case channel_voice_status::note_off:
        if (m.byte4() == 0) // no attributes in MIDI 1
        {
            return make_midi1_note_off_message(m.group(), m.channel(), m.byte3(), velocity{ uint16_t(m.data() >> 16) });
        }
        break;
    case channel_voice_status::note_on:
        if (m.byte4() == 0) // no attributes in MIDI 1
        {
            auto vel = velocity{ uint16_t(m.data() >> 16) };
            if (vel.as_uint7() == 0)
                vel = velocity{ uint7_t{ 1 } };

            return make_midi1_note_on_message(m.group(), m.channel(), m.byte3(), vel);
        }
        break;
    case channel_voice_status::poly_pressure:
        return make_midi1_poly_pressure_message(m.group(), m.channel(), m.byte3(), controller_value{ m.data() });
    case channel_voice_status::control_change:
        switch (m.byte3())
        {
        // reserved, do not send
        case control_change::bank_select_msb:
        case control_change::data_entry_msb:
        case control_change::bank_select_lsb:
        case control_change::data_entry_lsb:
        case control_change::hi_res_velocity_prefix:
        case control_change::nrpn_lsb:
        case control_change::nrpn_msb:
        case control_change::rpn_lsb:
        case control_change::rpn_msb:
            break;
        default:
            return make_midi1_control_change_message(m.group(), m.channel(), m.byte3(), controller_value{ m.data() });
        }
        break;
    case channel_voice_status::program_change:
        if ((m.byte4() & 0b1) == 0) // bank valid bit clear?
            return make_midi1_program_change_message(m.group(), m.channel(), uint7_t(m.data() >> 24));
        break;
    case channel_voice_status::channel_pressure:
        return make_midi1_channel_pressure_message(m.group(), m.channel(), controller_value{ m.data() });
    case channel_voice_status::pitch_bend:
        return make_midi1_pitch_bend_message(m.group(), m.channel(), pitch_bend{ m.data() });
    default:
        break;
    }

    return std::nullopt;
}

//--------------------------------------------------------------------------
//! \note this is not a complete MIDI 1 -> 2 translator
constexpr std::optional<midi2_channel_voice_message> as_midi2_channel_voice_message(
  const midi1_channel_voice_message_view& m)
{
    switch (m.status())
    {
    case channel_voice_status::note_off:
        return make_midi2_note_off_message(m.group(), m.channel(), m.data_byte_1(), velocity{ m.data_byte_2() });
    case channel_voice_status::note_on:
        if (m.data_byte_2() == 0)
            return make_midi2_note_off_message(m.group(), m.channel(), m.data_byte_1(), velocity{ uint7_t{ 64 } });
        return make_midi2_note_on_message(m.group(), m.channel(), m.data_byte_1(), velocity{ m.data_byte_2() });
    case channel_voice_status::poly_pressure:
        return make_midi2_poly_pressure_message(
          m.group(), m.channel(), m.data_byte_1(), controller_value{ m.data_byte_2() });
    case channel_voice_status::control_change:
        switch (m.data_byte_1())
        {
        // reserved, do not translate
        case control_change::bank_select_msb:
        case control_change::data_entry_msb:
        case control_change::bank_select_lsb:
        case control_change::data_entry_lsb:
        case control_change::hi_res_velocity_prefix:
        case control_change::nrpn_lsb:
        case control_change::nrpn_msb:
        case control_change::rpn_lsb:
        case control_change::rpn_msb:
            break;
        default:
            return make_midi2_control_change_message(
              m.group(), m.channel(), m.data_byte_1(), controller_value{ m.data_byte_2() });
        }
        break;
    case channel_voice_status::program_change:
        return make_midi2_program_change_message(m.group(), m.channel(), m.data_byte_1());
    case channel_voice_status::channel_pressure:
        return make_midi2_channel_pressure_message(m.group(), m.channel(), controller_value{ m.data_byte_1() });
    case channel_voice_status::pitch_bend:
        return make_midi2_pitch_bend_message(m.group(), m.channel(), pitch_bend{ m.get_14bit_value() });
    default:
        break;
    }

    return std::nullopt;
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
