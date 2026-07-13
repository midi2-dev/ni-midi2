//
// Copyright (c) 2026 Native Instruments
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

#include <midi/extended_data_message.h>
#include <midi/manufacturer.h>
#include <midi/sysex.h>
#include <midi/types.h>

#include <algorithm>
#include <cstdint>
#include <vector>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

//! MIDI Mixed Data Set
struct mixed_data_set
{
    using data_type = sysex::data_type; //!< same data type (and allocator configuration) as sysex

    manufacturer_t manufacturerID{ 0 }; //!< manufacturer ID, \see midi::manufacturer
    uint16_t       deviceID{ 0 };       //!< device ID, 0xFFFF means 'all call'
    uint16_t       subID1{ 0 };         //!< sub ID #1
    uint16_t       subID2{ 0 };         //!< sub ID #2
    data_type      data;                //!< Mixed Data Set payload data

    mixed_data_set() = default;

    /*! \param manufacturer manufacturer */
    explicit mixed_data_set(manufacturer_t manufacturer NIMIDI2_PMR_SYSEX_DATA_ARG)
      : manufacturerID(manufacturer)
      , data(NIMIDI2_PMR_SYSEX_DATA_DATA_INITIALIZER)
    {
    }

    /*! \param manufacturer manufacturer
        \param device_id device ID, 0xFFFF means 'all call'
        \param sub_id_1 sub ID #1
        \param sub_id_2 sub ID #2
    */
    mixed_data_set(manufacturer_t    manufacturer,
                   uint16_t          device_id,
                   uint16_t          sub_id_1,
                   uint16_t sub_id_2 NIMIDI2_PMR_SYSEX_DATA_ARG)
      : manufacturerID(manufacturer)
      , deviceID(device_id)
      , subID1(sub_id_1)
      , subID2(sub_id_2)
      , data(NIMIDI2_PMR_SYSEX_DATA_DATA_INITIALIZER)
    {
    }

    /*! \param manufacturer manufacturer
        \param device_id device ID, 0xFFFF means 'all call'
        \param sub_id_1 sub ID #1
        \param sub_id_2 sub ID #2
        \param d payload data
    */
    mixed_data_set(manufacturer_t manufacturer,
                   uint16_t       device_id,
                   uint16_t       sub_id_1,
                   uint16_t       sub_id_2,
                   data_type d    NIMIDI2_PMR_SYSEX_DATA_ARG)
      : manufacturerID(manufacturer)
      , deviceID(device_id)
      , subID1(sub_id_1)
      , subID2(sub_id_2)
      , data(std::move(d) NIMIDI2_PMR_SYSEX_DATA_DATA_INITIALIZER_SUFFIX)
    {
    }

    /*! \param manufacturer manufacturer
        \param device_id device ID, 0xFFFF means 'all call'
        \param sub_id_1 sub ID #1
        \param sub_id_2 sub ID #2
        \param d payload data
    */
    mixed_data_set(manufacturer_t                   manufacturer,
                   uint16_t                         device_id,
                   uint16_t                         sub_id_1,
                   uint16_t                         sub_id_2,
                   std::initializer_list<uint8_t> d NIMIDI2_PMR_SYSEX_DATA_ARG)
      : manufacturerID(manufacturer)
      , deviceID(device_id)
      , subID1(sub_id_1)
      , subID2(sub_id_2)
      , data(d.begin(), d.end() NIMIDI2_PMR_SYSEX_DATA_DATA_INITIALIZER_SUFFIX)
    {
    }

#if NIMIDI2_PMR_SYSEX_DATA
    mixed_data_set(manufacturer_t              manufacturer,
                   uint16_t                    device_id,
                   uint16_t                    sub_id_1,
                   uint16_t                    sub_id_2,
                   const std::vector<uint8_t>& d,
                   std::pmr::memory_resource*  mr = std::pmr::get_default_resource())
      : manufacturerID(manufacturer)
      , deviceID(device_id)
      , subID1(sub_id_1)
      , subID2(sub_id_2)
      , data(d.begin(), d.end(), mr)
    {
    }
#endif

