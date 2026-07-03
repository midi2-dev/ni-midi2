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

#include <midi/mixed_data_set_collector.h>

#include <midi/extended_data_message.h>
#include <midi/manufacturer.h>

#include "mixed_data_set_test_data.h"

#include <string>
#include <vector>

//-----------------------------------------------

TEST(mixed_data_set_collector, constructor)
{
    using namespace midi;

    auto c = midi::mixed_data_set_collector({});
}

//-----------------------------------------------

TEST(mixed_data_set_collector, set_callback)
{
    using namespace midi;

    auto c = midi::mixed_data_set_collector({});

    bool called = false;
    auto f      = [&called](const mixed_data_set&, uint4_t) { called = true; };
    c.set_callback(f);

    // header only chunk (1 of 1) completes a mixed data set
    c.feed({ 0x50800010, 0x00010001, 0x007E0000, 0 });
    EXPECT_TRUE(called);
}

//-----------------------------------------------

TEST(mixed_data_set_collector, regular_collect)
{
    using namespace midi;

    {
        for (const auto& entry : mixed_data_set_test_cases)
        {
            bool output_generated = false;
            auto cb               = [&](const midi::mixed_data_set& m, uint4_t mds_id) {
                output_generated = true;
                EXPECT_EQ(mds_id, entry.mds_id) << entry.description;
                EXPECT_EQ(m, entry.mds) << entry.description;
            };

            auto c = midi::mixed_data_set_collector{ cb };
            for (const auto& p : entry.packets)
            {
                c.feed(p);
            }

            EXPECT_TRUE(output_generated) << entry.description;
        }
    }

    {
        bool     output_generated = false;
        unsigned curEntry         = 0;
        auto     cb               = [&](const midi::mixed_data_set& m, uint4_t mds_id) {
            output_generated = true;
            EXPECT_EQ(mds_id, mixed_data_set_test_cases[curEntry].mds_id)
              << mixed_data_set_test_cases[curEntry].description;
            EXPECT_EQ(m, mixed_data_set_test_cases[curEntry].mds) << mixed_data_set_test_cases[curEntry].description;
        };

        auto c = midi::mixed_data_set_collector{ cb };

        for (const auto& entry : mixed_data_set_test_cases)
        {
            for (const auto& p : entry.packets)
            {
                c.feed(p);
            }

            EXPECT_TRUE(output_generated) << entry.description;
            output_generated = false;
            ++curEntry;
        }
    }
}

//-----------------------------------------------

TEST(mixed_data_set_collector, interleaved_mds_ids)
{
    using namespace midi;

    // two mixed data sets with different mds ids can be collected simultaneously
    const auto& a = mixed_data_set_test_cases[3]; // "two chunks", mds_id 0x7
    const auto& b = mixed_data_set_test_cases[4]; // "unknown number of chunks", mds_id 0x1

    unsigned deliveries = 0;
    auto     cb         = [&](const midi::mixed_data_set& m, uint4_t mds_id) {
        ++deliveries;
        if (mds_id == a.mds_id)
        {
            EXPECT_EQ(m, a.mds);
        }
        else
        {
            EXPECT_EQ(mds_id, b.mds_id);
            EXPECT_EQ(m, b.mds);
        }
    };

    auto c = midi::mixed_data_set_collector{ cb };

    ASSERT_EQ(a.packets.size(), b.packets.size());
    for (size_t i = 0; i < a.packets.size(); ++i)
    {
        c.feed(a.packets[i]);
        c.feed(b.packets[i]);
    }

    EXPECT_EQ(2u, deliveries);
}

//-----------------------------------------------

