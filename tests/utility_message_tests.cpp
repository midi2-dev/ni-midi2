#include <gtest/gtest.h>

#include <midi/utility_message.h>

//-----------------------------------------------

class utility_message : public ::testing::Test
{
  public:
};

//-----------------------------------------------

TEST_F(utility_message, constructors)
{
    using namespace midi;

    {
        midi::utility_message m;

        EXPECT_EQ(0u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(1u, m.size());
        EXPECT_EQ(packet_type::utility, m.type());
        EXPECT_EQ(utility_status::noop, m.status());
    }

    {
        midi::utility_message m{ utility_status::jr_clock };

        EXPECT_EQ(0x00100000u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(1u, m.size());
        EXPECT_EQ(packet_type::utility, m.type());
        EXPECT_EQ(utility_status::jr_clock, m.status());
    }

    {
        midi::utility_message m{ utility_status::jr_timestamp, 0xABCD };

        EXPECT_EQ(0x0020ABCDu, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(1u, m.size());
        EXPECT_EQ(packet_type::utility, m.type());
        EXPECT_EQ(utility_status::jr_timestamp, m.status());
    }
}

//-----------------------------------------------

TEST_F(utility_message, view)
{
    using namespace midi;

    {
        auto m = midi::utility_message{};
        auto v = utility_message_view{ m };

        EXPECT_EQ(utility_status::noop, v.status());
        EXPECT_EQ(0u, v.payload());
    }

    {
        auto m = midi::utility_message{ utility_status::jr_clock };
        auto v = utility_message_view{ m };

        EXPECT_EQ(utility_status::jr_clock, v.status());
        EXPECT_EQ(0u, v.payload());
    }

    {
        auto m = midi::utility_message{ utility_status::jr_timestamp, 0xABCD };
        auto v = utility_message_view{ m };

        EXPECT_EQ(utility_status::jr_timestamp, v.status());
        EXPECT_EQ(0xABCDu, v.payload());
    }
}

//-----------------------------------------------

TEST_F(utility_message, make_utility_message)
{
    using namespace midi;

    {
        constexpr auto m = make_utility_message(utility_status::jr_clock, 0xF499);
        EXPECT_EQ(universal_packet{ 0x0010F499 }, m);

        auto v = utility_message_view{ m };

        EXPECT_EQ(utility_status::jr_clock, v.status());
        EXPECT_EQ(0xF499u, v.payload());
    }

    {
        constexpr auto m = make_utility_message(utility_status::jr_timestamp, 0x17CC);
        EXPECT_EQ(universal_packet{ 0x002017CC }, m);

        auto v = utility_message_view{ m };

        EXPECT_EQ(utility_status::jr_timestamp, v.status());
        EXPECT_EQ(0x17CCu, v.payload());
    }
}

//-----------------------------------------------

TEST_F(utility_message, utility_message_view)
{
    using namespace midi;

    {
        static constexpr auto m = make_utility_message(utility_status::jr_clock, 0xF499);
        EXPECT_EQ(universal_packet{ 0x0010F499 }, m);

        constexpr auto v = utility_message_view{ m };

        EXPECT_EQ(utility_status::jr_clock, v.status());
        EXPECT_EQ(0xF499u, v.payload());
    }
}

//-----------------------------------------------
