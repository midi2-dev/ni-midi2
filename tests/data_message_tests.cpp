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

#include <midi/data_message.h>

//-----------------------------------------------

TEST(data_message, constructors)
{
    using namespace midi;

    {
        constexpr midi::data_message m;

        EXPECT_EQ(packet_type::data, m.type());
        EXPECT_EQ(0u, m.group());
        EXPECT_EQ(data_status::sysex7_complete, m.status());
        EXPECT_EQ(0u, m.payload_size());
    }

    {
        constexpr midi::data_message m{ data_status::sysex7_end };

        EXPECT_EQ(packet_type::data, m.type());
        EXPECT_EQ(0u, m.group());
        EXPECT_EQ(data_status::sysex7_end, m.status());
        EXPECT_EQ(0u, m.payload_size());
    }
}

//-----------------------------------------------

TEST(data_message, make_sysex7_complete_packet)
{
    using namespace midi;

    {
        constexpr auto m = make_sysex7_complete_packet();

        EXPECT_EQ(packet_type::data, m.type());
        EXPECT_EQ(0u, m.group());
        EXPECT_EQ(data_status::sysex7_complete, m.status());
        EXPECT_EQ(0u, m.payload_size());
    }

    {
        constexpr auto m = make_sysex7_complete_packet(5);

        EXPECT_EQ(packet_type::data, m.type());
        EXPECT_EQ(5u, m.group());
        EXPECT_EQ(data_status::sysex7_complete, m.status());
        EXPECT_EQ(0u, m.payload_size());
    }
}

//-----------------------------------------------

TEST(data_message, make_sysex7_start_packet)
{
    using namespace midi;

    {
        constexpr auto m = make_sysex7_start_packet();

        EXPECT_EQ(packet_type::data, m.type());
        EXPECT_EQ(0u, m.group());
        EXPECT_EQ(data_status::sysex7_start, m.status());
        EXPECT_EQ(0u, m.payload_size());
    }

    {
        constexpr auto m = make_sysex7_start_packet(0xF);

        EXPECT_EQ(packet_type::data, m.type());
        EXPECT_EQ(0xFu, m.group());
        EXPECT_EQ(data_status::sysex7_start, m.status());
        EXPECT_EQ(0u, m.payload_size());
    }
}

//-----------------------------------------------

TEST(data_message, make_sysex7_continue_packet)
{
    using namespace midi;

    {
        constexpr auto m = make_sysex7_continue_packet();

        EXPECT_EQ(packet_type::data, m.type());
        EXPECT_EQ(0u, m.group());
        EXPECT_EQ(data_status::sysex7_continue, m.status());
        EXPECT_EQ(0u, m.payload_size());
    }

    {
        constexpr auto m = make_sysex7_continue_packet(9);

        EXPECT_EQ(packet_type::data, m.type());
        EXPECT_EQ(9u, m.group());
        EXPECT_EQ(data_status::sysex7_continue, m.status());
        EXPECT_EQ(0u, m.payload_size());
    }
}

//-----------------------------------------------

TEST(data_message, make_sysex7_end_packet)
{
    using namespace midi;

    {
        constexpr auto m = make_sysex7_end_packet();

        EXPECT_EQ(packet_type::data, m.type());
        EXPECT_EQ(0u, m.group());
        EXPECT_EQ(data_status::sysex7_end, m.status());
        EXPECT_EQ(0u, m.payload_size());
    }

    {
        constexpr auto m = make_sysex7_end_packet(1);

        EXPECT_EQ(packet_type::data, m.type());
        EXPECT_EQ(1u, m.group());
        EXPECT_EQ(data_status::sysex7_end, m.status());
        EXPECT_EQ(0u, m.payload_size());
    }
}

//-----------------------------------------------

TEST(data_message, payload_size)
{
    using namespace midi;

    auto m = make_sysex7_complete_packet();

    EXPECT_EQ(0u, m.payload_size());

    {
        m.set_byte(1, data_status::sysex7_complete + 3);
        EXPECT_EQ(3u, m.payload_size());
    }

    {
        m.set_byte(1, data_status::sysex7_complete + 6);
        EXPECT_EQ(6u, m.payload_size());
    }

    {
        m.set_byte(1, data_status::sysex7_complete + 8);
        EXPECT_EQ(8u, m.payload_size());
    }

    {
        m.set_byte(1, data_status::sysex7_complete + 0);
        EXPECT_EQ(0u, m.payload_size());
    }
}

//-----------------------------------------------

TEST(data_message, set_payload_size)
{
    using namespace midi;

    {
        auto m = make_sysex7_complete_packet();
        EXPECT_EQ(0u, m.payload_size());

        m.set_payload_size(6);
        EXPECT_EQ(6u + data_status::sysex7_complete, m.status());
        EXPECT_EQ(6u, m.payload_size());
    }

    {
        auto m = make_sysex7_start_packet();
        EXPECT_EQ(0u, m.payload_size());

        m.set_payload_size(3);
        EXPECT_EQ(3u + data_status::sysex7_start, m.status());
        EXPECT_EQ(3u, m.payload_size());
    }

    {
        auto m = make_sysex7_continue_packet();
        EXPECT_EQ(0u, m.payload_size());

        m.set_payload_size(0);
        EXPECT_EQ(0u + data_status::sysex7_continue, m.status());
        EXPECT_EQ(0u, m.payload_size());
    }
}

