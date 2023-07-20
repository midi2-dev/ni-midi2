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
//
//                       MIDI CI (Capability Inquiry)
//
//   M2-101-UM_v1-2_MIDI-CI_Specification, published June 15, 2023
//
//--------------------------------------------------------------------------

#include <midi/types.h>
#include <midi/universal_sysex.h>

#include <cassert>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

bool is_capability_inquiry_message(const sysex7&);

//-----------------------------------------------
//! base class for all Capability Inquiry message views
struct capability_inquiry_view : universal_sysex::message_view
{
    explicit capability_inquiry_view(const midi::sysex7&);
    explicit capability_inquiry_view(const message_view&);

    uint7_t message_version() const;
    muid_t  source_muid() const;
    muid_t  destination_muid() const;

    static bool validate(const sysex7&);

    struct field_offsets
    {
        static constexpr auto message_version  = 3u;
        static constexpr auto source_muid      = 4u;
        static constexpr auto destination_muid = 8u;
        static constexpr auto payload          = 12u;
    };
};

//-----------------------------------------------

} // namespace midi

namespace midi::ci {

//-----------------------------------------------

template<typename view>
std::optional<view> as(const sysex7&);
template<typename view>
std::optional<view> as(const capability_inquiry_view&);

//-----------------------------------------------

constexpr uint7_t message_version_1 = 0x01;
constexpr uint7_t message_version_2 = 0x02;
constexpr uint7_t version           = message_version_2;

using subtype_t  = universal_sysex::subtype_t;
using identity_t = device_identity;

//-----------------------------------------------
//! broadcast muid
constexpr muid_t broadcast_muid = 0x0FFFFFFF;

//-----------------------------------------------
//! Capability Inquiry categories
namespace category {
    constexpr uint7_t profile_configuration = (1 << 2); //!< Profile Configuration
    constexpr uint7_t property_exchange     = (1 << 3); //!< Property Exchange
    constexpr uint7_t process_inquiry       = (1 << 4); //!< Process Inquiry
} // namespace category

//-----------------------------------------------
//! base class for all Capability Inquiry messages
struct message : universal_sysex::message
{
    static constexpr size_t offset_of_data = 12;

    uint7_t message_version() const;
    muid_t  source_muid() const;
    muid_t  destination_muid() const;

    void set_source_muid(muid_t);
    void set_destination_muid(muid_t);

    message(subtype_t subtype, muid_t source_muid, muid_t destination_muid, uint7_t device_id = 0x7F);

    static message make_with_payload_size(
      size_t payload_size, subtype_t subtype, muid_t source_muid, muid_t destination_muid, uint7_t device_id = 0x7F);

  protected:
    explicit message(size_t capacity)
      : universal_sysex::message(manufacturer::universal_non_realtime, capacity)
    {
    }

    static constexpr auto offset_of_smuid = 4;
    static constexpr auto offset_of_dmuid = 8;
};

//-----------------------------------------------
//
//             MANAGEMENT MESSAGES (0x7x)
//
//-----------------------------------------------

//! Capability Inquiry subtypes
namespace subtype {
    // Management Messages
    constexpr subtype_t discovery_inquiry            = 0x70; //!< discover connected MIDI-CI devices
    constexpr subtype_t discovery_reply              = 0x71; //!< reply to discovery message
    constexpr subtype_t endpoint_information_inquiry = 0x72; //!< Inquiry: Endpoint information
    constexpr subtype_t endpoint_information_reply   = 0x73; //!< Reply to Endpoint information
    constexpr subtype_t ack                          = 0x7D; //!< ACK (acknowledge)
    constexpr subtype_t invalidate_muid              = 0x7E; //!< invalidate Device MUID
    constexpr subtype_t nak                          = 0x7F; //!< NAK
} // namespace subtype

//-----------------------------------------------

//! Discovery helpers
namespace discovery {
    //! base class for discovery message views
    struct message_view : capability_inquiry_view
    {
        using capability_inquiry_view::capability_inquiry_view;

        identity_t identity() const;
        uint7_t    categories() const;
        uint28_t   maximum_message_size() const;
        uint7_t    output_path_id() const;

        struct field_offsets;
    };
} // namespace discovery

//---- subtype::discovery_inquiry

//! discovery inquiry view
struct discovery_inquiry_view : discovery::message_view
{
    using discovery::message_view::message_view;

    static bool validate(const sysex7&);
};

message make_discovery_inquiry(muid_t            source_muid,
                               const identity_t& identity,
                               uint7_t           categories,
                               uint28_t          max_message_size,
                               uint7_t           output_path_id = 0);
message make_discovery_inquiry_v1(muid_t            source_muid,
                                  const identity_t& identity,
                                  uint7_t           categories,
                                  uint28_t          max_message_size);

//---- subtype::discovery_reply

//! discovery reply view
struct discovery_reply_view : discovery::message_view
{
    using discovery::message_view::message_view;

    uint7_t function_block() const;

    static bool validate(const sysex7&);

    struct field_offsets;
};

message make_discovery_reply(muid_t            source_muid,
                             muid_t            destination_muid,
                             const identity_t& identity,
                             uint7_t           categories,
                             uint28_t          max_message_size,
                             uint7_t           output_path_id,
                             uint7_t           function_block = 0x7F);

message make_discovery_reply_v1(muid_t            source_muid,
                                muid_t            destination_muid,
                                const identity_t& identity,
                                uint7_t           categories,
                                uint28_t          max_message_size);

//---- subtype::endpoint_information_inquiry

//! endpoint information inquiry view
struct endpoint_information_inquiry_view : capability_inquiry_view
{
    using capability_inquiry_view::capability_inquiry_view;

    uint7_t status() const;

    static bool validate(const sysex7&);

    struct field_offsets;
};

message make_endpoint_information_inquiry(muid_t source_muid, muid_t destination_muid, uint7_t status);

//---- subtype::endpoint_information_reply

//! endpoint information reply view
struct endpoint_information_reply_view : capability_inquiry_view
{
    using capability_inquiry_view::capability_inquiry_view;

    uint7_t        status() const;
    uint14_t       information_data_length() const;
    const uint7_t* information_data() const;

    static bool validate(const sysex7&);

    struct field_offsets;
};

message make_endpoint_information_reply(
  muid_t source_muid, muid_t destination_muid, uint7_t status, const uint7_t* data, size_t data_size);

//! ACK/NAK helpers
namespace nack {
    //! base class for ACK/NAK view
    struct view : capability_inquiry_view
    {
        using capability_inquiry_view::capability_inquiry_view;

        uint7_t          transaction() const;
        uint7_t          status_code() const;
        uint7_t          status_data() const;
        const uint7_t*   details() const;
        uint14_t         message_length() const;
        const uint7_t*   message_data() const;
        std::string_view message() const;

        struct field_offsets;
    };
} // namespace nack

//---- subtype::ack

//! ack message view
struct ack_view : nack::view
{
    using nack::view::view;

    static bool validate(const sysex7&);
};

message make_ack_message(muid_t    source_muid,
                         muid_t    destination_muid,
                         uint7_t   device_id,
                         subtype_t transaction,
                         uint7_t   status_code,
                         uint7_t   status_data,
                         uint7_t   details[5],
                         const std::string_view&);

//---- subtype::invalidate_muid

//! invalidate MUID view
struct invalidate_muid_view : capability_inquiry_view
{
    using capability_inquiry_view::capability_inquiry_view;

    static bool validate(const sysex7&);
};

message make_invalidate_muid_message(muid_t);

//---- subtype::nak

//! nak message view
struct nak_view : nack::view
{
    using nack::view::view;

    static bool validate(const sysex7&);
};

message make_nak_message(muid_t    source_muid,
                         muid_t    destination_muid,
                         uint7_t   device_id,
                         subtype_t transaction,
                         uint7_t   status_code,
                         uint7_t   status_data,
                         uint7_t   details[5],
                         const std::string_view&);
message make_nak_message(const capability_inquiry_view& r,
                         uint7_t                        status_code,
                         uint7_t                        status_data,
                         uint7_t                        details[5],
                         const std::string_view&);

message make_nak_message_v1(muid_t source_muid, muid_t destination_muid, uint7_t device_id);
message make_nak_message_v1(const capability_inquiry_view&);

//-----------------------------------------------
//
//             PROFILE CONFIGURATION
//
//-----------------------------------------------

namespace subtype {
    // Profile Configuration Messages
    constexpr subtype_t profile_inquiry         = 0x20; //!< Profile Inquiry
    constexpr subtype_t profile_inquiry_reply   = 0x21; //!< Responder Reply to Profile Inquiry
    constexpr subtype_t set_profile_on          = 0x22; //!< Set Profile On
    constexpr subtype_t set_profile_off         = 0x23; //!< Set Profile Off
    constexpr subtype_t profile_enabled         = 0x24; //!< Profile Enabled Report
    constexpr subtype_t profile_disabled        = 0x25; //!< Profile Disabled Report
    constexpr subtype_t profile_added           = 0x26; //!< Profile Added Report
    constexpr subtype_t profile_removed         = 0x27; //!< Profile Removed Report
    constexpr subtype_t profile_details_inquiry = 0x28; //!< Profile Details Inquiry
    constexpr subtype_t profile_details_reply   = 0x29; //!< Profile Details Reply
    constexpr subtype_t profile_specific_data   = 0x2F; //!< Profile Specific Data
} // namespace subtype

//-----------------------------------------------

#pragma pack(push, 1)
//! Capability Inquiry Profile ID
struct profile_id
{
    uint7_t byte1{ 0x7E }; //!< 0x7E: Standard Defined, or Manufacturer SysEx ID 1
    uint7_t byte2{ 0x00 }; //!< Profile Number MSB, or Manufacturer SysEx ID 2
    uint7_t byte3{ 0x00 }; //!< Profile Number LSB, or Manufacturer SysEx ID 3
    uint7_t byte4{ 0x00 }; //!< Profile Version, or Manufacturer Specific Info
    uint7_t byte5{ 0x00 }; //!< Profile Level, or Manufacturer Specific Info

