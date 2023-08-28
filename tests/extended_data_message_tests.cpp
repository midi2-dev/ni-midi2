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

#include <midi/extended_data_message.h>

//-----------------------------------------------

TEST(extended_data_message, constructors)
{
    using namespace midi;

    {
        constexpr midi::extended_data_message m;

        EXPECT_EQ(packet_type::extended_data, m.type());
        EXPECT_EQ(0u, m.group());
        EXPECT_EQ(0u, m.status());
    }

    {
        constexpr midi::extended_data_message m{ extended_data_status::sysex8_end + 1 };

        EXPECT_EQ(packet_type::extended_data, m.type());
        EXPECT_EQ(0u, m.group());
        EXPECT_EQ(extended_data_status::sysex8_end + 1, m.status());
    }
}

//-----------------------------------------------

TEST(extended_data_message, is_extended_data_message)
{
    using namespace midi;

    EXPECT_TRUE(is_extended_data_message(universal_packet{ 0x51010000, 1, 2, 3 }));
    EXPECT_TRUE(is_extended_data_message(universal_packet{ 0x5F140000, 4, 5, 6 }));
    EXPECT_TRUE(is_extended_data_message(universal_packet{ 0x53260000, 7, 8, 9 }));
    EXPECT_TRUE(is_extended_data_message(universal_packet{ 0x53880000, 10, 11, 12 }));

    EXPECT_FALSE(is_extended_data_message(universal_packet{ 0x01000000 }));
    EXPECT_FALSE(is_extended_data_message(universal_packet{ 0x19321200 }));
    EXPECT_FALSE(is_extended_data_message(universal_packet{ 0x2691447F }));
    EXPECT_FALSE(is_extended_data_message(universal_packet{ 0x40885060, 0x12340000 }));
    EXPECT_FALSE(is_extended_data_message(universal_packet{ 0x3F0D, 1, 2, 3 }));
}

//-----------------------------------------------

