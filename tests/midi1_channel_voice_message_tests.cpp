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

#include <midi/midi1_channel_voice_message.h>

#include <array>
#include <utility> // pair

//-----------------------------------------------

class midi1_channel_voice_message : public ::testing::Test
{
  public:
    using universal_packet = midi::universal_packet;

    std::array<midi::midi1_channel_voice_message, 4> make_message_samples()
    {
        std::array<midi::midi1_channel_voice_message, 4> messages = {
            {
              midi::make_midi1_note_on_message(1, 1, 1, midi::velocity{ midi::uint7_t{ 1 } }),
              midi::make_midi1_channel_pressure_message(1, 1, midi::controller_value{ midi::uint7_t{ 1 } }),
              midi::make_midi1_channel_voice_message(1, midi::midi1_channel_voice_status::note_on, 1, 0x1, 0x1),
              midi::make_midi1_program_change_message(1, 1, 1),
            },
        };
        return messages;
    }
};

//-----------------------------------------------

TEST_F(midi1_channel_voice_message, constructors)
{
    using namespace midi;

    {
        constexpr midi::midi1_channel_voice_message m(4, midi1_channel_voice_status::note_on + 6, 44, 101);

        EXPECT_TRUE(is_midi1_channel_voice_message(m));

        EXPECT_EQ(0x24962C65u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(1u, m.size());
        EXPECT_EQ(packet_type::midi1_channel_voice, m.type());
        EXPECT_EQ(4u, m.group());
        EXPECT_EQ(midi1_channel_voice_status::note_on + 6, m.status());
        EXPECT_EQ(0x96u, m.byte2());
        EXPECT_EQ(0x2Cu, m.byte3());
        EXPECT_EQ(0x65u, m.byte4());

        EXPECT_TRUE(as_midi1_channel_voice_message_view(m));

        auto v = midi1_channel_voice_message_view{ m };
        EXPECT_EQ(4u, v.group());
        EXPECT_EQ(midi1_channel_voice_status::note_on, v.status());
        EXPECT_EQ(6u, v.channel());
        EXPECT_EQ(0x2Cu, v.data_byte_1());
        EXPECT_EQ(0x65u, v.data_byte_2());
    }
}

//-----------------------------------------------

TEST_F(midi1_channel_voice_message, midi1_channel_voice_message_view)
{
    using namespace midi;

    {
        static constexpr midi::midi1_channel_voice_message m(4, midi1_channel_voice_status::note_on + 6, 44, 101);

        EXPECT_TRUE(is_midi1_channel_voice_message(m));
        EXPECT_TRUE(as_midi1_channel_voice_message_view(m));

        constexpr auto v = midi1_channel_voice_message_view{ m };
        EXPECT_EQ(4u, v.group());
        EXPECT_EQ(midi1_channel_voice_status::note_on, v.status());
        EXPECT_EQ(6u, v.channel());
        EXPECT_EQ(0x2Cu, v.data_byte_1());
        EXPECT_EQ(0x65u, v.data_byte_2());
    }
}

//-----------------------------------------------

TEST_F(midi1_channel_voice_message, channel)
{
    constexpr std::pair<unsigned, unsigned> test_cases[]{
        { 0x208E2345u, 0xEu }, { 0x23922345u, 0x2u }, { 0x2AA54621u, 0x5u }, { 0x26B12345u, 0x1u },
        { 0x27CD2345u, 0xDu }, { 0x29CA2345u, 0xAu }, { 0x2CE74621u, 0x7u },
    };

    for (const auto& c : test_cases)
    {
        midi::midi1_channel_voice_message m{ universal_packet{ c.first } };
        EXPECT_EQ(c.second, m.channel());
    }
}

//-----------------------------------------------

TEST_F(midi1_channel_voice_message, data_bytes)
{
    {
        constexpr midi::midi1_channel_voice_message m(universal_packet{ 0x20814433 });

        auto v = midi::midi1_channel_voice_message_view{ m };
        EXPECT_EQ(v.data_byte_1(), 0x44);
        EXPECT_EQ(v.data_byte_2(), 0x33);
    }

    {
        constexpr midi::midi1_channel_voice_message m(universal_packet{ 0x28F2173C });

        auto v = midi::midi1_channel_voice_message_view{ m };
        EXPECT_EQ(0x17, v.data_byte_1());
        EXPECT_EQ(0x3C, v.data_byte_2());
    }

    {
        constexpr midi::midi1_channel_voice_message m(universal_packet{ 0x2CA60102 });

        auto v = midi::midi1_channel_voice_message_view{ m };
        EXPECT_EQ(0x01, v.data_byte_1());
        EXPECT_EQ(0x02, v.data_byte_2());
    }
}

//-----------------------------------------------

TEST_F(midi1_channel_voice_message, get_14bit_value)
{
    {
        constexpr midi::midi1_channel_voice_message m(universal_packet{ 0x25F10000 });

        auto v = midi::midi1_channel_voice_message_view{ m };
        EXPECT_EQ(0x0000, v.get_14bit_value());
    }

    {
        constexpr midi::midi1_channel_voice_message m(universal_packet{ 0x20814433 });

        auto v = midi::midi1_channel_voice_message_view{ m };
        EXPECT_EQ(0x19C4, v.get_14bit_value());
    }

    {
        constexpr midi::midi1_channel_voice_message m(universal_packet{ 0x28F2173C });

        auto v = midi::midi1_channel_voice_message_view{ m };
        EXPECT_EQ(0x1E17, v.get_14bit_value());
    }

    {
        constexpr midi::midi1_channel_voice_message m(universal_packet{ 0x2CA60102 });

        auto v = midi::midi1_channel_voice_message_view{ m };
        EXPECT_EQ(0x101, v.get_14bit_value());
    }

    {
        constexpr midi::midi1_channel_voice_message m(universal_packet{ 0x2CE60040 });

        auto v = midi::midi1_channel_voice_message_view{ m };
        EXPECT_EQ(0x2000, v.get_14bit_value());
    }

    {
        constexpr midi::midi1_channel_voice_message m(universal_packet{ 0x2CE67F7F });

        auto v = midi::midi1_channel_voice_message_view{ m };
        EXPECT_EQ(0x3FFF, v.get_14bit_value());
    }
}

//-----------------------------------------------

TEST_F(midi1_channel_voice_message, make_midi1_channel_voice_message)
{
    {
        constexpr auto m =
          midi::make_midi1_channel_voice_message(9, midi::midi1_channel_voice_status::note_on, 5, 0x74, 0x78);
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x29957478 } }), m);
    }

    {
        constexpr auto m =
          midi::make_midi1_channel_voice_message(12, midi::midi1_channel_voice_status::program_change, 10, 0x04);
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x2CCA0400 } }), m);
    }

    { // provide channel with status
        constexpr auto m = midi::make_midi1_channel_voice_message(
          0, midi::midi1_channel_voice_status::poly_pressure + 11, 0, 0x3C, 0x76);
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x20AB3C76 } }), m);
    }

    { // invalid channel bits
        constexpr auto m =
          midi::make_midi1_channel_voice_message(4, midi::midi1_channel_voice_status::control_change, 0x35, 0x45, 0x77);
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x24B54577 } }), m);
    }
}

