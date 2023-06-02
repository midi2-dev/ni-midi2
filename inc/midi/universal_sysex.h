#pragma once

//--------------------------------------------------------------------------

#include <midi/sysex.h>
#include <midi/types.h>

#include <optional>

//--------------------------------------------------------------------------

namespace midi::universal_sysex {

//--------------------------------------------------------------------------

using type_t    = uint16_t;
using subtype_t = uint7_t;

//--------------------------------------------------------------------------
//! Universal SysEx types
namespace type {
    // Non-Real Time (7EH)
    constexpr type_t sample_dump_header           = 0x7E01; //!< Sample Dump Header
    constexpr type_t sample_data_packet           = 0x7E02; //!< Sample Data Packet
    constexpr type_t sample_dump_request          = 0x7E03; //!< Sample Dump Request
    constexpr type_t midi_time_code_non_real_time = 0x7E04; //!< MIDI Time Code (MTC)
    constexpr type_t sample_dump_extensions       = 0x7E05; //!< Sample Dump Extensions (SDE)
    constexpr type_t general_information          = 0x7E06; //!< General Information (IDENTITY)
    constexpr type_t file_dump                    = 0x7E07; //!< File Dump
    constexpr type_t midi_tuning_non_real_time    = 0x7E08; //!< MIDI Tuning Standard (Non-Real Time)
    constexpr type_t general_midi                 = 0x7E09; //!< General MIDI (GM)
    constexpr type_t downloadable_sounds          = 0x7E0A; //!< Downloadable Sounds (DLS)
    constexpr type_t file_reference_message       = 0x7E0B; //!< File Reference Message (FRM)
    constexpr type_t midi_visual_control          = 0x7E0C; //!< MIDI Visual Control (MVC)
    constexpr type_t capability_inquiry           = 0x7E0D; //!< MIDI Capability Inquiry (CI)
    constexpr type_t end_of_file                  = 0x7E7B; //!< End of File
    constexpr type_t wait                         = 0x7E7C; //!< Wait
    constexpr type_t cancel                       = 0x7E7D; //!< Cancel
    constexpr type_t nak                          = 0x7E7E; //!< NAK
    constexpr type_t ack                          = 0x7E7F; //!< ACK

    // real time (7fh)
    constexpr type_t midi_time_code_real_time       = 0x7F01; //!< MIDI Time Code (MTC)
    constexpr type_t midi_show_control              = 0x7F02; //!< MIDI Show Control (MSC)
    constexpr type_t notation_information           = 0x7F03; //!< Notation Information
    constexpr type_t device_control                 = 0x7F04; //!< Device Control
    constexpr type_t real_time_mtc_cueing           = 0x7F05; //!< Real Time MTC Cueing
    constexpr type_t midi_machine_control_commands  = 0x7F06; //!< MIDI Machine Control Commands (MMC)
    constexpr type_t midi_machine_control_responses = 0x7F07; //!< MIDI Machine Control Responses (MMC)
    constexpr type_t midi_tuning_real_time          = 0x7F08; //!< MIDI Tuning Standard (Real Time)
    constexpr type_t controller_destination_setting =
      0x7F09; //!< Controller Destination Setting (See GM2 Documentation)

    // special
    constexpr type_t none = 0x0000; //!< not available or 0
} // namespace type

//--------------------------------------------------------------------------
//! Universal SysEx subtypes
namespace subtype {
    // General Information
    constexpr subtype_t identity_request = 0x01; //!< General Information - Identity Request
    constexpr subtype_t identity_reply   = 0x02; //!< General Information - Identity Reply

    // General MIDI
    constexpr subtype_t gm_1_system_on = 0x01; //!< General General MIDI 1 System On
    constexpr subtype_t gm_system_off  = 0x02; //!< General General MIDI System Off
    constexpr subtype_t gm_2_system_on = 0x03; //!< General General MIDI 2 System On

    // MIDI Time Code
    constexpr subtype_t mtc_full_message = 0x01; //!< MIDI Time Code - Full Message
    constexpr subtype_t mtc_user_bits    = 0x02; //!< MIDI Time Code - User Bits
} // namespace subtype

struct message_view
{
    explicit message_view(const midi::sysex7& s)
      : sx(s)
    {
        assert((sx.manufacturerID == manufacturer::universal_realtime) ||
               (sx.manufacturerID == manufacturer::universal_non_realtime));
        assert(sx.data.size() >= 2);
    }

    uint7_t   device_id() const { return sx.data[0]; }
    type_t    type() const { return static_cast<type_t>((sx.manufacturerID >> 8) | sx.data[1]); }
    subtype_t subtype() const { return (sx.data.size() > 2) ? sx.data[2] : 0; }
    size_t    data_size() const { return sx.data.size(); }

    template<typename view_class>
    inline std::optional<view_class> as() const
    {
        if (view_class::validate(sx))
            return view_class{ sx };
        else
            return std::nullopt;
    }

    template<typename view_class>
    static inline std::optional<view_class> make_optional(const midi::sysex7& s)
    {
        if (view_class::validate(s))
            return view_class{ s };
        else
            return std::nullopt;
    }

    const midi::sysex7& sx;
};

//--------------------------------------------------------------------------
//! Helper struct for dealing with Universal SysEx messages
struct message : sysex7
{
    uint7_t   device_id() const;
    type_t    type() const;
    subtype_t subtype() const;

    void set_device_id(uint7_t);

    using sysex7::sysex7;
};

//--------------------------------------------------------------------------
//! Universal SysEx Identity Request message
struct identity_request : message
{
    explicit identity_request(uint7_t device_id = 0x7F);
};

message make_identity_request(uint7_t device_id = 0x7F);

bool is_identity_request(const midi::sysex7&);

//--------------------------------------------------------------------------
//! Universal SysEx Identity Reply view
struct identity_reply_view : message_view
{
    using message_view::message_view;

