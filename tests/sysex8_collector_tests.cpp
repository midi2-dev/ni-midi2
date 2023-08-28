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

#include <gtest/gtest.h>

#include <midi/sysex_collector.h>

#include <midi/extended_data_message.h>

#include "sysex8_test_data.h"

#include <string>
#include <vector>

//-----------------------------------------------

class sysex8_collector : public ::testing::Test
{
  public:
    using packet_vector = std::vector<midi::universal_packet>;
    struct manufacturer_id_test_case
    {
        std::string          name;
        midi::manufacturer_t manufacturer_id;
        packet_vector        packets;

        void run() const
        {
            bool output_generated = false;
            auto cb               = [&](const midi::sysex8& sx, uint8_t) {
                output_generated = true;
                EXPECT_EQ(manufacturer_id, sx.manufacturerID) << name;
            };

            auto c = midi::sysex8_collector{ cb };
            for (const auto& p : packets)
            {
                c.feed(p);
            }
            EXPECT_TRUE(output_generated) << name;
        }
    };
};

//-----------------------------------------------

TEST_F(sysex8_collector, constructor)
{
    using namespace midi;

    auto c = midi::sysex8_collector({});
}

//-----------------------------------------------

TEST_F(sysex8_collector, set_callback)
{
    using namespace midi;

    auto c = midi::sysex8_collector({});

    bool called = false;
    auto f      = [&called](const sysex8&, uint8_t) { called = true; };
    c.set_callback(f);

    c.feed(make_sysex8_complete_packet(0));
    EXPECT_TRUE(called);
}

//-----------------------------------------------

TEST_F(sysex8_collector, one_byte_manufacturer_id)
{
    using namespace midi;

    for (const auto& t : std::vector<manufacturer_id_test_case>{
           { "universal non-realtime single message",
             manufacturer::universal_non_realtime,
             { { 0x55030F00, 0x7E000000 } } },
           { "universal non-realtime multi message",
             manufacturer::universal_non_realtime,
             { { 0x55120F00, 0 }, { 0x55320F7E, 0 } } },
           { "universal realtime", manufacturer::universal_realtime, { { 0x55030F00, 0x7F000000 } } },
           { "ppg", manufacturer::ppg, { { 0x55030F00, 0x29000000 } } },
         })
    {
        t.run();
    }
}

//-----------------------------------------------

TEST_F(sysex8_collector, three_byte_manufacturer_id)
{
    using namespace midi;

    for (const auto& t : std::vector<manufacturer_id_test_case>{
           { "motu", manufacturer::motu, { { 0x55030F80, 0x3B000000 } } },
           { "mackie", manufacturer::mackie, { { 0x55120F80, 0 }, { 0x55320F66, 0 } } },
           { "microsoft", manufacturer::microsoft, { { 0x55030F81, 0x0A000000 } } },
           { "google", manufacturer::google, { { 0x55030F82, 0x0D000000 } } },
           { "native instruments", manufacturer::native_instruments, { { 0x55030FA1, 0x09000000 } } },
         })
    {
        t.run();
    }
}

//-----------------------------------------------

TEST_F(sysex8_collector, invalid_manufacturer_id)
{
    using namespace midi;

    for (const auto& t : std::vector<manufacturer_id_test_case>{
           { "message too short", 0x000000, { { 0x50014482, 0x23000000 } } },
           { "one byte non-zero first byte", 0x000000, { { 0x55030F39, 0x5B000000 } } },
           { "one byte zero", 0x000000, { { 0x55030F80, 0 } } },
           { "three byte zero", 0x000000, { { 0x55030F00, 0 } } },
           { "one byte ignore high bit", 0x7B0000, { { 0x55030F00, 0xFB000000 } } },
           { "three byte ignore high bit", 0x00147B, { { 0x55030F94, 0xFB000000 } } },
         })
    {
        t.run();
    }
}

//-----------------------------------------------

TEST_F(sysex8_collector, stream_id)
{
    auto cb = [](const midi::sysex8&, uint8_t) {};

    // default stream id is zero
    {
        midi::sysex8_collector c(cb);
        EXPECT_EQ(0u, c.stream_id());
    }

    // stream id is 0 after complete packet
    {
        midi::sysex8_collector c(cb);
        c.feed({ 0x55030F80, 0x3B000000 });
        EXPECT_EQ(0u, c.stream_id());
    }

    // extract stream id from start packet
    {
        midi::sysex8_collector c(cb);
        c.feed({ 0x5512AC80, 0 });
        EXPECT_EQ(0xACu, c.stream_id());
    }

    // ignore stream ids from continue or end packets
    {
        midi::sysex8_collector c(cb);

        c.feed({ 0x55230FA1, 0x09000000 });
        EXPECT_EQ(0u, c.stream_id());

        c.feed({ 0x55339FA1, 0x09000000 });
        EXPECT_EQ(0u, c.stream_id());
    }
}

