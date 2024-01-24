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

//--------------------------------------------------------------------------

#include <cassert>
#include <optional>
#include <string>
#include <string_view>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

struct flex_data_message : universal_packet
{
    constexpr explicit flex_data_message(group_t = 0);
    constexpr flex_data_message(
      group_t, packet_format, packet_address, uint4_t, status_t, status_t, uint32_t = 0, uint32_t = 0, uint32_t = 0);
    ~flex_data_message() = default;

    constexpr packet_format  format() const { return packet_format((byte2() & 0xC0u) >> 6); }
    constexpr packet_address address() const { return packet_address((byte2() & 0x30u) >> 4); }
    constexpr status_t       status_bank() const { return byte3(); }
    constexpr status_t       status() const { return byte4(); }

    std::string        payload_as_string() const;
    static std::string payload_as_string(const universal_packet&);
};

//--------------------------------------------------------------------------

constexpr bool is_flex_data_message(const universal_packet&);

//--------------------------------------------------------------------------

struct flex_data_message_view
{
    constexpr explicit flex_data_message_view(const universal_packet& ump)
      : p(ump)
    {
        assert(p.type() == packet_type::flex_data);
    }

    constexpr group_t        group() const { return p.group(); }
    constexpr packet_format  format() const { return packet_format((p.byte2() & 0xC0u) >> 6); }
    constexpr packet_address address() const { return packet_address((p.byte2() & 0x30u) >> 4); }
    constexpr channel_t      channel() const { return (p.byte2() & 0x0Fu); }
    constexpr status_t       status_bank() const { return p.byte3(); }
    constexpr status_t       status() const { return p.byte4(); }
    constexpr uint32_t       data1() const { return p.data[1]; }
    constexpr uint32_t       data2() const { return p.data[2]; }
    constexpr uint32_t       data3() const { return p.data[3]; }