    profile_id() = default;
    profile_id(uint7_t b1, uint7_t b2, uint7_t b3, uint7_t b4, uint7_t b5);
};
#pragma pack(pop)

//---- subtype::profile_inquiry

//! Profile Inquiry view
struct profile_inquiry_view : capability_inquiry_view
{
    using capability_inquiry_view::capability_inquiry_view;

    static bool validate(const sysex7&);
};

message make_profile_inquiry_message(muid_t source_muid, muid_t destination_muid, uint7_t device_id = 0x7F);

//---- subtype::profile_inquiry_reply

//! Profile Inquiry Reply view
struct profile_inquiry_reply_view : capability_inquiry_view
{
    using capability_inquiry_view::capability_inquiry_view;

    bool has_enabled_profiles() const;
    bool has_disabled_profiles() const;

    uint14_t num_enabled_profiles() const;
    uint14_t num_disabled_profiles() const;

    std::vector<profile_id> enabled_profiles() const;
    std::vector<profile_id> disabled_profiles() const;

    static bool validate(const sysex7& sx);

    struct field_offsets;

  protected:
    size_t                  offset_of_disabled_profiles() const;
    std::vector<profile_id> make_profiles(size_t pos) const;
};

message make_profile_inquiry_reply(muid_t            source_muid,
                                   muid_t            destination_muid,
                                   const profile_id* enabled_profiles,
                                   uint14_t          num_enabled_profiles,
                                   const profile_id* disabled_profiles,
                                   uint14_t          num_disabled_profiles,
                                   uint7_t           device_id = 0x7F);
message make_profile_inquiry_reply(muid_t                         source_muid,
                                   muid_t                         destination_muid,
                                   const std::vector<profile_id>& enabled_profiles,
                                   const std::vector<profile_id>& disabled_profiles,
                                   uint7_t                        device_id = 0x7F);

//----
//! view class for Profile messages carrying one ID (On/Off/Enabled/Disabled/ProfileSpecificData)
struct profile_id_view : capability_inquiry_view
{
    using capability_inquiry_view::capability_inquiry_view;

    profile_id profile() const;

    static bool validate(const sysex7& sx);

    struct field_offsets;
};

//---- subtype::set_profile_on

message make_profile_on_request(muid_t source_muid,
                                muid_t destination_muid,
                                const profile_id&,
                                uint7_t device_id = 0x7F);

//---- subtype::set_profile_off

message make_profile_off_request(muid_t source_muid,
                                 muid_t destination_muid,
                                 const profile_id&,
                                 uint7_t device_id = 0x7F);

//---- subtype::profile_enabled

message make_profile_enabled_notification(muid_t source_muid,
                                          muid_t destination_muid,
                                          const profile_id&,
                                          uint7_t device_id = 0x7F);

//---- subtype::profile_disabled

message make_profile_disabled_notification(muid_t source_muid,
                                           muid_t destination_muid,
                                           const profile_id&,
                                           uint7_t device_id = 0x7F);

//---- subtype::profile_added

message make_profile_added_notification(muid_t source_muid,
                                        muid_t destination_muid,
                                        const profile_id&,
                                        uint7_t device_id = 0x7F);

//---- subtype::profile_removed

message make_profile_removed_notification(muid_t source_muid,
                                          muid_t destination_muid,
                                          const profile_id&,
                                          uint7_t device_id = 0x7F);

//---- subtype::profile_details_inquiry

struct profile_details_inquiry_view : capability_inquiry_view
{
    using capability_inquiry_view::capability_inquiry_view;

    profile_id profile() const;
    uint7_t    target() const;

    static bool validate(const sysex7&);

    struct field_offsets;
};

message make_profile_details_inquiry(
  muid_t source_muid, muid_t destination_muid, const profile_id&, uint7_t target, uint7_t device_id = 0x7F);

//---- subtype::profile_details_reply

struct profile_details_reply_view : capability_inquiry_view
{
    using capability_inquiry_view::capability_inquiry_view;

    profile_id     profile() const;
    uint7_t        target() const;
    uint14_t       target_data_length() const;
    const uint7_t* target_data() const;

    static bool validate(const sysex7&);

    struct field_offsets;
};

message make_profile_details_reply(muid_t source_muid,
                                   muid_t destination_muid,
                                   const profile_id&,
                                   uint7_t        target,
                                   const uint7_t* data,
                                   size_t         data_size,
                                   uint7_t        device_id = 0x7F);

//---- subtype::profile_specific_data

struct profile_specific_data_view : profile_id_view
{
    using profile_id_view::profile_id_view;

    const uint7_t* data_begin() const;
    const uint7_t* data_end() const;
    size_t         data_size() const;

    static bool validate(const sysex7&);

    struct field_offsets;
};

message make_profile_specific_data_message(muid_t source_muid,
                                           muid_t destination_muid,
                                           const profile_id&,
                                           const uint7_t* data,
                                           size_t         data_size,
                                           uint7_t        device_id = 0x7F);

message make_profile_specific_data_message(muid_t source_muid,
                                           muid_t destination_muid,
                                           const profile_id&,
                                           const std::vector<uint7_t>& data,
                                           uint7_t                     device_id = 0x7F);

//-----------------------------------------------
//
//               PROPERTY EXCHANGE
//
//-----------------------------------------------

namespace subtype {
    // Property Exchange Messages
    constexpr subtype_t property_exchange_capabilities_inquiry = 0x30; //!< Inquiry Property Exchange Capabilities
    constexpr subtype_t property_exchange_capabilities_reply   = 0x31; //!< Reply to Property Exchange Capabilities
    constexpr subtype_t _pe_reserved_1_                        = 0x32; //!< former Inquiry Has Property, reserved
    constexpr subtype_t _pe_reserved_2_                        = 0x33; //!< former Reply to Has Property, reserved
    constexpr subtype_t get_property_data_inquiry              = 0x34; //!< Inquiry Get Property Data
    constexpr subtype_t get_property_data_reply                = 0x35; //!< Reply to Get Property Data
    constexpr subtype_t set_property_data_inquiry              = 0x36; //!< Inquiry Set Property
    constexpr subtype_t set_property_data_reply                = 0x37; //!< Reply to Set Property
    constexpr subtype_t subscription_inquiry                   = 0x38; //!< Inquiry Subscription
    constexpr subtype_t subscription_reply                     = 0x39; //!< Reply to Subscription
    constexpr subtype_t notify                                 = 0x3F; //!< Notify message
} // namespace subtype

//! Property Exchange types and constants
namespace property_exchange {
    constexpr uint7_t version_major = 0x00;
    constexpr uint7_t version_minor = 0x00;

    struct header;
    using header_options = std::vector<std::pair<std::string_view, std::string_view>>;
    struct chunk;

    //! header JSON tags
    struct tags
    {
        static constexpr std::string_view resource{ "resource" };
        static constexpr std::string_view command{ "command" };
        static constexpr std::string_view status{ "status" };
        static constexpr std::string_view id{ "id" };
        static constexpr std::string_view offset{ "offset" };
        static constexpr std::string_view limit{ "limit" };
        static constexpr std::string_view encoding{ "encoding" };
        static constexpr std::string_view message{ "message" };
        static constexpr std::string_view subscribeId{ "subscribeId" };
    };

    std::string make_rjson(std::string_view key, std::string_view value);
    std::string make_rjson(std::string_view key, int value);
    std::string make_rjson(std::string_view key, std::string_view value, const header_options& options);
    std::string make_rjson(std::string_view key, int value, const header_options& options);

    // common resource names
    // Foundational Resources
    inline constexpr std::string_view ResourceList{ "ResourceList" };
    inline constexpr std::string_view DeviceInfo{ "DeviceInfo" };
    inline constexpr std::string_view ChannelList{ "ChannelList" };
    inline constexpr std::string_view JSONSchema{ "JSONSchema" };
    // Basic Resources
    inline constexpr std::string_view ModeList{ "ModeList" };
    inline constexpr std::string_view CurrentMode{ "CurrentMode" };
    inline constexpr std::string_view ProgramList{ "ProgramList" };

    // Basic Simple Resources
    inline constexpr std::string_view ClockMode{ "ClockMode" };
    inline constexpr std::string_view LocalOn{ "LocalOn" };
    inline constexpr std::string_view ChannelMode{ "ChannelMode" };
    inline constexpr std::string_view BasicChannelRx{ "BasicChannelRx" };
    inline constexpr std::string_view BasicChannelTx{ "BasicChannelTx" };
    inline constexpr std::string_view MaxSysex8Streams{ "MaxSysex8Streams" };

    // Other Resources
    inline constexpr std::string_view StateList{ "StateList" };
    inline constexpr std::string_view State{ "State" };

    // Controller Resources
    inline constexpr std::string_view AllCtrlList{ "AllCtrlList" };
    inline constexpr std::string_view ChCtrlList{ "ChCtrlList" };
    inline constexpr std::string_view CtrlMapList{ "CtrlMapList" };

    struct property_data_message_view : capability_inquiry_view
    {
        using capability_inquiry_view::capability_inquiry_view;

        uint7_t request_id() const;

        const uint7_t* header_begin() const;
        const uint7_t* header_end() const;
        size_t         header_size() const;

        uint14_t number_of_chunks() const;
        uint14_t number_of_this_chunk() const;

        const uint7_t* chunk_begin() const;
        const uint7_t* chunk_end() const;
        size_t         chunk_size() const;

        static bool validate(const sysex7&);

        struct field_offsets;
    };
} // namespace property_exchange

//---- subtype::property_exchange_capabilities_inquiry

struct property_exchange_capabilities_view : capability_inquiry_view
{
    using capability_inquiry_view::capability_inquiry_view;

