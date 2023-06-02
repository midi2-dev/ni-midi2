#pragma once

//--------------------------------------------------------------------------

#include <midi/types.h>
#include <midi/universal_packet.h>

//--------------------------------------------------------------------------

#include <algorithm>
#include <cassert>
#include <optional>
#include <string>
#include <string_view>

//--------------------------------------------------------------------------
/* M2-104-UM_v1-0-28_UMP_and_MIDI_2-0_Protocol_Specification, March 10, 2023 */

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

struct stream_message : universal_packet
{
    constexpr stream_message();
    constexpr explicit stream_message(status_t, packet_format_t = packet_format::complete);
    ~stream_message() = default;

    constexpr packet_format_t format() const { return ((data[0] >> 26u) & 0x03); }
    constexpr void            set_format(packet_format_t);

    // UMP stream messages are groupless
    constexpr group_t group() const      = delete;
    constexpr void    set_group(group_t) = delete;

    static std::string payload_as_string(const universal_packet& p, uint8_t offset)
    {
        std::string result;
        result.reserve(16 - offset);
        for (uint8_t b = offset; b < 16; ++b)
        {
            if (auto c = char(p.get_byte_7bit(b)))
            {
                result.push_back(c);
            }
            else
                break;
        }
        return result;
    }
};

//--------------------------------------------------------------------------

constexpr bool is_stream_message(const universal_packet&);

//--------------------------------------------------------------------------

struct endpoint_discovery_view
{
    constexpr explicit endpoint_discovery_view(const universal_packet& ump)
      : p(ump)
    {
        assert(p.type() == packet_type::stream);
        assert(p.status() == stream_status::endpoint_discovery);
    }

    constexpr uint8_t  ump_version_major() const { return p.byte3(); }
    constexpr uint8_t  ump_version_minor() const { return p.byte4(); }
    constexpr uint16_t ump_version() const { return uint16_t(p.data[0] & 0xFFFFu); }

    constexpr uint8_t filter() const { return uint8_t(p.data[1] & 0b11111); }

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr std::optional<endpoint_discovery_view> as_endpoint_discovery_view(const universal_packet&);

//--------------------------------------------------------------------------

struct endpoint_info_view
{
    constexpr explicit endpoint_info_view(const universal_packet& ump)
      : p(ump)
    {
        assert(p.type() == packet_type::stream);
        assert(p.status() == stream_status::endpoint_info);
    }

    constexpr uint8_t  ump_version_major() const { return p.byte3(); }
    constexpr uint8_t  ump_version_minor() const { return p.byte4(); }
    constexpr uint16_t ump_version() const { return uint16_t(p.data[0] & 0xFFFFu); }

    constexpr uint8_t num_function_blocks() const { return p.get_byte(4) & 0x7Fu; }
    constexpr bool    static_function_blocks() const { return (p.get_byte(4) & 0x80u) != 0; }
    constexpr uint8_t protocols() const { return p.get_byte(6) & 0b11; }
    constexpr uint8_t extensions() const { return p.get_byte(7) & 0b11; }

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr std::optional<endpoint_info_view> as_endpoint_info_view(const universal_packet&);

//--------------------------------------------------------------------------

struct device_identity_view
{
    constexpr explicit device_identity_view(const universal_packet& ump)
      : p(ump)
    {
        assert(p.type() == packet_type::stream);
        assert(p.status() == stream_status::device_identity);
    }

    constexpr device_identity identity() const
    {
        return { p.data[1] & 0x007F7F7Fu,
                 // TODO: fix this (we need to assemble the numbers, not mask)
                 uint16_t((p.data[2] >> 16) & 0x7F7Fu),
                 uint16_t(p.data[2] & 0x7F7Fu),
                 p.data[3] & 0x7F7F7F7Fu };
    }

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr std::optional<device_identity_view> as_device_identity_view(const universal_packet&);

//--------------------------------------------------------------------------

struct endpoint_name_view
{
    constexpr explicit endpoint_name_view(const universal_packet& ump)
      : p(ump)
    {
        assert(p.type() == packet_type::stream);
        assert(p.status() == stream_status::endpoint_name);
    }

    constexpr packet_format_t format() const { return ((p.data[0] >> 26u) & 0b11); }

    std::string payload() const { return stream_message::payload_as_string(p, 2); }

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr std::optional<endpoint_name_view> as_endpoint_name_view(const universal_packet&);

//--------------------------------------------------------------------------

struct product_instance_id_view
{
    constexpr explicit product_instance_id_view(const universal_packet& ump)
      : p(ump)
    {
        assert(p.type() == packet_type::stream);
        assert(p.status() == stream_status::product_instance_id);
    }

    constexpr packet_format_t format() const { return ((p.data[0] >> 26u) & 0b11); }

