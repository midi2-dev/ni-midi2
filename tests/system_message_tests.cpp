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

#include <midi/system_message.h>

//-----------------------------------------------

class system_message : public ::testing::Test
{
  public:
};

//-----------------------------------------------

TEST_F(system_message, constructors)
{
    using namespace midi;

    {
        midi::system_message m;

        EXPECT_TRUE(is_system_message(m));

        EXPECT_EQ(0x10000000u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(1u, m.size());
        EXPECT_EQ(packet_type::system, m.type());
        EXPECT_EQ(0u, m.status());
        EXPECT_EQ(0u, m.group());
    }

    {
        midi::system_message m{ 4, system_status::clock };

        EXPECT_TRUE(is_system_message(m));

        EXPECT_EQ(0x14F80000u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(1u, m.size());
        EXPECT_EQ(packet_type::system, m.type());
        EXPECT_EQ(system_status::clock, m.status());
        EXPECT_EQ(4u, m.group());
    }

    {
        midi::system_message m{ 9, system_status::mtc_quarter_frame, 0x46u };

        EXPECT_TRUE(is_system_message(m));

        EXPECT_EQ(0x19F14600u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(1u, m.size());
        EXPECT_EQ(packet_type::system, m.type());
        EXPECT_EQ(system_status::mtc_quarter_frame, m.status());
        EXPECT_EQ(9u, m.group());
    }

    {
        midi::system_message m{ 12, system_status::song_select, 66 };

        EXPECT_TRUE(is_system_message(m));

        EXPECT_EQ(0x1CF34200u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(1u, m.size());
        EXPECT_EQ(packet_type::system, m.type());
        EXPECT_EQ(system_status::song_select, m.status());
        EXPECT_EQ(12u, m.group());
    }
}

//-----------------------------------------------

TEST_F(system_message, system_message_view)
{
    using namespace midi;

    {
        auto m = midi::system_message{};
        EXPECT_TRUE(is_system_message(m));

        EXPECT_TRUE(as_system_message_view(m));

        auto v = system_message_view{ m };

        EXPECT_EQ(0u, v.group());
        EXPECT_EQ(0u, v.status());
        EXPECT_EQ(0u, v.data_byte_1());
        EXPECT_EQ(0u, v.data_byte_2());
        EXPECT_EQ(0u, v.get_song_position());
    }

    {
        static constexpr auto m = midi::system_message{ 5, system_status::clock };
        EXPECT_TRUE(is_system_message(m));

        EXPECT_TRUE(as_system_message_view(m));

        constexpr auto v = system_message_view{ m };

        EXPECT_EQ(5u, v.group());
        EXPECT_EQ(system_status::clock, v.status());
        EXPECT_EQ(0u, v.data_byte_1());
        EXPECT_EQ(0u, v.data_byte_2());
        EXPECT_EQ(0u, v.get_song_position());
    }

    {
        auto m = midi::system_message{ 11, system_status::mtc_quarter_frame, 0x46 };
        EXPECT_TRUE(is_system_message(m));

        EXPECT_TRUE(as_system_message_view(m));

        auto v = system_message_view{ m };

        EXPECT_EQ(11u, v.group());
        EXPECT_EQ(system_status::mtc_quarter_frame, v.status());
        EXPECT_EQ(0x46, v.data_byte_1());
        EXPECT_EQ(0u, v.data_byte_2());
        EXPECT_EQ(0u, v.get_song_position());
    }

    {
        midi::system_message m{ 12, system_status::song_select, 66 };
        EXPECT_TRUE(is_system_message(m));

        EXPECT_TRUE(as_system_message_view(m));

        auto v = system_message_view{ m };

        EXPECT_EQ(12u, v.group());
        EXPECT_EQ(system_status::song_select, v.status());
        EXPECT_EQ(66, v.data_byte_1());
        EXPECT_EQ(0u, v.data_byte_2());
        EXPECT_EQ(0u, v.get_song_position());
    }

    {
        midi::system_message m{ 3, system_status::song_position, 0x34u, 0x24u };
        EXPECT_TRUE(is_system_message(m));

        EXPECT_TRUE(as_system_message_view(m));

        auto v = system_message_view{ m };

        EXPECT_EQ(3u, v.group());
        EXPECT_EQ(system_status::song_position, v.status());
        EXPECT_EQ(0x34u, v.data_byte_1());
        EXPECT_EQ(0x24u, v.data_byte_2());
        EXPECT_EQ(0x1234u, v.get_song_position());
    }

    {
        EXPECT_FALSE(as_system_message_view(universal_packet{ 0x21112233 }));
    }
}

//-----------------------------------------------

TEST_F(system_message, make_system_message)
{
    using namespace midi;

    {
        static constexpr auto m = make_system_message(9, system_status::song_position, 0x74, 0x69);
        EXPECT_EQ(universal_packet{ 0x19F27469 }, m);
        EXPECT_TRUE(is_system_message(m));

        EXPECT_TRUE(as_system_message_view(m));

        constexpr auto v = system_message_view{ m };

        EXPECT_EQ(9u, v.group());
        EXPECT_EQ(system_status::song_position, v.status());
        EXPECT_EQ(0x74u, v.data_byte_1());
        EXPECT_EQ(0x69u, v.data_byte_2());
        EXPECT_EQ(0x34F4u, v.get_song_position());
    }

    {
        const auto m = make_system_message(4, system_status::mtc_quarter_frame, 0x43);
        EXPECT_EQ(universal_packet{ 0x14F14300 }, m);
        EXPECT_TRUE(is_system_message(m));

        EXPECT_TRUE(as_system_message_view(m));

        auto v = system_message_view{ m };

        EXPECT_EQ(4u, v.group());
        EXPECT_EQ(system_status::mtc_quarter_frame, v.status());
        EXPECT_EQ(0x43u, v.data_byte_1());
        EXPECT_EQ(0u, v.data_byte_2());
        EXPECT_EQ(0u, v.get_song_position());
    }

    {
        constexpr auto m = make_system_message(12, system_status::clock);
        EXPECT_EQ(universal_packet{ 0x1CF80000 }, m);
        EXPECT_TRUE(is_system_message(m));

        EXPECT_TRUE(as_system_message_view(m));

        auto v = system_message_view{ m };

        EXPECT_EQ(12u, v.group());
        EXPECT_EQ(system_status::clock, v.status());
        EXPECT_EQ(0u, v.data_byte_1());
        EXPECT_EQ(0u, v.data_byte_2());
        EXPECT_EQ(0u, v.get_song_position());
    }
}

//-----------------------------------------------

TEST_F(system_message, make_song_position_message)
{
    using namespace midi;

    {
        static constexpr auto m = make_song_position_message(3, 0x1234);
        EXPECT_EQ(universal_packet{ 0x13F23424 }, m);
        EXPECT_TRUE(is_system_message(m));

        EXPECT_TRUE(as_system_message_view(m));

        constexpr auto v = system_message_view{ m };

        EXPECT_EQ(3u, v.group());
        EXPECT_EQ(system_status::song_position, v.status());
        EXPECT_EQ(0x34u, v.data_byte_1());
        EXPECT_EQ(0x24u, v.data_byte_2());
        EXPECT_EQ(0x1234u, v.get_song_position());
    }

    {
        const auto m = make_song_position_message(13, 0xFAFA);
        EXPECT_EQ(universal_packet{ 0x1DF27A75 }, m);
        EXPECT_TRUE(is_system_message(m));

        EXPECT_TRUE(as_system_message_view(m));

        auto v = system_message_view{ m };

        EXPECT_EQ(13u, v.group());
        EXPECT_EQ(system_status::song_position, v.status());
        EXPECT_EQ(0x7Au, v.data_byte_1());
        EXPECT_EQ(0x75u, v.data_byte_2());
        EXPECT_EQ(0x3AFAu, v.get_song_position());
    }
}

//-----------------------------------------------