    const std::string payload_as_string() const { return flex_data_message::payload_as_string(p); }

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr std::optional<flex_data_message_view> as_flex_data_message_view(const universal_packet&);

//--------------------------------------------------------------------------

constexpr flex_data_message make_flex_data_message(group_t,
                                                   packet_format,
                                                   packet_address,
                                                   channel_t channel,
                                                   status_t  status_bank,
                                                   status_t  status,
                                                   uint32_t  data1 = 0,
                                                   uint32_t  data2 = 0,
                                                   uint32_t  data3 = 0);
constexpr flex_data_message make_flex_data_text_message(group_t,
                                                        packet_format,
                                                        packet_address,
                                                        channel_t               channel,
                                                        status_t                status_bank,
                                                        status_t                status,
                                                        const std::string_view& text);

constexpr flex_data_message make_set_tempo_message(group_t, uint32_t ten_ns_per_quarter_note);
constexpr flex_data_message make_set_time_signature_message(group_t,
                                                            uint8_t numerator,
                                                            uint8_t denominator,
                                                            uint8_t nr_32rd_notes);
constexpr flex_data_message make_set_metronome_message(group_t,
                                                       uint8_t num_clocks_per_primary_click,
                                                       uint8_t bar_accent_part1,
                                                       uint8_t bar_accent_part2,
                                                       uint8_t bar_accent_part3,
                                                       uint8_t num_subdivision_clicks1,
                                                       uint8_t num_subdivision_clicks2);
constexpr flex_data_message make_set_key_signature_message(
  group_t, packet_address, channel_t channel, uint4_t sharps_or_flats, uint4_t tonic_note);
constexpr flex_data_message make_set_chord_message(
  group_t, packet_address, channel_t channel, uint32_t data1, uint32_t data2, uint32_t data3);

//--------------------------------------------------------------------------
// constexpr implementations
//--------------------------------------------------------------------------

constexpr flex_data_message::flex_data_message(group_t group)
  : universal_packet(0xD0000000u | ((group & 0x0F) << 24))
{
}
constexpr flex_data_message::flex_data_message(group_t        group,
                                               packet_format  format,
                                               packet_address address,
                                               channel_t      channel,
                                               status_t       status_bank,
                                               status_t       status,
                                               uint32_t       data1,
                                               uint32_t       data2,
                                               uint32_t       data3)
  : universal_packet(0xD0000000u | ((group & 0x0F) << 24) |
                       (((uint32_t(format) << 6) | (uint32_t(address) << 4) | (channel & 0x0F)) << 16) |
                       (status_bank << 8) | status,
                     data1,
                     data2,
                     data3)
{
    assert(channel <= 15u);
    assert((address == packet_address::channel) || (channel == 0));
}

//--------------------------------------------------------------------------

inline std::string flex_data_message::payload_as_string() const
{
    return payload_as_string(*this);
}

inline std::string flex_data_message::payload_as_string(const universal_packet& p)
{
    std::string result;
    result.reserve(12);
    for (uint8_t b = 4; b < 16; ++b)
    {
        if (auto c = char(p.get_byte(b)))
        {
            result.push_back(c);
        }
        else
            break;
    }
    return result;
}

//--------------------------------------------------------------------------

constexpr bool is_flex_data_message(const universal_packet& p)
{
    return p.type() == packet_type::flex_data;
}

//--------------------------------------------------------------------------

constexpr std::optional<flex_data_message_view> as_flex_data_message_view(const universal_packet& p)
{
    if (is_flex_data_message(p))
        return flex_data_message_view{ p };
    else
        return std::nullopt;
}

//--------------------------------------------------------------------------

constexpr flex_data_message make_flex_data_message(group_t        group,
                                                   packet_format  format,
                                                   packet_address addr,
                                                   channel_t      channel,
                                                   status_t       status_bank,
                                                   status_t       status,
                                                   uint32_t       data1,
                                                   uint32_t       data2,
                                                   uint32_t       data3)
{
    return flex_data_message(group, format, addr, channel, status_bank, status, data1, data2, data3);
}

//--------------------------------------------------------------------------

constexpr flex_data_message make_flex_data_text_message(group_t                 group,
                                                        packet_format           format,
                                                        packet_address          addr,
                                                        channel_t               channel,
                                                        status_t                status_bank,
                                                        status_t                status,
                                                        const std::string_view& text)
{
    assert(text.length() <= 12);
    assert((format == packet_format::complete) || (format == packet_format::end) || (text.length() == 12));

    auto result = flex_data_message{ group, format, addr, channel, status_bank, status };

    unsigned byte = 4;
    for (auto c : text)
    {
        result.set_byte(byte, c);
        if (++byte >= 16)
            break;
    }
    return result;
}

//--------------------------------------------------------------------------

constexpr flex_data_message make_set_tempo_message(group_t group, uint32_t ten_ns_per_quarter_note)
{
    return make_flex_data_message(
      group, packet_format::complete, packet_address::group, 0, 0x00, 0x00, ten_ns_per_quarter_note);
}

constexpr flex_data_message make_set_time_signature_message(group_t group,
                                                            uint8_t numerator,
                                                            uint8_t denominator,
                                                            uint8_t nr_32rd_notes)
{
    auto result = make_flex_data_message(group, packet_format::complete, packet_address::group, 0, 0x00, 0x01);
    result.set_byte(4, numerator);
    result.set_byte(5, denominator);
    result.set_byte(6, nr_32rd_notes);
    return result;
}

constexpr flex_data_message make_set_metronome_message(group_t group,
                                                       uint8_t num_clocks_per_primary_click,
                                                       uint8_t bar_accent_part1,
                                                       uint8_t bar_accent_part2,
                                                       uint8_t bar_accent_part3,
                                                       uint8_t num_subdivision_clicks1,
                                                       uint8_t num_subdivision_clicks2)
{
    auto result = make_flex_data_message(group, packet_format::complete, packet_address::group, 0, 0x00, 0x02);
    result.set_byte(4, num_clocks_per_primary_click);
    result.set_byte(5, bar_accent_part1);
    result.set_byte(6, bar_accent_part2);
    result.set_byte(7, bar_accent_part3);
    result.set_byte(8, num_subdivision_clicks1);
    result.set_byte(9, num_subdivision_clicks2);
    return result;
}

constexpr flex_data_message make_set_key_signature_message(
  group_t group, packet_address addr, channel_t channel, uint4_t sharps_or_flats, uint4_t tonic_note)
{
    auto result = make_flex_data_message(group, packet_format::complete, addr, channel, 0x00, 0x05);
    result.set_byte(4, (sharps_or_flats << 4) | (tonic_note & 0x0F));
    return result;
}

constexpr flex_data_message make_set_chord_message(
  group_t group, packet_address addr, channel_t channel, uint32_t data1, uint32_t data2, uint32_t data3)
{
    return make_flex_data_message(group, packet_format::complete, addr, channel, 0x00, 0x06, data1, data2, data3);
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
