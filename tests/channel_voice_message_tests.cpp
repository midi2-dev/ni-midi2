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

#include <midi/channel_voice_message.h>
#include <midi/system_message.h>
#include <midi/utility_message.h>

//-----------------------------------------------

class channel_voice_message : public ::testing::Test
{
  public:
    inline midi::universal_packet make_packet(midi::packet_type t, midi::group_t g, midi::status_t s, midi::uint32_t d)
    {
        return midi::universal_packet{ (midi::uint32_t(t) << 28) | ((g & 0x0F) << 24) | (s << 16), d };
    }
};

//-----------------------------------------------

TEST_F(channel_voice_message, is_channel_voice_message_with_status)
{
    using namespace midi;
    for (auto match = 0x00; match <= 0xF0; match += 0x10)
    {
        for (auto t = 0; t < 16; ++t)
        {
            const bool channel_voice = (t == 2) || (t == 4);

            for (auto s = 0x00; s <= 0xF0; s += 0x10)
            {
                auto p = universal_packet{ uint32_t((t << 28) | (s << 16) | t | (s >> 4)) };

                EXPECT_EQ(channel_voice && (s == match), is_channel_voice_message_with_status(p, status_t(match)));
            }
        }
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, is_note_on_message)
{
    using namespace midi;

    // MIDI 1 Channel Voice messages
    {
        EXPECT_FALSE(is_note_on_message(make_midi1_note_off_message(0, 2, 67)));
        EXPECT_TRUE(is_note_on_message(make_midi1_note_on_message(4, 7, 99, velocity{ uint7_t{ 100 } })));
        EXPECT_FALSE(is_note_on_message(make_midi1_note_on_message(13, 5, 60, velocity{ uint7_t{ 0 } })));
        EXPECT_FALSE(
          is_note_on_message(make_midi1_poly_pressure_message(14, 2, 64, controller_value{ uint7_t{ 77 } })));
        EXPECT_FALSE(
          is_note_on_message(make_midi1_control_change_message(5, 15, 7, controller_value{ uint7_t{ 100 } })));
        EXPECT_FALSE(is_note_on_message(make_midi1_program_change_message(14, 7, 42)));
        EXPECT_FALSE(is_note_on_message(make_midi1_channel_pressure_message(8, 8, controller_value{ uint7_t{ 77 } })));
        EXPECT_FALSE(is_note_on_message(make_midi1_pitch_bend_message(1, 13, pitch_bend{ uint14_t{ 8177 } })));
    }

    // MIDI 2 Channel Voice messages
    {
        EXPECT_FALSE(
          is_note_on_message(make_registered_per_note_controller_message(15, 10, 44, 2, controller_value{ 123456u })));
        EXPECT_FALSE(
          is_note_on_message(make_assignable_per_note_controller_message(3, 7, 64, 99, controller_value{ 987654u })));
        EXPECT_FALSE(is_note_on_message(make_registered_controller_message(2, 9, 0, 4, controller_value{ 123456u })));
        EXPECT_FALSE(is_note_on_message(make_assignable_controller_message(8, 0, 4, 12, controller_value{ 987654u })));
        EXPECT_FALSE(
          is_note_on_message(make_relative_registered_controller_message(2, 9, 0, 4, controller_increment{ -1234 })));
        EXPECT_FALSE(
          is_note_on_message(make_relative_assignable_controller_message(8, 0, 4, 12, controller_increment{ 111 })));
        EXPECT_FALSE(is_note_on_message(make_per_note_pitch_bend_message(11, 12, 13, pitch_bend{ 0x80000001u })));

        EXPECT_FALSE(is_note_on_message(make_midi2_channel_voice_message(1, 0x70, 2, 3, 4, 0xABABABAB)));

        EXPECT_FALSE(is_note_on_message(make_midi2_note_off_message(0, 2, 67, velocity{ uint16_t{ 0x1234 } })));
        EXPECT_TRUE(is_note_on_message(make_midi2_note_on_message(4, 7, 99, velocity{ uint16_t{ 0x4567 } })));
        EXPECT_TRUE(is_note_on_message(make_midi2_note_on_message(13, 5, 60, velocity{ uint16_t{ 0 } })));
        EXPECT_TRUE(is_note_on_message(
          make_midi2_note_on_message(9, 10, 127, velocity{ uint16_t{ 0xA000 } }, pitch_7_9{ note_nr_t{ 60 } })));
        EXPECT_FALSE(is_note_on_message(make_midi2_poly_pressure_message(14, 2, 64, controller_value{ 0x6789ABCDu })));
        EXPECT_FALSE(is_note_on_message(make_midi2_control_change_message(5, 15, 7, controller_value{ 0x89ABCDEFu })));
        EXPECT_FALSE(is_note_on_message(make_midi2_program_change_message(14, 7, 0, 42)));
        EXPECT_FALSE(is_note_on_message(make_midi2_channel_pressure_message(8, 8, controller_value{ 0xABCDEF01 })));
        EXPECT_FALSE(is_note_on_message(make_midi2_pitch_bend_message(1, 13, pitch_bend{ 0x8FFFFFFF })));
    }

    // Other messages
    {
        for (auto i = 0; i < 0x100; ++i)
        {
            const auto status = status_t(i);
            const auto group  = group_t(status & 0x0F);
            const auto data   = uint32_t(status * status);

            EXPECT_FALSE(is_note_on_message(make_utility_message(status, uint16_t(data))));
            EXPECT_FALSE(is_note_on_message(make_system_message(group, status, uint8_t(data))));
            EXPECT_FALSE(is_note_on_message(make_packet(packet_type::data, group, status, data)));
            EXPECT_FALSE(is_note_on_message(make_packet(packet_type::extended_data, group, status, data)));
            EXPECT_FALSE(is_note_on_message(make_packet(packet_type(0x6), group, status, data)));
            EXPECT_FALSE(is_note_on_message(make_packet(packet_type(0x7), group, status, data)));
            EXPECT_FALSE(is_note_on_message(make_packet(packet_type(0x8), group, status, data)));
            EXPECT_FALSE(is_note_on_message(make_packet(packet_type(0x9), group, status, data)));
            EXPECT_FALSE(is_note_on_message(make_packet(packet_type(0xA), group, status, data)));
            EXPECT_FALSE(is_note_on_message(make_packet(packet_type(0xB), group, status, data)));
            EXPECT_FALSE(is_note_on_message(make_packet(packet_type(0xC), group, status, data)));
            EXPECT_FALSE(is_note_on_message(make_packet(packet_type(0xD), group, status, data)));
            EXPECT_FALSE(is_note_on_message(make_packet(packet_type(0xE), group, status, data)));
            EXPECT_FALSE(is_note_on_message(make_packet(packet_type(0xF), group, status, data)));
        }
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, is_note_off_message)
{
    using namespace midi;

    // MIDI 1 Channel Voice messages
    {
        EXPECT_TRUE(is_note_off_message(make_midi1_note_off_message(0, 2, 67)));
        EXPECT_FALSE(is_note_off_message(make_midi1_note_on_message(4, 7, 99, velocity{ uint7_t{ 100 } })));
        EXPECT_TRUE(is_note_off_message(make_midi1_note_on_message(13, 5, 60, velocity{ uint7_t{ 0 } })));
        EXPECT_FALSE(
          is_note_off_message(make_midi1_poly_pressure_message(14, 2, 64, controller_value{ uint7_t{ 77 } })));
        EXPECT_FALSE(
          is_note_off_message(make_midi1_control_change_message(5, 15, 7, controller_value{ uint7_t{ 100 } })));
        EXPECT_FALSE(is_note_off_message(make_midi1_program_change_message(14, 7, 42)));
        EXPECT_FALSE(is_note_off_message(make_midi1_channel_pressure_message(8, 8, controller_value{ uint7_t{ 77 } })));
        EXPECT_FALSE(is_note_off_message(make_midi1_pitch_bend_message(1, 13, pitch_bend{ uint14_t{ 8177 } })));
    }

    // MIDI 2 Channel Voice messages
    {
        EXPECT_FALSE(
          is_note_off_message(make_registered_per_note_controller_message(15, 10, 44, 2, controller_value{ 123456u })));
        EXPECT_FALSE(
          is_note_off_message(make_assignable_per_note_controller_message(3, 7, 64, 99, controller_value{ 987654u })));
        EXPECT_FALSE(is_note_off_message(make_registered_controller_message(2, 9, 0, 4, controller_value{ 123456u })));
        EXPECT_FALSE(is_note_off_message(make_assignable_controller_message(8, 0, 4, 12, controller_value{ 987654u })));
        EXPECT_FALSE(
          is_note_off_message(make_relative_registered_controller_message(2, 9, 0, 4, controller_increment{ -1234 })));
        EXPECT_FALSE(
          is_note_off_message(make_relative_assignable_controller_message(8, 0, 4, 12, controller_increment{ 111 })));
        EXPECT_FALSE(
          is_note_off_message(make_per_note_pitch_bend_message(11, 12, 13, pitch_bend{ uint32_t{ 0x80000001u } })));

        EXPECT_FALSE(is_note_off_message(make_midi2_channel_voice_message(1, 0x70, 2, 3, 4, 0xABABABAB)));

        EXPECT_TRUE(is_note_off_message(make_midi2_note_off_message(0, 2, 67, velocity{ uint16_t{ 0x1234 } })));
        EXPECT_FALSE(is_note_off_message(make_midi2_note_on_message(4, 7, 99, velocity{ uint16_t{ 0x4567 } })));
        EXPECT_FALSE(is_note_off_message(make_midi2_note_on_message(13, 5, 60, velocity{ uint16_t{ 0 } })));
        EXPECT_FALSE(is_note_off_message(
          make_midi2_note_on_message(9, 10, 127, velocity{ uint16_t{ 0xA000 } }, pitch_7_9{ note_nr_t{ 60 } })));
        EXPECT_FALSE(is_note_off_message(make_midi2_poly_pressure_message(14, 2, 64, controller_value{ 0x6789ABCDu })));
        EXPECT_FALSE(is_note_off_message(make_midi2_control_change_message(5, 15, 7, controller_value{ 0x89ABCDEFu })));
        EXPECT_FALSE(is_note_off_message(make_midi2_program_change_message(14, 7, 0, 42)));
        EXPECT_FALSE(is_note_off_message(make_midi2_channel_pressure_message(8, 8, controller_value{ 0xABCDEF01u })));
        EXPECT_FALSE(is_note_off_message(make_midi2_pitch_bend_message(1, 13, pitch_bend{ 0x8FFFFFFF })));
    }

    // Other messages
    {
        for (auto i = 0; i < 0x100; ++i)
        {
            const auto status = uint8_t(i);
            const auto group  = group_t(status & 0x0F);
            const auto data   = uint32_t(status * status);

            EXPECT_FALSE(is_note_off_message(make_utility_message(status, uint16_t(data))));
            EXPECT_FALSE(is_note_off_message(make_system_message(group, status, uint8_t(data))));
            EXPECT_FALSE(is_note_off_message(make_packet(packet_type::data, group, status, data)));
            EXPECT_FALSE(is_note_off_message(make_packet(packet_type::extended_data, group, status, data)));
            EXPECT_FALSE(is_note_off_message(make_packet(packet_type(0x6), group, status, data)));
            EXPECT_FALSE(is_note_off_message(make_packet(packet_type(0x7), group, status, data)));
            EXPECT_FALSE(is_note_off_message(make_packet(packet_type(0x8), group, status, data)));
            EXPECT_FALSE(is_note_off_message(make_packet(packet_type(0x9), group, status, data)));
            EXPECT_FALSE(is_note_off_message(make_packet(packet_type(0xA), group, status, data)));
            EXPECT_FALSE(is_note_off_message(make_packet(packet_type(0xB), group, status, data)));
            EXPECT_FALSE(is_note_off_message(make_packet(packet_type(0xC), group, status, data)));
            EXPECT_FALSE(is_note_off_message(make_packet(packet_type(0xD), group, status, data)));
            EXPECT_FALSE(is_note_off_message(make_packet(packet_type(0xE), group, status, data)));
            EXPECT_FALSE(is_note_off_message(make_packet(packet_type(0xF), group, status, data)));
        }
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, get_note_nr)
{
    using namespace midi;

    // MIDI 1 Channel Voice messages
    {
        EXPECT_EQ(get_note_nr(make_midi1_note_off_message(0, 2, 67)), 67u);
        EXPECT_EQ(get_note_nr(make_midi1_note_on_message(13, 5, 60, velocity{ uint7_t{ 0 } })), 60u);
        EXPECT_EQ(get_note_nr(make_midi1_poly_pressure_message(14, 2, 64, controller_value{ uint7_t{ 77 } })), 64u);
    }

    // MIDI 2 Channel Voice messages
    {
        EXPECT_EQ(get_note_nr(make_registered_per_note_controller_message(15, 10, 44, 2, controller_value{ 123456u })),
                  44u);
        EXPECT_EQ(get_note_nr(make_assignable_per_note_controller_message(3, 7, 64, 99, controller_value{ 987654u })),
                  64u);
        EXPECT_EQ(get_note_nr(make_per_note_pitch_bend_message(11, 12, 13, pitch_bend{ uint32_t{ 0x80000001u } })),
                  13u);

        EXPECT_EQ(get_note_nr(make_midi2_note_off_message(0, 2, 67, velocity{ uint16_t{ 0x1234 } })), 67u);
        EXPECT_EQ(get_note_nr(make_midi2_note_on_message(4, 7, 99, velocity{ uint16_t{ 0x4567 } })), 99u);
        EXPECT_EQ(get_note_nr(make_midi2_note_on_message(13, 5, 60, velocity{ uint16_t{ 0 } })), 60u);
        EXPECT_EQ(get_note_nr(make_midi2_note_on_message(
                    9, 10, 127, velocity{ uint16_t{ 0xA000 } }, pitch_7_9{ note_nr_t{ 60 } })),
                  127u);
        EXPECT_EQ(get_note_nr(make_midi2_poly_pressure_message(14, 2, 64, controller_value{ 0x6789ABCDu })), 64u);
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, get_note_pitch)
{
    using namespace midi;

    // MIDI 1 Note messages
    {
        EXPECT_EQ(get_note_pitch(make_midi1_note_off_message(0, 2, 67)), pitch_7_9{ note_nr_t{ 67u } });
        EXPECT_EQ(get_note_pitch(make_midi1_note_on_message(13, 5, 60, velocity{ uint7_t{ 0 } })),
                  pitch_7_9{ note_nr_t{ 60u } });
    }

    // MIDI 2 Channel Voice messages
    {
        EXPECT_EQ(get_note_pitch(make_midi2_note_off_message(0, 2, 67, velocity{ uint16_t{ 0x1234 } })),
                  pitch_7_9{ note_nr_t{ 67u } });
        EXPECT_EQ(get_note_pitch(make_midi2_note_on_message(4, 7, 99, velocity{ uint16_t{ 0x4567 } })),
                  pitch_7_9{ note_nr_t{ 99u } });
        EXPECT_EQ(get_note_pitch(make_midi2_note_on_message(13, 5, 60, velocity{ uint16_t{ 0 } })),
                  pitch_7_9{ note_nr_t{ 60u } });
        EXPECT_EQ(get_note_pitch(make_midi2_note_on_message(
                    9, 10, 127, velocity{ uint16_t{ 0xA000 } }, pitch_7_9{ note_nr_t{ 60 } })),
                  pitch_7_9{ note_nr_t{ 60u } });
        EXPECT_EQ(
          get_note_pitch(make_midi2_note_on_message(9, 10, 127, velocity{ uint16_t{ 0xA000 } }, pitch_7_9{ 89.45f })),
          pitch_7_9{ 89.45f });
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, get_note_velocity)
{
    using namespace midi;

    // MIDI 1 Note messages
    {
        EXPECT_EQ(get_note_velocity(make_midi1_note_off_message(0, 2, 67)), velocity{ uint7_t{ 64 } });
        // In MIDI1 Note On with velocity 0 is Note Off with velocity 64:
        EXPECT_EQ(get_note_velocity(make_midi1_note_on_message(13, 5, 60, velocity{ uint7_t{ 0 } })),
                  velocity{ uint7_t{ 64 } });
        EXPECT_EQ(get_note_velocity(make_midi1_note_on_message(9, 12, 32, velocity{ uint7_t{ 44 } })),
                  velocity{ uint7_t{ 44 } });
        EXPECT_EQ(get_note_velocity(make_midi1_note_on_message(5, 6, 7, velocity{ uint7_t{ 127 } })),
                  velocity{ uint7_t{ 127 } });
    }

    // MIDI 2 Channel Voice messages
    {
        EXPECT_EQ(get_note_velocity(make_midi2_note_off_message(0, 2, 67, velocity{ uint16_t{ 0x1234 } })),
                  velocity{ uint16_t{ 0x1234 } });
        EXPECT_EQ(get_note_velocity(make_midi2_note_on_message(4, 7, 99, velocity{ uint16_t{ 0x4567 } })),
                  velocity{ uint16_t{ 0x4567 } });
        EXPECT_EQ(get_note_velocity(make_midi2_note_on_message(13, 5, 60, velocity{ uint16_t{ 0 } })),
                  velocity{ uint16_t{ 0 } });
        EXPECT_EQ(get_note_velocity(make_midi2_note_on_message(
                    9, 10, 127, velocity{ uint16_t{ 0xA000 } }, pitch_7_9{ note_nr_t{ 60 } })),
                  velocity{ uint16_t{ 0xA000 } });
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, is_poly_pressure_message)
{
    using namespace midi;

    // MIDI 1 Channel Voice messages
    {
        EXPECT_FALSE(is_poly_pressure_message(make_midi1_note_off_message(0, 2, 67)));
        EXPECT_FALSE(is_poly_pressure_message(make_midi1_note_on_message(4, 7, 99, velocity{ uint7_t{ 100 } })));
        EXPECT_FALSE(is_poly_pressure_message(make_midi1_note_on_message(13, 5, 60, velocity{ uint7_t{ 0 } })));
        EXPECT_TRUE(
          is_poly_pressure_message(make_midi1_poly_pressure_message(14, 2, 64, controller_value{ uint7_t{ 77 } })));
        EXPECT_FALSE(
          is_poly_pressure_message(make_midi1_control_change_message(5, 15, 7, controller_value{ uint7_t{ 100 } })));
        EXPECT_FALSE(is_poly_pressure_message(make_midi1_program_change_message(14, 7, 42)));
        EXPECT_FALSE(
          is_poly_pressure_message(make_midi1_channel_pressure_message(8, 8, controller_value{ uint7_t{ 77 } })));
        EXPECT_FALSE(is_poly_pressure_message(make_midi1_pitch_bend_message(1, 13, pitch_bend{ uint14_t{ 8177 } })));
    }

    // MIDI 2 Channel Voice messages
    {
        EXPECT_FALSE(is_poly_pressure_message(
          make_registered_per_note_controller_message(15, 10, 44, 2, controller_value{ 123456u })));
        EXPECT_FALSE(is_poly_pressure_message(
          make_assignable_per_note_controller_message(3, 7, 64, 99, controller_value{ 987654u })));
        EXPECT_FALSE(
          is_poly_pressure_message(make_registered_controller_message(2, 9, 0, 4, controller_value{ 123456u })));
        EXPECT_FALSE(
          is_poly_pressure_message(make_assignable_controller_message(8, 0, 4, 12, controller_value{ 987654u })));
        EXPECT_FALSE(is_poly_pressure_message(
          make_relative_registered_controller_message(2, 9, 0, 4, controller_increment{ -1234 })));
        EXPECT_FALSE(is_poly_pressure_message(
          make_relative_assignable_controller_message(8, 0, 4, 12, controller_increment{ 111 })));
        EXPECT_FALSE(is_poly_pressure_message(
          make_per_note_pitch_bend_message(11, 12, 13, pitch_bend{ uint32_t{ 0x80000001u } })));

        EXPECT_FALSE(is_poly_pressure_message(make_midi2_channel_voice_message(1, 0x70, 2, 3, 4, 0xABABABAB)));

        EXPECT_FALSE(is_poly_pressure_message(make_midi2_note_off_message(0, 2, 67, velocity{ uint16_t{ 0x1234 } })));
        EXPECT_FALSE(is_poly_pressure_message(make_midi2_note_on_message(4, 7, 99, velocity{ uint16_t{ 0x4567 } })));
        EXPECT_FALSE(is_poly_pressure_message(make_midi2_note_on_message(13, 5, 60, velocity{ uint16_t{ 0 } })));
        EXPECT_FALSE(is_poly_pressure_message(
          make_midi2_note_on_message(9, 10, 127, velocity{ uint16_t{ 0xA000 } }, pitch_7_9{ note_nr_t{ 60 } })));
        EXPECT_TRUE(
          is_poly_pressure_message(make_midi2_poly_pressure_message(14, 2, 64, controller_value{ 0x6789ABCDu })));
        EXPECT_FALSE(
          is_poly_pressure_message(make_midi2_control_change_message(5, 15, 7, controller_value{ 0x89ABCDEFu })));
        EXPECT_FALSE(is_poly_pressure_message(make_midi2_program_change_message(14, 7, 0, 42)));
        EXPECT_FALSE(
          is_poly_pressure_message(make_midi2_channel_pressure_message(8, 8, controller_value{ 0xABCDEF01u })));
        EXPECT_FALSE(is_poly_pressure_message(make_midi2_pitch_bend_message(1, 13, pitch_bend{ 0x8FFFFFFF })));
    }

    // Other messages
    {
        for (auto i = 0; i < 0x100; ++i)
        {
            const auto status = uint8_t(i);
            const auto group  = group_t(status & 0x0F);
            const auto data   = uint32_t(status * status);

            EXPECT_FALSE(is_poly_pressure_message(make_utility_message(status, uint16_t(data))));
            EXPECT_FALSE(is_poly_pressure_message(make_system_message(group, status, uint8_t(data))));
            EXPECT_FALSE(is_poly_pressure_message(make_packet(packet_type::data, group, status, data)));
            EXPECT_FALSE(is_poly_pressure_message(make_packet(packet_type::extended_data, group, status, data)));
            EXPECT_FALSE(is_poly_pressure_message(make_packet(packet_type(0x6), group, status, data)));
            EXPECT_FALSE(is_poly_pressure_message(make_packet(packet_type(0x7), group, status, data)));
            EXPECT_FALSE(is_poly_pressure_message(make_packet(packet_type(0x8), group, status, data)));
            EXPECT_FALSE(is_poly_pressure_message(make_packet(packet_type(0x9), group, status, data)));
            EXPECT_FALSE(is_poly_pressure_message(make_packet(packet_type(0xA), group, status, data)));
            EXPECT_FALSE(is_poly_pressure_message(make_packet(packet_type(0xB), group, status, data)));
            EXPECT_FALSE(is_poly_pressure_message(make_packet(packet_type(0xC), group, status, data)));
            EXPECT_FALSE(is_poly_pressure_message(make_packet(packet_type(0xD), group, status, data)));
            EXPECT_FALSE(is_poly_pressure_message(make_packet(packet_type(0xE), group, status, data)));
            EXPECT_FALSE(is_poly_pressure_message(make_packet(packet_type(0xF), group, status, data)));
        }
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, get_poly_pressure_value)
{
    using namespace midi;

    {
        EXPECT_EQ(
          get_poly_pressure_value(make_midi1_poly_pressure_message(5, 15, 7, controller_value{ uint7_t{ 38 } })),
          controller_value{ uint7_t{ 38 } });
        EXPECT_EQ(
          get_poly_pressure_value(make_midi1_poly_pressure_message(3, 8, 66, controller_value{ uint7_t{ 77 } })),
          controller_value{ uint7_t{ 77 } });
    }

    {
        EXPECT_EQ(
          get_poly_pressure_value(make_midi2_poly_pressure_message(1, 4, 99, controller_value{ float{ 0.345 } })),
          controller_value{ float{ 0.345 } });
        EXPECT_EQ(
          get_poly_pressure_value(make_midi2_poly_pressure_message(9, 0, 49, controller_value{ uint32_t{ 12345 } })),
          controller_value{ uint32_t{ 12345 } });
        EXPECT_EQ(
          get_poly_pressure_value(make_midi2_poly_pressure_message(2, 14, 8, controller_value{ uint7_t{ 109 } })),
          controller_value{ uint7_t{ 109 } });
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, is_control_change_message)
{
    using namespace midi;

    // MIDI 1 Channel Voice messages
    {
        EXPECT_FALSE(is_control_change_message(make_midi1_note_off_message(0, 2, 67)));
        EXPECT_FALSE(is_control_change_message(make_midi1_note_on_message(4, 7, 99, velocity{ uint7_t{ 100 } })));
        EXPECT_FALSE(is_control_change_message(make_midi1_note_on_message(13, 5, 60, velocity{ uint7_t{ 0 } })));
        EXPECT_FALSE(
          is_control_change_message(make_midi1_poly_pressure_message(14, 2, 64, controller_value{ uint7_t{ 77 } })));
        EXPECT_TRUE(
          is_control_change_message(make_midi1_control_change_message(5, 15, 7, controller_value{ uint7_t{ 100 } })));
        EXPECT_FALSE(is_control_change_message(make_midi1_program_change_message(14, 7, 42)));
        EXPECT_FALSE(
          is_control_change_message(make_midi1_channel_pressure_message(8, 8, controller_value{ uint7_t{ 77 } })));
        EXPECT_FALSE(is_control_change_message(make_midi1_pitch_bend_message(1, 13, pitch_bend{ uint14_t{ 8177 } })));
    }

    // MIDI 2 Channel Voice messages
    {
        EXPECT_FALSE(is_control_change_message(
          make_registered_per_note_controller_message(15, 10, 44, 2, controller_value{ 123456u })));
        EXPECT_FALSE(is_control_change_message(
          make_assignable_per_note_controller_message(3, 7, 64, 99, controller_value{ 987654u })));
        EXPECT_FALSE(
          is_control_change_message(make_registered_controller_message(2, 9, 0, 4, controller_value{ 123456u })));
        EXPECT_FALSE(
          is_control_change_message(make_assignable_controller_message(8, 0, 4, 12, controller_value{ 987654u })));
        EXPECT_FALSE(is_control_change_message(
          make_relative_registered_controller_message(2, 9, 0, 4, controller_increment{ -1234 })));
        EXPECT_FALSE(is_control_change_message(
          make_relative_assignable_controller_message(8, 0, 4, 12, controller_increment{ 111 })));
        EXPECT_FALSE(is_control_change_message(
          make_per_note_pitch_bend_message(11, 12, 13, pitch_bend{ uint32_t{ 0x80000001u } })));

        EXPECT_FALSE(is_control_change_message(make_midi2_channel_voice_message(1, 0x70, 2, 3, 4, 0xABABABAB)));

        EXPECT_FALSE(is_control_change_message(make_midi2_note_off_message(0, 2, 67, velocity{ uint16_t{ 0x1234 } })));
        EXPECT_FALSE(is_control_change_message(make_midi2_note_on_message(4, 7, 99, velocity{ uint16_t{ 0x4567 } })));
        EXPECT_FALSE(is_control_change_message(make_midi2_note_on_message(13, 5, 60, velocity{ uint16_t{ 0 } })));
        EXPECT_FALSE(is_control_change_message(
          make_midi2_note_on_message(9, 10, 127, velocity{ uint16_t{ 0xA000 } }, pitch_7_9{ note_nr_t{ 60 } })));
        EXPECT_FALSE(
          is_control_change_message(make_midi2_poly_pressure_message(14, 2, 64, controller_value{ 0x6789ABCDu })));
        EXPECT_TRUE(
          is_control_change_message(make_midi2_control_change_message(5, 15, 7, controller_value{ 0x89ABCDEFu })));
        EXPECT_FALSE(is_control_change_message(make_midi2_program_change_message(14, 7, 0, 42)));
        EXPECT_FALSE(
          is_control_change_message(make_midi2_channel_pressure_message(8, 8, controller_value{ 0xABCDEF01u })));
        EXPECT_FALSE(is_control_change_message(make_midi2_pitch_bend_message(1, 13, pitch_bend{ 0x8FFFFFFF })));
    }

    // Other messages
    {
        for (auto i = 0; i < 0x100; ++i)
        {
            const auto status = uint8_t(i);
            const auto group  = group_t(status & 0x0F);
            const auto data   = uint32_t(status * status);

            EXPECT_FALSE(is_control_change_message(make_utility_message(status, uint16_t(data))));
            EXPECT_FALSE(is_control_change_message(make_system_message(group, status, uint8_t(data))));
            EXPECT_FALSE(is_control_change_message(make_packet(packet_type::data, group, status, data)));
            EXPECT_FALSE(is_control_change_message(make_packet(packet_type::extended_data, group, status, data)));
            EXPECT_FALSE(is_control_change_message(make_packet(packet_type(0x6), group, status, data)));
            EXPECT_FALSE(is_control_change_message(make_packet(packet_type(0x7), group, status, data)));
            EXPECT_FALSE(is_control_change_message(make_packet(packet_type(0x8), group, status, data)));
            EXPECT_FALSE(is_control_change_message(make_packet(packet_type(0x9), group, status, data)));
            EXPECT_FALSE(is_control_change_message(make_packet(packet_type(0xA), group, status, data)));
            EXPECT_FALSE(is_control_change_message(make_packet(packet_type(0xB), group, status, data)));
            EXPECT_FALSE(is_control_change_message(make_packet(packet_type(0xC), group, status, data)));
            EXPECT_FALSE(is_control_change_message(make_packet(packet_type(0xD), group, status, data)));
            EXPECT_FALSE(is_control_change_message(make_packet(packet_type(0xE), group, status, data)));
            EXPECT_FALSE(is_control_change_message(make_packet(packet_type(0xF), group, status, data)));
        }
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, get_controller_id)
{
    using namespace midi;

    {
        EXPECT_EQ(get_controller_nr(make_midi1_control_change_message(5, 15, 7, controller_value{ uint7_t{ 100 } })),
                  controller_t{ 7 });
        EXPECT_EQ(get_controller_nr(make_midi1_control_change_message(0, 5, 66, controller_value{ uint7_t{ 100 } })),
                  controller_t{ 66 });
    }

    {
        EXPECT_EQ(get_controller_nr(make_midi2_control_change_message(5, 15, 7, controller_value{ uint7_t{ 100 } })),
                  controller_t{ 7 });
        EXPECT_EQ(get_controller_nr(make_midi2_control_change_message(0, 5, 66, controller_value{ uint7_t{ 100 } })),
                  controller_t{ 66 });
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, get_controller_value)
{
    using namespace midi;

    {
        EXPECT_EQ(get_controller_value(make_midi1_control_change_message(5, 15, 7, controller_value{ uint7_t{ 100 } })),
                  controller_value{ uint7_t{ 100 } });
        EXPECT_EQ(get_controller_value(make_midi1_control_change_message(3, 4, 13, controller_value{ uint7_t{ 31 } })),
                  controller_value{ uint7_t{ 31 } });
    }

    {
        EXPECT_EQ(get_controller_value(make_midi2_control_change_message(5, 15, 7, controller_value{ uint7_t{ 100 } })),
                  controller_value{ uint7_t{ 100 } });
        EXPECT_EQ(get_controller_value(make_midi2_control_change_message(1, 4, 99, controller_value{ float{ 0.345 } })),
                  controller_value{ float{ 0.345 } });
        EXPECT_EQ(
          get_controller_value(make_midi2_control_change_message(9, 0, 49, controller_value{ uint32_t{ 12345 } })),
          controller_value{ uint32_t{ 12345 } });
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, is_program_change_message)
{
    using namespace midi;

    // MIDI 1 Channel Voice messages
    {
        EXPECT_FALSE(is_program_change_message(make_midi1_note_off_message(0, 2, 67)));
        EXPECT_FALSE(is_program_change_message(make_midi1_note_on_message(4, 7, 99, velocity{ uint7_t{ 100 } })));
        EXPECT_FALSE(is_program_change_message(make_midi1_note_on_message(13, 5, 60, velocity{ uint7_t{ 0 } })));
        EXPECT_FALSE(
          is_program_change_message(make_midi1_poly_pressure_message(14, 2, 64, controller_value{ uint7_t{ 77 } })));
        EXPECT_FALSE(
          is_program_change_message(make_midi1_control_change_message(5, 15, 7, controller_value{ uint7_t{ 100 } })));
        EXPECT_TRUE(is_program_change_message(make_midi1_program_change_message(14, 7, 42)));
        EXPECT_FALSE(
          is_program_change_message(make_midi1_channel_pressure_message(8, 8, controller_value{ uint7_t{ 77 } })));
        EXPECT_FALSE(is_program_change_message(make_midi1_pitch_bend_message(1, 13, pitch_bend{ uint14_t{ 8177 } })));
    }

    // MIDI 2 Channel Voice messages
    {
        EXPECT_FALSE(is_program_change_message(
          make_registered_per_note_controller_message(15, 10, 44, 2, controller_value{ 123456u })));
        EXPECT_FALSE(is_program_change_message(
          make_assignable_per_note_controller_message(3, 7, 64, 99, controller_value{ 987654u })));
        EXPECT_FALSE(
          is_program_change_message(make_registered_controller_message(2, 9, 0, 4, controller_value{ 123456u })));
        EXPECT_FALSE(
          is_program_change_message(make_assignable_controller_message(8, 0, 4, 12, controller_value{ 987654u })));
        EXPECT_FALSE(is_program_change_message(
          make_relative_registered_controller_message(2, 9, 0, 4, controller_increment{ -1234 })));
        EXPECT_FALSE(is_program_change_message(
          make_relative_assignable_controller_message(8, 0, 4, 12, controller_increment{ 111 })));
        EXPECT_FALSE(is_program_change_message(
          make_per_note_pitch_bend_message(11, 12, 13, pitch_bend{ uint32_t{ 0x80000001u } })));

        EXPECT_FALSE(is_program_change_message(make_midi2_channel_voice_message(1, 0x70, 2, 3, 4, 0xABABABAB)));

        EXPECT_FALSE(is_program_change_message(make_midi2_note_off_message(0, 2, 67, velocity{ uint16_t{ 0x1234 } })));
        EXPECT_FALSE(is_program_change_message(make_midi2_note_on_message(4, 7, 99, velocity{ uint16_t{ 0x4567 } })));
        EXPECT_FALSE(is_program_change_message(make_midi2_note_on_message(13, 5, 60, velocity{ uint16_t{ 0 } })));
        EXPECT_FALSE(is_program_change_message(
          make_midi2_note_on_message(9, 10, 127, velocity{ uint16_t{ 0xA000 } }, pitch_7_9{ note_nr_t{ 60 } })));
        EXPECT_FALSE(
          is_program_change_message(make_midi2_poly_pressure_message(14, 2, 64, controller_value{ 0x6789ABCDu })));
        EXPECT_FALSE(
          is_program_change_message(make_midi2_control_change_message(5, 15, 7, controller_value{ 0x89ABCDEFu })));
        EXPECT_TRUE(is_program_change_message(make_midi2_program_change_message(14, 7, 0, 42)));
        EXPECT_FALSE(
          is_program_change_message(make_midi2_channel_pressure_message(8, 8, controller_value{ 0xABCDEF01u })));
        EXPECT_FALSE(is_program_change_message(make_midi2_pitch_bend_message(1, 13, pitch_bend{ 0x8FFFFFFF })));
    }

    // Other messages
    {
        for (auto i = 0; i < 0x100; ++i)
        {
            const auto status = uint8_t(i);
            const auto group  = group_t(status & 0x0F);
            const auto data   = uint32_t(status * status);

            EXPECT_FALSE(is_program_change_message(make_utility_message(status, uint16_t(data))));
            EXPECT_FALSE(is_program_change_message(make_system_message(group, status, uint8_t(data))));
            EXPECT_FALSE(is_program_change_message(make_packet(packet_type::data, group, status, data)));
            EXPECT_FALSE(is_program_change_message(make_packet(packet_type::extended_data, group, status, data)));
            EXPECT_FALSE(is_program_change_message(make_packet(packet_type(0x6), group, status, data)));
            EXPECT_FALSE(is_program_change_message(make_packet(packet_type(0x7), group, status, data)));
            EXPECT_FALSE(is_program_change_message(make_packet(packet_type(0x8), group, status, data)));
            EXPECT_FALSE(is_program_change_message(make_packet(packet_type(0x9), group, status, data)));
            EXPECT_FALSE(is_program_change_message(make_packet(packet_type(0xA), group, status, data)));
            EXPECT_FALSE(is_program_change_message(make_packet(packet_type(0xB), group, status, data)));
            EXPECT_FALSE(is_program_change_message(make_packet(packet_type(0xC), group, status, data)));
            EXPECT_FALSE(is_program_change_message(make_packet(packet_type(0xD), group, status, data)));
            EXPECT_FALSE(is_program_change_message(make_packet(packet_type(0xE), group, status, data)));
            EXPECT_FALSE(is_program_change_message(make_packet(packet_type(0xF), group, status, data)));
        }
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, get_program_value)
{
    using namespace midi;

    {
        EXPECT_EQ(get_program_value(make_midi1_program_change_message(5, 15, 99)), 99u);
        EXPECT_EQ(get_program_value(make_midi1_program_change_message(3, 8, 13)), 13u);
    }

    {
        EXPECT_EQ(get_program_value(make_midi2_program_change_message(1, 4, 18)), 18u);
        EXPECT_EQ(get_program_value(make_midi2_program_change_message(9, 0, 44)), 44u);
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, is_channel_pressure_message)
{
    using namespace midi;

    // MIDI 1 Channel Voice messages
    {
        EXPECT_FALSE(is_channel_pressure_message(make_midi1_note_off_message(0, 2, 67)));
        EXPECT_FALSE(is_channel_pressure_message(make_midi1_note_on_message(4, 7, 99, velocity{ uint7_t{ 100 } })));
        EXPECT_FALSE(is_channel_pressure_message(make_midi1_note_on_message(13, 5, 60, velocity{ uint7_t{ 0 } })));
        EXPECT_FALSE(
          is_channel_pressure_message(make_midi1_poly_pressure_message(14, 2, 64, controller_value{ uint7_t{ 77 } })));
        EXPECT_FALSE(
          is_channel_pressure_message(make_midi1_control_change_message(5, 15, 7, controller_value{ uint7_t{ 100 } })));
        EXPECT_FALSE(is_channel_pressure_message(make_midi1_program_change_message(14, 7, 42)));
        EXPECT_TRUE(
          is_channel_pressure_message(make_midi1_channel_pressure_message(8, 8, controller_value{ uint7_t{ 77 } })));
        EXPECT_FALSE(is_channel_pressure_message(make_midi1_pitch_bend_message(1, 13, pitch_bend{ uint14_t{ 8177 } })));
    }

    // MIDI 2 Channel Voice messages
    {
        EXPECT_FALSE(is_channel_pressure_message(
          make_registered_per_note_controller_message(15, 10, 44, 2, controller_value{ 123456u })));
        EXPECT_FALSE(is_channel_pressure_message(
          make_assignable_per_note_controller_message(3, 7, 64, 99, controller_value{ 987654u })));
        EXPECT_FALSE(
          is_channel_pressure_message(make_registered_controller_message(2, 9, 0, 4, controller_value{ 123456u })));
        EXPECT_FALSE(
          is_channel_pressure_message(make_assignable_controller_message(8, 0, 4, 12, controller_value{ 987654u })));
        EXPECT_FALSE(is_channel_pressure_message(
          make_relative_registered_controller_message(2, 9, 0, 4, controller_increment{ -1234 })));
        EXPECT_FALSE(is_channel_pressure_message(
          make_relative_assignable_controller_message(8, 0, 4, 12, controller_increment{ 111 })));
        EXPECT_FALSE(is_channel_pressure_message(
          make_per_note_pitch_bend_message(11, 12, 13, pitch_bend{ uint32_t{ 0x80000001u } })));

        EXPECT_FALSE(is_channel_pressure_message(make_midi2_channel_voice_message(1, 0x70, 2, 3, 4, 0xABABABAB)));

        EXPECT_FALSE(
          is_channel_pressure_message(make_midi2_note_off_message(0, 2, 67, velocity{ uint16_t{ 0x1234 } })));
        EXPECT_FALSE(is_channel_pressure_message(make_midi2_note_on_message(4, 7, 99, velocity{ uint16_t{ 0x4567 } })));
        EXPECT_FALSE(is_channel_pressure_message(make_midi2_note_on_message(13, 5, 60, velocity{ uint16_t{ 0 } })));
        EXPECT_FALSE(is_channel_pressure_message(
          make_midi2_note_on_message(9, 10, 127, velocity{ uint16_t{ 0xA000 } }, pitch_7_9{ note_nr_t{ 60 } })));
        EXPECT_FALSE(
          is_channel_pressure_message(make_midi2_poly_pressure_message(14, 2, 64, controller_value{ 0x6789ABCDu })));
        EXPECT_FALSE(
          is_channel_pressure_message(make_midi2_control_change_message(5, 15, 7, controller_value{ 0x89ABCDEFu })));
        EXPECT_FALSE(is_channel_pressure_message(make_midi2_program_change_message(14, 7, 0, 42)));
        EXPECT_TRUE(
          is_channel_pressure_message(make_midi2_channel_pressure_message(8, 8, controller_value{ 0xABCDEF01u })));
        EXPECT_FALSE(is_channel_pressure_message(make_midi2_pitch_bend_message(1, 13, pitch_bend{ 0x8FFFFFFF })));
    }

    // Other messages
    {
        for (auto i = 0; i < 0x100; ++i)
        {
            const auto status = uint8_t(i);
            const auto group  = group_t(status & 0x0F);
            const auto data   = uint32_t(status * status);

            EXPECT_FALSE(is_channel_pressure_message(make_utility_message(status, uint16_t(data))));
            EXPECT_FALSE(is_channel_pressure_message(make_system_message(group, status, uint8_t(data))));
            EXPECT_FALSE(is_channel_pressure_message(make_packet(packet_type::data, group, status, data)));
            EXPECT_FALSE(is_channel_pressure_message(make_packet(packet_type::extended_data, group, status, data)));
            EXPECT_FALSE(is_channel_pressure_message(make_packet(packet_type(0x6), group, status, data)));
            EXPECT_FALSE(is_channel_pressure_message(make_packet(packet_type(0x7), group, status, data)));
            EXPECT_FALSE(is_channel_pressure_message(make_packet(packet_type(0x8), group, status, data)));
            EXPECT_FALSE(is_channel_pressure_message(make_packet(packet_type(0x9), group, status, data)));
            EXPECT_FALSE(is_channel_pressure_message(make_packet(packet_type(0xA), group, status, data)));
            EXPECT_FALSE(is_channel_pressure_message(make_packet(packet_type(0xB), group, status, data)));
            EXPECT_FALSE(is_channel_pressure_message(make_packet(packet_type(0xC), group, status, data)));
            EXPECT_FALSE(is_channel_pressure_message(make_packet(packet_type(0xD), group, status, data)));
            EXPECT_FALSE(is_channel_pressure_message(make_packet(packet_type(0xE), group, status, data)));
            EXPECT_FALSE(is_channel_pressure_message(make_packet(packet_type(0xF), group, status, data)));
        }
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, get_channel_pressure_value)
{
    using namespace midi;

    {
        EXPECT_EQ(
          get_channel_pressure_value(make_midi1_channel_pressure_message(5, 15, controller_value{ uint7_t{ 38 } })),
          controller_value{ uint7_t{ 38 } });
        EXPECT_EQ(
          get_channel_pressure_value(make_midi1_channel_pressure_message(3, 8, controller_value{ uint7_t{ 77 } })),
          controller_value{ uint7_t{ 77 } });
    }

    {
        EXPECT_EQ(
          get_channel_pressure_value(make_midi2_channel_pressure_message(1, 4, controller_value{ float{ 0.345 } })),
          controller_value{ float{ 0.345 } });
        EXPECT_EQ(
          get_channel_pressure_value(make_midi2_channel_pressure_message(9, 0, controller_value{ uint32_t{ 12345 } })),
          controller_value{ uint32_t{ 12345 } });
        EXPECT_EQ(
          get_channel_pressure_value(make_midi2_channel_pressure_message(2, 14, controller_value{ uint7_t{ 109 } })),
          controller_value{ uint7_t{ 109 } });
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, is_channel_pitch_bend_message)
{
    using namespace midi;

    // MIDI 1 Channel Voice messages
    {
        EXPECT_FALSE(is_channel_pitch_bend_message(make_midi1_note_off_message(0, 2, 67)));
        EXPECT_FALSE(is_channel_pitch_bend_message(make_midi1_note_on_message(4, 7, 99, velocity{ uint7_t{ 100 } })));
        EXPECT_FALSE(is_channel_pitch_bend_message(make_midi1_note_on_message(13, 5, 60, velocity{ uint7_t{ 0 } })));
        EXPECT_FALSE(is_channel_pitch_bend_message(
          make_midi1_poly_pressure_message(14, 2, 64, controller_value{ uint7_t{ 77 } })));
        EXPECT_FALSE(is_channel_pitch_bend_message(
          make_midi1_control_change_message(5, 15, 7, controller_value{ uint7_t{ 100 } })));
        EXPECT_FALSE(is_channel_pitch_bend_message(make_midi1_program_change_message(14, 7, 42)));
        EXPECT_FALSE(
          is_channel_pitch_bend_message(make_midi1_channel_pressure_message(8, 8, controller_value{ uint7_t{ 77 } })));
        EXPECT_TRUE(
          is_channel_pitch_bend_message(make_midi1_pitch_bend_message(1, 13, pitch_bend{ uint14_t{ 8177 } })));
    }

    // MIDI 2 Channel Voice messages
    {
        EXPECT_FALSE(is_channel_pitch_bend_message(
          make_registered_per_note_controller_message(15, 10, 44, 2, controller_value{ 123456u })));
        EXPECT_FALSE(is_channel_pitch_bend_message(
          make_assignable_per_note_controller_message(3, 7, 64, 99, controller_value{ 987654u })));
        EXPECT_FALSE(
          is_channel_pitch_bend_message(make_registered_controller_message(2, 9, 0, 4, controller_value{ 123456u })));
        EXPECT_FALSE(
          is_channel_pitch_bend_message(make_assignable_controller_message(8, 0, 4, 12, controller_value{ 987654u })));
        EXPECT_FALSE(is_channel_pitch_bend_message(
          make_relative_registered_controller_message(2, 9, 0, 4, controller_increment{ -1234 })));
        EXPECT_FALSE(is_channel_pitch_bend_message(
          make_relative_assignable_controller_message(8, 0, 4, 12, controller_increment{ 111 })));
        EXPECT_FALSE(is_channel_pitch_bend_message(
          make_per_note_pitch_bend_message(11, 12, 13, pitch_bend{ uint32_t{ 0x80000001u } })));

        EXPECT_FALSE(is_channel_pitch_bend_message(make_midi2_channel_voice_message(1, 0x70, 2, 3, 4, 0xABABABAB)));

        EXPECT_FALSE(
          is_channel_pitch_bend_message(make_midi2_note_off_message(0, 2, 67, velocity{ uint16_t{ 0x1234 } })));
        EXPECT_FALSE(
          is_channel_pitch_bend_message(make_midi2_note_on_message(4, 7, 99, velocity{ uint16_t{ 0x4567 } })));
        EXPECT_FALSE(is_channel_pitch_bend_message(make_midi2_note_on_message(13, 5, 60, velocity{ uint16_t{ 0 } })));
        EXPECT_FALSE(is_channel_pitch_bend_message(
          make_midi2_note_on_message(9, 10, 127, velocity{ uint16_t{ 0xA000 } }, pitch_7_9{ note_nr_t{ 60 } })));
        EXPECT_FALSE(
          is_channel_pitch_bend_message(make_midi2_poly_pressure_message(14, 2, 64, controller_value{ 0x6789ABCDu })));
        EXPECT_FALSE(
          is_channel_pitch_bend_message(make_midi2_control_change_message(5, 15, 7, controller_value{ 0x89ABCDEFu })));
        EXPECT_FALSE(is_channel_pitch_bend_message(make_midi2_program_change_message(14, 7, 0, 42)));
        EXPECT_FALSE(
          is_channel_pitch_bend_message(make_midi2_channel_pressure_message(8, 8, controller_value{ 0xABCDEF01u })));
        EXPECT_TRUE(is_channel_pitch_bend_message(make_midi2_pitch_bend_message(1, 13, pitch_bend{ 0x8FFFFFFF })));
    }

    // Other messages
    {
        for (auto i = 0; i < 0x100; ++i)
        {
            const auto status = uint8_t(i);
            const auto group  = group_t(status & 0x0F);
            const auto data   = uint32_t(status * status);

            EXPECT_FALSE(is_channel_pitch_bend_message(make_utility_message(status, uint16_t(data))));
            EXPECT_FALSE(is_channel_pitch_bend_message(make_system_message(group, status, uint8_t(data))));
            EXPECT_FALSE(is_channel_pitch_bend_message(make_packet(packet_type::data, group, status, data)));
            EXPECT_FALSE(is_channel_pitch_bend_message(make_packet(packet_type::extended_data, group, status, data)));
            EXPECT_FALSE(is_channel_pitch_bend_message(make_packet(packet_type(0x6), group, status, data)));
            EXPECT_FALSE(is_channel_pitch_bend_message(make_packet(packet_type(0x7), group, status, data)));
            EXPECT_FALSE(is_channel_pitch_bend_message(make_packet(packet_type(0x8), group, status, data)));
            EXPECT_FALSE(is_channel_pitch_bend_message(make_packet(packet_type(0x9), group, status, data)));
            EXPECT_FALSE(is_channel_pitch_bend_message(make_packet(packet_type(0xA), group, status, data)));
            EXPECT_FALSE(is_channel_pitch_bend_message(make_packet(packet_type(0xB), group, status, data)));
            EXPECT_FALSE(is_channel_pitch_bend_message(make_packet(packet_type(0xC), group, status, data)));
            EXPECT_FALSE(is_channel_pitch_bend_message(make_packet(packet_type(0xD), group, status, data)));
            EXPECT_FALSE(is_channel_pitch_bend_message(make_packet(packet_type(0xE), group, status, data)));
            EXPECT_FALSE(is_channel_pitch_bend_message(make_packet(packet_type(0xF), group, status, data)));
        }
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, get_channel_pitch_bend_value)
{
    using namespace midi;

    {
        EXPECT_EQ(get_channel_pitch_bend_value(make_midi1_pitch_bend_message(5, 15, pitch_bend{ uint14_t{ 12500 } })),
                  pitch_bend{ uint14_t{ 12500 } });
        EXPECT_EQ(get_channel_pitch_bend_value(make_midi1_pitch_bend_message(3, 8, pitch_bend{ uint14_t{ 0xAFED } })),
                  pitch_bend{ uint14_t{ 0xAFED } });
    }

    {
        EXPECT_EQ(get_channel_pitch_bend_value(make_midi2_pitch_bend_message(1, 4, pitch_bend{ float{ 0.345 } })),
                  pitch_bend{ float{ 0.345 } });
        EXPECT_EQ(get_channel_pitch_bend_value(make_midi2_pitch_bend_message(9, 0, pitch_bend{ uint14_t{ 1234 } })),
                  pitch_bend{ uint14_t{ 1234 } });
        EXPECT_EQ(
          get_channel_pitch_bend_value(make_midi2_pitch_bend_message(2, 14, pitch_bend{ uint32_t{ 0x81234567 } })),
          pitch_bend{ uint32_t{ 0x81234567 } });
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, as_midi1_channel_voice_message)
{
    using namespace midi;

    // note_on
    {
        auto m1 = as_midi1_channel_voice_message(
          midi2_channel_voice_message_view{ make_midi2_note_on_message(4, 7, 99, velocity{ uint16_t{ 0x4567 } }) });
        EXPECT_TRUE(m1);
        if (m1)
            EXPECT_EQ(*m1, make_midi1_note_on_message(4, 7, 99, velocity{ uint16_t{ 0x4567 } }));

        auto m2 = as_midi1_channel_voice_message(
          midi2_channel_voice_message_view{ make_midi2_note_on_message(13, 5, 60, velocity{ uint16_t{ 0 } }) });
        EXPECT_TRUE(m2);
        if (m2)
            // MIDI 2 note on velocity 0 must be translated to MIDI 1 note velocity 1
            EXPECT_EQ(*m2, make_midi1_note_on_message(13, 5, 60, velocity{ uint7_t{ 1 } }));

        EXPECT_FALSE(as_midi1_channel_voice_message(midi2_channel_voice_message_view{
          make_midi2_note_on_message(9, 10, 127, velocity{ uint16_t{ 0xA000 } }, pitch_7_9{ note_nr_t{ 60 } }) }));
    }

    // note_off
    {
        auto m1 = as_midi1_channel_voice_message(
          midi2_channel_voice_message_view{ make_midi2_note_off_message(3, 9, 66, velocity{ uint16_t{ 0x1234 } }) });
        EXPECT_TRUE(m1);
        if (m1)
            EXPECT_EQ(*m1, make_midi1_note_off_message(3, 9, 66, velocity{ uint16_t{ 0x1234 } }));

        auto m2 = as_midi1_channel_voice_message(
          midi2_channel_voice_message_view{ make_midi2_note_off_message(13, 5, 60, velocity{ uint7_t{ 0 } }) });
        EXPECT_TRUE(m2);
        if (m2)
            EXPECT_EQ(*m2, make_midi1_note_off_message(13, 5, 60, velocity{ uint7_t{ 0 } }));

        // MIDI 2 note on with attribute is not converted!
        EXPECT_FALSE(as_midi1_channel_voice_message(midi2_channel_voice_message_view{
          make_midi2_note_off_message(9, 10, 127, velocity{ uint16_t{ 0xA000 } }, 4, 16) }));
    }

    // poly_pressure
    {
        auto m1 = as_midi1_channel_voice_message(midi2_channel_voice_message_view{
          make_midi2_poly_pressure_message(14, 2, 64, controller_value{ uint7_t{ 77 } }) });
        EXPECT_TRUE(m1);
        if (m1)
            EXPECT_EQ(*m1, make_midi1_poly_pressure_message(14, 2, 64, controller_value{ uint7_t{ 77 } }));

        auto m2 = as_midi1_channel_voice_message(midi2_channel_voice_message_view{
          make_midi2_poly_pressure_message(14, 2, 64, controller_value{ 0x6789ABCDu }) });
        EXPECT_TRUE(m2);
        if (m2)
            EXPECT_EQ(*m2, make_midi1_poly_pressure_message(14, 2, 64, controller_value{ 0x6789ABCDu }));
    }

    // control_change
    {
        auto m1 = as_midi1_channel_voice_message(midi2_channel_voice_message_view{
          make_midi2_control_change_message(5, 15, 7, controller_value{ 0x89ABCDEFu }) });
        EXPECT_TRUE(m1);
        if (m1)
            EXPECT_EQ(*m1, make_midi1_control_change_message(5, 15, 7, controller_value{ 0x89ABCDEFu }));

        auto m2 = as_midi1_channel_voice_message(midi2_channel_voice_message_view{
          make_midi2_control_change_message(9, 0, 49, controller_value{ uint32_t{ 12345 } }) });
        EXPECT_TRUE(m2);
        if (m2)
            EXPECT_EQ(*m2, make_midi1_control_change_message(9, 0, 49, controller_value{ uint32_t{ 12345 } }));

        // 7.4.6 MIDI 2.0 Control Change Message
        // Implementation Recommendations
        // * Devices receiving the MIDI 2.0 Protocol should ignore Control Change messages with indexes of 0, 6, 32, 38,
        // 98, 99, 100, and 101.
        // * In MIDI 2.0 Protocol, Control Change 88 shall not be used for High Resolution Velocity. The Note On 16 bit
        // Velocity value has a higher range than the MIDI 1.0 High Resolution Velocity controller and Note On combined.
        for (midi::uint7_t index : { 0, 6, 32, 38, 88, 98, 99, 100, 101 })
        {
            EXPECT_FALSE(as_midi1_channel_voice_message(midi2_channel_voice_message_view{
              make_midi2_control_change_message(9, 0, index, controller_value{ uint32_t{ 12345 } }) }));
        }
    }

    // program_change
    {
        auto m1 = as_midi1_channel_voice_message(
          midi2_channel_voice_message_view{ make_midi2_program_change_message(14, 7, 42) });
        EXPECT_TRUE(m1);
        if (m1)
            EXPECT_EQ(*m1, make_midi1_program_change_message(14, 7, 42));

        auto m2 = as_midi1_channel_voice_message(
          midi2_channel_voice_message_view{ make_midi2_program_change_message(9, 0, 44) });
        EXPECT_TRUE(m2);
        if (m2)
            EXPECT_EQ(*m2, make_midi1_program_change_message(9, 0, 44));

        // MIDI 2 program change with bank set is not converted!
        EXPECT_FALSE(as_midi1_channel_voice_message(
          midi2_channel_voice_message_view{ make_midi2_program_change_message(14, 7, 0, 42) }));
    }

    // channel pressure
    {
        auto m1 = as_midi1_channel_voice_message(midi2_channel_voice_message_view{
          make_midi2_channel_pressure_message(8, 8, controller_value{ 0xABCDEF01 }) });
        EXPECT_TRUE(m1);
        if (m1)
            EXPECT_EQ(*m1, make_midi1_channel_pressure_message(8, 8, controller_value{ 0xABCDEF01 }));

        auto m2 = as_midi1_channel_voice_message(midi2_channel_voice_message_view{
          make_midi2_channel_pressure_message(2, 14, controller_value{ uint7_t{ 109 } }) });
        EXPECT_TRUE(m2);
        if (m2)
            EXPECT_EQ(*m2, make_midi1_channel_pressure_message(2, 14, controller_value{ uint7_t{ 109 } }));
    }

    // pitch_bend
    {
        auto m1 = as_midi1_channel_voice_message(
          midi2_channel_voice_message_view{ make_midi2_pitch_bend_message(3, 8, pitch_bend{ uint14_t{ 0xAFED } }) });
        EXPECT_TRUE(m1);
        if (m1)
            EXPECT_EQ(*m1, make_midi1_pitch_bend_message(3, 8, pitch_bend{ uint14_t{ 0xAFED } }));

        auto m2 = as_midi1_channel_voice_message(midi2_channel_voice_message_view{
          make_midi2_pitch_bend_message(2, 14, pitch_bend{ uint32_t{ 0x81234567 } }) });
        EXPECT_TRUE(m2);
        if (m2)
            EXPECT_EQ(*m2, make_midi1_pitch_bend_message(2, 14, pitch_bend{ uint32_t{ 0x81234567 } }));
    }

    // unsupported
    {
        EXPECT_FALSE(as_midi1_channel_voice_message(midi2_channel_voice_message_view{
          make_registered_per_note_controller_message(15, 10, 44, 2, controller_value{ 123456u }) }));

        EXPECT_FALSE(as_midi1_channel_voice_message(midi2_channel_voice_message_view{
          make_assignable_per_note_controller_message(3, 7, 64, 99, controller_value{ 987654u }) }));

        EXPECT_FALSE(as_midi1_channel_voice_message(midi2_channel_voice_message_view{
          make_registered_controller_message(2, 9, 0, 4, controller_value{ 123456u }) }));

        EXPECT_FALSE(as_midi1_channel_voice_message(midi2_channel_voice_message_view{
          make_assignable_per_note_controller_message(3, 7, 64, 99, controller_value{ 987654u }) }));

        EXPECT_FALSE(as_midi1_channel_voice_message(midi2_channel_voice_message_view{
          make_relative_registered_controller_message(2, 9, 0, 4, controller_increment{ -1234 }) }));

        EXPECT_FALSE(as_midi1_channel_voice_message(midi2_channel_voice_message_view{
          make_relative_assignable_controller_message(8, 0, 4, 12, controller_increment{ 111 }) }));

        EXPECT_FALSE(as_midi1_channel_voice_message(
          midi2_channel_voice_message_view{ make_per_note_pitch_bend_message(11, 12, 13, pitch_bend{ 0x80000001u }) }));

        EXPECT_FALSE(as_midi1_channel_voice_message(
          midi2_channel_voice_message_view{ make_per_note_management_message(2, 7, 0x42, note_management::reset) }));
    }
}

//-----------------------------------------------

TEST_F(channel_voice_message, as_midi2_channel_voice_message)
{
    using namespace midi;

    // note_on
    {
        EXPECT_EQ(*as_midi2_channel_voice_message(midi1_channel_voice_message_view{
                    make_midi1_note_on_message(4, 7, 99, velocity{ uint7_t{ 75 } }) }),
                  make_midi2_note_on_message(4, 7, 99, velocity{ uint7_t{ 75 } }));

        EXPECT_EQ(*as_midi2_channel_voice_message(midi1_channel_voice_message_view{
                    make_midi1_note_on_message(13, 5, 60, velocity{ uint7_t{ 0 } }) }),
                  make_midi2_note_off_message(13, 5, 60, velocity{ uint7_t{ 64 } }));
    }

    // note_off
    {
        EXPECT_EQ(*as_midi2_channel_voice_message(midi1_channel_voice_message_view{
                    make_midi1_note_off_message(3, 9, 66, velocity{ uint7_t{ 112 } }) }),
                  make_midi2_note_off_message(3, 9, 66, velocity{ uint7_t{ 112 } }));

        EXPECT_EQ(*as_midi2_channel_voice_message(midi1_channel_voice_message_view{
                    make_midi1_note_off_message(13, 5, 60, velocity{ uint7_t{ 0 } }) }),
                  make_midi2_note_off_message(13, 5, 60, velocity{ uint7_t{ 0 } }));
    }

    // poly_pressure
    {
        EXPECT_EQ(*as_midi2_channel_voice_message(midi1_channel_voice_message_view{
                    make_midi1_poly_pressure_message(14, 2, 64, controller_value{ uint7_t{ 77 } }) }),
                  make_midi2_poly_pressure_message(14, 2, 64, controller_value{ uint7_t{ 77 } }));

        EXPECT_EQ(*as_midi2_channel_voice_message(midi1_channel_voice_message_view{
                    make_midi1_poly_pressure_message(14, 2, 64, controller_value{ uint7_t{ 115 } }) }),
                  make_midi2_poly_pressure_message(14, 2, 64, controller_value{ uint7_t{ 115 } }));
    }

    // control_change
    {
        EXPECT_EQ(*as_midi2_channel_voice_message(midi1_channel_voice_message_view{
                    make_midi1_control_change_message(5, 15, 7, controller_value{ uint7_t{ 19 } }) }),
                  make_midi2_control_change_message(5, 15, 7, controller_value{ uint7_t{ 19 } }));

        EXPECT_EQ(*as_midi2_channel_voice_message(midi1_channel_voice_message_view{
                    make_midi1_control_change_message(9, 0, 49, controller_value{ uint7_t{ 65 } }) }),
                  make_midi2_control_change_message(9, 0, 49, controller_value{ uint7_t{ 65 } }));

        // 7.4.6 MIDI 2.0 Control Change Message
        // Implementation Recommendations
        // Devices sending the MIDI 2.0 Protocol should not transmit Control Change messages with indexes of 6, 38, 98,
        // 99, 100, or 101. Instead, they should transmit the new Assignable Controller messages and Registered
        // Controller messages (see Section 7.4.7). These new messages are more friendly to send, to receive, and to
        // edit in a sequencer.
        // * Devices sending the MIDI 2.0 Protocol should not transmit Control Change messages with indexes of 0 and 32.
        // Instead they should transmit the new MIDI 2.0 Program Change message (see Section 7.4.9).
        // * In MIDI 2.0 Protocol, Control Change 88 shall not be used for High Resolution Velocity. The Note On 16 bit
        // Velocity value has a higher range than the MIDI 1.0 High Resolution Velocity controller and Note On combined.
        for (midi::uint7_t index : { 0, 6, 32, 38, 88, 98, 99, 100, 101 })
        {
            EXPECT_FALSE(as_midi2_channel_voice_message(midi1_channel_voice_message_view{
              make_midi1_control_change_message(9, 0, index, controller_value{ uint32_t{ 12345 } }) }));
        }
    }

    // program_change
    {
        EXPECT_EQ(*as_midi2_channel_voice_message(
                    midi1_channel_voice_message_view{ make_midi1_program_change_message(14, 7, 42) }),
                  make_midi2_program_change_message(14, 7, 42));

        EXPECT_EQ(*as_midi2_channel_voice_message(
                    midi1_channel_voice_message_view{ make_midi1_program_change_message(9, 0, 44) }),
                  make_midi2_program_change_message(9, 0, 44));
    }

    // channel_pressure
    {
        EXPECT_EQ(*as_midi2_channel_voice_message(midi1_channel_voice_message_view{
                    make_midi1_channel_pressure_message(8, 8, controller_value{ uint7_t{ 81 } }) }),
                  make_midi2_channel_pressure_message(8, 8, controller_value{ uint7_t{ 81 } }));

        EXPECT_EQ(*as_midi2_channel_voice_message(midi1_channel_voice_message_view{
                    make_midi1_channel_pressure_message(2, 14, controller_value{ uint7_t{ 109 } }) }),
                  make_midi2_channel_pressure_message(2, 14, controller_value{ uint7_t{ 109 } }));
    }

    // pitch_bend
    {
        EXPECT_EQ(*as_midi2_channel_voice_message(midi1_channel_voice_message_view{
                    make_midi1_pitch_bend_message(3, 8, pitch_bend{ uint14_t{ 0xAFED } }) }),
                  make_midi2_pitch_bend_message(3, 8, pitch_bend{ uint14_t{ 0xAFED } }));

        EXPECT_EQ(*as_midi2_channel_voice_message(midi1_channel_voice_message_view{
                    make_midi1_pitch_bend_message(2, 14, pitch_bend{ uint14_t{ 1234 } }) }),
                  make_midi2_pitch_bend_message(2, 14, pitch_bend{ uint14_t{ 1234 } }));
    }

    // invalid
    {
        EXPECT_FALSE(as_midi2_channel_voice_message(
          midi1_channel_voice_message_view{ make_midi1_channel_voice_message(5, 0, 4, 1, 2) }));
    }
}

//-----------------------------------------------
