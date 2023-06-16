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

#include <midi/capability_inquiry.h>

#include <array>
#include <cassert>

//--------------------------------------------------------------------------

namespace midi::ci {

//-----------------------------------------------

message::message(subtype_t      subtype,
                 muid_t         source_muid,
                 muid_t         destination_muid,
                 const uint7_t* pData,
                 size_t         data_size,
                 uint7_t        device_id)
  : universal_sysex::message(manufacturer::universal_non_realtime,
                             { device_id,
                               0x0D,
                               subtype,
                               version,
                               static_cast<uint7_t>(source_muid & 0x7F),
                               static_cast<uint7_t>((source_muid >> 7) & 0x7F),
                               static_cast<uint7_t>((source_muid >> 14) & 0x7F),
                               static_cast<uint7_t>((source_muid >> 21) & 0x7F),
                               static_cast<uint7_t>(destination_muid & 0x7F),
                               static_cast<uint7_t>((destination_muid >> 7) & 0x7F),
                               static_cast<uint7_t>((destination_muid >> 14) & 0x7F),
                               static_cast<uint7_t>((destination_muid >> 21) & 0x7F) })
{
    assert(subtype <= uint7_max);
    assert(source_muid <= uint28_max);
    assert(destination_muid <= uint28_max);
    assert(data_size <= uint28_max);
    assert(device_id <= uint7_max);

    if (pData && data_size)
    {
        data.reserve(offset_of_data + data_size);
        data.insert(data.end(), pData, pData + data_size);
    }
}

//-----------------------------------------------

message::message(
  subtype_t subtype, muid_t source_muid, muid_t destination_muid, const std::vector<uint7_t>& d, uint7_t device_id)
  : universal_sysex::message(manufacturer::universal_non_realtime,
                             { device_id,
                               0x0D,
                               subtype,
                               version,
                               static_cast<uint7_t>(source_muid & 0x7F),
                               static_cast<uint7_t>((source_muid >> 7) & 0x7F),
                               static_cast<uint7_t>((source_muid >> 14) & 0x7F),
                               static_cast<uint7_t>((source_muid >> 21) & 0x7F),
                               static_cast<uint7_t>(destination_muid & 0x7F),
                               static_cast<uint7_t>((destination_muid >> 7) & 0x7F),
                               static_cast<uint7_t>((destination_muid >> 14) & 0x7F),
                               static_cast<uint7_t>((destination_muid >> 21) & 0x7F) })
{
    assert(subtype <= uint7_max);
    assert(source_muid <= uint28_max);
    assert(destination_muid <= uint28_max);
    assert(d.size() <= uint28_max);
    assert(device_id <= uint7_max);

    if (d.size())
    {
        data.reserve(offset_of_data + d.size());
        data.insert(data.end(), d.begin(), d.end());
    }
}

//-----------------------------------------------

void message::set_source_muid(muid_t m)
{
    assert(m <= uint28_max);

    data[offset_of_smuid]     = static_cast<uint7_t>(m & 0x7F);
    data[offset_of_smuid + 1] = static_cast<uint7_t>((m >> 7) & 0x7F);
    data[offset_of_smuid + 2] = static_cast<uint7_t>((m >> 14) & 0x7F);
    data[offset_of_smuid + 3] = static_cast<uint7_t>((m >> 21) & 0x7F);
}

//-----------------------------------------------

void message::set_destination_muid(muid_t m)
{
    assert(m <= uint28_max);

    data[offset_of_dmuid]     = static_cast<uint7_t>(m & 0x7F);
    data[offset_of_dmuid + 1] = static_cast<uint7_t>((m >> 7) & 0x7F);
    data[offset_of_dmuid + 2] = static_cast<uint7_t>((m >> 14) & 0x7F);
    data[offset_of_dmuid + 3] = static_cast<uint7_t>((m >> 21) & 0x7F);
}

//-----------------------------------------------

message message::make_with_capacity(
  size_t capacity, subtype_t subtype, muid_t source_muid, muid_t destination_muid, uint7_t device_id)
{
    const uint7_t data[] = { device_id,
                             0x0D,
                             subtype,
                             version,
                             static_cast<uint7_t>(source_muid & 0x7F),
                             static_cast<uint7_t>((source_muid >> 7) & 0x7F),
                             static_cast<uint7_t>((source_muid >> 14) & 0x7F),
                             static_cast<uint7_t>((source_muid >> 21) & 0x7F),
                             static_cast<uint7_t>(destination_muid & 0x7F),
                             static_cast<uint7_t>((destination_muid >> 7) & 0x7F),
                             static_cast<uint7_t>((destination_muid >> 14) & 0x7F),
                             static_cast<uint7_t>((destination_muid >> 21) & 0x7F) };

    auto result = message{ capacity };
    result.data.insert(result.data.end(), data, data + sizeof(data));
    return result;
}

//-----------------------------------------------

bool profile_inquiry_reply_view::validate(const sysex7& sx)
{
    size_t expected_size = field_offsets::minimum_message_size;
    if ((sx.manufacturerID == manufacturer::universal_non_realtime) && (sx.data.size() >= expected_size) &&
        (sx.data[1] == 0x0D) && (sx.data[2] == subtype::profile_inquiry_reply))
    {
        // we need at least the enabled profiles payload plus the num disabled profiles field
        expected_size += sx.make_uint14(field_offsets::num_enabled_profiles) * sizeof(profile_id);
        if (sx.data.size() >= expected_size)
        {
            // disabled profiles payload
            expected_size += sx.make_uint14(expected_size - 2) * sizeof(profile_id);
            return (sx.data.size() >= expected_size);
        }
    }

    return false;
}

//-----------------------------------------------

message make_profile_inquiry_reply(muid_t            source_muid,
                                   muid_t            destination_muid,
                                   const profile_id* enabled_profiles,
                                   uint14_t          num_enabled_profiles,
                                   const profile_id* disabled_profiles,
                                   uint14_t          num_disabled_profiles,
                                   uint7_t           deviceID)
{
    assert(num_enabled_profiles <= sysex7::uint14_max);
    assert(num_disabled_profiles <= sysex7::uint14_max);

    message result(subtype::profile_inquiry_reply, source_muid, destination_muid, nullptr, 0, deviceID);

    auto add_profile = [&result](const profile_id& profile) {
        result.add_uint7(profile.byte1);
        result.add_uint7(profile.byte2);
        result.add_uint7(profile.byte3);
        result.add_uint7(profile.byte4);
        result.add_uint7(profile.byte5);
    };

    result.data.reserve(profile_inquiry_reply_view::field_offsets::minimum_message_size +
                        (num_enabled_profiles + num_disabled_profiles) * sizeof(profile_id));

    result.add_uint14(num_enabled_profiles);
    for (auto profile = 0u; profile < num_enabled_profiles; ++profile)
    {
        add_profile(enabled_profiles[profile]);
    }

    result.add_uint14(num_disabled_profiles);
    for (auto profile = 0u; profile < num_disabled_profiles; ++profile)
    {
        add_profile(disabled_profiles[profile]);
    }

    return result;
}

//-----------------------------------------------

std::vector<profile_id> profile_inquiry_reply_view::make_profiles(size_t pos) const
{
    std::vector<profile_id> result;

    assert(sx.data.size() > pos + 1);
    const auto num_profiles = sx.make_uint14(pos);

    assert(sx.data.size() > pos + 1 + num_profiles * sizeof(profile_id));

    result.reserve(num_profiles);
    const uint7_t* d = sx.data.data() + pos + 2;
    for (auto i = 0u; i < num_profiles; ++i)
    {
        result.push_back(profile_id{ d[0], d[1], d[2], d[3], d[4] });
        d += sizeof(profile_id);
    }

    return result;
}

//-----------------------------------------------

message make_profile_specific_data_message(muid_t            source_muid,
                                           muid_t            destination_muid,
                                           const profile_id& p,
                                           const uint7_t*    psd_data,
                                           size_t            psd_data_size,
                                           uint7_t           deviceID)
{
    message result(subtype::profile_specific_data, source_muid, destination_muid, &p.byte1, 5, deviceID);
    result.data.reserve(21 + psd_data_size);

    result.add_uint28(static_cast<uint28_t>(psd_data_size));
    if (psd_data_size)
    {
        result.data.insert(result.data.end(), psd_data, psd_data + psd_data_size);
    }

    return result;
}

//-----------------------------------------------

bool property_exchange::property_data_message_view::validate(const sysex7& sx)
{
    constexpr size_t min_message_size = size_t(field_offsets::chunk_data);

    if ((sx.manufacturerID != manufacturer::universal_non_realtime) || (sx.data.size() < min_message_size) ||
        (sx.data[1] != 0x0D) || (sx.data[2] < subtype::get_property_data_inquiry))
        return false;

    if ((sx.data[2] <= subtype::subscription_reply) || (sx.data[2] == subtype::notify))
    {
        const auto header_bytes = sx.make_uint14(field_offsets::header_size);

        if (sx.data.size() < size_t(field_offsets::header_data + header_bytes + 6))
            return false;

        const auto num_chunks  = sx.make_uint14(field_offsets::num_chunks + header_bytes);
        const auto cur_chunk   = sx.make_uint14(field_offsets::this_chunk + header_bytes);
        const auto chunk_bytes = sx.make_uint14(field_offsets::chunk_size + header_bytes);

        if (cur_chunk > num_chunks)
            return false;

        if (header_bytes && (cur_chunk > 1))
            return false;
        if (chunk_bytes && (cur_chunk < 1))
            return false;

        return (min_message_size + header_bytes + chunk_bytes <= sx.data.size());
    }

    return false;
}

//-----------------------------------------------

message property_exchange::make_property_data_message(subtype_t     subtype,
                                                      muid_t        source_muid,
                                                      muid_t        destination_muid,
                                                      const header& header,
                                                      uint14_t      number_of_chunks,
                                                      uint14_t      number_of_this_chunk,
                                                      const chunk&  chunk,
                                                      uint7_t       request_id,
                                                      uint7_t       device_id)
{
    message result(subtype, source_muid, destination_muid, nullptr, 0, device_id);

    assert((number_of_chunks && number_of_this_chunk) || ((chunk.data == nullptr) && (chunk.size == 0)));
    assert(number_of_this_chunk <= number_of_chunks);

    assert(!header.size || header.data);
    assert(!chunk.size || chunk.data);

    result.data.reserve(result.data.size() + 9 + header.size + chunk.size);

    result.add_uint7(request_id);

    result.add_uint14(static_cast<uint14_t>(header.size));
    if (header.size)
    {
        result.data.insert(result.data.end(), header.data, header.data + header.size);
    }

    result.add_uint14(number_of_chunks);
    result.add_uint14(number_of_this_chunk);

    result.add_uint14(static_cast<uint14_t>(chunk.size));
    if (chunk.size)
    {
        result.data.insert(result.data.end(), chunk.data, chunk.data + chunk.size);
    }

    return result;
}

//-----------------------------------------------

namespace {
    bool is_number(std::string_view s)
    {
        if (s[0] == '-')
            s.remove_prefix(1);
        while (!s.empty())
        {
            if ((s[0] < '0') || (s[0] > '9'))
                return false;
            s.remove_prefix(1);
        }
        return true;
    }
} // namespace

//-----------------------------------------------

std::string property_exchange::make_rjson(std::string_view                         key,
                                          std::string_view                         value,
                                          const property_exchange::header_options& options)
{
    auto result = "{\"" + std::string{ key } + "\":\"" + std::string{ value } + '"';
    for (auto o : options)
    {
        if (is_number(o.second))
            result += ",\"" + std::string{ o.first } + "\":" + std::string{ o.second };
        else
            result += ",\"" + std::string{ o.first } + "\":\"" + std::string{ o.second } + "\"";
    }
    result.push_back('}');

    return result;
}

//-----------------------------------------------

std::string property_exchange::make_rjson(std::string_view                         key,
                                          int                                      value,
                                          const property_exchange::header_options& options)
{
    auto result = "{\"" + std::string{ key } + "\":" + std::to_string(value);
    for (auto o : options)
    {
        if (is_number(o.second))
            result += ",\"" + std::string{ o.first } + "\":" + std::string{ o.second };
        else
            result += ",\"" + std::string{ o.first } + "\":\"" + std::string{ o.second } + "\"";
    }
    result.push_back('}');

    return result;
}

//-----------------------------------------------

} // namespace midi::ci

//--------------------------------------------------------------------------