    uint7_t maximum_number_of_requests() const;
    uint7_t pe_version_major() const;
    uint7_t pe_version_minor() const;

    static bool validate(const sysex7&);

    struct field_offsets;
};

message make_property_exchange_capabilities_inquiry(muid_t  source_muid,
                                                    muid_t  destination_muid,
                                                    uint7_t max_num_requests = 1,
                                                    uint7_t device_id        = 0x7F);

//---- subtype::property_exchange_capabilities_reply

message make_property_exchange_capabilities_reply(muid_t  source_muid,
                                                  muid_t  destination_muid,
                                                  uint7_t max_num_requests = 1,
                                                  uint7_t device_id        = 0x7F);

//---- subtype::get_property_data_inquiry

struct get_property_data_view : property_exchange::property_data_message_view
{
    using property_exchange::property_data_message_view::property_data_message_view;

    static bool validate(const sysex7&);
};

message make_get_property_data_inquiry(muid_t           source_muid,
                                       muid_t           destination_muid,
                                       std::string_view resource,
                                       uint7_t          request_id = 0,
                                       uint7_t          device_id  = 0x7F);

message make_get_property_data_inquiry(muid_t           source_muid,
                                       muid_t           destination_muid,
                                       std::string_view resource,
                                       const property_exchange::header_options&,
                                       uint7_t request_id = 0,
                                       uint7_t device_id  = 0x7F);

//---- subtype::get_property_data_reply

message make_get_property_data_reply(muid_t source_muid,
                                     muid_t destination_muid,
                                     const property_exchange::header&,
                                     uint14_t number_of_chunks,
                                     uint14_t number_of_this_chunk,
                                     const property_exchange::chunk&,
                                     uint7_t request_id = 0,
                                     uint7_t device_id  = 0x7F);

message make_get_property_data_reply(muid_t                          source_muid,
                                     muid_t                          destination_muid,
                                     int                             status,
                                     uint14_t                        number_of_chunks,
                                     uint14_t                        number_of_this_chunk,
                                     const property_exchange::chunk& chunk,
                                     uint7_t                         request_id = 0,
                                     uint7_t                         device_id  = 0x7F);

message make_get_property_data_reply(muid_t                          source_muid,
                                     muid_t                          destination_muid,
                                     int                             status,
                                     std::string_view                message,
                                     uint14_t                        number_of_chunks,
                                     uint14_t                        number_of_this_chunk,
                                     const property_exchange::chunk& chunk,
                                     uint7_t                         request_id = 0,
                                     uint7_t                         device_id  = 0x7F);

message make_get_property_data_reply(muid_t                          source_muid,
                                     muid_t                          destination_muid,
                                     uint14_t                        number_of_chunks,
                                     uint14_t                        number_of_this_chunk,
                                     const property_exchange::chunk& chunk,
                                     uint7_t                         request_id = 0,
                                     uint7_t                         device_id  = 0x7F);

//---- subtype::set_property_data_inquiry

struct set_property_data_view : property_exchange::property_data_message_view
{
    using property_exchange::property_data_message_view::property_data_message_view;

    static bool validate(const sysex7&);
};

message make_set_property_data_inquiry(muid_t           source_muid,
                                       muid_t           destination_muid,
                                       std::string_view resource,
                                       uint14_t         number_of_chunks,
                                       uint14_t         number_of_this_chunk,
                                       const property_exchange::chunk&,
                                       uint7_t request_id = 0,
                                       uint7_t device_id  = 0x7F);

message make_set_property_data_inquiry(muid_t                                   source_muid,
                                       muid_t                                   destination_muid,
                                       std::string_view                         resource,
                                       const property_exchange::header_options& options,
                                       uint14_t                                 number_of_chunks,
                                       uint14_t                                 number_of_this_chunk,
                                       const property_exchange::chunk&          chunk,
                                       uint7_t                                  request_id = 0,
                                       uint7_t                                  device_id  = 0x7F);

message make_set_property_data_inquiry(muid_t                          source_muid,
                                       muid_t                          destination_muid,
                                       uint14_t                        number_of_chunks,
                                       uint14_t                        number_of_this_chunk,
                                       const property_exchange::chunk& chunk,
                                       uint7_t                         request_id = 0,
                                       uint7_t                         device_id  = 0x7F);

//---- subtype::set_property_data_reply

message make_set_property_data_reply(
  muid_t source_muid, muid_t destination_muid, int status, uint7_t request_id = 0, uint7_t device_id = 0x7F);

message make_set_property_data_reply(muid_t           source_muid,
                                     muid_t           destination_muid,
                                     int              status,
                                     std::string_view message,
                                     uint7_t          request_id = 0,
                                     uint7_t          device_id  = 0x7F);

//---- subtype::subscription_inquiry

struct subscription_view : property_exchange::property_data_message_view
{
    using property_exchange::property_data_message_view::property_data_message_view;

    static bool validate(const sysex7&);
};

message make_subscription_inquiry(muid_t           source_muid,
                                  muid_t           destination_muid,
                                  std::string_view resource,
                                  std::string_view command,
                                  std::string_view subscribeId,
                                  uint7_t          request_id = 0,
                                  uint7_t          device_id  = 0x7F);

message make_subscription_inquiry(muid_t                          source_muid,
                                  muid_t                          destination_muid,
                                  std::string_view                resource,
                                  std::string_view                command,
                                  std::string_view                subscribeId,
                                  uint14_t                        number_of_chunks,
                                  uint14_t                        number_of_this_chunk,
                                  const property_exchange::chunk& chunk,
                                  uint7_t                         request_id = 0,
                                  uint7_t                         device_id  = 0x7F);

message make_subscription_inquiry(muid_t                          source_muid,
                                  muid_t                          destination_muid,
                                  uint14_t                        number_of_chunks,
                                  uint14_t                        number_of_this_chunk,
                                  const property_exchange::chunk& chunk,
                                  uint7_t                         request_id = 0,
                                  uint7_t                         device_id  = 0x7F);

//---- subtype::subscription_reply

message make_subscription_reply(
  muid_t source_muid, muid_t destination_muid, int status, uint7_t request_id = 0, uint7_t device_id = 0x7F);

message make_subscription_reply(muid_t           source_muid,
                                muid_t           destination_muid,
                                int              status,
                                std::string_view message,
                                uint7_t          request_id = 0,
                                uint7_t          device_id  = 0x7F);

//---- subtype::notify

struct notify_view : property_exchange::property_data_message_view
{
    using property_exchange::property_data_message_view::property_data_message_view;

    static bool validate(const sysex7&);
};

message make_notify_message(
  muid_t source_muid, muid_t destination_muid, int status, uint7_t request_id = 0, uint7_t device_id = 0x7F);

message make_notify_message(muid_t           source_muid,
                            muid_t           destination_muid,
                            int              status,
                            std::string_view message,
                            uint7_t          request_id = 0,
                            uint7_t          device_id  = 0x7F);

//-----------------------------------------------
//
//               PROCESS INQUIRY
//
//-----------------------------------------------

namespace subtype {
    // Process Inquiry Messages
    constexpr subtype_t process_inquiry_capabilities_inquiry = 0x40;
    constexpr subtype_t process_inquiry_capabilities_reply   = 0x41;
    constexpr subtype_t midi_message_report_inquiry          = 0x42;
    constexpr subtype_t midi_message_report_reply            = 0x43;
    constexpr subtype_t midi_message_report_end              = 0x44;
} // namespace subtype

//---- subtype::process_inquiry_capabilities_inquiry

message make_process_inquiry_capabilities_inquiry(muid_t  source_muid,
                                                  muid_t  destination_muid,
                                                  uint7_t device_id = 0x7F);

//---- subtype::process_inquiry_capabilities_reply

struct process_inquiry_capabilities_reply_view : capability_inquiry_view
{
    using capability_inquiry_view::capability_inquiry_view;

    uint7_t supported_features() const;

    static bool validate(const sysex7&);

    struct field_offsets;
};

message make_process_inquiry_capabilities_reply(muid_t  source_muid,
                                                muid_t  destination_muid,
                                                uint7_t features,
                                                uint7_t device_id = 0x7F);

//---- subtype::midi_message_report_inquiry

struct midi_message_report_inquiry_view : capability_inquiry_view
{
    using capability_inquiry_view::capability_inquiry_view;

    uint7_t message_data_control() const;
    uint7_t system_message_types() const;
    uint7_t channel_controller_message_types() const;
    uint7_t note_data_message_types() const;

    static bool validate(const sysex7&);

    struct field_offsets;
};

message make_midi_message_report_inquiry(muid_t  source_muid,
                                         muid_t  destination_muid,
                                         uint7_t data_control,
                                         uint7_t system_messages,
                                         uint7_t channel_controller_messages,
                                         uint7_t note_data_messages,
                                         uint7_t device_id = 0x7F);

//---- subtype::midi_message_report_reply

struct midi_message_report_reply_view : capability_inquiry_view
{
    using capability_inquiry_view::capability_inquiry_view;

    uint7_t system_message_types() const;
    uint7_t channel_controller_message_types() const;
    uint7_t note_data_message_types() const;

    static bool validate(const sysex7&);

    struct field_offsets;
};

message make_midi_message_report_reply(muid_t  source_muid,
                                       uint7_t system_messages,
                                       uint7_t channel_controller_messages,
                                       uint7_t note_data_messages,
                                       uint7_t device_id = 0x7F);

//---- subtype::midi_message_report_end

message make_midi_message_report_end(muid_t source_muid, uint7_t device_id = 0x7F);

//-----------------------------------------------
//
//            IMPLEMENTATION SECTION
//
//-----------------------------------------------

template<typename view>
inline std::optional<view> as(const sysex7& sx)
{
    if (view::validate(sx))
        return view{ sx };
    return {};
}

template<typename view>
std::optional<view> as(const capability_inquiry_view& v)
{
    const sysex7& sx7 = v.sx;
    return as<view>(sx7);
}

inline uint7_t message::message_version() const
{
    return data[3];
}

inline std::uint32_t message::source_muid() const
{
    return make_uint28(offset_of_smuid);
}

inline std::uint32_t message::destination_muid() const
{
    return make_uint28(offset_of_dmuid);
}

//-----------------------------------------------
//
//       MANAGEMENT MESSAGE implementations
//
//-----------------------------------------------

namespace discovery {