    mixed_data_set(const mixed_data_set&) = default;
    mixed_data_set(mixed_data_set&&)      = default;

    ~mixed_data_set() = default;

    mixed_data_set& operator=(const mixed_data_set&) = default;
    mixed_data_set& operator=(mixed_data_set&&)      = default;

    bool operator==(const mixed_data_set&) const;
    bool operator!=(const mixed_data_set&) const;

    void clear();
};

//--------------------------------------------------------------------------

template<typename Sender>
void send_mixed_data_set(const mixed_data_set&, uint4_t mds_id, group_t, Sender&&);

std::vector<extended_data_message> as_mixed_data_set_packets(const mixed_data_set&, uint4_t mds_id, group_t = 0);

//----------------------------------------------- inline implementations

inline bool mixed_data_set::operator==(const mixed_data_set& other) const
{
    return (manufacturerID == other.manufacturerID) && (deviceID == other.deviceID) && (subID1 == other.subID1) &&
           (subID2 == other.subID2) && (data == other.data);
}

inline bool mixed_data_set::operator!=(const mixed_data_set& other) const
{
    return !operator==(other);
}

inline void mixed_data_set::clear()
{
    manufacturerID = 0;
    deviceID       = 0;
    subID1         = 0;
    subID2         = 0;
    data.clear();
}

//--------------------------------------------------------------------------

template<typename Sender>
void send_mixed_data_set(const mixed_data_set& mds, uint4_t mds_id, group_t group, Sender&& sender)
{
    constexpr size_t payload_bytes_per_packet = mixed_data_set_payload_packet::payload_size;

    // the valid bytes in a chunk (16 header bytes plus 16 for every full payload packet)
    // are limited to 65535, resulting in at most 4094 payload packets per chunk
    constexpr size_t max_packets_per_chunk = 4094;
    constexpr size_t max_chunk_data_size   = max_packets_per_chunk * payload_bytes_per_packet;

    const auto manufacturer = manufacturer_id_16bit(mds.manufacturerID);

    const size_t   total_data_size = mds.data.size();
    const uint16_t nr_of_chunks =
      (total_data_size == 0) ? 1 : uint16_t((total_data_size + max_chunk_data_size - 1) / max_chunk_data_size);

    size_t offset = 0;
    for (uint16_t chunk_nr = 1; chunk_nr <= nr_of_chunks; ++chunk_nr)
    {
        const size_t chunk_data_size = std::min(total_data_size - offset, max_chunk_data_size);
        const size_t nr_of_packets   = (chunk_data_size + payload_bytes_per_packet - 1) / payload_bytes_per_packet;

        auto header = make_mixed_data_set_header_packet(mds_id, group);
        header.set_valid_bytes_in_chunk(uint16_t(16u + 2u * nr_of_packets + chunk_data_size));
        header.set_nr_of_chunks(nr_of_chunks);
        header.set_chunk_nr(chunk_nr);
        header.set_manufacturer_id(manufacturer);
        header.set_device_id(mds.deviceID);
        header.set_sub_id_1(mds.subID1);
        header.set_sub_id_2(mds.subID2);
        sender(header);

        for (size_t packet_nr = 0; packet_nr < nr_of_packets; ++packet_nr)
        {
            auto p = make_mixed_data_set_payload_packet(mds_id, group);

            const size_t packet_offset = packet_nr * payload_bytes_per_packet;
            const size_t payload_bytes = std::min(payload_bytes_per_packet, chunk_data_size - packet_offset);
            for (size_t b = 0; b < payload_bytes; ++b)
            {
                p.set_payload_byte(b, mds.data[offset + packet_offset + b]);
            }
            sender(p);
        }

        offset += chunk_data_size;
    }
}

//--------------------------------------------------------------------------

inline std::vector<extended_data_message> as_mixed_data_set_packets(const mixed_data_set& mds,
                                                                    uint4_t               mds_id,
                                                                    group_t               group)
{
    std::vector<extended_data_message> result;
    result.reserve(mds.data.size() / mixed_data_set_payload_packet::payload_size + 2);

    send_mixed_data_set(mds, mds_id, group, [&](const extended_data_message& p) { result.push_back(p); });

    return result;
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
