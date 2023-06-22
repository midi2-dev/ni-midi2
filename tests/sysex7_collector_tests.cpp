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

#include "sysex7_test_data.h"

//-----------------------------------------------

class sysex7_collector : public ::testing::Test
{
  public:
};

//-----------------------------------------------

TEST_F(sysex7_collector, constructor)
{
    using namespace midi;

    auto c = midi::sysex7_collector({});
}

//-----------------------------------------------

TEST_F(sysex7_collector, set_callback)
{
    using namespace midi;

    auto c = midi::sysex7_collector({});

    bool called = false;
    auto f      = [&called](const sysex7&) { called = true; };
    c.set_callback(f);

    c.feed(make_sysex7_complete_packet(0));
    EXPECT_TRUE(called);
}

//-----------------------------------------------

TEST_F(sysex7_collector, regular_collect)
{
    using namespace midi;

    {
        for (const auto& entry : sysex7_test_cases)
        {
            bool output_generated = false;
            auto cb               = [&](midi::sysex sx) {
                output_generated = true;
                EXPECT_EQ(sx, entry.sysex) << entry.description;
            };

            auto c = midi::sysex7_collector{ cb };
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
        auto     cb               = [&](midi::sysex sx) {
            output_generated = true;
            EXPECT_EQ(sx, sysex7_test_cases[curEntry].sysex) << sysex7_test_cases[curEntry].description;
        };

        auto c = midi::sysex7_collector{ cb };

        for (const auto& entry : sysex7_test_cases)
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

TEST_F(sysex7_collector, exceptional_collect)
{
    using namespace midi;

    // single end packet will be ignored
    {
        auto cb = [&](midi::sysex) { GTEST_FAIL(); };

        auto c = midi::sysex7_collector{ cb };

        auto p = make_sysex7_end_packet();
        p.add_payload_byte(11);
        c.feed(p);
    }

    // continue + end packet will be ignored
    {
        auto cb = [&](midi::sysex) { GTEST_FAIL(); };

        auto c = midi::sysex7_collector{ cb };

        auto p1 = make_sysex7_continue_packet();
        p1.add_payload_byte(10);
        c.feed(p1);

        auto p2 = make_sysex7_end_packet();
        p1.add_payload_byte(11);
        c.feed(p2);
    }

    // sysex will be collected after invalid packets
    {
        std::vector<universal_packet> packets{ { 0x31441234, 0 }, { 0x31060F1E, 0x2D3C4B5A } };

        midi::sysex result{ 0x0F0000, { 0x1E, 0x2D, 0x3C, 0x4B, 0x5A } };

        bool output_generated = false;
        auto cb               = [&](midi::sysex sx) {
            output_generated = true;
            EXPECT_EQ(sx, result);
        };

        auto c = midi::sysex7_collector{ cb };

        for (const auto& p : packets)
        {
            c.feed(p);
        }

        EXPECT_TRUE(output_generated);
    }

    // incomplete sysex will be discarded
    {
        std::vector<universal_packet> packets{ { 0x35160F1E, 0x2D3C4B5A },
                                               { 0x35226978, 0x00000000 },
                                               { 0x35017D00, 0 } };

        midi::sysex result{ 0x7D0000 };

        bool output_generated = false;
        auto cb               = [&](midi::sysex sx) {
            output_generated = true;
            EXPECT_EQ(sx, result);
        };

        auto c = midi::sysex7_collector{ cb };

        for (const auto& p : packets)
        {
            c.feed(p);
        }

        EXPECT_TRUE(output_generated);
    }

    // empty sysex end packet triggers callback
    {
        std::vector<universal_packet> packets{ { 0x3E167E12, 0x34567809 },
                                               { 0x3E261A2B, 0x3C4D5E6F },
                                               { 0x3E300000, 0 } };

        midi::sysex result{ 0x7E0000, { 0x12, 0x34, 0x56, 0x78, 0x09, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F } };

        bool output_generated = false;
        auto cb               = [&](midi::sysex sx) {
            output_generated = true;
            EXPECT_EQ(sx, result);
        };

        auto c = midi::sysex7_collector{ cb };

        for (const auto& p : packets)
        {
            c.feed(p);
        }

        EXPECT_TRUE(output_generated);
    }
}

//-----------------------------------------------
