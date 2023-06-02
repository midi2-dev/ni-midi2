#include <gtest/gtest.h>

#include <midi/jitter_reduction_timestamps.h>

#include <chrono>
#include <random>
#include <thread>

//-----------------------------------------------

class jr_timestamps : public ::testing::Test
{
  public:
};

//-----------------------------------------------

TEST_F(jr_timestamps, jr_timestamp_t_constructor)
{
    using namespace midi;

    jr_timestamp_t t0;
    EXPECT_EQ(0u, t0.value);

    jr_timestamp_t t1{ 55u };
    EXPECT_EQ(55u, t1.value);

    jr_timestamp_t t2{ 17283u };
    EXPECT_EQ(17283u, t2.value);

    jr_timestamp_t t3{ 0xFFFF };
    EXPECT_EQ(0xFFFF, t3.value);
}

//-----------------------------------------------

TEST_F(jr_timestamps, jr_timestamp_t_equality)
{
    using namespace midi;

    jr_timestamp_t t1{ 55u };
    jr_timestamp_t t2{ 17283u };
    jr_timestamp_t t3{ 0xFFFF };
    jr_timestamp_t t4{ 55u };

    EXPECT_FALSE(t1 == t2);
    EXPECT_TRUE(t1 != t2);
    EXPECT_FALSE(t2 == t1);
    EXPECT_TRUE(t2 != t1);

    EXPECT_FALSE(t2 == t3);
    EXPECT_TRUE(t2 != t3);
    EXPECT_FALSE(t3 == t2);
    EXPECT_TRUE(t3 != t2);

    EXPECT_FALSE(t3 == t4);
    EXPECT_TRUE(t3 != t4);
    EXPECT_FALSE(t4 == t3);
    EXPECT_TRUE(t4 != t3);

    EXPECT_FALSE(t1 == t3);
    EXPECT_TRUE(t1 != t3);
    EXPECT_FALSE(t3 == t1);
    EXPECT_TRUE(t3 != t1);

    EXPECT_FALSE(t2 == t4);
    EXPECT_TRUE(t2 != t4);
    EXPECT_FALSE(t4 == t2);
    EXPECT_TRUE(t4 != t2);

    EXPECT_TRUE(t1 == t4);
    EXPECT_FALSE(t1 != t4);
    EXPECT_TRUE(t4 == t1);
    EXPECT_FALSE(t4 != t1);
}

//-----------------------------------------------

TEST_F(jr_timestamps, jr_timestamp_t_minus)
{
    using namespace midi;

    jr_timestamp_t t1{ 55u };
    jr_timestamp_t t2{ 17283u };
    jr_timestamp_t t3{ 0xFFFF };
    jr_timestamp_t t4{ 55u };

    const auto t2_minus_t1 = t2 - t1;
    const auto t3_minus_t2 = t3 - t2;
    const auto t4_minus_t3 = t4 - t3;
    const auto t1_minus_t4 = t1 - t4;

    EXPECT_EQ(17228, t2_minus_t1.count());
    EXPECT_EQ(48252, t3_minus_t2.count());
    EXPECT_EQ(56, t4_minus_t3.count());
    EXPECT_EQ(0, t1_minus_t4.count());

    const auto t1_minus_t2 = t1 - t2;
    const auto t2_minus_t3 = t2 - t3;
    const auto t3_minus_t4 = t3 - t4;
    const auto t4_minus_t1 = t4 - t1;

    EXPECT_EQ(48308, t1_minus_t2.count());
    EXPECT_EQ(17284, t2_minus_t3.count());
    EXPECT_EQ(65480, t3_minus_t4.count());
    EXPECT_EQ(0, t4_minus_t1.count());
}

//-----------------------------------------------

TEST_F(jr_timestamps, jr_clock)
{
    // needs improvement, fails on virtual CI servers
    GTEST_SKIP() << "Skipping jr_clock test";

    std::random_device                           rd;
    std::mt19937                                 gen(rd());
    std::uniform_int_distribution<std::uint32_t> dist(0, 1000);

    for (auto i = 0; i < 20; ++i)
    {
        const auto delay = std::chrono::milliseconds(dist(gen));

        const auto one = midi::jr_clock::now();

        std::this_thread::sleep_for(delay);

        const auto two = midi::jr_clock::now();

        const midi::jr_ticks diff = two - one;

        EXPECT_GE(diff, delay);
        EXPECT_LT(diff, delay + std::chrono::milliseconds(75));
    }
}

//-----------------------------------------------

TEST_F(jr_timestamps, jr_clock_message)
{
    using namespace midi;

    // default constructor
    {
        const jr_clock_message jrc;

        EXPECT_EQ(1u, jrc.size());

        EXPECT_EQ(midi::packet_type::utility, jrc.type());

        EXPECT_EQ(midi::utility_status::jr_clock, jrc.byte2());
        EXPECT_EQ(0u, jrc.byte3());
        EXPECT_EQ(0u, jrc.byte4());
    }

    // timestamp constructor
    {
        const auto             ts = jr_clock::now();
        const jr_clock_message jrc{ ts };

        EXPECT_EQ(1u, jrc.size());

        EXPECT_EQ(midi::packet_type::utility, jrc.type());
        EXPECT_EQ(midi::utility_status::jr_clock, jrc.byte2());

        EXPECT_EQ(ts, jrc.timestamp());
    }

    // set timestamp
    {
        jr_clock_message jrc;

        jrc.set_timestamp(jr_timestamp_t{ 77 });
        EXPECT_EQ(77u, jrc.timestamp().value);

        EXPECT_EQ(0u, jrc.byte3());
        EXPECT_EQ(77u, jrc.byte4());

        jrc.set_timestamp(jr_timestamp_t{ 0xF3F4 });
        EXPECT_EQ(jr_timestamp_t{ 0xF3F4 }, jrc.timestamp());

        EXPECT_EQ(0xF3u, jrc.byte3());
        EXPECT_EQ(0xF4u, jrc.byte4());
    }
}