//-----------------------------------------------

TEST(data_message, payload_byte)
{
    using namespace midi;

    auto m = make_sysex7_complete_packet();

    m.set_payload_size(6);
    m.set_byte(2, 0x11);
    m.set_byte(3, 0x22);
    m.set_byte(4, 0x33);
    m.set_byte(5, 0x44);
    m.set_byte(6, 0x55);
    m.set_byte(7, 0x66);

    EXPECT_EQ(0x11u, m.payload_byte(0));
    EXPECT_EQ(0x22u, m.payload_byte(1));
    EXPECT_EQ(0x33u, m.payload_byte(2));
    EXPECT_EQ(0x44u, m.payload_byte(3));
    EXPECT_EQ(0x55u, m.payload_byte(4));
    EXPECT_EQ(0x66u, m.payload_byte(5));
}

//-----------------------------------------------

TEST(data_message, add_payload_byte)
{
    using namespace midi;

    auto m = make_sysex7_continue_packet();

    EXPECT_EQ(0u, m.payload_size());

    {
        m.add_payload_byte(0x12);
        EXPECT_EQ(0x12u, m.get_byte(2));
        EXPECT_EQ(0x12u, m.payload_byte(0));
        EXPECT_EQ(1u, m.payload_size());
        EXPECT_EQ(1u, m.payload_size());
        EXPECT_EQ(1u + data_status::sysex7_continue, m.status());
    }

    {
        m.add_payload_byte(0x7A);
        EXPECT_EQ(0x7Au, m.get_byte(3));
        EXPECT_EQ(0x7Au, m.payload_byte(1));
        EXPECT_EQ(2u, m.payload_size());
        EXPECT_EQ(2u + data_status::sysex7_continue, m.status());
    }

    {
        m.add_payload_byte(0x5B);
        EXPECT_EQ(0x5Bu, m.get_byte(4));
        EXPECT_EQ(0x5Bu, m.payload_byte(2));
        EXPECT_EQ(3u, m.payload_size());
        EXPECT_EQ(3u + data_status::sysex7_continue, m.status());
    }

    {
        m.add_payload_byte(42);
        EXPECT_EQ(42u, m.get_byte(5));
        EXPECT_EQ(42u, m.payload_byte(3));
        EXPECT_EQ(4u, m.payload_size());
        EXPECT_EQ(4u + data_status::sysex7_continue, m.status());
    }

    {
        m.add_payload_byte(99);
        EXPECT_EQ(99u, m.get_byte(6));
        EXPECT_EQ(99u, m.payload_byte(4));
        EXPECT_EQ(5u, m.payload_size());
        EXPECT_EQ(5u + data_status::sysex7_continue, m.status());
    }

    {
        m.add_payload_byte(0xF6);
        // Sysex7 is 7bit, results in 0x76u
        EXPECT_EQ(0x76u, m.get_byte(7));
        EXPECT_EQ(0x76u, m.payload_byte(5));
        EXPECT_EQ(6u, m.payload_size());
        EXPECT_EQ(6u + data_status::sysex7_continue, m.status());
    }
}

//-----------------------------------------------

TEST(data_message, set_payload_byte)
{
    using namespace midi;

    auto m = make_sysex7_complete_packet();

    {
        m.set_payload_byte(0, 0x12);
        EXPECT_EQ(0x12u, m.get_byte(2));
        EXPECT_EQ(0x12u, m.payload_byte(0));
    }

    {
        m.set_payload_byte(5, 0x7A);
        EXPECT_EQ(0x7Au, m.get_byte(7));
        EXPECT_EQ(0x7Au, m.payload_byte(5));
    }

    {
        m.set_payload_byte(4, 0x5B);
        EXPECT_EQ(0x5Bu, m.get_byte(6));
        EXPECT_EQ(0x5Bu, m.payload_byte(4));
    }

    {
        m.set_payload_byte(1, 42);
        EXPECT_EQ(42u, m.get_byte(3));
        EXPECT_EQ(42u, m.payload_byte(1));
    }

    {
        m.set_payload_byte(3, 99);
        EXPECT_EQ(99u, m.get_byte(5));
        EXPECT_EQ(99u, m.payload_byte(3));
    }

    {
        m.set_payload_byte(2, 0xF6);
        // Sysex7 is 7bit, results in 0x76u
        EXPECT_EQ(0x76u, m.get_byte(4));
        EXPECT_EQ(0x76u, m.payload_byte(2));
    }
}

//-----------------------------------------------

