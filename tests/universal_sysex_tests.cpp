#include <gtest/gtest.h>

#include <midi/universal_sysex.h>

#include "sysex_tests.h"

//-----------------------------------------------

class universal_sysex : public ::testing::Test
{ };

//-----------------------------------------------

TEST_F(universal_sysex, sysex7)
{
    using namespace midi;
    using namespace midi::universal_sysex;
    using message = midi::universal_sysex::message;

    {
        sysex7 sx{ manufacturer::universal_non_realtime };

        EXPECT_FALSE(midi::is_universal_sysex_message(sx));

        EXPECT_EQ(0xFF, universal_sysex_device_id_of(sx));
        EXPECT_EQ(0, universal_sysex_type_of(sx));
        EXPECT_EQ(0, universal_sysex_subtype_of(sx));
    }

    {
        sysex7 sx{ manufacturer::universal_realtime, midi::sysex7::data_type({ 0xAA }) };

        EXPECT_FALSE(midi::is_universal_sysex_message(sx));

        EXPECT_EQ(0xFF, universal_sysex_device_id_of(sx));
        EXPECT_EQ(0, universal_sysex_type_of(sx));
        EXPECT_EQ(0, universal_sysex_subtype_of(sx));
    }

    {
        sysex7 sx{ manufacturer::moog, { 0x12, 0x34, 0x56, 0x78 } };

        EXPECT_FALSE(midi::is_universal_sysex_message(sx));
    }

    {
        sysex7 sx{ manufacturer::universal_non_realtime, { 0x7F, 0x01 } };

        EXPECT_TRUE(midi::is_universal_sysex_message(sx));

        EXPECT_EQ(0x7F, universal_sysex_device_id_of(sx));
        EXPECT_EQ(type::sample_dump_header, universal_sysex_type_of(sx));
        EXPECT_EQ(0, universal_sysex_subtype_of(sx));

        const auto usx = midi::universal_sysex_view{ sx };

        EXPECT_EQ(0x7F, usx.device_id());
        EXPECT_EQ(type::sample_dump_header, usx.type());
        EXPECT_EQ(0, usx.subtype());
    }

    {
        sysex7 sx{ manufacturer::universal_realtime, { 0x04, 0x01, 0x01, 0x1A, 0x2B, 0x3C, 0x4D } };

        EXPECT_TRUE(midi::is_universal_sysex_message(sx));

        EXPECT_EQ(4, universal_sysex_device_id_of(sx));
        EXPECT_EQ(type::midi_time_code_real_time, universal_sysex_type_of(sx));
        EXPECT_EQ(subtype::mtc_full_message, universal_sysex_subtype_of(sx));

        const auto usx = midi::universal_sysex_view{ sx };

        EXPECT_EQ(4, usx.device_id());
        EXPECT_EQ(type::midi_time_code_real_time, usx.type());
        EXPECT_EQ(subtype::mtc_full_message, usx.subtype());

        EXPECT_EQ(7u, usx.data_size());
    }
}

//-----------------------------------------------

TEST_F(universal_sysex, set_device_id)
{
    using namespace midi;
    using namespace midi::universal_sysex;

    {
        message usx{ manufacturer::universal_realtime, { 0x04, 0x01, 0x01, 0x1A, 0x2B, 0x3C, 0x4D } };

        EXPECT_TRUE(midi::is_universal_sysex_message(usx));

        EXPECT_EQ(4, usx.device_id());

        usx.set_device_id(9);
        EXPECT_EQ(9, usx.device_id());
    }
}

//-----------------------------------------------

TEST_F(universal_sysex, universal_sysex_view)
{
    using namespace midi;
    using namespace midi::universal_sysex;

    {
        sysex7 sx{ manufacturer::universal_non_realtime };

        EXPECT_FALSE(as_universal_sysex_view(sx));
    }

    {
        sysex7 sx{ manufacturer::universal_realtime, sysex7::data_type({ 0xAA }) };

        EXPECT_FALSE(as_universal_sysex_view(sx));
    }

    {
        sysex7 sx{ manufacturer::moog, { 0x12, 0x34, 0x56, 0x78 } };

        EXPECT_FALSE(as_universal_sysex_view(sx));
    }

    {
        sysex7 sx{ manufacturer::universal_non_realtime, { 0x7F, 0x01 } };

        const auto usx = as_universal_sysex_view(sx);
        EXPECT_TRUE(usx);

        EXPECT_EQ(0x7F, usx->device_id());
        EXPECT_EQ(type::sample_dump_header, usx->type());
        EXPECT_EQ(0, usx->subtype());
    }

    {
        sysex7 sx{ manufacturer::universal_realtime, { 0x04, 0x01, 0x01, 0x1A, 0x2B, 0x3C, 0x4D } };

        const auto usx = as_universal_sysex_view(sx);
        EXPECT_TRUE(usx);

        EXPECT_EQ(4, usx->device_id());
        EXPECT_EQ(type::midi_time_code_real_time, usx->type());
        EXPECT_EQ(subtype::mtc_full_message, usx->subtype());

        EXPECT_EQ(7u, usx->data_size());
    }
}