TEST(mixed_data_set_collector, abort)
{
    using namespace midi;

    bool output_generated = false;
    auto cb               = [&](const midi::mixed_data_set&, uint4_t) { output_generated = true; };

    auto c = midi::mixed_data_set_collector{ cb };

    // first chunk of two
    c.feed({ 0x50870020, 0x00020001, 0x007F0001, 0x00020003 });
    c.feed({ 0x5097A0A1, 0xA2A3A4A5, 0xA6A7A8A9, 0xAAABACAD });
    EXPECT_FALSE(output_generated);

    // abort: chunk_nr == 0
    c.feed({ 0x50870010, 0x00020000, 0x007F0001, 0x00020003 });
    EXPECT_FALSE(output_generated);

    // second chunk of the aborted set is ignored
    c.feed({ 0x50870015, 0x00020002, 0x007F0001, 0x00020003 });
    c.feed({ 0x5097AEAF, 0xB0000000, 0, 0 });
    EXPECT_FALSE(output_generated);

    // a new mixed data set with the same mds id is collected
    const auto& entry = mixed_data_set_test_cases[3];
    for (const auto& p : entry.packets)
    {
        c.feed(p);
    }
    EXPECT_TRUE(output_generated);
}

//-----------------------------------------------

TEST(mixed_data_set_collector, exceptional_collect)
{
    using namespace midi;

    // payload packet without header is ignored
    {
        auto cb = [&](const midi::mixed_data_set&, uint4_t) { GTEST_FAIL(); };

        auto c = midi::mixed_data_set_collector{ cb };
        c.feed({ 0x50901122, 0x33445500, 0, 0 });
    }

    // chunk 2 without chunk 1 is ignored
    {
        auto cb = [&](const midi::mixed_data_set&, uint4_t) { GTEST_FAIL(); };

        auto c = midi::mixed_data_set_collector{ cb };
        c.feed({ 0x50870015, 0x00020002, 0x007F0001, 0x00020003 });
        c.feed({ 0x5097AEAF, 0xB0000000, 0, 0 });
    }

    // out of order chunk resets the collection
    {
        bool output_generated = false;
        auto cb               = [&](const midi::mixed_data_set&, uint4_t) { output_generated = true; };

        auto c = midi::mixed_data_set_collector{ cb };

        // chunk 1 of 3
        c.feed({ 0x50870020, 0x00030001, 0x007F0001, 0x00020003 });
        c.feed({ 0x5097A0A1, 0xA2A3A4A5, 0xA6A7A8A9, 0xAAABACAD });

        // chunk 3 of 3 out of order
        c.feed({ 0x50870015, 0x00030003, 0x007F0001, 0x00020003 });
        c.feed({ 0x5097AEAF, 0xB0000000, 0, 0 });

        EXPECT_FALSE(output_generated);
    }

    // invalid valid_bytes_in_chunk resets the collection
    {
        bool output_generated = false;
        auto cb               = [&](const midi::mixed_data_set&, uint4_t) { output_generated = true; };

        auto c = midi::mixed_data_set_collector{ cb };

        // valid bytes < 16 is invalid
        c.feed({ 0x5087000F, 0x00010001, 0x007F0001, 0x00020003 });
        EXPECT_FALSE(output_generated);

        // regular message on same mds id is collected afterwards
        const auto& entry = mixed_data_set_test_cases[3];
        for (const auto& p : entry.packets)
        {
            c.feed(p);
        }
        EXPECT_TRUE(output_generated);
    }

    // non mixed data set packets are ignored
    {
        bool output_generated = false;
        auto cb               = [&](const midi::mixed_data_set& m, uint4_t) {
            output_generated = true;
            EXPECT_EQ(m, mixed_data_set_test_cases[1].mds);
        };

        auto c = midi::mixed_data_set_collector{ cb };

        const auto& entry = mixed_data_set_test_cases[1];
        c.feed(entry.packets[0]);
        c.feed({ 0x51010000, 0, 1, 2 });        // sysex8 packet
        c.feed(universal_packet{ 0x21903C7F }); // midi1 note on
        c.feed(entry.packets[1]);

        EXPECT_TRUE(output_generated);
    }
}

//-----------------------------------------------

