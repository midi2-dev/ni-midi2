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

class extended_data_message : public ::testing::Test
{ };

//-----------------------------------------------

TEST_F(extended_data_message, constructors)
{
    using namespace midi;

    {
        constexpr midi::extended_data_message m;

        EXPECT_EQ(packet_type::extended_data, m.type());
        EXPECT_EQ(0u, m.group());
        EXPECT_EQ(extended_data_status::sysex8_complete, m.status());
    }

    {
        constexpr midi::extended_data_message m{ extended_data_status::sysex8_end };

        EXPECT_EQ(packet_type::extended_data, m.type());
        EXPECT_EQ(0u, m.group());
        EXPECT_EQ(extended_data_status::sysex8_end, m.status());
    }
}

//-----------------------------------------------

TEST_F(extended_data_message, is_extended_data_message)
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