    inline message make_discovery_message_v1(subtype_t         subtype,
                                             muid_t            source_muid,
                                             muid_t            destination_muid,
                                             const identity_t& i,
                                             uint7_t           categories,
                                             uint28_t          max_message_size)
    {
        auto result = message::make_with_payload_size(16, subtype, source_muid, destination_muid);
        result.add_device_identity(i);
        result.add_uint7(categories);
        result.add_uint28(max_message_size);
        result.data[3] = message_version_1;
        return result;
    }

    inline message make_discovery_message(subtype_t         subtype,
                                          muid_t            source_muid,
                                          muid_t            destination_muid,
                                          const identity_t& i,
                                          uint7_t           categories,
                                          uint28_t          max_message_size,
                                          uint7_t           output_path_id)
    {
        auto result = message::make_with_payload_size(18, subtype, source_muid, destination_muid);
        result.add_device_identity(i);
        result.add_uint7(categories);
        result.add_uint28(max_message_size);
        result.add_uint7(output_path_id);
        return result;
    }
} // namespace discovery

struct discovery::message_view::field_offsets : capability_inquiry_view::field_offsets
{
    static constexpr auto identity             = payload;
    static constexpr auto categories           = payload + 11u;
    static constexpr auto maximum_message_size = payload + 12u;
    static constexpr auto output_path_id       = payload + 16u;
};

inline identity_t discovery::message_view::identity() const
{
    return sx.make_device_identity(field_offsets::identity);
}

inline uint7_t discovery::message_view::categories() const
{
    return sx.data[field_offsets::categories];
}

inline uint28_t discovery::message_view::maximum_message_size() const
{
    return sx.make_uint28(field_offsets::maximum_message_size);
}

inline uint7_t discovery::message_view::output_path_id() const
{
    return (message_version() >= message_version_2) ? sx.data[field_offsets::output_path_id] : 0;
}

inline bool discovery_inquiry_view::validate(const sysex7& sx)
{
    return (sx.manufacturerID == manufacturer::universal_non_realtime) &&
           (sx.data.size() >= field_offsets::output_path_id + ((sx.data[3] > 1) ? 1u : 0u)) && (sx.data[1] == 0x0D) &&
           (sx.data[2] == subtype::discovery_inquiry);
}

inline message make_discovery_inquiry_v1(muid_t            source_muid,
                                         const identity_t& i,
                                         uint7_t           categories,
                                         uint28_t          max_message_size)
{
    return discovery::make_discovery_message_v1(
      subtype::discovery_inquiry, source_muid, broadcast_muid, i, categories, max_message_size);
}

inline message make_discovery_inquiry(
  muid_t source_muid, const identity_t& i, uint7_t categories, uint28_t max_message_size, uint7_t output_path_id)
{
    return discovery::make_discovery_message(
      subtype::discovery_inquiry, source_muid, broadcast_muid, i, categories, max_message_size, output_path_id);
}

//---- subtype::discovery_reply

struct discovery_reply_view::field_offsets : discovery::message_view::field_offsets
{
    static constexpr auto function_block = payload + 17u;
};

inline uint7_t discovery_reply_view::function_block() const
{
    return (message_version() >= message_version_2) ? sx.data[field_offsets::function_block] : 0x7F;
}

inline bool discovery_reply_view::validate(const sysex7& sx)
{
    return (sx.manufacturerID == manufacturer::universal_non_realtime) &&
           (sx.data.size() >= field_offsets::output_path_id + ((sx.data[3] > 1) ? 2u : 0u)) && (sx.data[1] == 0x0D) &&
           (sx.data[2] == subtype::discovery_reply);
}

inline message make_discovery_reply_v1(
  muid_t source_muid, muid_t destination_muid, const identity_t& i, uint7_t categories, uint28_t max_message_size)
{
    return discovery::make_discovery_message_v1(
      subtype::discovery_reply, source_muid, destination_muid, i, categories, max_message_size);
}

inline message make_discovery_reply(muid_t            source_muid,
                                    muid_t            destination_muid,
                                    const identity_t& i,
                                    uint7_t           categories,
                                    uint28_t          max_message_size,
                                    uint7_t           output_path_id,
                                    uint7_t           function_block)
{
    auto result = discovery::make_discovery_message(
      subtype::discovery_reply, source_muid, destination_muid, i, categories, max_message_size, output_path_id);
    result.add_uint7(function_block);
    return result;
}

//---- subtype::endpoint_information_inquiry

struct endpoint_information_inquiry_view::field_offsets : capability_inquiry_view::field_offsets
{
    static constexpr auto status = payload;
};

inline uint7_t endpoint_information_inquiry_view::status() const
{
    return sx.data[field_offsets::status];
}

inline bool endpoint_information_inquiry_view::validate(const sysex7& sx)
{
    return (sx.manufacturerID == manufacturer::universal_non_realtime) &&
           (sx.data.size() >= field_offsets::payload + 1u) && (sx.data[1] == 0x0D) &&
           (sx.data[2] == subtype::endpoint_information_inquiry);
}

inline message make_endpoint_information_inquiry(muid_t source_muid, muid_t destination_muid, uint7_t status)
{
    auto result =
      message::make_with_payload_size(1, subtype::endpoint_information_inquiry, source_muid, destination_muid);
    result.add_uint7(status);
    return result;
}

//---- subtype::endpoint_information_reply

struct endpoint_information_reply_view::field_offsets : capability_inquiry_view::field_offsets
{
    static constexpr auto status                  = payload;
    static constexpr auto information_data_length = payload + 1u;
    static constexpr auto information_data        = payload + 3u;
};

inline uint7_t endpoint_information_reply_view::status() const
{
    return sx.data[field_offsets::status];
}

inline uint14_t endpoint_information_reply_view::information_data_length() const
{
    return sx.make_uint14(field_offsets::information_data_length);
}

inline const uint7_t* endpoint_information_reply_view::information_data() const
{
    return sx.data.data() + field_offsets::information_data;
}

inline bool endpoint_information_reply_view::validate(const sysex7& sx)
{
    return (sx.manufacturerID == manufacturer::universal_non_realtime) &&
           (sx.data.size() >= field_offsets::information_data) &&
           (sx.data.size() >=
            field_offsets::information_data + sx.make_uint14(field_offsets::information_data_length)) &&
           (sx.data[1] == 0x0D) && (sx.data[2] == subtype::endpoint_information_reply);
}

inline message make_endpoint_information_reply(
  muid_t source_muid, muid_t destination_muid, uint7_t status, const uint7_t* data, size_t data_size)
{
    auto m = message::make_with_payload_size(
      3 + data_size, subtype::endpoint_information_reply, source_muid, destination_muid);
    m.add_uint7(status);
    m.add_uint14(static_cast<uint14_t>(data_size));
    m.data.insert(m.data.end(), data, data + data_size);
    return m;
}

//---- nack::view

struct nack::view::field_offsets : capability_inquiry_view::field_offsets
{
    static constexpr auto transaction    = payload;
    static constexpr auto status_code    = payload + 1u;
    static constexpr auto status_data    = payload + 2u;
    static constexpr auto details        = payload + 3u;
    static constexpr auto message_length = payload + 8u;
    static constexpr auto message_data   = payload + 10u;
};

inline uint7_t nack::view::transaction() const
{
    return sx.data[field_offsets::transaction];
}

inline uint7_t nack::view::status_code() const
{
    return sx.data[field_offsets::status_code];
}

inline uint7_t nack::view::status_data() const
{
    return sx.data[field_offsets::status_data];
}

inline const uint7_t* nack::view::details() const
{
    return sx.data.data() + field_offsets::details;
}

inline uint14_t nack::view::message_length() const
{
    return sx.make_uint14(field_offsets::message_length);
}

inline const uint7_t* nack::view::message_data() const
{
    return sx.data.data() + field_offsets::message_data;
}

inline std::string_view nack::view::message() const
{
    return std::string_view{ (const char*)message_data(), size_t(message_length()) };
}

//---- subtype::ack

inline bool ack_view::validate(const sysex7& sx)
{
    return (sx.manufacturerID == manufacturer::universal_non_realtime) &&
           (sx.data.size() >= field_offsets::message_data) &&
           (sx.data.size() >= field_offsets::message_data + sx.make_uint14(field_offsets::message_length)) &&
           (sx.data[1] == 0x0D) && (sx.data[2] == subtype::ack);
}

//! ACK message
inline message make_ack_message(muid_t                  source_muid,
                                muid_t                  destination_muid,
                                uint7_t                 device_id,
                                subtype_t               transaction,
                                uint7_t                 status_code,
                                uint7_t                 status_data,
                                uint7_t                 details[5],
                                const std::string_view& msg)
{
    uint7_t d[] = { transaction, status_code, status_data, details[0], details[1], details[2], details[3], details[4] };
    auto m = message::make_with_payload_size(10 + msg.size(), subtype::ack, source_muid, destination_muid, device_id);
    m.data.insert(m.data.end(), d, d + sizeof(d));
    m.add_uint14(static_cast<uint14_t>(msg.size()));
    m.data.insert(m.data.end(), msg.data(), msg.data() + msg.size());
    return m;
}

//---- subtype::nak

inline bool nak_view::validate(const sysex7& sx)
{
    return (sx.manufacturerID == manufacturer::universal_non_realtime) &&
           (sx.data.size() >= field_offsets::message_data) &&
           (sx.data.size() >= field_offsets::message_data + sx.make_uint14(field_offsets::message_length)) &&
           (sx.data[1] == 0x0D) && (sx.data[2] == subtype::nak) && (sx.data[3] >= message_version_2);
}

inline message make_nak_message_v1(muid_t source_muid, muid_t destination_muid, uint7_t device_id)
{
    auto result    = message{ subtype::nak, source_muid, destination_muid, device_id };
    result.data[3] = message_version_1;
    return result;
}

inline message make_nak_message_v1(const capability_inquiry_view& r)
{
    auto result    = message{ subtype::nak, r.destination_muid(), r.source_muid(), r.device_id() };
    result.data[3] = message_version_1;
    return result;
}

inline message make_nak_message(muid_t                  source_muid,
                                muid_t                  destination_muid,
                                uint7_t                 device_id,
                                subtype_t               transaction,
                                uint7_t                 status_code,
                                uint7_t                 status_data,
                                uint7_t                 details[5],
                                const std::string_view& msg)
{
    uint7_t d[] = { transaction, status_code, status_data, details[0], details[1], details[2], details[3], details[4] };
    auto m = message::make_with_payload_size(10 + msg.size(), subtype::nak, source_muid, destination_muid, device_id);
    m.data.insert(m.data.end(), d, d + sizeof(d));
    m.add_uint14(static_cast<uint14_t>(msg.size()));
    m.data.insert(m.data.end(), msg.data(), msg.data() + msg.size());
    return m;
}

inline message make_nak_message(const capability_inquiry_view& r,
                                uint7_t                        status_code,
                                uint7_t                        status_data,
                                uint7_t                        details[5],
                                const std::string_view&        msg)
{
    uint7_t d[] = { r.subtype(), status_code, status_data, details[0], details[1], details[2], details[3], details[4] };
    auto    m   = message::make_with_payload_size(
      10 + msg.size(), subtype::nak, r.destination_muid(), r.source_muid(), r.device_id());
    m.data.insert(m.data.end(), d, d + sizeof(d));
    m.add_uint14(static_cast<uint14_t>(msg.size()));
    m.data.insert(m.data.end(), msg.data(), msg.data() + msg.size());
    return m;
}

//---- subtype::invalidate_muid

inline bool invalidate_muid_view::validate(const sysex7& sx)
{
    return ((universal_sysex_type_of(sx) == universal_sysex::type::capability_inquiry) &&
            (universal_sysex_subtype_of(sx) == subtype::invalidate_muid));
}

inline message make_invalidate_muid_message(muid_t MUID)
{
    return message(subtype::invalidate_muid, MUID, broadcast_muid, 0x7F);
}

//-----------------------------------------------
//
//     PROFILE CONFIGURATION implementations
//
//-----------------------------------------------

//---- profile_id

inline profile_id::profile_id(uint7_t b1, uint7_t b2, uint7_t b3, uint7_t b4, uint7_t b5)
  : byte1(b1)
  , byte2(b2)
  , byte3(b3)
  , byte4(b4)
  , byte5(b5)
{
}

inline bool operator==(const profile_id& a, const profile_id& b)
{
    return (a.byte1 == b.byte1) && (a.byte2 == b.byte2) && (a.byte3 == b.byte3) && (a.byte4 == b.byte4) &&
           (a.byte5 == b.byte5);
}

//---- subtype::profile_inquiry

inline bool profile_inquiry_view::validate(const sysex7& sx)
{
    return ((universal_sysex_type_of(sx) == universal_sysex::type::capability_inquiry) &&
            (universal_sysex_subtype_of(sx) == subtype::profile_inquiry) && (sx.data.size() >= field_offsets::payload));
}

inline message make_profile_inquiry_message(muid_t source_muid, muid_t destination_muid, uint7_t device_id)
{
    return message(subtype::profile_inquiry, source_muid, destination_muid, device_id);
}

//---- subtype::profile_inquiry_reply

struct profile_inquiry_reply_view::field_offsets : capability_inquiry_view::field_offsets
{
    static constexpr auto num_enabled_profiles  = payload;
    static constexpr auto num_disabled_profiles = payload + 2u;
    static constexpr auto minimum_message_size  = payload + 4u;
};

inline size_t profile_inquiry_reply_view::offset_of_disabled_profiles() const
{
    return field_offsets::num_disabled_profiles +
           sx.make_uint14(field_offsets::num_enabled_profiles) * sizeof(profile_id);
}

inline bool profile_inquiry_reply_view::has_enabled_profiles() const
{
    return ((sx.data[field_offsets::num_enabled_profiles] | sx.data[field_offsets::num_enabled_profiles + 1]) & 0x7F) !=
           0;
}

inline bool profile_inquiry_reply_view::has_disabled_profiles() const
{
    const auto p = offset_of_disabled_profiles();
    return ((sx.data[p] | sx.data[p + 1]) & 0x7F) != 0;
}

inline std::uint16_t profile_inquiry_reply_view::num_enabled_profiles() const
{
    return sx.make_uint14(field_offsets::num_enabled_profiles);
}

inline std::uint16_t profile_inquiry_reply_view::num_disabled_profiles() const
{
    const auto p = offset_of_disabled_profiles();
    return sx.make_uint14(p);
}

inline std::vector<profile_id> profile_inquiry_reply_view::enabled_profiles() const
{
    return make_profiles(field_offsets::num_enabled_profiles);
}

inline std::vector<profile_id> profile_inquiry_reply_view::disabled_profiles() const
{
    return make_profiles(offset_of_disabled_profiles());
}

inline message make_profile_inquiry_reply(muid_t                         source_muid,
                                          muid_t                         destination_muid,
                                          const std::vector<profile_id>& enabled_profiles,
                                          const std::vector<profile_id>& disabled_profiles,
                                          uint7_t                        device_id)
{
    return make_profile_inquiry_reply(source_muid,
                                      destination_muid,
                                      enabled_profiles.data(),
                                      static_cast<uint14_t>(enabled_profiles.size()),
                                      disabled_profiles.data(),
                                      static_cast<uint14_t>(disabled_profiles.size()),
                                      device_id);
}

//---- profile_id_view

struct profile_id_view::field_offsets : capability_inquiry_view::field_offsets
{
    static constexpr auto profile_id = payload;
};

inline profile_id profile_id_view::profile() const
{
    return profile_id{ sx.data[field_offsets::profile_id],
                       sx.data[field_offsets::profile_id + 1],
                       sx.data[field_offsets::profile_id + 2],
                       sx.data[field_offsets::profile_id + 3],
                       sx.data[field_offsets::profile_id + 4] };
}

inline bool profile_id_view::validate(const sysex7& sx)
{
    return (sx.manufacturerID == manufacturer::universal_non_realtime) &&
           (sx.data.size() >= field_offsets::profile_id + 5u) && (sx.data[1] == 0x0D) &&
           (sx.data[2] >= subtype::set_profile_on) && (sx.data[2] <= subtype::profile_removed);
}

//---- subtype::set_profile_on

inline message make_profile_on_request(muid_t            source_muid,
                                       muid_t            destination_muid,
                                       const profile_id& p,
                                       uint7_t           device_id)
{
    auto result = message::make_with_payload_size(5, subtype::set_profile_on, source_muid, destination_muid, device_id);
    result.add_data(&p.byte1, 5);
    return result;
}

//---- subtype::set_profile_off

inline message make_profile_off_request(muid_t            source_muid,
                                        muid_t            destination_muid,
                                        const profile_id& p,
                                        uint7_t           device_id)
{
    auto result =
      message::make_with_payload_size(5, subtype::set_profile_off, source_muid, destination_muid, device_id);
    result.add_data(&p.byte1, 5);
    return result;
}

//---- subtype::profile_enabled

inline message make_profile_enabled_notification(muid_t            source_muid,
                                                 muid_t            destination_muid,
                                                 const profile_id& p,
                                                 uint7_t           device_id)
{
    auto result =
      message::make_with_payload_size(5, subtype::profile_enabled, source_muid, destination_muid, device_id);
    result.add_data(&p.byte1, 5);
    return result;
}

//---- subtype::profile_disabled

inline message make_profile_disabled_notification(muid_t            source_muid,
                                                  muid_t            destination_muid,
                                                  const profile_id& p,
                                                  uint7_t           device_id)
{
    auto result =
      message::make_with_payload_size(5, subtype::profile_disabled, source_muid, destination_muid, device_id);
    result.add_data(&p.byte1, 5);
    return result;
}

//---- subtype::profile_added

inline message make_profile_added_notification(muid_t            source_muid,
                                               muid_t            destination_muid,
                                               const profile_id& p,
                                               uint7_t           device_id)
{
    auto result = message::make_with_payload_size(5, subtype::profile_added, source_muid, destination_muid, device_id);
    result.add_data(&p.byte1, 5);
    return result;
}

//---- subtype::profile_removed

inline message make_profile_removed_notification(muid_t            source_muid,
                                                 muid_t            destination_muid,
                                                 const profile_id& p,
                                                 uint7_t           device_id)
{
    auto result =
      message::make_with_payload_size(5, subtype::profile_removed, source_muid, destination_muid, device_id);
    result.add_data(&p.byte1, 5);
    return result;
}

//---- subtype::profile_details_inquiry

struct profile_details_inquiry_view::field_offsets : capability_inquiry_view::field_offsets
{
    static constexpr auto profile_id = payload;
    static constexpr auto target     = payload + 5;
};

inline profile_id profile_details_inquiry_view::profile() const
{
    return profile_id{ sx.data[field_offsets::profile_id],
                       sx.data[field_offsets::profile_id + 1],
                       sx.data[field_offsets::profile_id + 2],
                       sx.data[field_offsets::profile_id + 3],
                       sx.data[field_offsets::profile_id + 4] };
}

inline uint7_t profile_details_inquiry_view::target() const
{
    return sx.data[field_offsets::target];
}

inline bool profile_details_inquiry_view::validate(const sysex7& sx)
{
    return (sx.manufacturerID == manufacturer::universal_non_realtime) &&
           (sx.data.size() >= field_offsets::target + 1u) && (sx.data[1] == 0x0D) &&
           (sx.data[2] == subtype::profile_details_inquiry);
}

inline message make_profile_details_inquiry(
  muid_t source_muid, muid_t destination_muid, const profile_id& p, uint7_t target, uint7_t device_id)
{
    auto m =
      message::make_with_payload_size(6, subtype::profile_details_inquiry, source_muid, destination_muid, device_id);
    m.add_data(&p.byte1, 5);
    m.add_uint7(target);
    return m;
}

inline message make_profile_details_reply(muid_t            source_muid,
                                          muid_t            destination_muid,
                                          const profile_id& p,
                                          uint7_t           target,
                                          const uint7_t*    data,
                                          size_t            data_size,
                                          uint7_t           device_id)
{
    auto m = message::make_with_payload_size(
      8 + data_size, subtype::profile_details_reply, source_muid, destination_muid, device_id);
    m.add_data(&p.byte1, 5);
    m.add_uint7(target);
    m.add_uint14(static_cast<uint14_t>(data_size));
    m.data.insert(m.data.end(), data, data + data_size);
    return m;
}

//---- subtype::profile_details_inquiry

struct profile_details_reply_view::field_offsets : capability_inquiry_view::field_offsets
{
    static constexpr auto profile_id         = payload;
    static constexpr auto target             = payload + 5u;
    static constexpr auto target_data_length = payload + 6u;
    static constexpr auto target_data        = payload + 8u;
};

inline profile_id profile_details_reply_view::profile() const
{
    return profile_id{ sx.data[field_offsets::profile_id],
                       sx.data[field_offsets::profile_id + 1],
                       sx.data[field_offsets::profile_id + 2],
                       sx.data[field_offsets::profile_id + 3],
                       sx.data[field_offsets::profile_id + 4] };
}

inline uint7_t profile_details_reply_view::target() const
{
    return sx.data[field_offsets::target];
}

inline uint14_t profile_details_reply_view::target_data_length() const
{
    return sx.make_uint14(field_offsets::target_data_length);
}

inline const uint7_t* profile_details_reply_view::target_data() const
{
    return sx.data.data() + field_offsets::target_data;
}

inline bool profile_details_reply_view::validate(const sysex7& sx)
{
    return (sx.manufacturerID == manufacturer::universal_non_realtime) &&
           (sx.data.size() >= field_offsets::target_data) &&
           (sx.data.size() >= field_offsets::target_data + sx.make_uint14(field_offsets::target_data_length)) &&
           (sx.data[1] == 0x0D) && (sx.data[2] == subtype::profile_details_reply);
}

//---- subtype::profile_specific_data

struct profile_specific_data_view::field_offsets : capability_inquiry_view::field_offsets
{
    static constexpr auto profile_id = payload;
    static constexpr auto data_size  = payload + 5u;
    static constexpr auto data       = payload + 9u;
};

inline bool profile_specific_data_view::validate(const sysex7& sx)
{
    return (sx.manufacturerID == manufacturer::universal_non_realtime) && (sx.data.size() >= field_offsets::data) &&
           (sx.data.size() >= field_offsets::data + sx.make_uint28(field_offsets::data_size)) && (sx.data[1] == 0x0D) &&
           (sx.data[2] == subtype::profile_specific_data);
}

inline const uint7_t* profile_specific_data_view::data_begin() const
{
    return sx.data.data() + field_offsets::data;
}

inline const uint7_t* profile_specific_data_view::data_end() const
{
    return data_begin() + data_size();
}

inline size_t profile_specific_data_view::data_size() const
{
    return sx.make_uint28(field_offsets::data_size);
}

inline message make_profile_specific_data_message(
  muid_t source_muid, muid_t destination_muid, const profile_id& p, const std::vector<uint7_t>& data, uint7_t device_id)
{
    return make_profile_specific_data_message(
      source_muid, destination_muid, p, data.data(), static_cast<uint14_t>(data.size()), device_id);
}

//-----------------------------------------------
//
//       PROPERTY EXCHANGE implementations
//
//-----------------------------------------------

namespace property_exchange {