//-----------------------------------------------

TEST_F(sysex8_collector, stream_id_switch)
{
    // change stream id for new message
    {
        bool                   output_generated = false;
        auto                   cb               = [&](const midi::sysex8&, uint8_t) { output_generated = true; };
        midi::sysex8_collector c(cb);

        c.feed({ 0x55130F80, 0x3B000000 });
        EXPECT_EQ(0x0Fu, c.stream_id());

        c.feed({ 0x55310F44, 0 });
        EXPECT_TRUE(output_generated);

        c.feed({ 0x59149A81, 0x3B440000 });
        EXPECT_EQ(0x9Au, c.stream_id());
    }

    // discard incomplete packet
    {
        bool                   output_generated = false;
        auto                   cb               = [&](const midi::sysex8& sx, uint8_t) { output_generated = true; };
        midi::sysex8_collector c(cb);

        c.feed({ 0x5512AC80, 0 });
        EXPECT_EQ(0xACu, c.stream_id());

        c.feed({ 0x59123F80, 0 });
        EXPECT_EQ(0x3Fu, c.stream_id());
        EXPECT_FALSE(output_generated);
    }
}

//-----------------------------------------------

TEST_F(sysex8_collector, other_stream_id_ignored)
{
    // ignore packets with other stream id
    {
        midi::sysex8 result{ 0x001234, { 0xAA, 0xBB, 0xCC } };

        bool output_generated = false;
        auto cb               = [&](const midi::sysex8& sx, uint8_t stream_id) {
            output_generated = true;
            EXPECT_EQ(sx, result);
        };
        midi::sysex8_collector c(cb);

        c.feed({ 0x5515AC92, 0x34AABB00 }); // establish stream id 0xAC
        EXPECT_EQ(0xACu, c.stream_id());

        c.feed({ 0x55243F12, 0x23560000 }); // ignore other stream ids and data
        EXPECT_EQ(0xACu, c.stream_id());
        EXPECT_FALSE(output_generated);

        c.feed({ 0x5532ACCC, 0 }); // finish message with stream id 0xAC
        EXPECT_EQ(0u, c.stream_id());
        EXPECT_TRUE(output_generated);
    }
}

//-----------------------------------------------

TEST_F(sysex8_collector, regular_collect)
{
    using namespace midi;

    {
        for (const auto& entry : sysex8_test_cases)
        {
            bool output_generated = false;
            auto cb               = [&](const midi::sysex8& sx, uint8_t stream_id) {
                output_generated = true;
                EXPECT_EQ(stream_id, entry.stream_id) << entry.description;
                EXPECT_EQ(sx, entry.sysex) << entry.description;
            };

            auto c = midi::sysex8_collector{ cb };
            for (const auto& p : entry.packets)
            {
                c.feed(p);
            }

            EXPECT_TRUE(output_generated);
        }
    }

    {
        bool     output_generated = false;
        unsigned curEntry         = 0;
        auto     cb               = [&](const midi::sysex8& sx, uint8_t stream_id) {
            output_generated = true;
            EXPECT_EQ(stream_id, sysex8_test_cases[curEntry].stream_id) << sysex8_test_cases[curEntry].description;
            EXPECT_EQ(sx, sysex8_test_cases[curEntry].sysex) << sysex8_test_cases[curEntry].description;
        };

        auto c = midi::sysex8_collector{ cb };

        for (const auto& entry : sysex8_test_cases)
        {
            for (const auto& p : entry.packets)
            {
                c.feed(p);
            }

            EXPECT_TRUE(output_generated);
            output_generated = false;
            ++curEntry;
        }
    }
}

//-----------------------------------------------