TEST(extended_data_message, sysex8_packet_constructors)
{
    using namespace midi;

    {
        constexpr midi::sysex8_packet m;

        EXPECT_EQ(packet_type::extended_data, m.type());
        EXPECT_EQ(0u, m.group());
        EXPECT_EQ(extended_data_status::sysex8_complete + 1, m.status());
        EXPECT_EQ(packet_format::complete, m.format());

        EXPECT_EQ(0x50010000u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_TRUE(is_extended_data_message(m));
    }

    {
        constexpr midi::sysex8_packet m{ extended_data_status::sysex8_end, 0xAC, 0xC };

        EXPECT_EQ(packet_type::extended_data, m.type());
        EXPECT_EQ(12u, m.group());
        EXPECT_EQ(extended_data_status::sysex8_end + 1, m.status());
        EXPECT_EQ(packet_format::end, m.format());

        EXPECT_EQ(0x5C31AC00u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_TRUE(is_extended_data_message(m));
    }
}

//-----------------------------------------------

TEST(extended_data_message, is_sysex8_packet)
{
    using namespace midi;

    EXPECT_TRUE(is_sysex8_packet(universal_packet{ 0x51010000, 0, 1, 2 }));
    EXPECT_TRUE(is_sysex8_packet(universal_packet{ 0x5F140000, 0, 1, 2 }));
    EXPECT_TRUE(is_sysex8_packet(universal_packet{ 0x53260000, 0, 1, 2 }));
    EXPECT_TRUE(is_sysex8_packet(universal_packet{ 0x53340000, 0, 1, 2 }));
    EXPECT_TRUE(is_sysex8_packet(universal_packet{ 0x51070000, 0, 1, 2 }));
    EXPECT_TRUE(is_sysex8_packet(universal_packet{ 0x53290000, 0, 1, 2 }));
    EXPECT_TRUE(is_sysex8_packet(universal_packet{ 0x533C0000, 0, 1, 2 }));
    EXPECT_TRUE(is_sysex8_packet(universal_packet{ 0x533E0000, 0, 1, 2 }));

    EXPECT_TRUE(is_sysex8_packet(sysex8_packet{}));
    EXPECT_TRUE(is_sysex8_packet(sysex8_packet{ extended_data_status::sysex8_continue, 39, 5 }));

    EXPECT_FALSE(is_sysex8_packet(universal_packet{ 0x5F1F0000, 0, 1, 2 }));

    EXPECT_FALSE(is_sysex8_packet(universal_packet{ 0x5C58000, 0, 1, 2 }));
    EXPECT_FALSE(is_sysex8_packet(universal_packet{ 0x59F8000, 0, 1, 2 }));
    EXPECT_FALSE(is_sysex8_packet(universal_packet{ 0x5160000, 0, 1, 2 }));
    EXPECT_FALSE(is_sysex8_packet(universal_packet{ 0x5470000, 0, 1, 2 }));
    EXPECT_FALSE(is_sysex8_packet(universal_packet{ 0x5080000, 0, 1, 2 }));
    EXPECT_FALSE(is_sysex8_packet(universal_packet{ 0x5C90000, 0, 1, 2 }));
    EXPECT_FALSE(is_sysex8_packet(universal_packet{ 0x5AA0000, 0, 1, 2 }));
    EXPECT_FALSE(is_sysex8_packet(universal_packet{ 0x53B0000, 0, 1, 2 }));
    EXPECT_FALSE(is_sysex8_packet(universal_packet{ 0x5CD0000, 0, 1, 2 }));
    EXPECT_FALSE(is_sysex8_packet(universal_packet{ 0x5FE0000, 0, 1, 2 }));

    EXPECT_FALSE(is_sysex8_packet(universal_packet{ 0x01000000 }));
    EXPECT_FALSE(is_sysex8_packet(universal_packet{ 0x19321200 }));
    EXPECT_FALSE(is_sysex8_packet(universal_packet{ 0x2691447F }));
    EXPECT_FALSE(is_sysex8_packet(universal_packet{ 0x40885060, 0x12340000 }));
    EXPECT_FALSE(is_sysex8_packet(universal_packet{ 0x5F0D, 1, 2, 3 }));
}

//-----------------------------------------------

TEST(extended_data_message, sysex8_packet_format)
{
    using namespace midi;

    {
        auto m = make_sysex8_complete_packet(0);
        EXPECT_EQ(packet_format::complete, m.format());
    }

    {
        auto m = make_sysex8_start_packet(0);
        EXPECT_EQ(packet_format::start, m.format());
    }

    {
        auto m = make_sysex8_continue_packet(0);
        EXPECT_EQ(packet_format::cont, m.format());
    }

    {
        auto m = make_sysex8_end_packet(0);
        EXPECT_EQ(packet_format::end, m.format());
    }
}

//-----------------------------------------------

TEST(extended_data_message, sysex8_packet_stream_id)
{
    using namespace midi;

    auto m = make_sysex8_complete_packet(0);

    EXPECT_EQ(0u, m.stream_id());

    {
        m.set_byte(2, 99);
        EXPECT_EQ(0x50016300u, m.data[0]);
        EXPECT_EQ(99u, m.stream_id());
    }
}

//-----------------------------------------------

TEST(extended_data_message, sysex8_packet_set_stream_id)
{
    using namespace midi;

    auto m = make_sysex8_continue_packet(44);

    EXPECT_EQ(44u, m.stream_id());

    {
        m.set_stream_id(244);
        EXPECT_EQ(0x5021F400u, m.data[0]);
        EXPECT_EQ(244u, m.stream_id());
    }
}

//-----------------------------------------------

TEST(extended_data_message, sysex8_packet_payload_size)
{
    using namespace midi;

    auto m = make_sysex8_complete_packet(0);

    EXPECT_EQ(0u, m.payload_size());

    {
        m.set_byte(1, extended_data_status::sysex8_complete + 3);
        EXPECT_EQ(0x50030000u, m.data[0]);
        EXPECT_EQ(2u, m.payload_size());
    }

    {
        m.set_byte(1, extended_data_status::sysex8_start + 6);
        EXPECT_EQ(0x50160000u, m.data[0]);
        EXPECT_EQ(5u, m.payload_size());
    }

    {
        m.set_byte(1, extended_data_status::sysex8_continue + 8);
        EXPECT_EQ(0x50280000u, m.data[0]);
        EXPECT_EQ(7u, m.payload_size());
    }

    {
        m.set_byte(1, extended_data_status::sysex8_end + 14);
        EXPECT_EQ(0x503E0000u, m.data[0]);
        EXPECT_EQ(13u, m.payload_size());
    }
}

//-----------------------------------------------

TEST(extended_data_message, sysex8_packet_set_payload_size)
{
    using namespace midi;

    {
        auto m = make_sysex8_complete_packet(0);
        EXPECT_EQ(0u, m.payload_size());

        m.set_payload_size(6);
        EXPECT_EQ(0x50070000u, m.data[0]);
        EXPECT_EQ(6u, m.payload_size());
    }

    {
        auto m = make_sysex8_start_packet(1);
        EXPECT_EQ(0u, m.payload_size());

        m.set_payload_size(3);
        EXPECT_EQ(0x50140100u, m.data[0]);
        EXPECT_EQ(3u, m.payload_size());
    }

    {
        auto m = make_sysex8_continue_packet(0xAA);
        EXPECT_EQ(0u, m.payload_size());

        m.set_payload_size(12);
        EXPECT_EQ(0x502DAA00u, m.data[0]);
        EXPECT_EQ(12u, m.payload_size());
    }
}

//-----------------------------------------------

TEST(extended_data_message, sysex8_packet_payload_byte)
{
    using namespace midi;

    auto m = make_sysex8_complete_packet(99);

    m.set_payload_size(13);
    m.set_byte(3, 0x11);
    m.set_byte(4, 0x22);
    m.set_byte(5, 0x33);
    m.set_byte(6, 0x44);
    m.set_byte(7, 0x55);
    m.set_byte(8, 0x66);
    m.set_byte(9, 0x77);
    m.set_byte(10, 0x88);
    m.set_byte(11, 0x99);
    m.set_byte(12, 0xAA);
    m.set_byte(13, 0xBB);
    m.set_byte(14, 0xCC);
    m.set_byte(15, 0xDD);

    EXPECT_EQ(0x11u, m.payload_byte(0));
    EXPECT_EQ(0x22u, m.payload_byte(1));
    EXPECT_EQ(0x33u, m.payload_byte(2));
    EXPECT_EQ(0x44u, m.payload_byte(3));
    EXPECT_EQ(0x55u, m.payload_byte(4));
    EXPECT_EQ(0x66u, m.payload_byte(5));
    EXPECT_EQ(0x77u, m.payload_byte(6));
    EXPECT_EQ(0x88u, m.payload_byte(7));
    EXPECT_EQ(0x99u, m.payload_byte(8));
    EXPECT_EQ(0xAAu, m.payload_byte(9));
    EXPECT_EQ(0xBBu, m.payload_byte(10));
    EXPECT_EQ(0xCCu, m.payload_byte(11));
    EXPECT_EQ(0xDDu, m.payload_byte(12));
}

//-----------------------------------------------

TEST(extended_data_message, sysex8_packet_set_payload_byte)
{
    using namespace midi;

    auto m = make_sysex8_complete_packet(0);

    {
        m.set_payload_byte(0, 0x12);
        EXPECT_EQ(0x12u, m.get_byte(3));
        EXPECT_EQ(0x12u, m.payload_byte(0));
    }

    {
        m.set_payload_byte(5, 0x7A);
        EXPECT_EQ(0x7Au, m.get_byte(8));
        EXPECT_EQ(0x7Au, m.payload_byte(5));
    }

    {
        m.set_payload_byte(4, 0x5B);
        EXPECT_EQ(0x5Bu, m.get_byte(7));
        EXPECT_EQ(0x5Bu, m.payload_byte(4));
    }

    {
        m.set_payload_byte(1, 42);
        EXPECT_EQ(42u, m.get_byte(4));
        EXPECT_EQ(42u, m.payload_byte(1));
    }

    {
        m.set_payload_byte(3, 99);
        EXPECT_EQ(99u, m.get_byte(6));
        EXPECT_EQ(99u, m.payload_byte(3));
    }

    {
        m.set_payload_byte(2, 0xF6);
        EXPECT_EQ(0xF6u, m.get_byte(5));
        EXPECT_EQ(0xF6u, m.payload_byte(2));
    }
}

//-----------------------------------------------

TEST(extended_data_message, sysex8_packet_add_payload_byte)
{
    using namespace midi;

    auto m = make_sysex8_continue_packet(0x3E);

    EXPECT_EQ(0u, m.payload_size());

    {
        m.add_payload_byte(0x12);
        EXPECT_EQ(0x12u, m.get_byte(3));
        EXPECT_EQ(0x12u, m.payload_byte(0));
        EXPECT_EQ(1u, m.payload_size());

        EXPECT_EQ(0x50223E12u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }

    {
        m.add_payload_byte(0x7A);
        EXPECT_EQ(0x7Au, m.get_byte(4));
        EXPECT_EQ(0x7Au, m.payload_byte(1));
        EXPECT_EQ(2u, m.payload_size());

        EXPECT_EQ(0x50233E12u, m.data[0]);
        EXPECT_EQ(0x7A000000u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }

    {
        m.add_payload_byte(0x5B);
        EXPECT_EQ(0x5Bu, m.get_byte(5));
        EXPECT_EQ(0x5Bu, m.payload_byte(2));
        EXPECT_EQ(3u, m.payload_size());

        EXPECT_EQ(0x50243E12u, m.data[0]);
        EXPECT_EQ(0x7A5B0000u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }

    {
        m.add_payload_byte(42);
        EXPECT_EQ(42u, m.get_byte(6));
        EXPECT_EQ(42u, m.payload_byte(3));
        EXPECT_EQ(4u, m.payload_size());

        EXPECT_EQ(0x50253E12u, m.data[0]);
        EXPECT_EQ(0x7A5B2A00u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }

    {
        m.add_payload_byte(99);
        EXPECT_EQ(99u, m.get_byte(7));
        EXPECT_EQ(99u, m.payload_byte(4));
        EXPECT_EQ(5u, m.payload_size());

        EXPECT_EQ(0x50263E12u, m.data[0]);
        EXPECT_EQ(0x7A5B2A63u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }

    {
        m.add_payload_byte(0xF6);
        EXPECT_EQ(0xF6u, m.get_byte(8));
        EXPECT_EQ(0xF6u, m.payload_byte(5));
        EXPECT_EQ(6u, m.payload_size());

        EXPECT_EQ(0x50273E12u, m.data[0]);
        EXPECT_EQ(0x7A5B2A63u, m.data[1]);
        EXPECT_EQ(0xF6000000u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }

    {
        m.add_payload_byte(0xAF);
        EXPECT_EQ(0xAFu, m.get_byte(9));
        EXPECT_EQ(0xAFu, m.payload_byte(6));
        EXPECT_EQ(7u, m.payload_size());

        EXPECT_EQ(0x50283E12u, m.data[0]);
        EXPECT_EQ(0x7A5B2A63u, m.data[1]);
        EXPECT_EQ(0xF6AF0000u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }

    {
        m.add_payload_byte(0x34);
        EXPECT_EQ(0x34u, m.get_byte(10));
        EXPECT_EQ(0x34u, m.payload_byte(7));
        EXPECT_EQ(8u, m.payload_size());

        EXPECT_EQ(0x50293E12u, m.data[0]);
        EXPECT_EQ(0x7A5B2A63u, m.data[1]);
        EXPECT_EQ(0xF6AF3400u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }

    {
        m.add_payload_byte(78);
        EXPECT_EQ(78u, m.get_byte(11));
        EXPECT_EQ(78u, m.payload_byte(8));
        EXPECT_EQ(9u, m.payload_size());

        EXPECT_EQ(0x502A3E12u, m.data[0]);
        EXPECT_EQ(0x7A5B2A63u, m.data[1]);
        EXPECT_EQ(0xF6AF344Eu, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }

    {
        m.add_payload_byte(0x3C);
        EXPECT_EQ(0x3Cu, m.get_byte(12));
        EXPECT_EQ(0x3Cu, m.payload_byte(9));
        EXPECT_EQ(10u, m.payload_size());

        EXPECT_EQ(0x502B3E12u, m.data[0]);
        EXPECT_EQ(0x7A5B2A63u, m.data[1]);
        EXPECT_EQ(0xF6AF344Eu, m.data[2]);
        EXPECT_EQ(0x3C000000u, m.data[3]);
    }
}

//-----------------------------------------------

TEST(extended_data_message, sysex8_packet_view)
{
    using namespace midi;

    {
        constexpr universal_packet p{ 0x5713490D, 0x1C2B3A49, 0x58677685, 0x94A3B2C1 };
        EXPECT_TRUE(is_sysex8_packet(p));
        EXPECT_TRUE(as_sysex8_packet_view(p));

        auto m = sysex8_packet_view{ p };
        EXPECT_EQ(7u, m.group());
        EXPECT_EQ(packet_format::start, m.format());
        EXPECT_EQ(0x49u, m.stream_id());
        EXPECT_EQ(2u, m.payload_size());
        EXPECT_EQ(0x0Du, m.payload_byte(0));
        EXPECT_EQ(0x1Cu, m.payload_byte(1));
        EXPECT_EQ(0x2Bu, m.payload_byte(2));
        EXPECT_EQ(0x3Au, m.payload_byte(3));
        EXPECT_EQ(0x49u, m.payload_byte(4));
        EXPECT_EQ(0x58u, m.payload_byte(5));
        EXPECT_EQ(0x67u, m.payload_byte(6));
        EXPECT_EQ(0x76u, m.payload_byte(7));
        EXPECT_EQ(0x85u, m.payload_byte(8));
        EXPECT_EQ(0x94u, m.payload_byte(9));
        EXPECT_EQ(0xA3u, m.payload_byte(10));
        EXPECT_EQ(0xB2u, m.payload_byte(11));
        EXPECT_EQ(0xC1u, m.payload_byte(12));
    }

    {
        constexpr universal_packet p{ 0x5E27C412, 0x34567890, 0xAB000000 };
        EXPECT_TRUE(is_sysex8_packet(p));
        EXPECT_TRUE(as_sysex8_packet_view(p));

        auto m = sysex8_packet_view{ p };
        EXPECT_EQ(0xEu, m.group());
        EXPECT_EQ(packet_format::cont, m.format());
        EXPECT_EQ(0xC4u, m.stream_id());
        EXPECT_EQ(6u, m.payload_size());
        EXPECT_EQ(0x12u, m.payload_byte(0));
        EXPECT_EQ(0x34u, m.payload_byte(1));
        EXPECT_EQ(0x56u, m.payload_byte(2));
        EXPECT_EQ(0x78u, m.payload_byte(3));
        EXPECT_EQ(0x90u, m.payload_byte(4));
        EXPECT_EQ(0xABu, m.payload_byte(5));
    }

    {
        static constexpr universal_packet p{ 0x57120605, 0x04030201 };

        constexpr auto m = sysex8_packet_view{ p };
        EXPECT_EQ(7u, m.group());
        EXPECT_EQ(packet_format::start, m.format());
        EXPECT_EQ(0x06u, m.stream_id());
        EXPECT_EQ(1u, m.payload_size());
        EXPECT_EQ(0x05u, m.payload_byte(0));
        EXPECT_EQ(0x04u, m.payload_byte(1));
        EXPECT_EQ(0x03u, m.payload_byte(2));
        EXPECT_EQ(0x02u, m.payload_byte(3));
        EXPECT_EQ(0x01u, m.payload_byte(4));
    }

    {
        EXPECT_FALSE(as_sysex8_packet_view(universal_packet{ 0 }));
        EXPECT_FALSE(as_sysex8_packet_view(universal_packet{ 0x12345678 }));
        EXPECT_FALSE(as_sysex8_packet_view(universal_packet{ 0x23456789 }));
        EXPECT_FALSE(as_sysex8_packet_view(universal_packet{ 0x3456789A }));
        EXPECT_FALSE(as_sysex8_packet_view(universal_packet{ 0x456789AB }));
        EXPECT_FALSE(as_sysex8_packet_view(universal_packet{ 0x6789ABCD }));
        EXPECT_FALSE(as_sysex8_packet_view(universal_packet{ 0x789ABCDE }));
        EXPECT_FALSE(as_sysex8_packet_view(universal_packet{ 0x89ABCDEF }));
        EXPECT_FALSE(as_sysex8_packet_view(universal_packet{ 0x9ABCDEF0 }));
        EXPECT_FALSE(as_sysex8_packet_view(universal_packet{ 0xABCDEF01 }));
        EXPECT_FALSE(as_sysex8_packet_view(universal_packet{ 0xBCDEF012 }));
        EXPECT_FALSE(as_sysex8_packet_view(universal_packet{ 0xCDEF0123 }));
        EXPECT_FALSE(as_sysex8_packet_view(universal_packet{ 0xDEF01234 }));
        EXPECT_FALSE(as_sysex8_packet_view(universal_packet{ 0xEF012345 }));
        EXPECT_FALSE(as_sysex8_packet_view(universal_packet{ 0xF0123456 }));
    }
}

//-----------------------------------------------

TEST(extended_data_message, make_sysex8_complete_packet)
{
    using namespace midi;

    {
        constexpr auto m = make_sysex8_complete_packet(99);

        EXPECT_EQ(packet_type::extended_data, m.type());
        EXPECT_EQ(0u, m.group());
        EXPECT_EQ(extended_data_status::sysex8_complete + 1, m.status());
        EXPECT_EQ(99u, m.stream_id());
        EXPECT_EQ(0u, m.payload_size());

        EXPECT_TRUE(is_extended_data_message(m));
        EXPECT_TRUE(is_sysex8_packet(m));
    }

    {
        constexpr auto m = make_sysex8_complete_packet(7, 5);

        EXPECT_EQ(packet_type::extended_data, m.type());
        EXPECT_EQ(5u, m.group());
        EXPECT_EQ(extended_data_status::sysex8_complete + 1, m.status());
        EXPECT_EQ(7u, m.stream_id());
        EXPECT_EQ(0u, m.payload_size());

        EXPECT_TRUE(is_extended_data_message(m));
        EXPECT_TRUE(is_sysex8_packet(m));
    }
}

//-----------------------------------------------

TEST(extended_data_message, make_sysex8_start_packet)
{
    using namespace midi;

    {
        constexpr auto m = make_sysex8_start_packet(32);

        EXPECT_EQ(packet_type::extended_data, m.type());
        EXPECT_EQ(0u, m.group());
        EXPECT_EQ(extended_data_status::sysex8_start + 1, m.status());
        EXPECT_EQ(32u, m.stream_id());
        EXPECT_EQ(0u, m.payload_size());

        EXPECT_TRUE(is_extended_data_message(m));
        EXPECT_TRUE(is_sysex8_packet(m));
    }

    {
        constexpr auto m = make_sysex8_start_packet(240, 0xF);

        EXPECT_EQ(packet_type::extended_data, m.type());
        EXPECT_EQ(0xFu, m.group());
        EXPECT_EQ(extended_data_status::sysex8_start + 1, m.status());
        EXPECT_EQ(240u, m.stream_id());
        EXPECT_EQ(0u, m.payload_size());

        EXPECT_TRUE(is_extended_data_message(m));
        EXPECT_TRUE(is_sysex8_packet(m));
    }
}

//-----------------------------------------------

TEST(extended_data_message, make_sysex8_continue_packet)
{
    using namespace midi;

    {
        constexpr auto m = make_sysex8_continue_packet(77);

        EXPECT_EQ(packet_type::extended_data, m.type());
        EXPECT_EQ(0u, m.group());
        EXPECT_EQ(extended_data_status::sysex8_continue + 1, m.status());
        EXPECT_EQ(77u, m.stream_id());
        EXPECT_EQ(0u, m.payload_size());

        EXPECT_TRUE(is_extended_data_message(m));
        EXPECT_TRUE(is_sysex8_packet(m));
    }

    {
        constexpr auto m = make_sysex8_continue_packet(0, 9);

        EXPECT_EQ(packet_type::extended_data, m.type());
        EXPECT_EQ(9u, m.group());
        EXPECT_EQ(extended_data_status::sysex8_continue + 1, m.status());
        EXPECT_EQ(0u, m.stream_id());
        EXPECT_EQ(0u, m.payload_size());

        EXPECT_TRUE(is_extended_data_message(m));
        EXPECT_TRUE(is_sysex8_packet(m));
    }
}

//-----------------------------------------------

TEST(extended_data_message, make_sysex8_end_packet)
{
    using namespace midi;

    {
        constexpr auto m = make_sysex8_end_packet(255);

        EXPECT_EQ(packet_type::extended_data, m.type());
        EXPECT_EQ(0u, m.group());
        EXPECT_EQ(extended_data_status::sysex8_end + 1, m.status());
        EXPECT_EQ(255u, m.stream_id());
        EXPECT_EQ(0u, m.payload_size());

        EXPECT_TRUE(is_extended_data_message(m));
        EXPECT_TRUE(is_sysex8_packet(m));
    }

    {
        constexpr auto m = make_sysex8_end_packet(24, 1);

        EXPECT_EQ(packet_type::extended_data, m.type());
        EXPECT_EQ(1u, m.group());
        EXPECT_EQ(extended_data_status::sysex8_end + 1, m.status());
        EXPECT_EQ(24u, m.stream_id());
        EXPECT_EQ(0u, m.payload_size());

        EXPECT_TRUE(is_extended_data_message(m));
        EXPECT_TRUE(is_sysex8_packet(m));
    }
}

//-----------------------------------------------
