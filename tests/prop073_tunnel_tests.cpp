#include <gtest/gtest.h>

#include <midi/prop073_tunnel.h>

#include <limits>
#include <random>

//-----------------------------------------------

class prop073_tunnel : public ::testing::Test
{
  public:
    void run_test(const char* text, const midi::universal_packet& from)
    {
        auto sx = midi::to_prop073_tunnel_message(from);
        auto to = midi::from_prop073_tunnel_message(sx);

        EXPECT_EQ(from, to) << text << "(" << from.data[0] << ", " << from.data[1] << ", " << from.data[2] << ", "
                            << from.data[3] << ")";
    }
};

//-----------------------------------------------

TEST_F(prop073_tunnel, random_messages)
{
    std::random_device                           rd;
    std::mt19937                                 gen(rd());
    std::uniform_int_distribution<std::uint32_t> dist;

    midi::universal_packet msg;
    for (auto i = 0; i < 1000000; ++i)
    {
        msg.data[0] = dist(gen);
        msg.data[1] = dist(gen);
        msg.data[2] = dist(gen);
        msg.data[3] = dist(gen);

        run_test("Random", msg);
    }
}

//--------------------------------------------------------------------------
