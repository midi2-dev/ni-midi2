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
#include <midi/midi2_channel_voice_message.h>

//-----------------------------------------------

class midi2_channel_voice_message : public ::testing::Test
{ };

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, constructors)
{
    using namespace midi;

    {
        constexpr auto m = midi::midi2_channel_voice_message{ 4, channel_voice_status::note_on, 6, 44, 88, 0xD0000000 };

        EXPECT_TRUE(is_midi2_channel_voice_message(m));

        EXPECT_EQ(0x44962C58u, m.data[0]);
        EXPECT_EQ(0xD0000000u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(2u, m.size());
        EXPECT_EQ(packet_type::midi2_channel_voice, m.type());
        EXPECT_EQ(4u, m.group());
        EXPECT_EQ(channel_voice_status::note_on + 6, m.status());
        EXPECT_EQ(0x96u, m.byte2());
        EXPECT_EQ(0x2Cu, m.byte3());
        EXPECT_EQ(88u, m.byte4());

        EXPECT_TRUE(as_midi2_channel_voice_message_view(m));

        auto v = midi2_channel_voice_message_view{ m };
        EXPECT_EQ(4u, v.group());
        EXPECT_EQ(midi1_channel_voice_status::note_on, v.status());
        EXPECT_EQ(6u, v.channel());
        EXPECT_EQ(0x2Cu, v.byte3());
        EXPECT_EQ(88u, v.byte4());
        EXPECT_EQ(0xD0000000u, v.data());
    }

    {
        const auto m = midi::midi2_channel_voice_message{ midi::universal_packet{ 0x43827A4C, 0x87654321 } };

        EXPECT_TRUE(is_midi2_channel_voice_message(m));

        EXPECT_EQ(0x43827A4Cu, m.data[0]);
        EXPECT_EQ(0x87654321u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(2u, m.size());
        EXPECT_EQ(packet_type::midi2_channel_voice, m.type());
        EXPECT_EQ(3u, m.group());
        EXPECT_EQ(channel_voice_status::note_off + 2, m.status());
        EXPECT_EQ(0x82u, m.byte2());
        EXPECT_EQ(0x7Au, m.byte3());
        EXPECT_EQ(0x4Cu, m.byte4());

        EXPECT_TRUE(as_midi2_channel_voice_message_view(m));

        auto v = midi2_channel_voice_message_view{ m };
        EXPECT_EQ(3u, v.group());
        EXPECT_EQ(midi1_channel_voice_status::note_off, v.status());
        EXPECT_EQ(2u, v.channel());
        EXPECT_EQ(0x7Au, v.byte3());
        EXPECT_EQ(0x4C, v.byte4());
        EXPECT_EQ(0x87654321, v.data());
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, midi2_channel_voice_message_view)
{
    using namespace midi;

    {
        static constexpr auto m =
          midi::midi2_channel_voice_message{ 4, channel_voice_status::note_on, 6, 44, 88, 0xD0000000 };

        EXPECT_TRUE(as_midi2_channel_voice_message_view(m));

        constexpr auto v = midi2_channel_voice_message_view{ m };
        EXPECT_EQ(4u, v.group());
        EXPECT_EQ(midi1_channel_voice_status::note_on, v.status());
        EXPECT_EQ(6u, v.channel());
        EXPECT_EQ(0x2Cu, v.byte3());
        EXPECT_EQ(88u, v.byte4());
        EXPECT_EQ(0xD0000000u, v.data());
    }

    {
        const auto m = midi::midi2_channel_voice_message{ 7, channel_voice_status::note_off, 5, 99, 10, 0x80000001 };

        EXPECT_TRUE(as_midi2_channel_voice_message_view(m));

        const auto v = midi2_channel_voice_message_view{ m };
        EXPECT_EQ(7u, v.group());
        EXPECT_EQ(midi1_channel_voice_status::note_off, v.status());
        EXPECT_EQ(5u, v.channel());
        EXPECT_EQ(99u, v.byte3());
        EXPECT_EQ(10u, v.byte4());
        EXPECT_EQ(0x80000001u, v.data());
    }

    {
        const auto m   = midi::make_midi1_note_on_message(1, 1, 70, midi::velocity{ uint7_t{ 120 } });
        const auto opt = as_midi2_channel_voice_message_view(m);
        EXPECT_FALSE(opt.has_value());
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_midi2_channel_voice_message)
{
    using namespace midi;

    {
        constexpr auto m =
          make_midi2_channel_voice_message(9, channel_voice_status::note_on, 5, 0x74, 0x00, 0x12345678);
        EXPECT_EQ((universal_packet{ 0x49957400, 0x12345678 }), m);
    }

    {
        const auto m =
          make_midi2_channel_voice_message(4, channel_voice_status::per_note_pitch_bend, 14, 0x43, 0x00, 0x8000D123);
        EXPECT_EQ((universal_packet{ 0x446E4300, 0x8000D123 }), m);
        EXPECT_TRUE(is_per_note_pitch_bend_message(m));
    }

    {
        constexpr auto m =
          make_midi2_channel_voice_message(12, channel_voice_status::registered_controller, 5, 4, 0x62, 0xABCDEF00);
        EXPECT_EQ((universal_packet{ 0x4C250462, 0xABCDEF00 }), m);
        EXPECT_TRUE(is_registered_controller_message(m));
    }

    { // invalid channel in status
        const auto m = make_midi2_channel_voice_message(0, channel_voice_status::per_note_management + 11, 9, 0, 3, 0);
        EXPECT_EQ((universal_packet{ 0x40F90003 }), m);
    }

    { // invalid channel bits
        constexpr auto m =
          make_midi2_channel_voice_message(0, channel_voice_status::control_change, 0x35, 0x45, 0x00, 0x77);
        EXPECT_EQ((universal_packet{ 0x40B54500, 0x00000077 }), m);
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_midi2_note_off_message)
{
    using namespace midi;

    {
        constexpr auto m = make_midi2_note_off_message(4, 9, 0x50, velocity{ uint14_t{ 0xD123 } });
        EXPECT_EQ((universal_packet{ 0x44895000, 0xD1230000 }), m);
    }

    {
        const auto m = make_midi2_note_off_message(13, 14, 0x14, velocity{ uint14_t{ 0xD123 } }, 0x01, 0x9876);
        EXPECT_EQ((universal_packet{ 0x4D8E1401, 0xD1239876 }), m);
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_midi2_note_on_message)
{
    using namespace midi;

    {
        constexpr auto m = make_midi2_note_on_message(8, 4, 0x12, velocity{ uint14_t{ 0x0102 } });
        EXPECT_EQ((universal_packet{ 0x48941200, 0x01020000 }), m);
    }

    {
        const auto m = make_midi2_note_on_message(7, 3, 0x77, velocity{ uint14_t{ 0xF001 } }, 0x03, 0x6135);
        EXPECT_EQ((universal_packet{ 0x47937703, 0xF0016135 }), m);
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_midi2_poly_pressure_message)
{
    using namespace midi;

    {
        constexpr auto m = make_midi2_poly_pressure_message(0, 1, 99, controller_value{ 1028733u });
        EXPECT_EQ((universal_packet{ 0x40A16300, 0x000FB27D }), m);
    }

    {
        const auto m = make_midi2_poly_pressure_message(6, 9, 64, controller_value{ 100000000u });
        EXPECT_EQ((universal_packet{ 0x46A94000, 0x05F5E100 }), m);
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_registered_per_note_controller_message)
{
    using namespace midi;

    {
        constexpr auto m =
          make_registered_per_note_controller_message(0xF, 0xE, 44, 77, controller_value{ 0x33445566u });
        EXPECT_EQ((universal_packet{ 0x4F0E2C4D, 0x33445566 }), m);
        EXPECT_TRUE(is_registered_per_note_controller_message(m));
        EXPECT_TRUE(get_per_note_controller_index(m) == 77);
        EXPECT_FALSE(is_registered_per_note_controller_pitch_message(m));
    }

    {
        const auto m = make_registered_per_note_controller_message(1, 5, 0x33, 0x44, controller_value{ 0x55u });
        EXPECT_EQ((universal_packet{ 0x41053344, 0x00000055 }), m);
        EXPECT_TRUE(is_registered_per_note_controller_message(m));
        EXPECT_TRUE(get_per_note_controller_index(m) == 0x44);
        EXPECT_FALSE(is_registered_per_note_controller_pitch_message(m));
    }

    {
        const auto m = make_registered_per_note_controller_message(
          7, 4, 64, registered_per_note_controller::pitch_7_25, controller_value{ 0x12345678u });
        EXPECT_EQ((universal_packet{ 0x47044003, 0x12345678u }), m);
        EXPECT_TRUE(is_registered_per_note_controller_message(m));
        EXPECT_TRUE(get_per_note_controller_index(m) == 3);
        EXPECT_TRUE(is_registered_per_note_controller_pitch_message(m));
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_assignable_per_note_controller_message)
{
    using namespace midi;

    {
        constexpr auto m =
          make_assignable_per_note_controller_message(0xE, 0xD, 0x12, 0x34, controller_value{ 0x44556677u });
        EXPECT_EQ((universal_packet{ 0x4E1D1234, 0x44556677 }), m);
        EXPECT_TRUE(is_assignable_per_note_controller_message(m));
        EXPECT_TRUE(get_per_note_controller_index(m) == 0x34);
    }

    {
        const auto m = make_assignable_per_note_controller_message(2, 4, 0x31, 0x43, controller_value{ 0x76000054u });
        EXPECT_EQ((universal_packet{ 0x42143143, 0x76000054 }), m);
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_per_note_management_message)
{
    using namespace midi;

    {
        constexpr auto m = make_per_note_management_message(6, 5, 4, note_management::detach);
        EXPECT_EQ(universal_packet{ 0x46F50402 }, m);
    }

    {
        const auto m = make_per_note_management_message(2, 7, 0x42, note_management::reset);
        EXPECT_EQ(universal_packet{ 0x42F74201 }, m);
    }

    {
        constexpr auto m = make_per_note_management_message(15, 13, 96, note_management::detach_and_reset);
        EXPECT_EQ(universal_packet{ 0x4FFD6003 }, m);
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_midi2_control_change_message)
{
    using namespace midi;

    {
        constexpr auto m = make_midi2_control_change_message(9, 4, 0x38, controller_value{ 0x98765432u });
        EXPECT_EQ((universal_packet{ 0x49B43800, 0x98765432 }), m);
    }

    { // invalid CC bits
        const auto m = make_midi2_control_change_message(11, 12, 0xC5, controller_value{ 0xDEADBEEFu });
        EXPECT_EQ((universal_packet{ 0x4BBC4500, 0xDEADBEEF }), m);
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_registered_controller_message)
{
    using namespace midi;

    {
        constexpr auto m = make_registered_controller_message(3, 7, 9, 0x45, controller_value{ 0x80101010u });
        EXPECT_EQ((universal_packet{ 0x43270945, 0x80101010 }), m);
        EXPECT_TRUE(is_registered_controller_message(m));
    }

    { // invalid bank bits
        const auto m = make_registered_controller_message(14, 3, 0xC5, 1, controller_value{ 0xDEADBEEFu });
        EXPECT_EQ((universal_packet{ 0x4E234501, 0xDEADBEEF }), m);
    }

    { // invalid controller bits
        constexpr auto m = make_registered_controller_message(11, 12, 9, 0xF3, controller_value{ 0xBADBABEu });
        EXPECT_EQ((universal_packet{ 0x4B2C0973, 0xBADBABE }), m);
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_assignable_controller_message)
{
    using namespace midi;

    {
        constexpr auto m = make_assignable_controller_message(3, 7, 9, 0x45, controller_value{ 0x80101010u });
        EXPECT_EQ((universal_packet{ 0x43370945, 0x80101010 }), m);
        EXPECT_TRUE(is_assignable_controller_message(m));
    }

    { // invalid bank bits
        const auto m = make_assignable_controller_message(14, 3, 0xC5, 1, controller_value{ 0xDEADBEEFu });
        EXPECT_EQ((universal_packet{ 0x4E334501, 0xDEADBEEF }), m);
    }

    { // invalid controller bits
        constexpr auto m = make_assignable_controller_message(11, 12, 9, 0xF3, controller_value{ 0xBADBABEu });
        EXPECT_EQ((universal_packet{ 0x4B3C0973, 0xBADBABE }), m);
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_relative_registered_controller_message)
{
    using namespace midi;

    {
        constexpr auto m = make_relative_registered_controller_message(3, 7, 9, 0x45, controller_increment{ -522 });
        EXPECT_EQ((universal_packet{ 0x43470945, 0xFFFFFDF6 }), m);
    }

    { // invalid bank bits
        const auto m = make_relative_registered_controller_message(14, 3, 0xC5, 1, controller_increment{ 1024 });
        EXPECT_EQ((universal_packet{ 0x4E434501, 0x00000400 }), m);
    }

    { // invalid controller bits
        constexpr auto m = make_relative_registered_controller_message(11, 12, 9, 0xF3, controller_increment{ -99 });
        EXPECT_EQ((universal_packet{ 0x4B4C0973, 0xFFFFFF9D }), m);
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_relative_assignable_controller_message)
{
    using namespace midi;

    {
        constexpr auto m = make_relative_assignable_controller_message(3, 7, 9, 0x45, controller_increment{ -522 });
        EXPECT_EQ((universal_packet{ 0x43570945, 0xFFFFFDF6 }), m);
    }

    { // invalid bank bits
        const auto m = make_relative_assignable_controller_message(14, 3, 0xC5, 1, controller_increment{ 1024 });
        EXPECT_EQ((universal_packet{ 0x4E534501, 0x00000400 }), m);
    }

    { // invalid controller bits
        constexpr auto m = make_relative_assignable_controller_message(11, 12, 9, 0xF3, controller_increment{ -99 });
        EXPECT_EQ((universal_packet{ 0x4B5C0973, 0xFFFFFF9D }), m);
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_midi2_program_change_message)
{
    using namespace midi;

    {
        constexpr auto m = make_midi2_program_change_message(14, 3, 99);
        EXPECT_EQ((universal_packet{ 0x4EC30000, 0x63000000 }), m);
    }

    { // invalid program bit
        const auto m = make_midi2_program_change_message(9, 11, 0xC5);
        EXPECT_EQ((universal_packet{ 0x49CB0000, 0x45000000 }), m);
    }

    {
        constexpr auto m = make_midi2_program_change_message(1, 8, 5, 0x1234);
        EXPECT_EQ((universal_packet{ 0x41C80001, 0x05002434 }), m);
    }

    { // invalid bank bits
        const auto m = make_midi2_program_change_message(9, 3, 127, 0xD21E);
        EXPECT_EQ((universal_packet{ 0x49C30001, 0x7F00241E }), m);
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_midi2_channel_pressure_message)
{
    using namespace midi;

    {
        constexpr auto m = make_midi2_channel_pressure_message(14, 3, controller_value{ 0x79273847u });
        EXPECT_EQ((universal_packet{ 0x4ED30000, 0x79273847 }), m);
    }

    {
        const auto m = make_midi2_channel_pressure_message(9, 11, controller_value{ 29384756u });
        EXPECT_EQ((universal_packet{ 0x49DB0000, 0x01C06034 }), m);
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_midi2_pitch_bend_message)
{
    using namespace midi;

    {
        constexpr auto m = make_midi2_pitch_bend_message(12, 9, pitch_bend{ uint32_t{ 0x80000000 } });
        EXPECT_EQ((universal_packet{ 0x4CE90000, 0x80000000 }), m);
    }

    {
        const auto m = make_midi2_pitch_bend_message(3, 7, pitch_bend{ uint32_t{ 0x44126535 } });
        EXPECT_EQ((universal_packet{ 0x43E70000, 0x44126535 }), m);
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_per_note_pitch_bend_message)
{
    using namespace midi;

    {
        constexpr auto pb = pitch_bend{ uint32_t{ 0x80000000 } };
        constexpr auto m  = make_per_note_pitch_bend_message(12, 9, 77, pb);
        EXPECT_EQ((universal_packet{ 0x4C694D00, 0x80000000 }), m);
        EXPECT_EQ(get_per_note_pitch_bend_value(m), pb);
    }

    {
        const auto pb = pitch_bend{ uint32_t{ 0x44126535 } };
        const auto m  = make_per_note_pitch_bend_message(3, 7, 0x94, pitch_bend{ uint32_t{ 0x44126535 } });
        EXPECT_EQ((universal_packet{ 0x43679400, 0x44126535 }), m);
        EXPECT_EQ(get_per_note_pitch_bend_value(m), pb);
    }
}

TEST_F(midi2_channel_voice_message, is_midi2_type_query_functions)
{
    using namespace midi;

    {
        constexpr auto m = make_midi1_note_on_message(1, 1, 60, velocity{ uint7_t{ 90 } });
        EXPECT_FALSE(is_midi2_channel_voice_message(m));
        EXPECT_FALSE(is_registered_controller_message(m));
        EXPECT_FALSE(is_assignable_controller_message(m));
        EXPECT_FALSE(is_registered_per_note_controller_message(m));
        EXPECT_FALSE(is_assignable_per_note_controller_message(m));
        EXPECT_FALSE(is_per_note_pitch_bend_message(m));
    }

    {
        constexpr auto m =
          make_midi2_channel_voice_message(12, channel_voice_status::registered_controller, 5, 4, 0x62, 0xABCDEF00);
        EXPECT_TRUE(is_midi2_channel_voice_message(m));
        EXPECT_TRUE(is_registered_controller_message(m));
        EXPECT_FALSE(is_assignable_controller_message(m));
        EXPECT_FALSE(is_registered_per_note_controller_message(m));
        EXPECT_FALSE(is_assignable_per_note_controller_message(m));
        EXPECT_FALSE(is_per_note_pitch_bend_message(m));
    }

    {
        constexpr auto m = make_assignable_controller_message(3, 7, 9, 0x45, controller_value{ 0x80101010u });
        EXPECT_TRUE(is_midi2_channel_voice_message(m));
        EXPECT_FALSE(is_registered_controller_message(m));
        EXPECT_TRUE(is_assignable_controller_message(m));
        EXPECT_FALSE(is_registered_per_note_controller_message(m));
        EXPECT_FALSE(is_assignable_per_note_controller_message(m));
        EXPECT_FALSE(is_per_note_pitch_bend_message(m));
    }

    {
        const auto m = make_registered_per_note_controller_message(1, 5, 0x33, 0x44, controller_value{ 0x55u });
        EXPECT_TRUE(is_midi2_channel_voice_message(m));
        EXPECT_FALSE(is_registered_controller_message(m));
        EXPECT_FALSE(is_assignable_controller_message(m));
        EXPECT_TRUE(is_registered_per_note_controller_message(m));
        EXPECT_EQ(get_per_note_controller_index(m), 0x44);
        EXPECT_FALSE(is_assignable_per_note_controller_message(m));
        EXPECT_FALSE(is_per_note_pitch_bend_message(m));
    }

    {
        const auto m =
          make_assignable_per_note_controller_message(0xE, 0xD, 0x12, 0x34, controller_value{ 0x44556677u });
        EXPECT_TRUE(is_midi2_channel_voice_message(m));
        EXPECT_FALSE(is_registered_controller_message(m));
        EXPECT_FALSE(is_assignable_controller_message(m));
        EXPECT_FALSE(is_registered_per_note_controller_message(m));
        EXPECT_TRUE(is_assignable_per_note_controller_message(m));
        EXPECT_EQ(get_per_note_controller_index(m), 0x34);
        EXPECT_FALSE(is_per_note_pitch_bend_message(m));
    }

    {
        const auto m =
          make_midi2_channel_voice_message(4, channel_voice_status::per_note_pitch_bend, 14, 0x43, 0x00, 0x8000D123);
        EXPECT_TRUE(is_midi2_channel_voice_message(m));
        EXPECT_FALSE(is_registered_controller_message(m));
        EXPECT_FALSE(is_assignable_controller_message(m));
        EXPECT_FALSE(is_registered_per_note_controller_message(m));
        EXPECT_FALSE(is_assignable_per_note_controller_message(m));
        EXPECT_TRUE(is_per_note_pitch_bend_message(m));
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, notes_with_attribute)
{
    using namespace midi;

    {
        const auto m = make_midi2_note_on_message(1, 1, 60, velocity{ 0.9 }, pitch_7_9{ 65.9 });
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::none));
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::manufacturer_specific));
        EXPECT_TRUE(is_note_on_with_attribute(m, note_attribute::pitch_7_9));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::none));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::manufacturer_specific));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::pitch_7_9));
        EXPECT_TRUE(is_note_on_with_pitch_7_9(m));

        EXPECT_EQ(get_midi2_note_attribute(m), note_attribute::pitch_7_9);
        EXPECT_EQ(get_midi2_note_attribute_data(m), pitch_7_9{ 65.9 }.value);
    }

    {
        constexpr auto m =
          make_midi2_note_on_message(5, 9, 88, velocity{ uint7_t{ 44 } }, note_attribute::manufacturer_specific, 1234);
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::none));
        EXPECT_TRUE(is_note_on_with_attribute(m, note_attribute::manufacturer_specific));
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::pitch_7_9));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::none));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::manufacturer_specific));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::pitch_7_9));
        EXPECT_FALSE(is_note_on_with_pitch_7_9(m));

        EXPECT_EQ(get_midi2_note_attribute(m), note_attribute::manufacturer_specific);
        EXPECT_EQ(get_midi2_note_attribute_data(m), 1234);
    }

    {
        const auto m = make_midi2_note_off_message(1, 1, 60, velocity{ 0.9 }, note_attribute::profile_specific, 0xABCD);
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::none));
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::manufacturer_specific));
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::pitch_7_9));
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::profile_specific));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::none));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::manufacturer_specific));
        EXPECT_TRUE(is_note_off_with_attribute(m, note_attribute::profile_specific));
        EXPECT_FALSE(is_note_on_with_pitch_7_9(m));

        EXPECT_EQ(get_midi2_note_attribute(m), note_attribute::profile_specific);
        EXPECT_EQ(get_midi2_note_attribute_data(m), 0xABCD);
    }

    {
        constexpr auto m = make_midi2_note_on_message(1, 1, 60, velocity{ uint7_t{ 90 } });
        EXPECT_TRUE(is_note_on_with_attribute(m, note_attribute::none));
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::manufacturer_specific));
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::pitch_7_9));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::none));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::manufacturer_specific));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::profile_specific));
        EXPECT_FALSE(is_note_on_with_pitch_7_9(m));
    }

    {
        constexpr auto m = make_midi1_note_on_message(1, 1, 60, velocity{ uint7_t{ 90 } });
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::none));
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::manufacturer_specific));
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::pitch_7_9));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::none));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::manufacturer_specific));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::profile_specific));
        EXPECT_FALSE(is_note_on_with_pitch_7_9(m));
    }

    {
        constexpr auto m =
          make_midi2_channel_voice_message(12, channel_voice_status::registered_controller, 5, 4, 0x62, 0xABCDEF00);
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::none));
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::manufacturer_specific));
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::pitch_7_9));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::none));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::manufacturer_specific));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::profile_specific));
        EXPECT_FALSE(is_note_on_with_pitch_7_9(m));
    }

    {
        constexpr auto m = make_assignable_controller_message(3, 7, 9, 0x45, controller_value{ 0x80101010u });
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::none));
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::manufacturer_specific));
        EXPECT_FALSE(is_note_on_with_attribute(m, note_attribute::pitch_7_9));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::none));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::manufacturer_specific));
        EXPECT_FALSE(is_note_off_with_attribute(m, note_attribute::profile_specific));
        EXPECT_FALSE(is_note_on_with_pitch_7_9(m));
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, pitch_bend_sensitivities)
{
    using namespace midi;

    {
        constexpr auto pbSens = pitch_bend_sensitivity{ note_nr_t{ 5 } };
        constexpr auto m      = make_registered_controller_message(
          0, 14, 0, registered_parameter_number::pitch_bend_sensitivity, controller_value{ pbSens.value });

        EXPECT_TRUE(is_pitch_bend_sensitivity_message(m));
        EXPECT_FALSE(is_per_note_pitch_bend_sensitivity_message(m));
        EXPECT_EQ(get_pitch_bend_sensitivity_value(m), pbSens);
    }

    {
        const auto pbSens = pitch_bend_sensitivity{ 3.25 };
        const auto m      = make_registered_controller_message(
          4, 7, 0, registered_parameter_number::pitch_bend_sensitivity, controller_value{ pbSens.value });

        EXPECT_TRUE(is_pitch_bend_sensitivity_message(m));
        EXPECT_FALSE(is_per_note_pitch_bend_sensitivity_message(m));
        EXPECT_EQ(get_pitch_bend_sensitivity_value(m), pbSens);
    }

    {
        constexpr auto pbSens = pitch_bend_sensitivity{ note_nr_t{ 8 } };
        constexpr auto m      = make_registered_controller_message(
          0, 14, 0, registered_parameter_number::per_note_pitch_bend_sensitivity, controller_value{ pbSens.value });

        EXPECT_FALSE(is_pitch_bend_sensitivity_message(m));
        EXPECT_TRUE(is_per_note_pitch_bend_sensitivity_message(m));
        EXPECT_EQ(get_per_note_pitch_bend_sensitivity_value(m), pbSens);
    }

    {
        const auto pbSens = pitch_bend_sensitivity{ 3.934 };
        const auto m      = make_registered_controller_message(
          4, 7, 0, registered_parameter_number::per_note_pitch_bend_sensitivity, controller_value{ pbSens.value });

        EXPECT_FALSE(is_pitch_bend_sensitivity_message(m));
        EXPECT_TRUE(is_per_note_pitch_bend_sensitivity_message(m));
        EXPECT_EQ(get_per_note_pitch_bend_sensitivity_value(m), pbSens);
    }

    {
        constexpr auto pbSens = pitch_bend_sensitivity{ note_nr_t{ 2 } };
        constexpr auto m      = make_registered_controller_message(
          0, 14, 1, registered_parameter_number::pitch_bend_sensitivity, controller_value{ pbSens.value });

        EXPECT_FALSE(is_pitch_bend_sensitivity_message(m));
        EXPECT_FALSE(is_per_note_pitch_bend_sensitivity_message(m));
    }

    {
        constexpr auto m = make_registered_controller_message(
          0, 14, 0, registered_parameter_number::coarse_tuning, controller_value{ uint7_t{ 4 } });

        EXPECT_FALSE(is_pitch_bend_sensitivity_message(m));
        EXPECT_FALSE(is_per_note_pitch_bend_sensitivity_message(m));
    }

    {
        constexpr auto m = make_midi2_channel_pressure_message(14, 3, controller_value{ 0x79273847u });

        EXPECT_FALSE(is_pitch_bend_sensitivity_message(m));
        EXPECT_FALSE(is_per_note_pitch_bend_sensitivity_message(m));
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, channel_tuning)
{
    using namespace midi;

    {
        constexpr auto m = make_registered_controller_message(
          0, 14, 0, registered_parameter_number::coarse_tuning, controller_value{ uint7_t{ 0 } });

        EXPECT_TRUE(is_channel_coarse_tuning_message(m));
        EXPECT_FALSE(is_channel_fine_tuning_message(m));
        EXPECT_EQ(get_channel_coarse_tuning_value(m), (pitch_increment{ -64.0 }));
    }

    {
        const auto m = make_registered_controller_message(
          4, 7, 0, registered_parameter_number::coarse_tuning, controller_value{ uint7_t{ 0x40 } });

        EXPECT_TRUE(is_channel_coarse_tuning_message(m));
        EXPECT_FALSE(is_channel_fine_tuning_message(m));
        EXPECT_EQ(get_channel_coarse_tuning_value(m), (pitch_increment{ 0.0 }));
    }

    {
        constexpr auto m = make_registered_controller_message(
          0, 14, 0, registered_parameter_number::coarse_tuning, controller_value{ uint7_t{ 0x7F } });

        EXPECT_TRUE(is_channel_coarse_tuning_message(m));
        EXPECT_FALSE(is_channel_fine_tuning_message(m));
        EXPECT_EQ(get_channel_coarse_tuning_value(m), (pitch_increment{ 63.0 }));
    }

    {
        // the lower 25 bits (everything but the 7-bit MSB) must be ignored
        constexpr auto m = make_registered_controller_message(
          0, 14, 0, registered_parameter_number::coarse_tuning, controller_value{ uint32_t{ 0x86000000u } });

        EXPECT_TRUE(is_channel_coarse_tuning_message(m));
        EXPECT_EQ(get_channel_coarse_tuning_value(m), (pitch_increment{ 3.0 }));

        constexpr auto m2 = make_registered_controller_message(
          0, 14, 0, registered_parameter_number::coarse_tuning, controller_value{ uint32_t{ 0x87FFFFFFu } });

        EXPECT_TRUE(is_channel_coarse_tuning_message(m2));
        EXPECT_EQ(get_channel_coarse_tuning_value(m2), (pitch_increment{ 3.0 }));
    }

    {
        constexpr auto m = make_registered_controller_message(
          0, 14, 0, registered_parameter_number::fine_tuning, controller_value{ uint14_t{ 0 } });

        EXPECT_FALSE(is_channel_coarse_tuning_message(m));
        EXPECT_TRUE(is_channel_fine_tuning_message(m));
        EXPECT_EQ(get_channel_fine_tuning_value(m), (pitch_increment{ -1.0 }));
    }

    {
        const auto m = make_registered_controller_message(
          4, 7, 0, registered_parameter_number::fine_tuning, controller_value{ uint14_t{ 0x2000 } });

        EXPECT_FALSE(is_channel_coarse_tuning_message(m));
        EXPECT_TRUE(is_channel_fine_tuning_message(m));
        EXPECT_EQ(get_channel_fine_tuning_value(m), (pitch_increment{ 0.0 }));
    }

    {
        constexpr auto m = make_registered_controller_message(
          0, 14, 0, registered_parameter_number::fine_tuning, controller_value{ uint14_t{ 0x3FFF } });

        EXPECT_FALSE(is_channel_coarse_tuning_message(m));
        EXPECT_TRUE(is_channel_fine_tuning_message(m));
        EXPECT_EQ(get_channel_fine_tuning_value(m), (pitch_increment{ 8191.0 / 8192.0 }));
    }

    {
        // the lower 18 bits (everything but the 14-bit RPN value) must be ignored
        constexpr auto m = make_registered_controller_message(
          0, 14, 0, registered_parameter_number::fine_tuning, controller_value{ uint32_t{ 0x80000000u } });

        EXPECT_TRUE(is_channel_fine_tuning_message(m));
        EXPECT_EQ(get_channel_fine_tuning_value(m), (pitch_increment{ 0.0 }));

        constexpr auto m2 = make_registered_controller_message(
          0, 14, 0, registered_parameter_number::fine_tuning, controller_value{ uint32_t{ 0x8003FFFFu } });

        EXPECT_TRUE(is_channel_fine_tuning_message(m2));
        EXPECT_EQ(get_channel_fine_tuning_value(m2), (pitch_increment{ 0.0 }));
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_channel_coarse_tuning_message)
{
    using namespace midi;

    {
        const auto m = make_channel_coarse_tuning_message(3, 7, pitch_increment{ 3.0 });

        EXPECT_EQ(0x43270002u, m.data[0]); // group 3, channel 7, bank 0, index = coarse_tuning (2)
        EXPECT_TRUE(is_channel_coarse_tuning_message(m));
        EXPECT_FALSE(is_channel_fine_tuning_message(m));
        EXPECT_EQ(get_channel_coarse_tuning_value(m), (pitch_increment{ 3.0 }));
    }

    {
        const auto m = make_channel_coarse_tuning_message(0, 14, pitch_increment{ -64.0 });

        EXPECT_TRUE(is_channel_coarse_tuning_message(m));
        EXPECT_EQ(get_channel_coarse_tuning_value(m), (pitch_increment{ -64.0 }));
    }

    {
        const auto m = make_channel_coarse_tuning_message(0, 14, pitch_increment{ 63.0 });

        EXPECT_TRUE(is_channel_coarse_tuning_message(m));
        EXPECT_EQ(get_channel_coarse_tuning_value(m), (pitch_increment{ 63.0 }));
    }

    { // out-of-range values clamp to the representable [-64, +63] semitone range
        const auto m = make_channel_coarse_tuning_message(0, 14, pitch_increment{ 100.0 });
        EXPECT_EQ(get_channel_coarse_tuning_value(m), (pitch_increment{ 63.0 }));

        const auto m2 = make_channel_coarse_tuning_message(0, 14, pitch_increment{ -100.0 });
        EXPECT_EQ(get_channel_coarse_tuning_value(m2), (pitch_increment{ -64.0 }));
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_channel_fine_tuning_message)
{
    using namespace midi;

    {
        const auto m = make_channel_fine_tuning_message(3, 7, pitch_increment{ 0.5 });

        EXPECT_EQ(0x43270001u, m.data[0]); // group 3, channel 7, bank 0, index = fine_tuning (1)
        EXPECT_FALSE(is_channel_coarse_tuning_message(m));
        EXPECT_TRUE(is_channel_fine_tuning_message(m));
        EXPECT_EQ(get_channel_fine_tuning_value(m), (pitch_increment{ 0.5 }));
    }

    {
        const auto m = make_channel_fine_tuning_message(0, 14, pitch_increment{ 0.0 });

        EXPECT_TRUE(is_channel_fine_tuning_message(m));
        EXPECT_EQ(get_channel_fine_tuning_value(m), (pitch_increment{ 0.0 }));
    }

    {
        const auto m = make_channel_fine_tuning_message(0, 14, pitch_increment{ -1.0 });

        EXPECT_TRUE(is_channel_fine_tuning_message(m));
        EXPECT_EQ(get_channel_fine_tuning_value(m), (pitch_increment{ -1.0 }));
    }

    { // out-of-range values clamp to the representable (-1, +1) semitone range
        const auto m = make_channel_fine_tuning_message(0, 14, pitch_increment{ 5.0 });
        EXPECT_EQ(get_channel_fine_tuning_value(m), (pitch_increment{ 8191.0 / 8192.0 }));

        const auto m2 = make_channel_fine_tuning_message(0, 14, pitch_increment{ -5.0 });
        EXPECT_EQ(get_channel_fine_tuning_value(m2), (pitch_increment{ -1.0 }));
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, make_channel_tuning_messages)
{
    using namespace midi;

    {
        // splitting a fractional semitone value yields a coarse message carrying the
        // nearest whole semitone and a fine message carrying the remainder; applying
        // both reconstructs the requested tuning (within the fine RPN's quantization
        // resolution of 100/8192 cents)
        const auto messages = make_channel_tuning_messages(3, 7, pitch_increment{ 5.3 });

        EXPECT_TRUE(is_channel_coarse_tuning_message(messages.first));
        EXPECT_TRUE(is_channel_fine_tuning_message(messages.second));

        const auto reconstructed = get_channel_coarse_tuning_value(messages.first).value / double(1 << 25) +
                                   get_channel_fine_tuning_value(messages.second).value / double(1 << 25);
        EXPECT_NEAR(5.3, reconstructed, 1.0 / 8192.0);
    }

    {
        const auto messages = make_channel_tuning_messages(0, 14, pitch_increment{ -12.75 });

        EXPECT_EQ(get_channel_coarse_tuning_value(messages.first), (pitch_increment{ -13.0 }));
        EXPECT_EQ(get_channel_fine_tuning_value(messages.second), (pitch_increment{ 0.25 }));
    }

    { // exactly on a semitone boundary: coarse absorbs it all, fine is zero
        const auto messages = make_channel_tuning_messages(0, 14, pitch_increment{ -6.0 });

        EXPECT_EQ(get_channel_coarse_tuning_value(messages.first), (pitch_increment{ -6.0 }));
        EXPECT_EQ(get_channel_fine_tuning_value(messages.second), (pitch_increment{ 0.0 }));
    }

    { // the split always rounds down (floor), not to the nearest semitone: 6.7 splits as 6 + 0.7, not 7 - 0.3
        const auto messages = make_channel_tuning_messages(0, 14, pitch_increment{ 6.7 });

        EXPECT_EQ(get_channel_coarse_tuning_value(messages.first), (pitch_increment{ 6.0 }));

        const auto reconstructed = get_channel_coarse_tuning_value(messages.first).value / double(1 << 25) +
                                   get_channel_fine_tuning_value(messages.second).value / double(1 << 25);
        EXPECT_NEAR(6.7, reconstructed, 1.0 / 8192.0);
    }

    { // the split is exact, pure integer math: applying both messages losslessly reconstructs any
      // value already aligned to the fine RPN's 1/8192 semitone resolution
        const auto messages = make_channel_tuning_messages(0, 14, pitch_increment{ int32_t{ -13 * (1 << 25) / 4 } });

        EXPECT_EQ(get_channel_coarse_tuning_value(messages.first), (pitch_increment{ -4.0 }));
        EXPECT_EQ(get_channel_fine_tuning_value(messages.second), (pitch_increment{ 0.75 }));
    }

    { // being pure integer math (no std::round), the round trip is usable in a constexpr context as
        // long as the pitch_increment isn't itself constructed from a float/double
        constexpr auto messages = make_channel_tuning_messages(0, 14, pitch_increment{ int32_t{ 3 * (1 << 25) } });

        static_assert(get_channel_coarse_tuning_value(messages.first) == pitch_increment{ int32_t{ 3 * (1 << 25) } });
        static_assert(get_channel_fine_tuning_value(messages.second) == pitch_increment{ int32_t{ 0 } });
    }
}

//-----------------------------------------------

TEST_F(midi2_channel_voice_message, deprecations)
{
    using namespace midi;

#if defined(_MSC_VER)
    __pragma( warning(push) )
    __pragma( warning(disable: 4996) )
#elif defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

    universal_packet p;
    EXPECT_EQ(is_midi2_registered_controller_message(p), is_registered_controller_message(p));
    EXPECT_EQ(is_midi2_assignable_controller_message(p), is_assignable_controller_message(p));
    EXPECT_EQ(is_midi2_registered_per_note_controller_message(p), is_registered_per_note_controller_message(p));
    EXPECT_EQ(is_midi2_assignable_per_note_controller_message(p), is_assignable_per_note_controller_message(p));
    EXPECT_EQ(is_midi2_per_note_pitch_bend_message(p), is_per_note_pitch_bend_message(p));
    if (is_registered_per_note_controller_message(p) || is_assignable_per_note_controller_message(p))
        EXPECT_EQ(get_midi2_per_note_controller_index(p), get_per_note_controller_index(p));

#if defined(_MSC_VER)
    __pragma( warning(pop) )
#elif defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic pop
#endif
}

//-----------------------------------------------
