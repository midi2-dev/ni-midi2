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

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

struct extended_data_message : universal_packet
{
    constexpr extended_data_message();
    constexpr explicit extended_data_message(status_t);
    ~extended_data_message() = default;
};

//--------------------------------------------------------------------------

struct sysex8_packet : extended_data_message
{
    constexpr sysex8_packet();
    constexpr sysex8_packet(status_t, uint8_t stream_id, group_t);
    ~sysex8_packet() = default;

    constexpr packet_format format() const { return packet_format((status() >> 4) & 0b11); }

    constexpr uint8_t stream_id() const { return get_byte(2); }
    constexpr void    set_stream_id(uint8_t);

    constexpr uint8_t payload_byte(size_t b) const { return get_byte(3 + b); }
    constexpr void    set_payload_byte(size_t, uint8_t);

    constexpr size_t payload_size() const;
    constexpr void   set_payload_size(size_t);

    constexpr void add_payload_byte(uint8_t);
};

//--------------------------------------------------------------------------

struct mixed_data_set_header_packet : extended_data_message
{
    constexpr mixed_data_set_header_packet();
    constexpr mixed_data_set_header_packet(uint4_t mds_id, group_t);
    ~mixed_data_set_header_packet() = default;

    constexpr uint4_t mds_id() const { return status() & 0x0F; }
    constexpr void    set_mds_id(uint4_t);

    constexpr uint16_t valid_bytes_in_chunk() const { return make_uint16(2); }
    constexpr void     set_valid_bytes_in_chunk(uint16_t v) { set_uint16(2, v); }

    constexpr uint16_t nr_of_chunks() const { return make_uint16(4); }
    constexpr void     set_nr_of_chunks(uint16_t v) { set_uint16(4, v); }

    constexpr uint16_t chunk_nr() const { return make_uint16(6); }
    constexpr void     set_chunk_nr(uint16_t v) { set_uint16(6, v); }

    constexpr uint16_t manufacturer_id() const { return make_uint16(8); }
    constexpr void     set_manufacturer_id(uint16_t v) { set_uint16(8, v); }

    constexpr uint16_t device_id() const { return make_uint16(10); }
    constexpr void     set_device_id(uint16_t v) { set_uint16(10, v); }

    constexpr uint16_t sub_id_1() const { return make_uint16(12); }
    constexpr void     set_sub_id_1(uint16_t v) { set_uint16(12, v); }

    constexpr uint16_t sub_id_2() const { return make_uint16(14); }
    constexpr void     set_sub_id_2(uint16_t v) { set_uint16(14, v); }

  private:
    constexpr uint16_t make_uint16(size_t b) const { return uint16_t((get_byte(b) << 8) | get_byte(b + 1)); }
    constexpr void     set_uint16(size_t b, uint16_t v)
    {
        set_byte(b, uint8_t(v >> 8));
        set_byte(b + 1, uint8_t(v & 0xFF));
    }
};

//--------------------------------------------------------------------------

struct mixed_data_set_payload_packet : extended_data_message
{
    constexpr mixed_data_set_payload_packet();
    constexpr mixed_data_set_payload_packet(uint4_t mds_id, group_t);
    ~mixed_data_set_payload_packet() = default;

    constexpr uint4_t mds_id() const { return status() & 0x0F; }
    constexpr void    set_mds_id(uint4_t);

    //! a mixed data set payload packet always carries 14 payload bytes
    static constexpr size_t payload_size = 14;

    constexpr uint8_t payload_byte(size_t b) const { return get_byte(2 + b); }
    constexpr void    set_payload_byte(size_t, uint8_t);
};

//--------------------------------------------------------------------------

constexpr bool is_extended_data_message(const universal_packet&);
constexpr bool is_sysex8_packet(const universal_packet&);
constexpr bool is_mixed_data_set_header_packet(const universal_packet&);
constexpr bool is_mixed_data_set_payload_packet(const universal_packet&);
constexpr bool is_mixed_data_set_packet(const universal_packet&);

//--------------------------------------------------------------------------

struct sysex8_packet_view
{
    constexpr explicit sysex8_packet_view(const universal_packet& ump)
      : p(ump)
    {
        assert(is_sysex8_packet(ump));
    }

