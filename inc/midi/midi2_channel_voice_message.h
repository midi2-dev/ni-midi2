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
#include <midi/universal_packet.h>

#include <cassert>
#include <optional>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------
//! MIDI 2 channel voice message
struct midi2_channel_voice_message : universal_packet
{
    constexpr midi2_channel_voice_message() = default;
    constexpr midi2_channel_voice_message(group_t, status_t, channel_t, uint8_t byte3, uint8_t byte4, uint32_t data);
    constexpr explicit midi2_channel_voice_message(const universal_packet& p)
      : universal_packet(p)
    {
        assert(p.type() == packet_type::midi2_channel_voice);
    }
};

//--------------------------------------------------------------------------

constexpr bool is_midi2_channel_voice_message(const universal_packet&);

//--------------------------------------------------------------------------

struct midi2_channel_voice_message_view
{
    constexpr explicit midi2_channel_voice_message_view(const universal_packet& ump)
      : p(ump)
    {
        assert(p.type() == packet_type::midi2_channel_voice);
    }

    constexpr group_t   group() const { return p.group(); }
    constexpr status_t  status() const { return p.status() & 0xF0; }
    constexpr channel_t channel() const { return p.status() & 0x0F; }
    constexpr uint7_t   byte3() const { return p.byte3() & 0x7F; }
    constexpr uint7_t   byte4() const { return p.byte4() & 0x7F; }
    constexpr uint32_t  data() const { return p.data[1]; }

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr std::optional<midi2_channel_voice_message_view> as_midi2_channel_voice_message_view(const universal_packet&);

//--------------------------------------------------------------------------

using note_management_flags = uint8_t;

namespace note_management {
    constexpr uint8_t reset            = 0x1;
    constexpr uint8_t detach           = 0x2;
    constexpr uint8_t detach_and_reset = 0x3;
} // namespace note_management

//--------------------------------------------------------------------------

namespace note_attribute {
    constexpr uint8_t none                  = 0x0;
    constexpr uint8_t manufacturer_specific = 0x1;
    constexpr uint8_t profile_specific      = 0x2;
    constexpr uint8_t pitch_7_9             = 0x3;
} // namespace note_attribute

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_midi2_channel_voice_message(
  group_t, status_t, channel_t, uint7_t index1, uint7_t index2, uint32_t data);

constexpr midi2_channel_voice_message make_midi2_note_off_message(
  group_t, status_t, note_nr_t, velocity, uint8_t attribute = 0, uint16_t attribute_data = 0);
constexpr midi2_channel_voice_message make_midi2_note_on_message(group_t, status_t, note_nr_t, velocity);
constexpr midi2_channel_voice_message make_midi2_note_on_message(group_t, status_t, note_nr_t, velocity, pitch_7_9);
constexpr midi2_channel_voice_message make_midi2_note_on_message(
  group_t, status_t, note_nr_t, velocity, uint8_t attribute, uint16_t attribute_data);

constexpr midi2_channel_voice_message make_midi2_poly_pressure_message(group_t, status_t, note_nr_t, controller_value);
constexpr midi2_channel_voice_message make_registered_per_note_controller_message(
  group_t, status_t, note_nr_t, uint8_t controller, controller_value);
constexpr midi2_channel_voice_message make_assignable_per_note_controller_message(
  group_t, status_t, note_nr_t, uint8_t controller, controller_value);
constexpr midi2_channel_voice_message make_per_note_management_message(group_t,
                                                                       status_t,
                                                                       note_nr_t,
                                                                       note_management_flags);

constexpr midi2_channel_voice_message make_midi2_control_change_message(group_t,
                                                                        channel_t,
                                                                        uint7_t controller,
                                                                        controller_value);
constexpr midi2_channel_voice_message make_registered_controller_message(
  group_t, channel_t, uint7_t bank, uint7_t index, controller_value);
constexpr midi2_channel_voice_message make_assignable_controller_message(
  group_t, channel_t, uint7_t bank, uint7_t index, controller_value);
constexpr midi2_channel_voice_message make_relative_registered_controller_message(
  group_t, channel_t, uint7_t bank, uint7_t index, controller_increment);
constexpr midi2_channel_voice_message make_relative_assignable_controller_message(
  group_t, channel_t, uint7_t bank, uint7_t index, controller_increment);

constexpr midi2_channel_voice_message make_midi2_program_change_message(group_t, channel_t, program_t);
constexpr midi2_channel_voice_message make_midi2_program_change_message(group_t, channel_t, program_t, uint14_t bank);

constexpr midi2_channel_voice_message make_midi2_channel_pressure_message(group_t, channel_t, controller_value);

constexpr midi2_channel_voice_message make_midi2_pitch_bend_message(group_t, channel_t, pitch_bend);
constexpr midi2_channel_voice_message make_per_note_pitch_bend_message(group_t, channel_t, note_nr_t, pitch_bend);

//--------------------------------------------------------------------------
// inline implementations
//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message::midi2_channel_voice_message(
  group_t group, status_t status, channel_t channel, uint8_t byte3, uint8_t byte4, uint32_t data)
  : universal_packet(
      0x40000000u | ((group & 0x0F) << 24) | (((status & 0xF0) | (channel & 0x0F)) << 16) | (byte3 << 8) | byte4, data)
{
}

//--------------------------------------------------------------------------

constexpr bool is_midi2_channel_voice_message(const universal_packet& p)
{
    return (p.type() == packet_type::midi2_channel_voice);
}

//--------------------------------------------------------------------------

