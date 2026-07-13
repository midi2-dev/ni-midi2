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

#include <gtest/gtest.h>

#include <midi/mixed_data_set.h>

#include <midi/manufacturer.h>
#include <midi/mixed_data_set_collector.h>

#include "mixed_data_set_test_data.h"

#include <numeric>

//-----------------------------------------------

TEST(mixed_data_set, manufacturer_id_conversion)
{
    using namespace midi;

    // one byte manufacturer IDs
    EXPECT_EQ(0x0004u, manufacturer_id_16bit(manufacturer::moog));
    EXPECT_EQ(0x007Eu, manufacturer_id_16bit(manufacturer::universal_non_realtime));
    EXPECT_EQ(0x007Fu, manufacturer_id_16bit(manufacturer::universal_realtime));
    EXPECT_EQ(0x0000u, manufacturer_id_16bit(0));

    // three byte manufacturer IDs
    EXPECT_EQ(0xA109u, manufacturer_id_16bit(manufacturer::native_instruments));
    EXPECT_EQ(0x820Du, manufacturer_id_16bit(manufacturer::google));

    // one byte manufacturer IDs
    EXPECT_EQ(manufacturer::moog, manufacturer_from_16bit_id(0x0004));
    EXPECT_EQ(manufacturer::universal_non_realtime, manufacturer_from_16bit_id(0x007E));
    EXPECT_EQ(manufacturer::universal_realtime, manufacturer_from_16bit_id(0x007F));
    EXPECT_EQ(0u, manufacturer_from_16bit_id(0x0000));

    // three byte manufacturer IDs
    EXPECT_EQ(manufacturer::native_instruments, manufacturer_from_16bit_id(0xA109));
    EXPECT_EQ(manufacturer::google, manufacturer_from_16bit_id(0x820D));

    // reserved bits are ignored
    EXPECT_EQ(manufacturer::moog, manufacturer_from_16bit_id(0x0084));
    EXPECT_EQ(manufacturer::native_instruments, manufacturer_from_16bit_id(0xA189));

    // round trip
    for (const auto m : { manufacturer::moog, manufacturer::universal_realtime, manufacturer::native_instruments })
    {
        EXPECT_EQ(m, manufacturer_from_16bit_id(manufacturer_id_16bit(m)));
    }
}

//-----------------------------------------------

TEST(mixed_data_set, constructors)
{
    using namespace midi;

    {
        mixed_data_set m;
        EXPECT_EQ(0u, m.manufacturerID);
        EXPECT_EQ(0u, m.deviceID);
        EXPECT_EQ(0u, m.subID1);
        EXPECT_EQ(0u, m.subID2);
        EXPECT_TRUE(m.data.empty());
    }

    {
        mixed_data_set m{ manufacturer::native_instruments };
        EXPECT_EQ(manufacturer::native_instruments, m.manufacturerID);
        EXPECT_EQ(0u, m.deviceID);
        EXPECT_TRUE(m.data.empty());
    }

    {
        mixed_data_set m{ manufacturer::moog, 0xFFFF, 0x0102, 0x0304 };
        EXPECT_EQ(manufacturer::moog, m.manufacturerID);
        EXPECT_EQ(0xFFFFu, m.deviceID);
        EXPECT_EQ(0x0102u, m.subID1);
        EXPECT_EQ(0x0304u, m.subID2);
        EXPECT_TRUE(m.data.empty());
    }

    {
        mixed_data_set m{ manufacturer::moog, 1, 2, 3, { 0xAA, 0xBB, 0xCC } };
        EXPECT_EQ(manufacturer::moog, m.manufacturerID);
        EXPECT_EQ((mixed_data_set::data_type{ 0xAA, 0xBB, 0xCC }), m.data);
    }
}

//-----------------------------------------------

TEST(mixed_data_set, equality)
{
    using namespace midi;

    const mixed_data_set a{ manufacturer::moog, 1, 2, 3, { 0xAA, 0xBB } };

    EXPECT_EQ(a, a);
    EXPECT_EQ(a, (mixed_data_set{ manufacturer::moog, 1, 2, 3, { 0xAA, 0xBB } }));

    EXPECT_NE(a, (mixed_data_set{ manufacturer::google, 1, 2, 3, { 0xAA, 0xBB } }));
    EXPECT_NE(a, (mixed_data_set{ manufacturer::moog, 9, 2, 3, { 0xAA, 0xBB } }));
    EXPECT_NE(a, (mixed_data_set{ manufacturer::moog, 1, 9, 3, { 0xAA, 0xBB } }));
    EXPECT_NE(a, (mixed_data_set{ manufacturer::moog, 1, 2, 9, { 0xAA, 0xBB } }));
    EXPECT_NE(a, (mixed_data_set{ manufacturer::moog, 1, 2, 3, { 0xAA } }));
}

//-----------------------------------------------

TEST(mixed_data_set, clear)
{
    using namespace midi;

    mixed_data_set m{ manufacturer::moog, 1, 2, 3, { 0xAA, 0xBB } };
    m.clear();

    EXPECT_EQ(m, mixed_data_set{});
}

//-----------------------------------------------

