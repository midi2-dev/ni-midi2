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

struct data_message : universal_packet
{
    constexpr data_message();
    constexpr explicit data_message(status_t);
    ~data_message() = default;

    constexpr uint8_t payload_byte(size_t b) const { return get_byte(2 + b); }
    void              set_payload_byte(size_t, uint8_t);

    constexpr size_t payload_size() const { return status() & 0x0F; }
    void             set_payload_size(size_t);

    void add_payload_byte(uint8_t);
};

//--------------------------------------------------------------------------

constexpr bool is_data_message(const universal_packet&);
constexpr bool is_sysex7_packet(const universal_packet&);

//--------------------------------------------------------------------------

struct sysex7_packet_view
{
    constexpr explicit sysex7_packet_view(const universal_packet& ump)
      : p(ump)
    {
        assert(is_sysex7_packet(ump));
    }

    constexpr group_t       group() const { return p.group(); }
    constexpr status_t      status() const { return p.status() & 0xF0; }
    constexpr packet_format format() const { return packet_format((p.status() >> 4) & 0b11); }
    constexpr size_t        payload_size() const { return p.status() & 0x0F; }
    constexpr uint8_t       payload_byte(size_t b) const { return p.get_byte(2 + b); }

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr std::optional<sysex7_packet_view> as_sysex7_packet_view(const universal_packet&);

//--------------------------------------------------------------------------

constexpr data_message make_sysex7_complete_packet(group_t = 0);
constexpr data_message make_sysex7_start_packet(group_t = 0);
constexpr data_message make_sysex7_continue_packet(group_t = 0);
constexpr data_message make_sysex7_end_packet(group_t = 0);

//--------------------------------------------------------------------------

constexpr data_message::data_message()
  : universal_packet(0x30000000)
{
}
constexpr data_message::data_message(status_t status)
  : universal_packet(0x30000000u | (status << 16))
{
}

inline void data_message::set_payload_byte(size_t b, uint8_t data)
{
    set_byte_7bit(2 + b, data);
}

constexpr data_message make_sysex7_complete_packet(group_t group)
{
    data_message result{ data_status::sysex7_complete };
    result.set_group(group);
    return result;
}
constexpr data_message make_sysex7_start_packet(group_t group)
{
    data_message result{ data_status::sysex7_start };
    result.set_group(group);
    return result;
}
constexpr data_message make_sysex7_continue_packet(group_t group)
{
    data_message result{ data_status::sysex7_continue };
    result.set_group(group);
    return result;
}
constexpr data_message make_sysex7_end_packet(group_t group)
{
    data_message result{ data_status::sysex7_end };
    result.set_group(group);
    return result;
}
constexpr bool is_data_message(const universal_packet& p)
{
    return p.type() == packet_type::data;
}
constexpr bool is_sysex7_packet(const universal_packet& p)
{
    return is_data_message(p) && ((p.status() & 0xF0) <= data_status::sysex7_end) && ((p.status() & 0x0F) <= 6);
}
constexpr std::optional<sysex7_packet_view> as_sysex7_packet_view(const universal_packet& p)
{
    if (is_sysex7_packet(p))
        return sysex7_packet_view{ p };
    else
        return std::nullopt;
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
