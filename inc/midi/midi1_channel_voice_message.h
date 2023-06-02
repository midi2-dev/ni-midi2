#pragma once

//--------------------------------------------------------------------------

#include <midi/types.h>
#include <midi/universal_packet.h>

#include <cassert>
#include <optional>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------
//! MIDI 1 channel voice message
struct midi1_channel_voice_message : universal_packet
{
    constexpr midi1_channel_voice_message() = default;
    constexpr midi1_channel_voice_message(group_t, status_t, uint7_t data1 = 0, uint7_t data2 = 0);
    constexpr explicit midi1_channel_voice_message(const universal_packet&);
};

//--------------------------------------------------------------------------

constexpr bool is_midi1_channel_voice_message(const universal_packet&);

//--------------------------------------------------------------------------

struct midi1_channel_voice_message_view
{
    constexpr explicit midi1_channel_voice_message_view(const universal_packet&);

    constexpr group_t   group() const { return p.group(); }
    constexpr status_t  status() const { return p.status() & 0xF0; }
    constexpr channel_t channel() const;
    constexpr uint7_t   data_byte_1() const;
    constexpr uint7_t   data_byte_2() const;

    constexpr uint14_t get_14bit_value() const;

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr std::optional<midi1_channel_voice_message_view> as_midi1_channel_voice_message_view(const universal_packet&);

//--------------------------------------------------------------------------

constexpr midi1_channel_voice_message make_midi1_channel_voice_message(
  group_t, status_t, channel_t, uint7_t data1, uint7_t data2 = 0);

constexpr midi1_channel_voice_message make_midi1_note_off_message(group_t, channel_t, note_nr_t, velocity);
constexpr midi1_channel_voice_message make_midi1_note_on_message(group_t, channel_t, note_nr_t, velocity);

constexpr midi1_channel_voice_message make_midi1_poly_pressure_message(group_t, channel_t, note_nr_t, controller_value);
constexpr midi1_channel_voice_message make_midi1_control_change_message(group_t,
                                                                        channel_t,
                                                                        controller_t,
                                                                        controller_value);
constexpr midi1_channel_voice_message make_midi1_program_change_message(group_t, channel_t, program_t);

constexpr midi1_channel_voice_message make_midi1_channel_pressure_message(group_t, channel_t, controller_value);
constexpr midi1_channel_voice_message make_midi1_pitch_bend_message(group_t, channel_t, pitch_bend);

//--------------------------------------------------------------------------
// constexpr implementations
//--------------------------------------------------------------------------

constexpr midi1_channel_voice_message::midi1_channel_voice_message(const universal_packet& p)
  : universal_packet(p)
{
    assert(p.type() == packet_type::midi1_channel_voice);
}

//--------------------------------------------------------------------------

constexpr midi1_channel_voice_message::midi1_channel_voice_message(group_t  group,
                                                                   status_t status,
                                                                   uint7_t  data1,
                                                                   uint7_t  data2)
  : universal_packet(0x20000000u | ((group & 0x0F) << 24) | (status << 16) | (data1 << 8) | data2)
{
}

//--------------------------------------------------------------------------

constexpr bool is_midi1_channel_voice_message(const universal_packet& p)
{
    return (p.type() == packet_type::midi1_channel_voice);
}

//--------------------------------------------------------------------------

constexpr midi1_channel_voice_message_view::midi1_channel_voice_message_view(const universal_packet& ump)
  : p(ump)
{
    assert(is_midi1_channel_voice_message(p));
}

//--------------------------------------------------------------------------

constexpr channel_t midi1_channel_voice_message_view::channel() const
{
    return p.status() & 0x0F;
}

//--------------------------------------------------------------------------

constexpr uint7_t midi1_channel_voice_message_view::data_byte_1() const
{
    return p.byte3() & 0x7F;
}

//--------------------------------------------------------------------------

constexpr uint7_t midi1_channel_voice_message_view::data_byte_2() const
{
    return p.byte4() & 0x7F;
}

//--------------------------------------------------------------------------

constexpr uint14_t midi1_channel_voice_message_view::get_14bit_value() const
{
    return data_byte_1() | (data_byte_2() << 7);
}

//--------------------------------------------------------------------------

constexpr std::optional<midi1_channel_voice_message_view> as_midi1_channel_voice_message_view(const universal_packet& p)
{
    if (is_midi1_channel_voice_message(p))
        return midi1_channel_voice_message_view{ p };
    else
        return std::nullopt;
}

//--------------------------------------------------------------------------

constexpr midi1_channel_voice_message make_midi1_channel_voice_message(
  group_t group, status_t status, channel_t channel, uint7_t data1, uint7_t data2)
{
    return midi1_channel_voice_message{ group,
                                        static_cast<status_t>(status | (channel & 0x0F)),
                                        static_cast<uint7_t>(data1 & 0x7F),
                                        static_cast<uint7_t>(data2 & 0x7F) };
}

//--------------------------------------------------------------------------

constexpr midi1_channel_voice_message make_midi1_note_off_message(group_t   group,
                                                                  channel_t channel,
                                                                  note_nr_t note_nr,
                                                                  velocity  vel = {})
{
    return make_midi1_channel_voice_message(
      group, midi1_channel_voice_status::note_off, channel, note_nr, vel.as_uint7());
}

//--------------------------------------------------------------------------

constexpr midi1_channel_voice_message make_midi1_note_on_message(group_t   group,
                                                                 channel_t channel,
                                                                 note_nr_t note_nr,
                                                                 velocity  vel)
{
    return make_midi1_channel_voice_message(
      group, midi1_channel_voice_status::note_on, channel, note_nr, vel.as_uint7());
}

//--------------------------------------------------------------------------

constexpr midi1_channel_voice_message make_midi1_poly_pressure_message(group_t          group,
                                                                       channel_t        channel,
                                                                       note_nr_t        note_nr,
                                                                       controller_value pressure)
{
    return make_midi1_channel_voice_message(
      group, midi1_channel_voice_status::poly_pressure, channel, note_nr, pressure.as_uint7());
}

//--------------------------------------------------------------------------

constexpr midi1_channel_voice_message make_midi1_control_change_message(group_t          group,
                                                                        channel_t        channel,
                                                                        controller_t     controller,
                                                                        controller_value value)
{
    return make_midi1_channel_voice_message(
      group, midi1_channel_voice_status::control_change, channel, controller, value.as_uint7());
}

//--------------------------------------------------------------------------

constexpr midi1_channel_voice_message make_midi1_program_change_message(group_t   group,
                                                                        channel_t channel,
                                                                        program_t program)
{
    return make_midi1_channel_voice_message(group, midi1_channel_voice_status::program_change, channel, program);
}

//--------------------------------------------------------------------------

constexpr midi1_channel_voice_message make_midi1_channel_pressure_message(group_t          group,
                                                                          channel_t        channel,
                                                                          controller_value pressure)
{
    return make_midi1_channel_voice_message(
      group, midi1_channel_voice_status::channel_pressure, channel, pressure.as_uint7(), 0);
}

//--------------------------------------------------------------------------

constexpr midi1_channel_voice_message make_midi1_pitch_bend_message(group_t group, channel_t channel, pitch_bend pb)
{
    const auto pb14bit = pb.as_uint14();
    return make_midi1_channel_voice_message(group,
                                            midi1_channel_voice_status::pitch_bend,
                                            channel,
                                            static_cast<uint7_t>(pb14bit & 0x7F),
                                            static_cast<uint7_t>((pb14bit >> 7) & 0x7F));
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