TEST(data_message, sysex7_packet_view)
{
    using namespace midi;

    {
        constexpr universal_packet p{ 0x37120605, 0x04030201 };
        EXPECT_TRUE(is_sysex7_packet(p));
        EXPECT_TRUE(as_sysex7_packet_view(p));

        auto m = sysex7_packet_view{ p };
        EXPECT_EQ(7u, m.group());
        EXPECT_EQ(data_status::sysex7_start, m.status());
        EXPECT_EQ(packet_format::start, m.format());
        EXPECT_EQ(2u, m.payload_size());
        EXPECT_EQ(0x06u, m.payload_byte(0));
        EXPECT_EQ(0x05u, m.payload_byte(1));
        EXPECT_EQ(0x04u, m.payload_byte(2));
        EXPECT_EQ(0x03u, m.payload_byte(3));
        EXPECT_EQ(0x02u, m.payload_byte(4));
        EXPECT_EQ(0x01u, m.payload_byte(5));
    }

    {
        constexpr universal_packet p{ 0x3E261234, 0x567890AB };
        EXPECT_TRUE(is_sysex7_packet(p));
        EXPECT_TRUE(as_sysex7_packet_view(p));

        auto m = sysex7_packet_view{ p };
        EXPECT_EQ(0xEu, m.group());
        EXPECT_EQ(data_status::sysex7_continue, m.status());
        EXPECT_EQ(packet_format::cont, m.format());
        EXPECT_EQ(6u, m.payload_size());
        EXPECT_EQ(0x12u, m.payload_byte(0));
        EXPECT_EQ(0x34u, m.payload_byte(1));
        EXPECT_EQ(0x56u, m.payload_byte(2));
        EXPECT_EQ(0x78u, m.payload_byte(3));
        EXPECT_EQ(0x90u, m.payload_byte(4));
        EXPECT_EQ(0xABu, m.payload_byte(5));
    }

    {
        static constexpr universal_packet p{ 0x37120605, 0x04030201 };

        constexpr auto m = sysex7_packet_view{ p };
        EXPECT_EQ(7u, m.group());
        EXPECT_EQ(data_status::sysex7_start, m.status());
        EXPECT_EQ(packet_format::start, m.format());
        EXPECT_EQ(2u, m.payload_size());
        EXPECT_EQ(0x06u, m.payload_byte(0));
        EXPECT_EQ(0x05u, m.payload_byte(1));
        EXPECT_EQ(0x04u, m.payload_byte(2));
        EXPECT_EQ(0x03u, m.payload_byte(3));
        EXPECT_EQ(0x02u, m.payload_byte(4));
        EXPECT_EQ(0x01u, m.payload_byte(5));
    }
}

//-----------------------------------------------

TEST(data_message, is_data_message)
{
    using namespace midi;

    EXPECT_TRUE(is_data_message(universal_packet{ 0x31010000, 0 }));
    EXPECT_TRUE(is_data_message(universal_packet{ 0x3F140000, 0 }));
    EXPECT_TRUE(is_data_message(universal_packet{ 0x33260000, 0 }));
    EXPECT_TRUE(is_data_message(universal_packet{ 0x33880000, 0 }));

    EXPECT_FALSE(is_data_message(universal_packet{ 0x01000000 }));
    EXPECT_FALSE(is_data_message(universal_packet{ 0x19321200 }));
    EXPECT_FALSE(is_data_message(universal_packet{ 0x2691447F }));
    EXPECT_FALSE(is_data_message(universal_packet{ 0x40885060, 0x12340000 }));
    EXPECT_FALSE(is_data_message(universal_packet{ 0x5F0D, 1, 2, 3 }));
}

//-----------------------------------------------

TEST(data_message, is_sysex7_packet)
{
    using namespace midi;

    EXPECT_TRUE(is_sysex7_packet(universal_packet{ 0x31010000, 0 }));
    EXPECT_TRUE(is_sysex7_packet(universal_packet{ 0x3F140000, 0 }));
    EXPECT_TRUE(is_sysex7_packet(universal_packet{ 0x33260000, 0 }));
    EXPECT_TRUE(is_sysex7_packet(universal_packet{ 0x33340000, 0 }));

    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x31070000, 0 }));
    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x3F1F0000, 0 }));
    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x33290000, 0 }));
    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x333C0000, 0 }));

    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x3C58000, 0 }));
    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x39F8000, 0 }));
    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x3160000, 0 }));
    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x3470000, 0 }));
    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x3080000, 0 }));
    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x3C90000, 0 }));
    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x3AA0000, 0 }));
    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x33B0000, 0 }));
    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x3CD0000, 0 }));
    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x3FE0000, 0 }));

    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x01000000 }));
    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x19321200 }));
    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x2691447F }));
    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x40885060, 0x12340000 }));
    EXPECT_FALSE(is_sysex7_packet(universal_packet{ 0x5F0D, 1, 2, 3 }));
}

//-----------------------------------------------