TEST(mixed_data_set_collector, limited_data_size_collect)
{
    using namespace midi;

    // message within limit is collected
    {
        bool output_generated = false;
        auto cb               = [&](const midi::mixed_data_set& m, uint4_t) {
            output_generated = true;
            EXPECT_EQ(m.data.size(), 20u);
        };

        auto c = midi::mixed_data_set_collector{ cb };
        c.set_max_data_size(20);

        for (const auto& p : mixed_data_set_test_cases[2].packets)
        {
            c.feed(p);
        }

        EXPECT_TRUE(output_generated);
    }

    // message exceeding limit is discarded
    {
        bool output_generated = false;
        auto cb               = [&](const midi::mixed_data_set&, uint4_t) { output_generated = true; };

        auto c = midi::mixed_data_set_collector{ cb };
        c.set_max_data_size(19);

        for (const auto& p : mixed_data_set_test_cases[2].packets)
        {
            c.feed(p);
        }

        EXPECT_FALSE(output_generated);
    }

    // limit applies to accumulated chunks
    {
        bool output_generated = false;
        auto cb               = [&](const midi::mixed_data_set&, uint4_t) { output_generated = true; };

        auto c = midi::mixed_data_set_collector{ cb };
        c.set_max_data_size(15);

        // "two chunks" test case carries 14 + 3 bytes
        for (const auto& p : mixed_data_set_test_cases[3].packets)
        {
            c.feed(p);
        }

        EXPECT_FALSE(output_generated);
    }
}

//-----------------------------------------------

TEST(mixed_data_set_collector, roundtrip)
{
    using namespace midi;

    for (const auto& entry : mixed_data_set_test_cases)
    {
        bool output_generated = false;
        auto cb               = [&](const midi::mixed_data_set& m, uint4_t mds_id) {
            output_generated = true;
            EXPECT_EQ(mds_id, entry.mds_id) << entry.description;
            EXPECT_EQ(m, entry.mds) << entry.description;
        };

        auto c = midi::mixed_data_set_collector{ cb };

        for (const auto& p : as_mixed_data_set_packets(entry.mds, entry.mds_id))
        {
            c.feed(p);
        }

        EXPECT_TRUE(output_generated) << entry.description;
    }
}

//-----------------------------------------------

TEST(mixed_data_set_collector, reset)
{
    using namespace midi;

    bool output_generated = false;
    auto cb               = [&](const midi::mixed_data_set&, uint4_t) { output_generated = true; };

    auto c = midi::mixed_data_set_collector{ cb };

    // first chunk of two
    c.feed({ 0x50870020, 0x00020001, 0x007F0001, 0x00020003 });
    c.feed({ 0x5097A0A1, 0xA2A3A4A5, 0xA6A7A8A9, 0xAAABACAD });

    c.reset();

    // second chunk of the reset collection is ignored
    c.feed({ 0x50870015, 0x00020002, 0x007F0001, 0x00020003 });
    c.feed({ 0x5097AEAF, 0xB0000000, 0, 0 });

    EXPECT_FALSE(output_generated);
}

//-----------------------------------------------

TEST(mixed_data_set_collector, restart_without_abort)
{
    using namespace midi;

    // a chunk 1 header while a collection is in progress starts a new mixed data set,
    // discarding the incomplete one
    unsigned deliveries = 0;
    auto     cb         = [&](const midi::mixed_data_set& m, uint4_t) {
        ++deliveries;
        EXPECT_EQ(m,
                  (midi::mixed_data_set{
                    midi::manufacturer::universal_realtime, 0x0001, 0x0002, 0x0003, { 0xAE, 0xAF, 0xB0 } }));
    };

    auto c = midi::mixed_data_set_collector{ cb };

    // first chunk of two
    c.feed({ 0x50870020, 0x00020001, 0x007F0001, 0x00020003 });
    c.feed({ 0x5097A0A1, 0xA2A3A4A5, 0xA6A7A8A9, 0xAAABACAD });
    EXPECT_EQ(0u, deliveries);

    // new single chunk mixed data set on the same mds id
    c.feed({ 0x50870015, 0x00010001, 0x007F0001, 0x00020003 });
    c.feed({ 0x5097AEAF, 0xB0000000, 0, 0 });

    EXPECT_EQ(1u, deliveries);
}

//-----------------------------------------------