    std::string payload() const { return stream_message::payload_as_string(p, 2); }

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr std::optional<product_instance_id_view> as_product_instance_id_view(const universal_packet&);

//--------------------------------------------------------------------------

struct stream_configuration_view
{
    constexpr explicit stream_configuration_view(const universal_packet& ump)
      : p(ump)
    {
        assert(p.type() == packet_type::stream);
        assert((p.status() == stream_status::stream_configuration_request) ||
               (p.status() == stream_status::stream_configuration_notify));
    }

    constexpr protocol_t   protocol() const { return p.byte3() & 0b11; }
    constexpr extensions_t extensions() const { return p.byte4() & 0b11; }

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr std::optional<stream_configuration_view> as_stream_configuration_view(const universal_packet&);

//--------------------------------------------------------------------------

struct function_block_discovery_view
{
    constexpr explicit function_block_discovery_view(const universal_packet& ump)
      : p(ump)
    {
        assert(p.type() == packet_type::stream);
        assert(p.status() == stream_status::function_block_discovery);
    }

    constexpr uint8_t function_block() const { return p.byte3(); }
    constexpr uint8_t filter() const { return p.byte4() & 0b1111; }

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr std::optional<function_block_discovery_view> as_function_block_discovery_view(const universal_packet&);

//--------------------------------------------------------------------------

struct function_block_options
{
    // active
    bool active = true;

    // directions
    static constexpr uint2_t direction_input  = 0b01; //!< Input, Function Block receives MIDI Messages only
    static constexpr uint2_t direction_output = 0b10; //!< Output, Function Block transmits MIDI Messages only
    static constexpr uint2_t bidirectional =
      0b11; //!<  Bidirectional Connections. Every Input Group member has a matching Output Group.

    uint2_t direction = bidirectional;

    // MIDI 1
    static constexpr uint2_t not_midi1          = 0b00; //!< Not MIDI 1.0
    static constexpr uint2_t midi1_unrestricted = 0b01; //!< MIDI 1.0 - don't restrict Bandwidth
    static constexpr uint2_t midi1_31250        = 0b10; //!< Restrict Bandwidth to 31.25Kbps

    uint2_t midi1 = not_midi1;

    // ui hints
    static constexpr uint2_t ui_hint_as_direction = 0b00;
    static constexpr uint2_t ui_hint_receiver     = 0b01;
    static constexpr uint2_t ui_hint_sender       = 0b10;

    uint2_t ui_hint = ui_hint_as_direction;

    uint8_t ci_message_version     = 0x00;
    uint8_t max_num_sysex8_streams = 0;
};

//--------------------------------------------------------------------------

struct function_block_info_view
{
    constexpr explicit function_block_info_view(const universal_packet& ump)
      : p(ump)
    {
        assert(p.type() == packet_type::stream);
        assert(p.status() == stream_status::function_block_info);
    }

    constexpr bool    active() const { return (p.data[0] & 0x00008000u) != 0; }
    constexpr uint8_t function_block() const { return p.get_byte_7bit(2); }

    constexpr uint8_t direction() const { return uint8_t(p.data[0] & 0b11); }
    constexpr uint8_t midi1() const { return uint8_t(p.data[0] >> 2) & 0b11; }
    constexpr uint8_t ui_hint() const { return uint8_t(p.data[0] >> 4) & 0b11; }

    constexpr uint8_t first_group() const { return p.get_byte(4); }
    constexpr uint8_t num_groups_spanned() const { return p.get_byte(5); }

    constexpr uint7_t ci_message_version() const { return p.get_byte(6); }
    constexpr uint8_t max_num_sysex8_streams() const { return p.get_byte(7); }

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr std::optional<function_block_info_view> as_function_block_info_view(const universal_packet&);

//--------------------------------------------------------------------------

struct function_block_name_view
{
    constexpr explicit function_block_name_view(const universal_packet& ump)
      : p(ump)
    {
        assert(p.type() == packet_type::stream);
        assert(p.status() == stream_status::function_block_name);
    }

    constexpr packet_format_t format() const { return ((p.data[0] >> 26u) & 0b11); }
    constexpr uint8_t         function_block() const { return p.byte3() & 0x7F; }