    constexpr group_t       group() const { return p.group(); }
    constexpr packet_format format() const { return packet_format((p.status() >> 4) & 0b11); }
    constexpr uint8_t       stream_id() const { return p.get_byte(2); }
    constexpr size_t        payload_size() const;
    constexpr uint8_t       payload_byte(size_t b) const { return p.get_byte(3 + b); }

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr std::optional<sysex8_packet_view> as_sysex8_packet_view(const universal_packet&);

//--------------------------------------------------------------------------

struct mixed_data_set_header_packet_view
{
    constexpr explicit mixed_data_set_header_packet_view(const universal_packet& ump)
      : p(ump)
    {
        assert(is_mixed_data_set_header_packet(ump));
    }

    constexpr group_t  group() const { return p.group(); }
    constexpr uint4_t  mds_id() const { return p.status() & 0x0F; }
    constexpr uint16_t valid_bytes_in_chunk() const { return make_uint16(2); }
    constexpr uint16_t nr_of_chunks() const { return make_uint16(4); }
    constexpr uint16_t chunk_nr() const { return make_uint16(6); }
    constexpr uint16_t manufacturer_id() const { return make_uint16(8); }
    constexpr uint16_t device_id() const { return make_uint16(10); }
    constexpr uint16_t sub_id_1() const { return make_uint16(12); }
    constexpr uint16_t sub_id_2() const { return make_uint16(14); }

  private:
    constexpr uint16_t make_uint16(size_t b) const { return uint16_t((p.get_byte(b) << 8) | p.get_byte(b + 1)); }

    const universal_packet& p;
};

//--------------------------------------------------------------------------

struct mixed_data_set_payload_packet_view
{
    constexpr explicit mixed_data_set_payload_packet_view(const universal_packet& ump)
      : p(ump)
    {
        assert(is_mixed_data_set_payload_packet(ump));
    }