//-----------------------------------------------

TEST_F(universal_sysex, identity_request)
{
    using namespace midi;
    using namespace midi::universal_sysex;
    using message = midi::universal_sysex::message;

    {
        sysex7 sx{ manufacturer::universal_non_realtime, { 0x7F, 0x06, 0x01 } };

        const auto usx = midi::universal_sysex_view{ sx };

        EXPECT_EQ(0x7F, usx.device_id());
        EXPECT_EQ(type::general_information, usx.type());
        EXPECT_EQ(subtype::identity_request, usx.subtype());

        EXPECT_TRUE(is_identity_request(sx));
    }

    {
        sysex7 sx{ manufacturer::universal_non_realtime, { 0x7F, 0x06, 0x02 } };

        EXPECT_FALSE(is_identity_request(sx));
    }

    {
        sysex7 sx{ manufacturer::universal_non_realtime, { 0x7F, 0x02, 0x01 } };

        EXPECT_FALSE(is_identity_request(sx));
    }

    {
        sysex7 sx{ manufacturer::native_instruments, { 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0xF7 } };

        EXPECT_FALSE(is_identity_request(sx));
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        identity_request idr;

        EXPECT_TRUE(midi::is_universal_sysex_message(idr));

        EXPECT_EQ(0x7F, idr.device_id());
        EXPECT_EQ(type::general_information, idr.type());
        EXPECT_EQ(subtype::identity_request, idr.subtype());

        EXPECT_TRUE(is_identity_request(idr));

        EXPECT_EQ(idr, make_identity_request());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 2);
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        identity_request idr(5);

        EXPECT_TRUE(midi::is_universal_sysex_message(idr));

        EXPECT_EQ(5, idr.device_id());
        EXPECT_EQ(type::general_information, idr.type());
        EXPECT_EQ(subtype::identity_request, idr.subtype());

        EXPECT_TRUE(is_identity_request(idr));

        EXPECT_EQ(idr, make_identity_request(5));

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 2);
    }
}

//-----------------------------------------------