constexpr std::optional<midi2_channel_voice_message_view> as_midi2_channel_voice_message_view(const universal_packet& p)
{
    if (is_midi2_channel_voice_message(p))
        return midi2_channel_voice_message_view{ p };
    else
        return std::nullopt;
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_midi2_channel_voice_message(
  group_t group, status_t status, channel_t channel, uint7_t index1, uint7_t index2, uint32_t data)
{
    return { group, status, channel, index1, index2, data };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_midi2_note_off_message(
  group_t group, status_t channel, note_nr_t note_nr, velocity vel, uint8_t attribute, uint16_t attribute_data)
{
    return { group,     channel_voice_status::note_off,
             channel,   static_cast<note_nr_t>(note_nr & 0x7F),
             attribute, static_cast<uint32_t>((vel.value << 16) | attribute_data) };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_midi2_note_on_message(group_t   group,
                                                                 status_t  channel,
                                                                 note_nr_t note_nr,
                                                                 velocity  vel)
{
    return { group, channel_voice_status::note_on, channel, note_nr, 0, static_cast<uint32_t>(vel.value << 16) };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_midi2_note_on_message(
  group_t group, status_t channel, note_nr_t note_nr, velocity vel, pitch_7_9 pitch)
{
    return { group,   channel_voice_status::note_on, channel,
             note_nr, note_attribute::pitch_7_9,     static_cast<uint32_t>((vel.value << 16) | pitch.value) };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_midi2_note_on_message(
  group_t group, status_t channel, note_nr_t note_nr, velocity vel, uint8_t attribute, uint16_t attribute_data)
{
    return { group,     channel_voice_status::note_on,
             channel,   note_nr,
             attribute, static_cast<uint32_t>((vel.value << 16) | attribute_data) };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_midi2_poly_pressure_message(group_t          group,
                                                                       status_t         channel,
                                                                       note_nr_t        note_nr,
                                                                       controller_value pressure)
{
    return { group, channel_voice_status::poly_pressure, channel, note_nr, 0, pressure.value };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_registered_per_note_controller_message(
  group_t group, status_t channel, note_nr_t note_nr, uint8_t controller, controller_value v)
{
    return { group, channel_voice_status::registered_per_note_controller, channel, note_nr, controller, v.value };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_assignable_per_note_controller_message(
  group_t group, status_t channel, note_nr_t note_nr, uint8_t controller, controller_value v)
{
    return { group, channel_voice_status::assignable_per_note_controller, channel, note_nr, controller, v.value };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_per_note_management_message(group_t               group,
                                                                       status_t              channel,
                                                                       note_nr_t             note_nr,
                                                                       note_management_flags flags)
{
    return { group, channel_voice_status::per_note_management, channel, note_nr, flags, 0 };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_midi2_control_change_message(group_t          group,
                                                                        status_t         channel,
                                                                        uint7_t          controller,
                                                                        controller_value v)
{
    return {
        group, channel_voice_status::control_change, channel, static_cast<uint7_t>(controller & 0x7F), 0, v.value
    };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_registered_controller_message(
  group_t group, status_t channel, uint7_t bank, uint7_t index, controller_value v)
{
    return { group,
             channel_voice_status::registered_controller,
             channel,
             static_cast<uint7_t>(bank & 0x7F),
             static_cast<uint7_t>(index & 0x7F),
             v.value };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_assignable_controller_message(
  group_t group, status_t channel, uint7_t bank, uint7_t index, controller_value v)
{
    return { group,
             channel_voice_status::assignable_controller,
             channel,
             static_cast<uint7_t>(bank & 0x7F),
             static_cast<uint7_t>(index & 0x7F),
             v.value };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_relative_registered_controller_message(
  group_t group, status_t channel, uint7_t bank, uint7_t index, controller_increment inc)
{
    return { group,
             channel_voice_status::relative_registered_controller,
             channel,
             static_cast<uint7_t>(bank & 0x7F),
             static_cast<uint7_t>(index & 0x7F),
             uint32_t(inc.value) };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_relative_assignable_controller_message(
  group_t group, status_t channel, uint7_t bank, uint7_t index, controller_increment inc)
{
    return { group,
             channel_voice_status::relative_assignable_controller,
             channel,
             static_cast<uint7_t>(bank & 0x7F),
             static_cast<uint7_t>(index & 0x7F),
             uint32_t(inc.value) };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_midi2_program_change_message(group_t   group,
                                                                        status_t  channel,
                                                                        program_t program)
{
    return {
        group, channel_voice_status::program_change, channel, 0, 0x0, static_cast<uint32_t>((program & 0x7F) << 24)
    };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_midi2_program_change_message(group_t   group,
                                                                        status_t  channel,
                                                                        program_t program,
                                                                        uint14_t  bank)
{
    return { group,   channel_voice_status::program_change,
             channel, 0,
             0x1,     static_cast<uint32_t>(((program & 0x7F) << 24) | ((bank & 0x3F80) << 1) | (bank & 0x7F)) };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_midi2_channel_pressure_message(group_t          group,
                                                                          status_t         channel,
                                                                          controller_value pressure)
{
    return { group, channel_voice_status::channel_pressure, channel, 0, 0, pressure.value };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_midi2_pitch_bend_message(group_t group, status_t channel, pitch_bend pb)
{
    return { group, channel_voice_status::pitch_bend, channel, 0, 0, pb.value };
}

//--------------------------------------------------------------------------

constexpr midi2_channel_voice_message make_per_note_pitch_bend_message(group_t    group,
                                                                       status_t   channel,
                                                                       note_nr_t  note_nr,
                                                                       pitch_bend pb)
{
    return { group, channel_voice_status::per_note_pitch_bend, channel, note_nr, 0, pb.value };
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