    device_identity identity() const;

    static bool validate(const sysex7& sx);
};

std::optional<identity_reply_view> as_identity_reply_view(const sysex7&);

//--------------------------------------------------------------------------
//! Universal SysEx Identity Reply message
struct identity_reply : message
{
    identity_reply(
      manufacturer_t sysex_id, uint14_t family, uint14_t family_member, uint28_t revision, uint7_t device_id = 0x7F);
    explicit identity_reply(const device_identity&);
};

message make_identity_reply(
  manufacturer_t sysex_id, uint14_t family, uint14_t family_member, uint28_t revision, uint7_t device_id = 0x7F);
message make_identity_reply(const device_identity&);

bool is_identity_reply(const midi::sysex7&);

//--------------------------------------------------------------------------

} // namespace midi::universal_sysex

namespace midi {

//--------------------------------------------------------------------------

bool is_universal_sysex_message(const sysex7&);

universal_sysex::type_t    universal_sysex_type_of(const sysex7&);
universal_sysex::subtype_t universal_sysex_subtype_of(const sysex7&);

using universal_sysex_view = universal_sysex::message_view;

inline std::optional<universal_sysex_view> as_universal_sysex_view(const midi::sysex7& sx)
{
    if (is_universal_sysex_message(sx))
        return universal_sysex_view{ sx };
    else
        return std::nullopt;
}

//--------------------------------------------------------------------------
// inline implementations

inline bool is_universal_sysex_message(const sysex7& sx)
{
    return ((sx.manufacturerID == manufacturer::universal_realtime) ||
            (sx.manufacturerID == manufacturer::universal_non_realtime)) &&
           (sx.data.size() >= 2);
}

inline universal_sysex::type_t universal_sysex_type_of(const sysex7& sx)
{
    if (is_universal_sysex_message(sx))
    {
        return static_cast<uint16_t>((sx.manufacturerID >> 8) | sx.data[1]);
    }

    return universal_sysex::type::none;
}

inline universal_sysex::subtype_t universal_sysex_subtype_of(const sysex7& sx)
{
    if (is_universal_sysex_message(sx) && (sx.data.size() > 2))
    {
        return sx.data[2];
    }

    return 0;
}

inline uint7_t universal_sysex_device_id_of(const sysex7& sx)
{
    if (is_universal_sysex_message(sx))
    {
        return sx.data[0];
    }

    return 0xFF; // invalid
}

inline universal_sysex::message universal_sysex::make_identity_request(uint7_t device_id)
{
    return identity_request(device_id);
}

inline bool universal_sysex::is_identity_request(const midi::sysex7& sx)
{
    return (universal_sysex_type_of(sx) == type::general_information) &&
           (universal_sysex_subtype_of(sx) == subtype::identity_request);
}

inline std::optional<universal_sysex::identity_reply_view> universal_sysex::as_identity_reply_view(const sysex7& sx)
{
    return message_view::make_optional<identity_reply_view>(sx);
}

inline universal_sysex::message universal_sysex::make_identity_reply(
  manufacturer_t sysex_id, uint14_t family, uint14_t family_member, uint28_t revision, uint7_t device_id)
{
    return identity_reply{ sysex_id, family, family_member, revision, device_id };
}

inline universal_sysex::message universal_sysex::make_identity_reply(const device_identity& i)
{
    return identity_reply{ i };
}

inline bool universal_sysex::is_identity_reply(const midi::sysex7& sx)
{
    return (universal_sysex_type_of(sx) == type::general_information) &&
           (universal_sysex_subtype_of(sx) == subtype::identity_reply) &&
           (((sx.data.size() == 12) && (sx.data[3] != 0)) || (sx.data.size() == 14));
}

//--------------------------------------------------------------------------

inline uint7_t universal_sysex::message::device_id() const
{
    return data.empty() ? 0xFF /* invalid */ : data[0];
}

inline universal_sysex::type_t universal_sysex::message::type() const
{
    return (data.size() < 2) ? type::none : static_cast<uint16_t>((manufacturerID >> 8) | data[1]);
}

inline universal_sysex::subtype_t universal_sysex::message::subtype() const
{
    return (data.size() > 2) ? data[2] : 0;
}

inline void universal_sysex::message::set_device_id(uint7_t i)
{
    assert(!data.empty());
    assert(i < 128);
    data[0] = i & 0x7F;
}

//-----------------------------------------------

inline universal_sysex::identity_request::identity_request(uint7_t device_id)
  : message(manufacturer::universal_non_realtime, { device_id, 0x06, subtype::identity_request })
{
}

//-----------------------------------------------

inline device_identity universal_sysex::identity_reply_view::identity() const
{
    const bool           short_sysex_id = (sx.data[3] != 0);
    const manufacturer_t manu           = short_sysex_id ? (sx.data[3] << 16) : ((sx.data[4] << 8) | sx.data[5]);
    const auto           offset         = short_sysex_id ? 4 : 6;

    return device_identity{ manu, sx.make_uint14(offset), sx.make_uint14(offset + 2), sx.make_uint28(offset + 4) };
}

inline bool universal_sysex::identity_reply_view::validate(const midi::sysex7& sx)
{
    return (universal_sysex_type_of(sx) == type::general_information) &&
           (universal_sysex_subtype_of(sx) == subtype::identity_reply) &&
           (((sx.data.size() == 12) && (sx.data[3] != 0)) || (sx.data.size() == 14));
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