    struct _private_data_view
    {
        const uint7_t* data{ nullptr };
        size_t         size{ 0 };

        _private_data_view() = default;
        _private_data_view(const uint7_t* d, size_t s)
          : data(d)
          , size(s)
        {
        }
        _private_data_view(const char* d, size_t s)
          : _private_data_view(std::string_view{ d, s })
        {
        }
        explicit _private_data_view(const std::vector<uint7_t>& d)
          : data(d.data())
          , size(d.size())
        {
        }
        explicit _private_data_view(const std::string& d)
          : _private_data_view(std::string_view{ d })
        {
        }
        explicit _private_data_view(std::string_view d)
          : data(reinterpret_cast<const uint7_t*>(d.data()))
          , size(d.length())
        {
        }

        std::string_view as_string_view() const { return std::string_view(reinterpret_cast<const char*>(data), size); }
    };

    struct header : _private_data_view
    {
        using _private_data_view::_private_data_view;
    };

    struct chunk : _private_data_view
    {
        using _private_data_view::_private_data_view;
    };

    message make_capabilities_message(
      subtype_t subtype, muid_t source_muid, muid_t destination_muid, uint7_t max_num_requests, uint7_t device_id);

    message make_property_data_message(subtype_t     subtype,
                                       muid_t        source_muid,
                                       muid_t        destination_muid,
                                       const header& header,
                                       uint14_t      number_of_chunks,
                                       uint14_t      number_of_this_chunk,
                                       const chunk&  chunk,
                                       uint7_t       request_id = 0,
                                       uint7_t       device_id  = 0x7F);

} // namespace property_exchange

//---- property_exchange::make_rjson

inline std::string property_exchange::make_rjson(std::string_view key, std::string_view value)
{
    return std::string{ "{\"" } + std::string{ key } + "\":\"" + std::string{ value } + "\"}";
}

inline std::string property_exchange::make_rjson(std::string_view key, int value)
{
    return std::string{ "{\"" } + std::string{ key } + "\":" + std::to_string(value) + "}";
}

//---- property_exchange::property_data_message_view

struct property_exchange::property_data_message_view::field_offsets : capability_inquiry_view::field_offsets
{
    static constexpr auto request_id  = payload;
    static constexpr auto header_size = payload + 1u;
    static constexpr auto header_data = payload + 3u;
    static constexpr auto num_chunks  = payload + 3u; // eventually moved by header size
    static constexpr auto this_chunk  = payload + 5u; // eventually moved by header size
    static constexpr auto chunk_size  = payload + 7u; // eventually moved by header size
    static constexpr auto chunk_data  = payload + 9u; // eventually moved by header size
};

inline uint7_t property_exchange::property_data_message_view::request_id() const
{
    return sx.data[field_offsets::request_id];
}

inline const uint7_t* property_exchange::property_data_message_view::header_begin() const
{
    return sx.data.data() + field_offsets::header_data;
}

inline const uint7_t* property_exchange::property_data_message_view::header_end() const
{
    return header_begin() + header_size();
}

inline size_t property_exchange::property_data_message_view::header_size() const
{
    return sx.make_uint14(field_offsets::header_size);
}

inline std::uint16_t property_exchange::property_data_message_view::number_of_chunks() const
{
    return sx.make_uint14(field_offsets::num_chunks + header_size());
}

inline std::uint16_t property_exchange::property_data_message_view::number_of_this_chunk() const
{
    return sx.make_uint14(field_offsets::this_chunk + header_size());
}

inline const uint7_t* property_exchange::property_data_message_view::chunk_begin() const
{
    return sx.data.data() + field_offsets::chunk_data + header_size();
}

inline const uint7_t* property_exchange::property_data_message_view::chunk_end() const
{
    return chunk_begin() + chunk_size();
}

inline size_t property_exchange::property_data_message_view::chunk_size() const
{
    return sx.make_uint14(field_offsets::chunk_size + header_size());
}

//---- property_exchange_capabilities_view

struct property_exchange_capabilities_view::field_offsets : capability_inquiry_view::field_offsets
{
    static constexpr auto max_num_requests = payload;
    static constexpr auto pe_version_major = payload + 1u;
    static constexpr auto pe_version_minor = payload + 2u;
};

inline uint7_t property_exchange_capabilities_view::maximum_number_of_requests() const
{
    return sx.data[field_offsets::max_num_requests];
}

inline uint7_t property_exchange_capabilities_view::pe_version_major() const
{
    return (message_version() >= message_version_2) ? sx.data[field_offsets::pe_version_major] : 0;
}

inline uint7_t property_exchange_capabilities_view::pe_version_minor() const
{
    return (message_version() >= message_version_2) ? sx.data[field_offsets::pe_version_minor] : 0;
}

inline bool property_exchange_capabilities_view::validate(const sysex7& sx)
{
    return (sx.manufacturerID == manufacturer::universal_non_realtime) &&
           (sx.data.size() >= field_offsets::max_num_requests + 1u) && (sx.data[1] == 0x0D) &&
           (sx.data[2] >= subtype::property_exchange_capabilities_inquiry) &&
           (sx.data[2] <= subtype::property_exchange_capabilities_reply) &&
           ((sx.data[3] < message_version_2) || (sx.data.size() >= field_offsets::pe_version_minor + 1u));
}

inline message property_exchange::make_capabilities_message(
  subtype_t subtype, muid_t source_muid, muid_t destination_muid, uint7_t max_num_requests, uint7_t device_id)
{
    auto result = message::make_with_payload_size(3, subtype, source_muid, destination_muid, device_id);

    const uint7_t data[3] = { max_num_requests, property_exchange::version_major, property_exchange::version_minor };
    result.add_data(data, sizeof(data));
    return result;
}

//---- subtype::property_exchange_capabilities_inquiry

inline message make_property_exchange_capabilities_inquiry(muid_t  source_muid,
                                                           muid_t  destination_muid,
                                                           uint7_t max_num_requests,
                                                           uint7_t device_id)
{
    return property_exchange::make_capabilities_message(
      subtype::property_exchange_capabilities_inquiry, source_muid, destination_muid, max_num_requests, device_id);
}

//---- subtype::property_exchange_capabilities_reply

inline message make_property_exchange_capabilities_reply(muid_t  source_muid,
                                                         muid_t  destination_muid,
                                                         uint7_t max_num_requests,
                                                         uint7_t device_id)
{
    return property_exchange::make_capabilities_message(
      subtype::property_exchange_capabilities_reply, source_muid, destination_muid, max_num_requests, device_id);
}

//---- get_property_data_view

inline bool get_property_data_view::validate(const sysex7& sx)
{
    return (property_exchange::property_data_message_view::validate(sx) &&
            ((sx.data[2] == subtype::get_property_data_inquiry) || (sx.data[2] == subtype::get_property_data_reply)));
}

//---- subtype::get_property_data_inquiry

inline message make_get_property_data_inquiry(
  muid_t source_muid, muid_t destination_muid, std::string_view resource, uint7_t request_id, uint7_t device_id)
{
    using namespace property_exchange;

    return make_property_data_message(subtype::get_property_data_inquiry,
                                      source_muid,
                                      destination_muid,
                                      header{
                                        make_rjson(tags::resource, resource),
                                      },
                                      1,
                                      1,
                                      {},
                                      request_id,
                                      device_id);
}

inline message make_get_property_data_inquiry(muid_t                                   source_muid,
                                              muid_t                                   destination_muid,
                                              std::string_view                         resource,
                                              const property_exchange::header_options& options,
                                              uint7_t                                  request_id,
                                              uint7_t                                  device_id)
{
    using namespace property_exchange;

    return make_property_data_message(subtype::get_property_data_inquiry,
                                      source_muid,
                                      destination_muid,
                                      header{
                                        make_rjson(tags::resource, resource, options),
                                      },
                                      1,
                                      1,
                                      {},
                                      request_id,
                                      device_id);
}

//---- subtype::get_property_data_reply

inline message make_get_property_data_reply(muid_t                           source_muid,
                                            muid_t                           destination_muid,
                                            const property_exchange::header& header,
                                            uint14_t                         number_of_chunks,
                                            uint14_t                         number_of_this_chunk,
                                            const property_exchange::chunk&  chunk,
                                            uint7_t                          request_id,
                                            uint7_t                          device_id)
{
    return make_property_data_message(subtype::get_property_data_reply,
                                      source_muid,
                                      destination_muid,
                                      header,
                                      number_of_chunks,
                                      number_of_this_chunk,
                                      chunk,
                                      request_id,
                                      device_id);
}

inline message make_get_property_data_reply(muid_t                          source_muid,
                                            muid_t                          destination_muid,
                                            int                             status,
                                            uint14_t                        number_of_chunks,
                                            uint14_t                        number_of_this_chunk,
                                            const property_exchange::chunk& chunk,
                                            uint7_t                         request_id,
                                            uint7_t                         device_id)
{
    using namespace property_exchange;

    return make_property_data_message(subtype::get_property_data_reply,
                                      source_muid,
                                      destination_muid,
                                      header{
                                        make_rjson(tags::status, status),
                                      },
                                      number_of_chunks,
                                      number_of_this_chunk,
                                      chunk,
                                      request_id,
                                      device_id);
}

inline message make_get_property_data_reply(muid_t                          source_muid,
                                            muid_t                          destination_muid,
                                            int                             status,
                                            std::string_view                message,
                                            uint14_t                        number_of_chunks,
                                            uint14_t                        number_of_this_chunk,
                                            const property_exchange::chunk& chunk,
                                            uint7_t                         request_id,
                                            uint7_t                         device_id)
{
    using namespace property_exchange;

    return make_property_data_message(subtype::get_property_data_reply,
                                      source_muid,
                                      destination_muid,
                                      header{
                                        make_rjson(tags::status, status, header_options{ { tags::message, message } }),
                                      },
                                      number_of_chunks,
                                      number_of_this_chunk,
                                      chunk,
                                      request_id,
                                      device_id);
}

inline message make_get_property_data_reply(muid_t                          source_muid,
                                            muid_t                          destination_muid,
                                            uint14_t                        number_of_chunks,
                                            uint14_t                        number_of_this_chunk,
                                            const property_exchange::chunk& chunk,
                                            uint7_t                         request_id,
                                            uint7_t                         device_id)
{
    return make_property_data_message(subtype::get_property_data_reply,
                                      source_muid,
                                      destination_muid,
                                      {},
                                      number_of_chunks,
                                      number_of_this_chunk,
                                      chunk,
                                      request_id,
                                      device_id);
}

//---- set_property_data_view

inline bool set_property_data_view::validate(const sysex7& sx)
{
    return (property_exchange::property_data_message_view::validate(sx) &&
            ((sx.data[2] == subtype::set_property_data_inquiry) || (sx.data[2] == subtype::set_property_data_reply)));
}

//---- subtype::set_property_data_inquiry

inline message make_set_property_data_inquiry(muid_t                          source_muid,
                                              muid_t                          destination_muid,
                                              std::string_view                resource,
                                              uint14_t                        number_of_chunks,
                                              uint14_t                        number_of_this_chunk,
                                              const property_exchange::chunk& chunk,
                                              uint7_t                         request_id,
                                              uint7_t                         device_id)
{
    using namespace property_exchange;

    return make_property_data_message(subtype::set_property_data_inquiry,
                                      source_muid,
                                      destination_muid,
                                      header{
                                        make_rjson(tags::resource, resource),
                                      },
                                      number_of_chunks,
                                      number_of_this_chunk,
                                      chunk,
                                      request_id,
                                      device_id);
}

inline message make_set_property_data_inquiry(muid_t                                   source_muid,
                                              muid_t                                   destination_muid,
                                              std::string_view                         resource,
                                              const property_exchange::header_options& options,
                                              uint14_t                                 number_of_chunks,
                                              uint14_t                                 number_of_this_chunk,
                                              const property_exchange::chunk&          chunk,
                                              uint7_t                                  request_id,
                                              uint7_t                                  device_id)
{
    using namespace property_exchange;

    return make_property_data_message(subtype::set_property_data_inquiry,
                                      source_muid,
                                      destination_muid,
                                      header{
                                        make_rjson(tags::resource, resource, options),
                                      },
                                      number_of_chunks,
                                      number_of_this_chunk,
                                      chunk,
                                      request_id,
                                      device_id);
}

inline message make_set_property_data_inquiry(muid_t                          source_muid,
                                              muid_t                          destination_muid,
                                              uint14_t                        number_of_chunks,
                                              uint14_t                        number_of_this_chunk,
                                              const property_exchange::chunk& chunk,
                                              uint7_t                         request_id,
                                              uint7_t                         device_id)
{
    using namespace property_exchange;

    return make_property_data_message(subtype::set_property_data_inquiry,
                                      source_muid,
                                      destination_muid,
                                      {},
                                      number_of_chunks,
                                      number_of_this_chunk,
                                      chunk,
                                      request_id,
                                      device_id);
}

//---- subtype::set_property_data_reply

inline message make_set_property_data_reply(
  muid_t source_muid, muid_t destination_muid, int status, uint7_t request_id, uint7_t device_id)
{
    using namespace property_exchange;

    return make_property_data_message(subtype::set_property_data_reply,
                                      source_muid,
                                      destination_muid,
                                      header{
                                        make_rjson(tags::status, status),
                                      },
                                      1,
                                      1,
                                      {},
                                      request_id,
                                      device_id);
}

inline message make_set_property_data_reply(muid_t           source_muid,
                                            muid_t           destination_muid,
                                            int              status,
                                            std::string_view message,
                                            uint7_t          request_id,
                                            uint7_t          device_id)
{
    using namespace property_exchange;

    return make_property_data_message(subtype::set_property_data_reply,
                                      source_muid,
                                      destination_muid,
                                      header{
                                        make_rjson(tags::status,
                                                   status,
                                                   { {
                                                     tags::message,
                                                     message,
                                                   } }),
                                      },
                                      1,
                                      1,
                                      {},
                                      request_id,
                                      device_id);
}

//---- subscription_view

inline bool subscription_view::validate(const sysex7& sx)
{
    return (property_exchange::property_data_message_view::validate(sx) &&
            ((sx.data[2] == subtype::subscription_inquiry) || (sx.data[2] == subtype::subscription_reply)));
}

//---- subtype::subscription_inquiry

inline message make_subscription_inquiry(muid_t           source_muid,
                                         muid_t           destination_muid,
                                         std::string_view resource,
                                         std::string_view command,
                                         std::string_view subscribeId,
                                         uint7_t          request_id,
                                         uint7_t          device_id)
{
    using namespace property_exchange;

    return make_property_data_message(subtype::subscription_inquiry,
                                      source_muid,
                                      destination_muid,
                                      header{
                                        make_rjson(tags::resource,
                                                   resource,
                                                   header_options{
                                                     { tags::command, command },
                                                     { tags::subscribeId, subscribeId },
                                                   }),
                                      },
                                      0,
                                      0,
                                      {},
                                      request_id,
                                      device_id);
}

inline message make_subscription_inquiry(muid_t                          source_muid,
                                         muid_t                          destination_muid,
                                         std::string_view                resource,
                                         std::string_view                command,
                                         std::string_view                subscribeId,
                                         uint14_t                        number_of_chunks,
                                         uint14_t                        number_of_this_chunk,
                                         const property_exchange::chunk& chunk,
                                         uint7_t                         request_id,
                                         uint7_t                         device_id)
{
    using namespace property_exchange;

    return make_property_data_message(subtype::subscription_inquiry,
                                      source_muid,
                                      destination_muid,
                                      header{
                                        make_rjson(tags::resource,
                                                   resource,
                                                   header_options{
                                                     { tags::command, command },
                                                     { tags::subscribeId, subscribeId },
                                                   }),
                                      },
                                      number_of_chunks,
                                      number_of_this_chunk,
                                      chunk,
                                      request_id,
                                      device_id);
}

inline message make_subscription_inquiry(muid_t                          source_muid,
                                         muid_t                          destination_muid,
                                         uint14_t                        number_of_chunks,
                                         uint14_t                        number_of_this_chunk,
                                         const property_exchange::chunk& chunk,
                                         uint7_t                         request_id,

                                         uint7_t device_id)
{
    using namespace property_exchange;

    return make_property_data_message(subtype::subscription_inquiry,
                                      source_muid,
                                      destination_muid,
                                      {},
                                      number_of_chunks,
                                      number_of_this_chunk,
                                      chunk,
                                      request_id,
                                      device_id);
}

//---- subtype::subscription_reply

inline message make_subscription_reply(
  muid_t source_muid, muid_t destination_muid, int status, uint7_t request_id, uint7_t device_id)
{
    using namespace property_exchange;

    return make_property_data_message(subtype::subscription_reply,
                                      source_muid,
                                      destination_muid,
                                      header{
                                        make_rjson(tags::status, status),
                                      },
                                      1,
                                      1,
                                      {},
                                      request_id,
                                      device_id);
}

inline message make_subscription_reply(muid_t           source_muid,
                                       muid_t           destination_muid,
                                       int              status,
                                       std::string_view message,
                                       uint7_t          request_id,
                                       uint7_t          device_id)
{
    using namespace property_exchange;

    return make_property_data_message(subtype::subscription_reply,
                                      source_muid,
                                      destination_muid,
                                      header{
                                        make_rjson(tags::status, status, header_options{ { tags::message, message } }),
                                      },
                                      1,
                                      1,
                                      {},
                                      request_id,
                                      device_id);
}

//---- notify_view

inline bool notify_view::validate(const sysex7& sx)
{
    return (property_exchange::property_data_message_view::validate(sx) && (sx.data[2] == subtype::notify));
}

//---- subtype::notify

inline message make_notify_message(
  muid_t source_muid, muid_t destination_muid, int status, uint7_t request_id, uint7_t device_id)
{
    using namespace property_exchange;

    return make_property_data_message(subtype::notify,
                                      source_muid,
                                      destination_muid,
                                      header{
                                        make_rjson(tags::status, status),
                                      },
                                      1,
                                      1,
                                      {},
                                      request_id,
                                      device_id);
}

inline message make_notify_message(muid_t           source_muid,
                                   muid_t           destination_muid,
                                   int              status,
                                   std::string_view message,
                                   uint7_t          request_id,
                                   uint7_t          device_id)
{
    using namespace property_exchange;

    return make_property_data_message(subtype::notify,
                                      source_muid,
                                      destination_muid,
                                      header{
                                        make_rjson(tags::status, status, header_options{ { tags::message, message } }),
                                      },
                                      1,
                                      1,
                                      {},
                                      request_id,
                                      device_id);
}

//-----------------------------------------------
//
//               PROCESS INQUIRY implementations
//
//-----------------------------------------------

//---- subtype::process_inquiry_capabilities_inquiry

inline message make_process_inquiry_capabilities_inquiry(muid_t source_muid, muid_t destination_muid, uint7_t device_id)
{
    return message(subtype::process_inquiry_capabilities_inquiry, source_muid, destination_muid, device_id);
}

//---- subtype::process_inquiry_capabilities_reply

struct process_inquiry_capabilities_reply_view::field_offsets : capability_inquiry_view::field_offsets
{
    static constexpr auto supported_features = payload;
};

inline uint7_t process_inquiry_capabilities_reply_view::supported_features() const
{
    return sx.data[field_offsets::supported_features];
}

inline bool process_inquiry_capabilities_reply_view::validate(const sysex7& sx)
{
    return (sx.manufacturerID == manufacturer::universal_non_realtime) &&
           (sx.data.size() > field_offsets::supported_features) && (sx.data[1] == 0x0D) &&
           (sx.data[2] == subtype::process_inquiry_capabilities_reply);
}

inline message make_process_inquiry_capabilities_reply(muid_t  source_muid,
                                                       muid_t  destination_muid,
                                                       uint7_t features,
                                                       uint7_t device_id)
{
    auto result = message::make_with_payload_size(
      1, subtype::process_inquiry_capabilities_reply, source_muid, destination_muid, device_id);
    result.add_data(&features, 1);
    return result;
}

//---- subtype::midi_message_report_inquiry

struct midi_message_report_inquiry_view::field_offsets : capability_inquiry_view::field_offsets
{
    static constexpr auto message_data_control             = payload;
    static constexpr auto system_message_types             = payload + 1u;
    static constexpr auto _reserved_control                = payload + 2u;
    static constexpr auto channel_controller_message_types = payload + 3u;
    static constexpr auto note_data_message_types          = payload + 4u;
};

inline uint7_t midi_message_report_inquiry_view::message_data_control() const
{
    return sx.data[field_offsets::message_data_control];
}

inline uint7_t midi_message_report_inquiry_view::system_message_types() const
{
    return sx.data[field_offsets::system_message_types];
}

inline uint7_t midi_message_report_inquiry_view::channel_controller_message_types() const
{
    return sx.data[field_offsets::channel_controller_message_types];
}

inline uint7_t midi_message_report_inquiry_view::note_data_message_types() const
{
    return sx.data[field_offsets::note_data_message_types];
}

inline bool midi_message_report_inquiry_view::validate(const sysex7& sx)
{
    return (sx.manufacturerID == manufacturer::universal_non_realtime) &&
           (sx.data.size() > field_offsets::note_data_message_types) && (sx.data[1] == 0x0D) &&
           (sx.data[2] == subtype::midi_message_report_inquiry);
}

inline message make_midi_message_report_inquiry(muid_t  source_muid,
                                                muid_t  destination_muid,
                                                uint7_t data_control,
                                                uint7_t system_messages,
                                                uint7_t channel_controller_messages,
                                                uint7_t note_data_messages,
                                                uint7_t device_id)
{
    auto result = message::make_with_payload_size(
      5, subtype::midi_message_report_inquiry, source_muid, destination_muid, device_id);

    const uint7_t data[] = { data_control, system_messages, 0, channel_controller_messages, note_data_messages };
    result.add_data(data, sizeof(data));
    return result;
}

//---- subtype::midi_message_report_reply

struct midi_message_report_reply_view::field_offsets : capability_inquiry_view::field_offsets
{
    static constexpr auto system_message_types             = payload;
    static constexpr auto _reserved_control                = payload + 1u;
    static constexpr auto channel_controller_message_types = payload + 2u;
    static constexpr auto note_data_message_types          = payload + 3u;
};

inline uint7_t midi_message_report_reply_view::system_message_types() const
{
    return sx.data[field_offsets::system_message_types];
}

inline uint7_t midi_message_report_reply_view::channel_controller_message_types() const
{
    return sx.data[field_offsets::channel_controller_message_types];
}

inline uint7_t midi_message_report_reply_view::note_data_message_types() const
{
    return sx.data[field_offsets::note_data_message_types];
}

inline bool midi_message_report_reply_view::validate(const sysex7& sx)
{
    return (sx.manufacturerID == manufacturer::universal_non_realtime) &&
           (sx.data.size() > field_offsets::note_data_message_types) && (sx.data[1] == 0x0D) &&
           (sx.data[2] == subtype::midi_message_report_reply);
}

inline message make_midi_message_report_reply(muid_t  source_muid,
                                              uint7_t system_messages,
                                              uint7_t channel_controller_messages,
                                              uint7_t note_data_messages,
                                              uint7_t device_id)
{
    auto result =
      message::make_with_payload_size(5, subtype::midi_message_report_reply, source_muid, broadcast_muid, device_id);

    const uint7_t data[] = { system_messages, 0, channel_controller_messages, note_data_messages };
    result.add_data(data, sizeof(data));
    return result;
}

//---- subtype::midi_message_report_end

inline message make_midi_message_report_end(muid_t source_muid, uint7_t device_id)
{
    return message(subtype::midi_message_report_end, source_muid, broadcast_muid, device_id);
}

//--------------------------------------------------------------------------

} // namespace midi::ci