TEST_F(sysex8_collector, exceptional_collect)
{
    using namespace midi;

    // single end packet will be ignored
    {
        auto cb = [&](const midi::sysex8&, uint8_t) { GTEST_FAIL(); };

        auto c = midi::sysex8_collector{ cb };

        auto p = make_sysex8_end_packet(0);
        p.add_payload_byte(11);
        c.feed(p);
    }

    // continue + end packet will be ignored
    {
        auto cb = [&](const midi::sysex8&, uint8_t) { GTEST_FAIL(); };

        auto c = midi::sysex8_collector{ cb };

        auto p1 = make_sysex8_continue_packet(0);
        p1.add_payload_byte(10);
        c.feed(p1);

        auto p2 = make_sysex8_end_packet(0);
        p1.add_payload_byte(11);
        c.feed(p2);
    }

    // sysex will be collected after invalid packets
    {
        std::vector<universal_packet> packets{ { 0x51441234, 0 }, { 0x5107018F, 0x1E2D3C4B, 0x5A000000 } };

        midi::sysex8 result{ 0x000F1E, { 0x2D, 0x3C, 0x4B, 0x5A } };

        bool output_generated = false;
        auto cb               = [&](const midi::sysex8& sx, uint8_t stream_id) {
            output_generated = true;
            EXPECT_EQ(1, stream_id);
            EXPECT_EQ(sx, result);
        };

        auto c = midi::sysex8_collector{ cb };

        for (const auto& p : packets)
        {
            c.feed(p);
        }

        EXPECT_TRUE(output_generated);
    }

    // incomplete sysex will be discarded
    {
        std::vector<universal_packet> packets{ { 0x55160F1E, 0x2D3C4B5A },
                                               { 0x55220F78, 0x00000000 },
                                               { 0x55033100, 0x7D000000 } };

        midi::sysex8 result{ 0x7D0000 };

        bool output_generated = false;
        auto cb               = [&](const midi::sysex8& sx, uint8_t stream_id) {
            output_generated = true;
            EXPECT_EQ(0x31u, stream_id);
            EXPECT_EQ(sx, result);
        };

        auto c = midi::sysex8_collector{ cb };

        for (const auto& p : packets)
        {
            c.feed(p);
        }

        EXPECT_TRUE(output_generated);
    }

    // empty sysex end packet triggers callback
    {
        std::vector<universal_packet> packets{ { 0x5E197C00, 0x7E123456, 0x789ABC00 },
                                               { 0x5E267CDE, 0xFEDCBA98 },
                                               { 0x5E317C00, 0 } };

        midi::sysex8 result{ 0x7E0000, { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xFE, 0xDC, 0xBA, 0x98 } };

        bool output_generated = false;
        auto cb               = [&](const midi::sysex8& sx, uint8_t stream_id) {
            output_generated = true;
            EXPECT_EQ(0x7Cu, stream_id);
            EXPECT_EQ(sx, result);
        };

        auto c = midi::sysex8_collector{ cb };

        for (const auto& p : packets)
        {
            c.feed(p);
        }

        EXPECT_TRUE(output_generated);
    }
}

//-----------------------------------------------

TEST_F(sysex8_collector, limited_data_size_collect)
{
    using namespace midi;

    {
        bool output_generated = false;
        auto cb               = [&](const midi::sysex8& sx, uint8_t) {
            output_generated = true;
            EXPECT_EQ(sx.data.size(), 13);
        };

        auto c = midi::sysex8_collector{ cb };
        c.set_max_sysex_data_size(256);

        std::vector<universal_packet> packets{ { 0x50182382, 0x54567809 },
                                               { 0x5026232B, 0x5C4D5E6F },
                                               { 0x50342300, 0 } };

        for (const auto& p : packets)
        {
            c.feed(p);
        }

        EXPECT_TRUE(output_generated);
    }

    {
        bool output_generated = false;
        auto cb               = [&](const midi::sysex8&, uint8_t) { output_generated = true; };

        auto c = midi::sysex8_collector{ cb };
        c.set_max_sysex_data_size(11);

        std::vector<universal_packet> packets{ { 0x50192382, 0x54567809, 0x54567809 },
                                               { 0x5026232B, 0x5C4D5E6F },
                                               { 0x50322300, 0 } };

        for (const auto& p : packets)
        {
            c.feed(p);
        }

        EXPECT_FALSE(output_generated);
    }

    {
        bool output_generated = false;
        auto cb               = [&](const midi::sysex8& sx, uint8_t) {
            output_generated = true;
            EXPECT_EQ(sx.data.size(), 11);
        };

        auto c = midi::sysex8_collector{ cb };
        c.set_max_sysex_data_size(12);

        std::vector<universal_packet> packets{ { 0x50182382, 0x54567809, 0x54567809 },
                                               { 0x5027232B, 0x5C4D5E6F },
                                               { 0x50312300, 0 } };

        for (const auto& p : packets)
        {
            c.feed(p);
        }

        EXPECT_TRUE(output_generated);
    }
}

//-----------------------------------------------
