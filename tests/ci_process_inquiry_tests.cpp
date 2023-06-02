#include <gtest/gtest.h>

#include <midi/capability_inquiry.h>

//-----------------------------------------------

class ci_process_inquiry : public ::testing::Test
{ };

//-----------------------------------------------

TEST_F(ci_process_inquiry, process_inquiry_capabilities_inquiry)
{
    using VUT = midi::capability_inquiry_view;

    {
        auto sx = midi::ci::make_process_inquiry_capabilities_inquiry(0x7665544, 0x24D2B78, 0x08);
        EXPECT_EQ(12u, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x08, m.device_id());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7665544u, m.source_muid());
        EXPECT_EQ(0x24D2B78u, m.destination_muid());
    }
}

//-----------------------------------------------

TEST_F(ci_process_inquiry, process_inquiry_capabilities_reply)
{
    using VUT = midi::ci::process_inquiry_capabilities_reply_view;

    {
        auto sx = midi::ci::make_process_inquiry_capabilities_reply(0x7665544, 0x24D2B78, 19, 0x08);
        EXPECT_EQ(13u, sx.data.size());

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7665544u, m.source_muid());
        EXPECT_EQ(0x24D2B78u, m.destination_muid());

        EXPECT_EQ(0x08, m.device_id());
        EXPECT_EQ(19u, m.supported_features());
    }
}

//-----------------------------------------------

TEST_F(ci_process_inquiry, midi_message_report_inquiry)
{
    using VUT = midi::ci::midi_message_report_inquiry_view;

    {
        auto sx = midi::ci::make_midi_message_report_inquiry(0x7665544, 0x24D2B78, 0x12, 0x34, 0x56, 0x78, 0x04);
        EXPECT_EQ(17u, sx.data.size());

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x12, m.message_data_control());
        EXPECT_EQ(0x34, m.system_message_types());
        EXPECT_EQ(0x56, m.channel_controller_message_types());
        EXPECT_EQ(0x78, m.note_data_message_types());

        EXPECT_EQ(0x04, m.device_id());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7665544u, m.source_muid());
        EXPECT_EQ(0x24D2B78u, m.destination_muid());
    }
}

//-----------------------------------------------

TEST_F(ci_process_inquiry, midi_message_report_reply)
{
    using VUT = midi::ci::midi_message_report_reply_view;

    {
        auto sx = midi::ci::make_midi_message_report_reply(0x7665544, 0x76, 0x54, 0x32, 0x0A);
        EXPECT_EQ(16u, sx.data.size());

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x76, m.system_message_types());
        EXPECT_EQ(0x54, m.channel_controller_message_types());
        EXPECT_EQ(0x32, m.note_data_message_types());

        EXPECT_EQ(0x0A, m.device_id());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7665544u, m.source_muid());
        EXPECT_EQ(0xFFFFFFFu, m.destination_muid());
    }
}

//-----------------------------------------------

TEST_F(ci_process_inquiry, midi_message_report_end)
{
    using VUT = midi::capability_inquiry_view;

    {
        auto sx = midi::ci::make_midi_message_report_end(0x7665544, 0x03);
        EXPECT_EQ(12u, sx.data.size());

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x03, m.device_id());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7665544u, m.source_muid());
        EXPECT_EQ(0xFFFFFFFu, m.destination_muid());
    }
}

//-----------------------------------------------