    constexpr group_t group() const { return p.group(); }
    constexpr uint4_t mds_id() const { return p.status() & 0x0F; }
    constexpr uint8_t payload_byte(size_t b) const { return p.get_byte(2 + b); }

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr std::optional<mixed_data_set_header_packet_view> as_mixed_data_set_header_packet_view(
  const universal_packet&);
constexpr std::optional<mixed_data_set_payload_packet_view> as_mixed_data_set_payload_packet_view(
  const universal_packet&);

//--------------------------------------------------------------------------

constexpr sysex8_packet make_sysex8_complete_packet(uint8_t stream_id, group_t = 0);
constexpr sysex8_packet make_sysex8_start_packet(uint8_t stream_id, group_t = 0);
constexpr sysex8_packet make_sysex8_continue_packet(uint8_t stream_id, group_t = 0);
constexpr sysex8_packet make_sysex8_end_packet(uint8_t stream_id, group_t = 0);

//--------------------------------------------------------------------------

constexpr mixed_data_set_header_packet  make_mixed_data_set_header_packet(uint4_t mds_id, group_t = 0);
constexpr mixed_data_set_payload_packet make_mixed_data_set_payload_packet(uint4_t mds_id, group_t = 0);

//--------------------------------------------------------------------------
// constexpr implementations
//--------------------------------------------------------------------------

constexpr extended_data_message::extended_data_message()
  : universal_packet(0x50000000)
{
}
constexpr extended_data_message::extended_data_message(status_t status)
  : universal_packet(0x50000000u | (status << 16))
{
}

//--------------------------------------------------------------------------

constexpr sysex8_packet::sysex8_packet()
{
    data[0] |= 0x00010000u;
}
constexpr sysex8_packet::sysex8_packet(status_t status, uint8_t stream_id, group_t group)
{
    data[0] = 0x50010000u | (group << 24) | (status << 16) | (stream_id << 8);
}

constexpr void sysex8_packet::set_stream_id(uint8_t i)
{
    set_byte(2, i);
}

constexpr size_t sysex8_packet::payload_size() const
{
    const auto s = static_cast<midi::size_t>(status() & 0x0F);
    return (s > 0 ? s - 1 : 0);
}

constexpr void sysex8_packet::set_payload_size(size_t size)
{
    assert(size <= 13);
    set_byte(1, (status() & 0xF0) + ((size + 1) & 0x0F));
}

constexpr void sysex8_packet::set_payload_byte(size_t b, uint8_t data)
{
    set_byte(3 + b, data);
}

constexpr void sysex8_packet::add_payload_byte(uint8_t byte)
{
    const auto size = payload_size();
    assert(size < 13);
    set_byte(3 + size, byte);
    set_payload_size(size + 1);
}

//--------------------------------------------------------------------------

constexpr bool is_extended_data_message(const universal_packet& p)
{
    return p.type() == packet_type::extended_data;
}

constexpr bool is_sysex8_packet(const universal_packet& p)
{
    return is_extended_data_message(p) && ((p.status() & 0xF0) <= extended_data_status::sysex8_end) &&
           ((p.status() & 0x0F) > 0) && ((p.status() & 0x0F) <= 14);
}

//--------------------------------------------------------------------------

constexpr sysex8_packet make_sysex8_complete_packet(uint8_t stream_id, group_t group)
{
    return sysex8_packet{ extended_data_status::sysex8_complete, stream_id, group };
}
constexpr sysex8_packet make_sysex8_start_packet(uint8_t stream_id, group_t group)
{
    return sysex8_packet{ extended_data_status::sysex8_start, stream_id, group };
}
constexpr sysex8_packet make_sysex8_continue_packet(uint8_t stream_id, group_t group)
{
    return sysex8_packet{ extended_data_status::sysex8_continue, stream_id, group };
}
constexpr sysex8_packet make_sysex8_end_packet(uint8_t stream_id, group_t group)
{
    return sysex8_packet{ extended_data_status::sysex8_end, stream_id, group };
}

//--------------------------------------------------------------------------

constexpr size_t sysex8_packet_view::payload_size() const
{
    const auto s = static_cast<midi::size_t>(p.status() & 0x0F);
    return (s > 0 ? s - 1 : 0);
}

constexpr std::optional<sysex8_packet_view> as_sysex8_packet_view(const universal_packet& p)
{
    if (is_sysex8_packet(p))
        return sysex8_packet_view{ p };
    else
        return std::nullopt;
}

//--------------------------------------------------------------------------

constexpr mixed_data_set_header_packet::mixed_data_set_header_packet()
  : extended_data_message(extended_data_status::mixed_data_set_header)
{
}
constexpr mixed_data_set_header_packet::mixed_data_set_header_packet(uint4_t mds_id, group_t group)
{
    data[0] =
      0x50000000u | ((group & 0x0F) << 24) | ((extended_data_status::mixed_data_set_header | (mds_id & 0x0F)) << 16);
}

constexpr void mixed_data_set_header_packet::set_mds_id(uint4_t i)
{
    set_byte(1, (status() & 0xF0) | (i & 0x0F));
}

//--------------------------------------------------------------------------

constexpr mixed_data_set_payload_packet::mixed_data_set_payload_packet()
  : extended_data_message(extended_data_status::mixed_data_set_payload)
{
}
constexpr mixed_data_set_payload_packet::mixed_data_set_payload_packet(uint4_t mds_id, group_t group)
{
    data[0] =
      0x50000000u | ((group & 0x0F) << 24) | ((extended_data_status::mixed_data_set_payload | (mds_id & 0x0F)) << 16);
}

constexpr void mixed_data_set_payload_packet::set_mds_id(uint4_t i)
{
    set_byte(1, (status() & 0xF0) | (i & 0x0F));
}

constexpr void mixed_data_set_payload_packet::set_payload_byte(size_t b, uint8_t byte)
{
    assert(b < payload_size);
    set_byte(2 + b, byte);
}

//--------------------------------------------------------------------------

constexpr bool is_mixed_data_set_header_packet(const universal_packet& p)
{
    return is_extended_data_message(p) && ((p.status() & 0xF0) == extended_data_status::mixed_data_set_header);
}

constexpr bool is_mixed_data_set_payload_packet(const universal_packet& p)
{
    return is_extended_data_message(p) && ((p.status() & 0xF0) == extended_data_status::mixed_data_set_payload);
}

constexpr bool is_mixed_data_set_packet(const universal_packet& p)
{
    return is_mixed_data_set_header_packet(p) || is_mixed_data_set_payload_packet(p);
}

//--------------------------------------------------------------------------

constexpr mixed_data_set_header_packet make_mixed_data_set_header_packet(uint4_t mds_id, group_t group)
{
    return mixed_data_set_header_packet{ mds_id, group };
}
constexpr mixed_data_set_payload_packet make_mixed_data_set_payload_packet(uint4_t mds_id, group_t group)
{
    return mixed_data_set_payload_packet{ mds_id, group };
}

//--------------------------------------------------------------------------

constexpr std::optional<mixed_data_set_header_packet_view> as_mixed_data_set_header_packet_view(
  const universal_packet& p)
{
    if (is_mixed_data_set_header_packet(p))
        return mixed_data_set_header_packet_view{ p };
    else
        return std::nullopt;
}

constexpr std::optional<mixed_data_set_payload_packet_view> as_mixed_data_set_payload_packet_view(
  const universal_packet& p)
{
    if (is_mixed_data_set_payload_packet(p))
        return mixed_data_set_payload_packet_view{ p };
    else
        return std::nullopt;
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