TEST(mixed_data_set_collector, header_only_final_chunk)
{
    using namespace midi;

    // a sender that runs out of payload data terminates the mixed data set with a
    // header only chunk (valid bytes = 16) declaring the final chunk counts
    unsigned deliveries = 0;
    auto     cb         = [&](const midi::mixed_data_set& m, uint4_t mds_id) {
        ++deliveries;
        EXPECT_EQ(0x1u, mds_id);
        EXPECT_EQ(m, (midi::mixed_data_set{ midi::manufacturer::ableton, 0, 0, 0, { 0xDE, 0xAD, 0xBE, 0xEF } }));
    };

    auto c = midi::mixed_data_set_collector{ cb };

    // chunk 1, unknown number of chunks
    c.feed({ 0x50810016, 0x00000001, 0xA11D0000, 0x00000000 });
    c.feed({ 0x5091DEAD, 0xBEEF0000, 0, 0 });
    EXPECT_EQ(0u, deliveries);

    // final chunk 2 of 2, header bytes only
    c.feed({ 0x50810010, 0x00020002, 0xA11D0000, 0x00000000 });
    EXPECT_EQ(1u, deliveries);
}

//-----------------------------------------------

TEST(mixed_data_set_collector, chunk_nr_beyond_nr_of_chunks)
{
    using namespace midi;

    bool output_generated = false;
    auto cb               = [&](const midi::mixed_data_set&, uint4_t) { output_generated = true; };

    auto c = midi::mixed_data_set_collector{ cb };

    // chunk 1 of 2
    c.feed({ 0x50870020, 0x00020001, 0x007F0001, 0x00020003 });
    c.feed({ 0x5097A0A1, 0xA2A3A4A5, 0xA6A7A8A9, 0xAAABACAD });

    // contradictory header: chunk 2 of 1
    c.feed({ 0x50870015, 0x00010002, 0x007F0001, 0x00020003 });
    c.feed({ 0x5097AEAF, 0xB0000000, 0, 0 });
    EXPECT_FALSE(output_generated);

    // regular message on same mds id is collected afterwards
    for (const auto& p : mixed_data_set_test_cases[3].packets)
    {
        c.feed(p);
    }
    EXPECT_TRUE(output_generated);
}

//-----------------------------------------------

TEST(mixed_data_set_collector, valid_bytes_payload_underflow)
{
    using namespace midi;

    bool output_generated = false;
    auto cb               = [&](const midi::mixed_data_set&, uint4_t) { output_generated = true; };

    auto c = midi::mixed_data_set_collector{ cb };

    // valid bytes = 17 declares a payload packet with less than its two status bytes
    c.feed({ 0x50870011, 0x00010001, 0x007F0001, 0x00020003 });
    c.feed({ 0x5097AEAF, 0xB0000000, 0, 0 });
    EXPECT_FALSE(output_generated);

    // regular message on same mds id is collected afterwards
    for (const auto& p : mixed_data_set_test_cases[3].packets)
    {
        c.feed(p);
    }
    EXPECT_TRUE(output_generated);
}

//-----------------------------------------------

TEST(mixed_data_set_collector, stray_payload_between_chunks)
{
    using namespace midi;

    // a payload packet between a completed chunk and the next chunk header is ignored
    unsigned deliveries = 0;
    auto     cb         = [&](const midi::mixed_data_set& m, uint4_t) {
        ++deliveries;
        EXPECT_EQ(m, mixed_data_set_test_cases[3].mds);
    };

    auto c = midi::mixed_data_set_collector{ cb };

    const auto& packets = mixed_data_set_test_cases[3].packets; // "two chunks"

    c.feed(packets[0]);                             // chunk 1 header
    c.feed(packets[1]);                             // chunk 1 payload, chunk complete
    c.feed({ 0x50971234, 0x56789ABC, 0xDEF00000 }); // stray payload packet
    c.feed(packets[2]);                             // chunk 2 header
    c.feed(packets[3]);                             // chunk 2 payload

    EXPECT_EQ(1u, deliveries);
}

//-----------------------------------------------

TEST(mixed_data_set_collector, no_callback)
{
    using namespace midi;

    // a collector without callback safely swallows complete messages
    auto c = midi::mixed_data_set_collector({});

    for (const auto& entry : mixed_data_set_test_cases)
    {
        for (const auto& p : entry.packets)
        {
            c.feed(p);
        }
    }
}