//-----------------------------------------------

TEST_F(jr_timestamps, jr_timestamp_message)
{
    using namespace midi;

    // default constructor
    {
        const jr_timestamp_message jrts;

        EXPECT_EQ(1u, jrts.size());

        EXPECT_EQ(midi::packet_type::utility, jrts.type());

        EXPECT_EQ(midi::utility_status::jr_timestamp, jrts.byte2());
        EXPECT_EQ(0u, jrts.byte3());
        EXPECT_EQ(0u, jrts.byte4());
    }

    // timestamp constructor, default channel group
    {
        const auto                 ts = jr_clock::now();
        const jr_timestamp_message jrts{ ts };

        EXPECT_EQ(1u, jrts.size());

        EXPECT_EQ(midi::packet_type::utility, jrts.type());

        EXPECT_EQ(midi::utility_status::jr_timestamp, jrts.byte2());

        EXPECT_EQ(ts, jrts.timestamp());
    }

    // set timestamp
    {
        jr_timestamp_message jrts;

        jrts.set_timestamp(jr_timestamp_t{ 18334 });
        EXPECT_EQ(18334u, jrts.timestamp().value);

        EXPECT_EQ(0x47u, jrts.byte3());
        EXPECT_EQ(0x9Eu, jrts.byte4());

        jrts.set_timestamp(jr_timestamp_t{ 0xABCD });
        EXPECT_EQ(jr_timestamp_t{ 0xABCD }, jrts.timestamp());

        EXPECT_EQ(0xABu, jrts.byte3());
        EXPECT_EQ(0xCDu, jrts.byte4());
    }
}

//-----------------------------------------------

TEST_F(jr_timestamps, jr_clock_follower)
{
    midi::jr_clock_follower jrcf;

    EXPECT_EQ(0u, jrcf.jitter().count());

    jrcf.set_security_offset(std::chrono::microseconds(1234));
    EXPECT_EQ(std::chrono::microseconds(1234), jrcf.security_offset());

    jrcf.set_security_offset(std::chrono::milliseconds(3));
    EXPECT_EQ(std::chrono::milliseconds(3), jrcf.security_offset());
}

//-----------------------------------------------

TEST_F(jr_timestamps, jr_clock_follower_schedule_message)
{
    // needs improvement, fails on virtual CI servers
    GTEST_SKIP() << "Skipping jr_clock_follower_schedule_message test";

    using namespace midi;

    jr_clock_follower jrcf;

    unsigned      security_offsets[] = { 543, 755, 1033, 2000, 3210 };
    std::uint16_t timestamps[]       = { 0, 100, 1733, 4822, 10999, 33488, 0xFFFF };

    const auto now = jr_clock_follower::system_clock::now();

    // no clocks received
    for (auto s : security_offsets)
    {
        const auto offset = std::chrono::microseconds(s);
        jrcf.set_security_offset(offset);

        for (auto ts : timestamps)
        {
            const auto at = jrcf.schedule_message(now, jr_timestamp_t{ ts });

            // no jitter reduction
            EXPECT_EQ(at, now + offset);
        }
    }

    // clock with timestamp 0 received
    for (auto s : security_offsets)
    {
        const auto offset = std::chrono::microseconds(s);

        jrcf.reset();
        jrcf.set_security_offset(offset);
        jrcf.process_clock(now, jr_timestamp_t{ 0 });

        for (auto ts : timestamps)
        {
            const auto expected =
              now + offset + std::chrono::duration_cast<jr_clock_follower::duration>(jr_ticks{ ts });
            const auto at = jrcf.schedule_message(now, jr_timestamp_t{ ts });

            EXPECT_EQ(at, expected);
        }
    }
}

//-----------------------------------------------

TEST_F(jr_timestamps, jr_clock_follower_process_clock)
{
    // needs improvement, fails on virtual CI servers
    GTEST_SKIP() << "Skipping jr_clock_follower_process_clock test";

    using namespace midi;

    jr_clock_follower jrcf;

    // TODO: improve this test - use artificial test data dor better test coverage
    jrcf.process_clock(jr_clock_follower::system_clock::now(), jr_timestamp_t{ 0 });
    jrcf.process_clock(jr_clock_follower::system_clock::now(), jr_timestamp_t{ 24 });
    jrcf.process_clock(jr_clock_follower::system_clock::now(), jr_timestamp_t{ 48 });
    EXPECT_NE(jr_clock_follower::duration{ 0 }, jrcf.jitter());
    jrcf.process_clock(jr_clock_follower::system_clock::now(), jr_timestamp_t{ 456 });
    EXPECT_NE(jr_clock_follower::duration{ 0 }, jrcf.jitter());
}

//-----------------------------------------------