namespace midi {

//--------------------------------------------------------------------------

inline bool is_capability_inquiry_message(const sysex7& sx)
{
    return (sx.manufacturerID == manufacturer::universal_non_realtime) &&
           (sx.data.size() >= capability_inquiry_view::field_offsets::payload) && (sx.data[1] == 0x0D);
}

//-----------------------------------------------
// capability_inquiry_view

inline capability_inquiry_view::capability_inquiry_view(const midi::sysex7& sx)
  : message_view(sx)
{
    assert(validate(sx));
}

inline capability_inquiry_view::capability_inquiry_view(const message_view& v)
  : capability_inquiry_view(v.sx)
{
}

inline uint7_t capability_inquiry_view::message_version() const
{
    return sx.data[field_offsets::message_version];
}

inline muid_t capability_inquiry_view::source_muid() const
{
    return sx.make_uint28(field_offsets::source_muid);
}

inline muid_t capability_inquiry_view::destination_muid() const
{
    return sx.make_uint28(field_offsets::destination_muid);
}

bool inline capability_inquiry_view::validate(const sysex7& sx)
{
    return is_capability_inquiry_message(sx);
}

inline std::optional<capability_inquiry_view> as_capability_inquiry_view(const midi::sysex7& sx)
{
    if (is_capability_inquiry_message(sx))
        return capability_inquiry_view{ sx };
    else
        return std::nullopt;
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