TEST_F(universal_sysex, identity_reply)
{
    using namespace midi;
    using namespace midi::universal_sysex;
    using message = midi::universal_sysex::message;

    {
        sysex7 sx{ midi::manufacturer::universal_non_realtime,
                   { 0x00, 0x06, 0x02, 0x41, 0x73, 0x52, 0x34, 0x12, 0x00, 0x07, 0x7C, 0x44 } };

        const auto usx = midi::universal_sysex_view{ sx };

        EXPECT_EQ(0x00, usx.device_id());
        EXPECT_EQ(type::general_information, usx.type());
        EXPECT_EQ(subtype::identity_reply, usx.subtype());

        EXPECT_TRUE(is_identity_reply(sx));
        EXPECT_TRUE(as_identity_reply_view(sx));
        EXPECT_TRUE(usx.as<identity_reply_view>());

        auto idr = identity_reply_view{ sx };
        EXPECT_EQ(0x00u, idr.device_id());

        const auto i = idr.identity();
        EXPECT_EQ(manufacturer::roland, i.manufacturer);
        EXPECT_EQ(0x2973u, i.family);
        EXPECT_EQ(0x0934u, i.model);
        EXPECT_EQ(0x89F0380u, i.revision);
    }

    {
        sysex7 sx{ midi::manufacturer::universal_non_realtime,
                   { 0x54, 0x06, 0x02, 0x00, 0x21, 0x09, 0x30, 0x2E, 0x31, 0x00, 0x05, 0x04, 0x04, 0x00 } };

        const auto usx = midi::universal_sysex_view{ sx };

        EXPECT_EQ(0x54u, usx.device_id());
        EXPECT_EQ(type::general_information, usx.type());
        EXPECT_EQ(subtype::identity_reply, usx.subtype());

        EXPECT_TRUE(is_identity_reply(sx));
        EXPECT_TRUE(as_identity_reply_view(sx));
        EXPECT_TRUE(usx.as<identity_reply_view>());

        auto idr = identity_reply_view{ sx };
        EXPECT_EQ(0x54u, idr.device_id());

        const auto i = idr.identity();
        EXPECT_EQ(manufacturer::native_instruments, i.manufacturer);
        EXPECT_EQ(0x1730u, i.family);
        EXPECT_EQ(49u, i.model);
        EXPECT_EQ(0x00010205u, i.revision);
    }

    {
        sysex7 sx{ midi::manufacturer::universal_non_realtime, { 0x7F, 0x06, 0x01 } };

        EXPECT_FALSE(is_identity_reply(sx));
        EXPECT_FALSE(as_identity_reply_view(sx));
    }

    {
        sysex7 sx{ manufacturer::native_instruments, { 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0xF7 } };

        EXPECT_FALSE(is_identity_reply(sx));
        EXPECT_FALSE(as_identity_reply_view(sx));
    }

    {
        sysex7 sx{ midi::manufacturer::universal_non_realtime,
                   { 0x54, 0x06, 0x02, 0x00, 0x21, 0x09, 0x30, 0x17, 0x31, 0x00, 0x05, 0x02, 0x1 } };

        EXPECT_FALSE(is_identity_reply(sx));
        EXPECT_FALSE(as_identity_reply_view(sx));
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        identity_reply idr{ manufacturer::native_instruments, 0x1800, 32, 0x00000100 };

        EXPECT_TRUE(midi::is_universal_sysex_message(idr));

        EXPECT_EQ(0x7Fu, idr.device_id());
        EXPECT_EQ(type::general_information, idr.type());
        EXPECT_EQ(subtype::identity_reply, idr.subtype());

        EXPECT_TRUE(is_identity_reply(idr));
        EXPECT_TRUE(as_identity_reply_view(idr));
        EXPECT_TRUE(message_view{ idr }.as<identity_reply_view>());

        const auto i = identity_reply_view{ idr }.identity();
        EXPECT_EQ(manufacturer::native_instruments, i.manufacturer);
        EXPECT_EQ(0x1800u, i.family);
        EXPECT_EQ(32u, i.model);
        EXPECT_EQ(0x00000100u, i.revision);

        EXPECT_EQ(idr, make_identity_reply(manufacturer::native_instruments, 0x1800, 32, 0x00000100));

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 2);
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        identity_reply idr{ manufacturer::waldorf, 0x1234, 0x2A6F, 0x01234567, 0x80 };

        EXPECT_TRUE(midi::is_universal_sysex_message(idr));

        EXPECT_EQ(0x80u, idr.device_id());
        EXPECT_EQ(type::general_information, idr.type());
        EXPECT_EQ(subtype::identity_reply, idr.subtype());

        EXPECT_TRUE(is_identity_reply(idr));
        EXPECT_TRUE(as_identity_reply_view(idr));
        EXPECT_TRUE(message_view{ idr }.as<identity_reply_view>());

        const auto i = identity_reply_view{ idr }.identity();
        EXPECT_EQ(manufacturer::waldorf, i.manufacturer);
        EXPECT_EQ(0x1234u, i.family);
        EXPECT_EQ(0x2A6Fu, i.model);
        EXPECT_EQ(0x1234567u, i.revision);

        EXPECT_EQ(idr, make_identity_reply(manufacturer::waldorf, 0x1234, 0x2A6F, 0x01234567, 0x80));

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 2);
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        identity_reply idr{ device_identity{ manufacturer::waldorf, 0x22A6, 0x3C3F, 0x2345678 } };

        EXPECT_TRUE(midi::is_universal_sysex_message(idr));

        EXPECT_EQ(0x7Fu, idr.device_id());
        EXPECT_EQ(type::general_information, idr.type());
        EXPECT_EQ(subtype::identity_reply, idr.subtype());

        EXPECT_TRUE(is_identity_reply(idr));
        EXPECT_TRUE(as_identity_reply_view(idr));
        EXPECT_TRUE(message_view{ idr }.as<identity_reply_view>());

        const auto i = identity_reply_view{ idr }.identity();
        EXPECT_EQ(manufacturer::waldorf, i.manufacturer);
        EXPECT_EQ(0x22A6u, i.family);
        EXPECT_EQ(0x3C3Fu, i.model);
        EXPECT_EQ(0x2345678u, i.revision);

        EXPECT_EQ(idr, make_identity_reply(device_identity{ manufacturer::waldorf, 0x22A6, 0x3C3F, 0x2345678 }));

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 2);
    }
}