//-----------------------------------------------

TEST_F(midi1_channel_voice_message, make_midi1_note_off_message)
{
    {
        constexpr auto m = midi::make_midi1_note_off_message(4, 9, 0x50, midi::velocity{ midi::uint7_t{ 0x23 } });
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x24895023 } }), m);
    }

    {
        constexpr auto m = midi::make_midi1_note_off_message(13, 14, 0x14, midi::velocity{ midi::uint7_t{ 0xD1 } });
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x2D8E1451 } }), m);
    }
}

//-----------------------------------------------

TEST_F(midi1_channel_voice_message, make_note_on_message)
{
    {
        constexpr auto m = midi::make_midi1_note_on_message(8, 4, 0x12, midi::velocity{ midi::uint7_t{ 0x01 } });
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x28941201 } }), m);
    }

    {
        constexpr auto m = midi::make_midi1_note_on_message(7, 3, 0x77, midi::velocity{ midi::uint7_t{ 0xF0 } });
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x27937770 } }), m);
    }
}

//-----------------------------------------------

TEST_F(midi1_channel_voice_message, make_midi1_poly_pressure_message)
{
    {
        constexpr auto m =
          midi::make_midi1_poly_pressure_message(0, 1, 99, midi::controller_value{ midi::uint7_t{ 114 } });
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x20A16372 } }), m);
    }

    {
        constexpr auto m =
          midi::make_midi1_poly_pressure_message(6, 9, 64, midi::controller_value{ midi::uint7_t{ 0xFF } });
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x26A9407F } }), m);
    }
}

//-----------------------------------------------

TEST_F(midi1_channel_voice_message, make_midi1_control_change_message)
{
    {
        constexpr auto m =
          midi::make_midi1_control_change_message(9, 4, 0x38, midi::controller_value{ midi::uint7_t{ 0x76 } });
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x29B43876 } }), m);
    }

    { // invalid CC bits
        constexpr auto m =
          midi::make_midi1_control_change_message(11, 12, 0xC5, midi::controller_value{ midi::uint7_t{ 0x60 } });
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x2BBC4560 } }), m);
    }
}

//-----------------------------------------------

TEST_F(midi1_channel_voice_message, make_midi1_program_change_message)
{
    {
        constexpr auto m = midi::make_midi1_program_change_message(14, 3, 99);
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x2EC36300 } }), m);
    }

    { // invalid program bit
        constexpr auto m = midi::make_midi1_program_change_message(9, 11, 0xC5);
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x29CB4500 } }), m);
    }
}

//-----------------------------------------------

TEST_F(midi1_channel_voice_message, make_midi1_channel_pressure_message)
{
    {
        constexpr auto m =
          midi::make_midi1_channel_pressure_message(14, 3, midi::controller_value{ midi::uint7_t{ 0x79 } });
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x2ED37900 } }), m);
    }

    {
        constexpr auto m =
          midi::make_midi1_channel_pressure_message(9, 11, midi::controller_value{ midi::uint7_t{ 99 } });
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x29DB6300 } }), m);
    }
}

//-----------------------------------------------

TEST_F(midi1_channel_voice_message, make_midi1_pitch_bend_message)
{
    {
        constexpr auto m = midi::make_midi1_pitch_bend_message(12, 9, midi::pitch_bend{ midi::uint14_t{ 0x2000 } });
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x2CE90040 } }), m);
    }

    {
        constexpr auto m = midi::make_midi1_pitch_bend_message(3, 7, midi::pitch_bend{ midi::uint14_t{ 0x2412 } });
        EXPECT_EQ((midi::midi1_channel_voice_message{ universal_packet{ 0x23E71248 } }), m);
    }
}

//-----------------------------------------------

TEST_F(midi1_channel_voice_message, construct_from_ump)
{
    auto messages = make_message_samples();
    for (auto message : messages)
    {
        midi::universal_packet            packet(message);
        midi::midi1_channel_voice_message m1{ packet };

        EXPECT_EQ(m1, message);
    }
}

//-----------------------------------------------
