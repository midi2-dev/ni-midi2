#pragma once

//--------------------------------------------------------------------------

#include <midi/midi1_channel_voice_message.h>
#include <midi/midi2_channel_voice_message.h>
#include <midi/types.h>

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

//--------------------------------------------------------------------------
// constexpr  implementations
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

} // namespace midi

//--------------------------------------------------------------------------