TEST(mixed_data_set, as_mixed_data_set_packets)
{
    using namespace midi;

    // empty mixed data set results in a single header only chunk
    {
        const mixed_data_set m{ manufacturer::universal_non_realtime, 0x1234, 0x0005, 0x0006 };

        const auto packets = as_mixed_data_set_packets(m, 0x4);

        ASSERT_EQ(1u, packets.size());
        EXPECT_EQ(universal_packet(0x50840010, 0x00010001, 0x007E1234, 0x00050006), packets[0]);
    }

    // single payload packet
    {
        const mixed_data_set m{ manufacturer::native_instruments, 0, 0, 0, { 0x11, 0x22, 0x33, 0x44, 0x55 } };

        const auto packets = as_mixed_data_set_packets(m, 0x0);

        ASSERT_EQ(2u, packets.size());
        EXPECT_EQ(universal_packet(0x50800017, 0x00010001, 0xA1090000, 0x00000000), packets[0]);
        EXPECT_EQ(universal_packet(0x50901122, 0x33445500, 0, 0), packets[1]);
    }

    // two payload packets, non-zero group
    {
        const mixed_data_set m{ manufacturer::moog, 0xFFFF, 0x0102, 0x0304, { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                                                              0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
                                                                              0x0F, 0x10, 0x11, 0x12, 0x13, 0x14 } };

        const auto packets = as_mixed_data_set_packets(m, 0xF, 0x3);

        ASSERT_EQ(3u, packets.size());
        EXPECT_EQ(universal_packet(0x538F0028, 0x00010001, 0x0004FFFF, 0x01020304), packets[0]);
        EXPECT_EQ(universal_packet(0x539F0102, 0x03040506, 0x0708090A, 0x0B0C0D0E), packets[1]);
        EXPECT_EQ(universal_packet(0x539F0F10, 0x11121314, 0, 0), packets[2]);
    }
}

//-----------------------------------------------

TEST(mixed_data_set, send_mixed_data_set_multiple_chunks)
{
    using namespace midi;

    // a chunk carries at most 4094 payload packets of 14 bytes
    constexpr size_t max_chunk_data_size = 4094 * 14;

    mixed_data_set m{ manufacturer::native_instruments, 0, 0, 0 };
    m.data.resize(max_chunk_data_size + 5);
    std::iota(m.data.begin(), m.data.end(), uint8_t{ 0 });

    unsigned headers  = 0;
    unsigned payloads = 0;
    send_mixed_data_set(m, 0x2, 0, [&](const universal_packet& p) {
        if (is_mixed_data_set_header_packet(p))
        {
            const auto h = mixed_data_set_header_packet_view{ p };
            EXPECT_EQ(0x2u, h.mds_id());
            EXPECT_EQ(2u, h.nr_of_chunks());
            EXPECT_EQ(headers + 1, h.chunk_nr());
            EXPECT_EQ(0xA109u, h.manufacturer_id());

            if (headers == 0)
            {
                // full chunk: 16 header bytes plus 4094 full payload packets
                EXPECT_EQ(16u + 16u * 4094u, h.valid_bytes_in_chunk());
            }
            else
            {
                // remainder: 16 header bytes plus one payload packet with 5 valid bytes
                EXPECT_EQ(16u + 2u + 5u, h.valid_bytes_in_chunk());
            }
            ++headers;
        }
        else
        {
            EXPECT_TRUE(is_mixed_data_set_payload_packet(p));
            ++payloads;
        }
    });

    EXPECT_EQ(2u, headers);
    EXPECT_EQ(4094u + 1u, payloads);
}

//-----------------------------------------------

TEST(mixed_data_set, send_mixed_data_set_exact_chunk_boundary)
{
    using namespace midi;

    // data of exactly one maximum size chunk results in a single chunk
    constexpr size_t max_chunk_data_size = 4094 * 14;

    mixed_data_set m{ manufacturer::native_instruments, 0, 0, 0 };
    m.data.resize(max_chunk_data_size);
    std::iota(m.data.begin(), m.data.end(), uint8_t{ 0 });

    unsigned headers  = 0;
    unsigned payloads = 0;
    send_mixed_data_set(m, 0x0, 0, [&](const universal_packet& p) {
        if (is_mixed_data_set_header_packet(p))
        {
            const auto h = mixed_data_set_header_packet_view{ p };
            EXPECT_EQ(1u, h.nr_of_chunks());
            EXPECT_EQ(1u, h.chunk_nr());
            EXPECT_EQ(16u + 16u * 4094u, h.valid_bytes_in_chunk());
            ++headers;
        }
        else
        {
            ++payloads;
        }
    });

    EXPECT_EQ(1u, headers);
    EXPECT_EQ(4094u, payloads);

    // and roundtrips through the collector
    bool collected = false;
    auto c         = midi::mixed_data_set_collector{ [&](const midi::mixed_data_set& result, uint4_t) {
        collected = true;
        EXPECT_EQ(result, m);
    } };
    for (const auto& p : as_mixed_data_set_packets(m, 0x0))
    {
        c.feed(p);
    }
    EXPECT_TRUE(collected);
}
