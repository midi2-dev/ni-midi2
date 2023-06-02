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