    std::string payload() const { return stream_message::payload_as_string(p, 3); }

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr std::optional<function_block_name_view> as_function_block_name_view(const universal_packet&);

//--------------------------------------------------------------------------

constexpr stream_message make_endpoint_discovery_message(uint8_t filter,
                                                         uint8_t ump_version_major = 1,
                                                         uint8_t ump_version_minor = 1);
constexpr stream_message make_endpoint_info_message(uint8_t num_function_blocks,
                                                    bool    static_function_blocks,
                                                    uint8_t protocols,
                                                    uint8_t extensions,
                                                    uint8_t ump_version_major = 1,
                                                    uint8_t ump_version_minor = 1);
constexpr stream_message make_device_identity_message(const device_identity&);
constexpr stream_message make_endpoint_name_message(packet_format_t, const std::string_view&);
constexpr stream_message make_product_instance_id_message(packet_format_t, const std::string_view&);
constexpr stream_message make_stream_configuration_request(protocol_t, extensions_t = 0);
constexpr stream_message make_stream_configuration_notification(protocol_t, extensions_t = 0);

constexpr stream_message make_function_block_discovery_message(uint8_t function_block, uint8_t filter);
constexpr stream_message make_function_block_info_message(uint7_t function_block,
                                                          uint2_t direction,
                                                          group_t first_group,
                                                          uint4_t num_groups_spanned = 1);
constexpr stream_message make_function_block_info_message(uint7_t function_block,
                                                          const function_block_options&,
                                                          group_t first_group,
                                                          uint4_t num_groups_spanned = 1);
constexpr stream_message make_function_block_name_message(packet_format_t,
                                                          uint7_t                 function_block,
                                                          const std::string_view& n);

//--------------------------------------------------------------------------
// constexpr implementations
//--------------------------------------------------------------------------

constexpr stream_message::stream_message()
  : universal_packet(0xF0000000u)
{
}

constexpr stream_message::stream_message(status_t status, packet_format_t format)
  : universal_packet(0xF0000000u | ((format & 0x03) << 26) | (status << 16))
{
}

constexpr void stream_message::set_format(packet_format_t f)
{
    assert(f < 4);

    data[0] = (data[0] & 0xF3FFFFFF) | ((f & 0x3) << 26);
}

//--------------------------------------------------------------------------

constexpr bool is_stream_message(const universal_packet& p)
{
    return (p.type() == packet_type::stream);
}

//--------------------------------------------------------------------------

constexpr std::optional<endpoint_discovery_view> as_endpoint_discovery_view(const universal_packet& p)
{
    if (is_stream_message(p) && (p.status() == stream_status::endpoint_discovery))
        return endpoint_discovery_view{ p };
    else
        return std::nullopt;
}

constexpr std::optional<endpoint_info_view> as_endpoint_info_view(const universal_packet& p)
{
    if (is_stream_message(p) && (p.status() == stream_status::endpoint_info))
        return endpoint_info_view{ p };
    else
        return std::nullopt;
}

constexpr std::optional<device_identity_view> as_device_identity_view(const universal_packet& p)
{
    if (is_stream_message(p) && (p.status() == stream_status::device_identity))
        return device_identity_view{ p };
    else
        return std::nullopt;
}

constexpr std::optional<endpoint_name_view> as_endpoint_name_view(const universal_packet& p)
{
    if (is_stream_message(p) && (p.status() == stream_status::endpoint_name))
        return endpoint_name_view{ p };
    else
        return std::nullopt;
}

constexpr std::optional<product_instance_id_view> as_product_instance_id_view(const universal_packet& p)
{
    if (is_stream_message(p) && (p.status() == stream_status::product_instance_id))
        return product_instance_id_view{ p };
    else
        return std::nullopt;
}

constexpr std::optional<stream_configuration_view> as_stream_configuration_view(const universal_packet& p)
{
    if (is_stream_message(p) && ((p.status() == stream_status::stream_configuration_request) ||
                                 (p.status() == stream_status::stream_configuration_notify)))
        return stream_configuration_view{ p };
    else
        return std::nullopt;
}

constexpr std::optional<function_block_discovery_view> as_function_block_discovery_view(const universal_packet& p)
{
    if (is_stream_message(p) && (p.status() == stream_status::function_block_discovery))
        return function_block_discovery_view{ p };
    else
        return std::nullopt;
}

constexpr std::optional<function_block_info_view> as_function_block_info_view(const universal_packet& p)
{
    if (is_stream_message(p) && (p.status() == stream_status::function_block_info))
        return function_block_info_view{ p };
    else
        return std::nullopt;
}

constexpr std::optional<function_block_name_view> as_function_block_name_view(const universal_packet& p)
{
    if (is_stream_message(p) && (p.status() == stream_status::function_block_name))
        return function_block_name_view{ p };
    else
        return std::nullopt;
}

//--------------------------------------------------------------------------

constexpr stream_message make_endpoint_discovery_message(uint8_t filter,
                                                         uint8_t ump_version_major,
                                                         uint8_t ump_version_minor)
{
    stream_message m{ stream_status::endpoint_discovery, packet_format::complete };
    m.set_byte(2, ump_version_major);
    m.set_byte(3, ump_version_minor);
    m.data[1] = filter;
    return m;
}

constexpr stream_message make_endpoint_info_message(uint8_t num_function_blocks,
                                                    bool    static_function_blocks,
                                                    uint8_t protocols,
                                                    uint8_t extensions,
                                                    uint8_t ump_version_major,
                                                    uint8_t ump_version_minor)
{
    stream_message m{ stream_status::endpoint_info, packet_format::complete };
    m.set_byte(2, ump_version_major);
    m.set_byte(3, ump_version_minor);
    m.set_byte(4, (static_function_blocks ? 0x80u : 0x00u) | num_function_blocks);
    m.set_byte(6, protocols);
    m.set_byte(7, extensions);
    return m;
}

constexpr stream_message make_device_identity_message(const device_identity& i)
{
    stream_message m{ stream_status::device_identity, packet_format::complete };
    m.data[1] = i.manufacturer;
    // TODO: we need byte arithmetic here, 7 bit data!
    m.data[2] = (i.family << 16) | i.model;
    m.data[3] = i.revision;
    return m;
}

constexpr stream_message make_endpoint_name_message(packet_format_t format, const std::string_view& n)
{
    assert(n.length() <= 14);
    stream_message m{ stream_status::endpoint_name, format };
    unsigned       b = 2;
    for (auto c : n)
    {
        m.set_byte(b, c);
        if (++b >= 16)
            break;
    }
    return m;
}

constexpr stream_message make_product_instance_id_message(packet_format_t format, const std::string_view& n)
{
    assert(n.length() <= 14);
    assert(format != packet_format::cont);
    stream_message m{ stream_status::product_instance_id, format };
    unsigned       b = 2;
    for (auto c : n)
    {
        m.set_byte_7bit(b, c);
        if (++b >= 16)
            break;
    }
    return m;
}

//--------------------------------------------------------------------------

constexpr stream_message make_stream_configuration_request(protocol_t p, extensions_t e)
{
    assert(p && (p < 0x3));
    stream_message m{ stream_status::stream_configuration_request, packet_format::complete };
    m.set_byte(2, p);
    m.set_byte(3, e);
    return m;
}

constexpr stream_message make_stream_configuration_notification(protocol_t p, extensions_t e)
{
    assert(p && (p < 0x3));
    stream_message m{ stream_status::stream_configuration_notify, packet_format::complete };
    m.set_byte(2, p);
    m.set_byte(3, e);
    return m;
}

//--------------------------------------------------------------------------

constexpr stream_message make_function_block_discovery_message(uint8_t function_block, uint8_t filter)
{
    assert((function_block == 0xFF) || (function_block < 32));
    stream_message m{ stream_status::function_block_discovery, packet_format::complete };
    m.set_byte(2, function_block);
    m.set_byte(3, filter);
    return m;
}

constexpr stream_message make_function_block_info_message(uint7_t function_block,
                                                          uint4_t direction,
                                                          group_t first_group,
                                                          uint4_t num_groups_spanned)
{
    assert(function_block < 32);
    assert((direction > 0 && direction < 4));
    stream_message m{ stream_status::function_block_info, packet_format::complete };
    m.set_byte(2, 0x80u | (function_block & 0x1F));
    m.set_byte(3, ((direction & 0x03u) << 4) | (direction & 0x03u));
    m.set_byte(4, first_group & 0xF);
    m.set_byte(5, num_groups_spanned & 0xF);
    return m;
}

constexpr stream_message make_function_block_info_message(uint7_t                       function_block,
                                                          const function_block_options& options,
                                                          group_t                       first_group,
                                                          uint4_t                       num_groups_spanned)
{
    assert(function_block < 32);
    assert((options.direction > 0 && options.direction < 4));
    assert((options.midi1 < 3));
    assert((options.ui_hint < 4));
    assert((options.ui_hint == 0) || (options.direction & options.ui_hint));

    stream_message m{ stream_status::function_block_info, packet_format::complete };
    m.set_byte(2, (options.active ? 0x80u : 0x00u) | (function_block & 0x1F));
    m.set_byte(3,
               (((options.ui_hint ? options.ui_hint : options.direction) & 0x03u) << 4) |
                 ((options.midi1 & 0x03u) << 2) | (options.direction & 0x03u));
    m.set_byte(4, first_group & 0xF);
    m.set_byte(5, num_groups_spanned & 0xF);
    m.set_byte(6, options.ci_message_version);
    m.set_byte(7, options.max_num_sysex8_streams);
    return m;
}

constexpr stream_message make_function_block_name_message(packet_format_t         format,
                                                          uint7_t                 function_block,
                                                          const std::string_view& n)
{
    assert(n.length() <= 13);
    stream_message m{ stream_status::function_block_name, format };
    m.set_byte(2, function_block);

    unsigned b = 3;
    for (auto c : n)
    {
        m.set_byte(b, c);
        if (++b >= 16)
            break;
    }
    return m;
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
