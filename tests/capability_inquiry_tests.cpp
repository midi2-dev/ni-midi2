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

#include <midi/capability_inquiry.h>

#include "sysex_tests.h"

//-----------------------------------------------

#include <cstring>

//-----------------------------------------------

class capability_inquiry : public ::testing::Test
{ };

//-----------------------------------------------

TEST_F(capability_inquiry, is_capability_inquiry_message)
{
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x01, 0x00, 0x78, 0x56, 0x34, 0x12, 0x7F, 0x7F, 0x7F, 0x7F } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));

        EXPECT_EQ(0x7F, midi::universal_sysex_device_id_of(sx));
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, midi::universal_sysex_type_of(sx));
        EXPECT_EQ(0x1, midi::universal_sysex_subtype_of(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x01, 0x00, 0x78, 0x56, 0x34, 0x12, 0x7F, 0x7F, 0x7F } };

        EXPECT_FALSE(midi::is_capability_inquiry_message(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x03, 0x01, 0x00, 0x78, 0x56, 0x34, 0x12 } };

        EXPECT_FALSE(midi::is_capability_inquiry_message(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime, { 0x7F, 0x0D, 0x01, 0x78, 0x56, 0x34, 0x12 } };

        EXPECT_FALSE(midi::is_capability_inquiry_message(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::native_instruments, { 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0xF7 } };

        EXPECT_FALSE(midi::is_capability_inquiry_message(sx));
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, capability_inquiry_view)
{
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x01, 0x00, 0x78, 0x56, 0x34, 0x12, 0x7F, 0x7F, 0x7F, 0x7F } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));

        EXPECT_EQ(0x7F, midi::universal_sysex_device_id_of(sx));
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, midi::universal_sysex_type_of(sx));
        EXPECT_EQ(0x1, midi::universal_sysex_subtype_of(sx));

        auto m = midi::as_capability_inquiry_view(sx);
        EXPECT_TRUE(m);

        EXPECT_EQ(0, m->message_version());
        EXPECT_EQ(0x24D2B78u, m->source_muid());
        EXPECT_EQ(0xFFFFFFFu, m->destination_muid());
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x01, 0x00, 0x78, 0x56, 0x34, 0x12, 0x7F, 0x7F, 0x7F } };

        EXPECT_FALSE(midi::as_capability_inquiry_view(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x03, 0x01, 0x00, 0x78, 0x56, 0x34, 0x12 } };

        EXPECT_FALSE(midi::as_capability_inquiry_view(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime, { 0x7F, 0x0D, 0x01, 0x78, 0x56, 0x34, 0x12 } };

        EXPECT_FALSE(midi::as_capability_inquiry_view(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::native_instruments, { 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0xF7 } };

        EXPECT_FALSE(midi::as_capability_inquiry_view(sx));
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, message_constructor)
{
    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        midi::ci::message ci{ 0x1A, 0x7665544, 0xFFFFFFF, 0x08 };

        EXPECT_TRUE(midi::is_capability_inquiry_message(ci));

        EXPECT_EQ(0x08, ci.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, ci.type());
        EXPECT_EQ(0x1A, ci.subtype());

        EXPECT_EQ(midi::ci::version, ci.message_version());
        EXPECT_EQ(0x7665544u, ci.source_muid());
        EXPECT_EQ(0xFFFFFFFu, ci.destination_muid());

        EXPECT_EQ(12u, ci.data.size());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto ci = midi::ci::message::make_with_payload_size(244, 0x04, 19922, 788, 6);

        EXPECT_EQ(256u, ci.data.capacity());

        EXPECT_EQ(0x06, ci.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, ci.type());
        EXPECT_EQ(0x04, ci.subtype());

        EXPECT_EQ(midi::ci::version, ci.message_version());
        EXPECT_EQ(19922u, ci.source_muid());
        EXPECT_EQ(788u, ci.destination_muid());

        EXPECT_EQ(12u, ci.data.size());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto ci = midi::ci::message::make_with_payload_size(4, 0x04, 19922, 788, 6);

        EXPECT_EQ(16u, ci.data.capacity());

        EXPECT_EQ(0x06, ci.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, ci.type());
        EXPECT_EQ(0x04, ci.subtype());

        EXPECT_EQ(midi::ci::version, ci.message_version());
        EXPECT_EQ(19922u, ci.source_muid());
        EXPECT_EQ(788u, ci.destination_muid());

        EXPECT_EQ(12u, ci.data.size());

        const midi::uint7_t payload[] = { 1, 2, 3, 4 };
        ci.add_data(payload, sizeof(payload));

        EXPECT_EQ(16u, ci.data.capacity());
        EXPECT_EQ(16u, ci.data.size());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, message_version)
{
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x01, 0x00, 0x78, 0x56, 0x34, 0x12, 0x7F, 0x7F, 0x7F, 0x7F } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));

        auto ci = midi::capability_inquiry_view{ sx };

        EXPECT_EQ(0, ci.message_version());
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x01, 0x05, 0x78, 0x56, 0x34, 0x12, 0x22, 0x33, 0x44, 0x55 } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));

        auto ci = midi::capability_inquiry_view{ sx };

        EXPECT_EQ(0x05, ci.message_version());
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x01, 0x77, 0x78, 0x56, 0x34, 0x12, 1, 2, 3, 4 } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));

        auto ci = midi::capability_inquiry_view{ sx };

        EXPECT_EQ(0x77, ci.message_version());
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, message_muids)
{
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x01, 0x00, 0x78, 0x56, 0x34, 0x12, 0x44, 0x33, 0x22, 0x11 } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));

        auto ci = midi::capability_inquiry_view{ sx };

        EXPECT_EQ(0x24D2B78u, ci.source_muid());
        EXPECT_EQ(0x22899C4u, ci.destination_muid());
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x01, 0x00, 0x44, 0x33, 0x22, 0x11, 0x45, 0x56, 0x47, 0x38 } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));

        auto ci = midi::capability_inquiry_view{ sx };

        EXPECT_EQ(0x22899C4u, ci.source_muid());
        EXPECT_EQ(0x711EB45u, ci.destination_muid());
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x01, 0x00, 0x45, 0x56, 0x47, 0x38, 0x44, 0x33, 0x22, 0x11 } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));

        auto ci = midi::capability_inquiry_view{ sx };

        EXPECT_EQ(0x711EB45u, ci.source_muid());
        EXPECT_EQ(0x22899C4u, ci.destination_muid());
    }

    {
        midi::ci::message m{ 0x33, 0x1234567, 0xFFFFFFF };

        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(0xFFFFFFFu, m.destination_muid());

        m.set_source_muid(0xEDCBA98);
        EXPECT_EQ(0xEDCBA98u, m.source_muid());

        m.set_destination_muid(0x8776655);
        EXPECT_EQ(0x8776655u, m.destination_muid());
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, discovery_message_view)
{
    {
        using VUT = midi::ci::discovery_inquiry_view;

        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x70, 0x01, 0x44, 0x33, 0x22, 0x11, 0x77, 0x55, 0x33, 0x11, 0x00, 0x21,
                           0x09, 0x00, 0x30, 25,   0,    0,    0,    4,    0,    0x20, 0x00, 0x08, 0x00, 0x00 } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::discovery_inquiry, m.subtype());

        EXPECT_EQ(1, m.message_version());
        EXPECT_EQ(0x22899C4u, m.source_muid());
        EXPECT_EQ(0x22CEAF7u, m.destination_muid());

        const auto identity = m.identity();
        EXPECT_EQ(midi::manufacturer::native_instruments, identity.manufacturer);
        EXPECT_EQ(0x1800u, identity.family);
        EXPECT_EQ(25u, identity.model);
        EXPECT_EQ(0x00010000u, identity.revision);

        EXPECT_EQ(0x20, m.categories());
        EXPECT_EQ(0x400u, m.maximum_message_size());
        EXPECT_EQ(0x00u, m.output_path_id());
    }

    {
        using VUT = midi::ci::discovery_reply_view;

        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x71, 0x02, 0x44, 0x33, 0x22, 0x11, 0x77, 0x55, 0x33, 0x11, 0x00, 0x21, 0x09,
                           0x00, 0x30, 25,   0,    0,    0,    1,    0,    0x20, 0x00, 0x08, 0x00, 0x00, 0x14, 12 } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::discovery_reply, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x22899C4u, m.source_muid());
        EXPECT_EQ(0x22CEAF7u, m.destination_muid());

        const auto identity = m.identity();
        EXPECT_EQ(midi::manufacturer::native_instruments, identity.manufacturer);
        EXPECT_EQ(0x1800u, identity.family);
        EXPECT_EQ(25u, identity.model);
        EXPECT_EQ(0x00004000u, identity.revision);

        EXPECT_EQ(0x20, m.categories());
        EXPECT_EQ(0x400u, m.maximum_message_size());
        EXPECT_EQ(0x14u, m.output_path_id());
        EXPECT_EQ(12u, m.function_block());
    }

    // invalid id
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0C, 0x70, 0x01, 0x44, 0x33, 0x22, 0x11, 0x77, 0x55, 0x33, 0x11, 0x00, 0x21,
                           0x09, 0x00, 0x18, 25,   0,    0,    0,    1,    0,    0x20, 0x00, 0x08, 0x00, 0x00 } };

        EXPECT_FALSE(midi::ci::discovery_inquiry_view::validate(sx));
    }

    // invalid subtype
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x74, 0x01, 0x44, 0x33, 0x22, 0x11, 0x77, 0x55, 0x33, 0x11, 0x00, 0x21,
                           0x09, 0x00, 0x18, 25,   0,    0,    0,    1,    0,    0x20, 0x00, 0x08, 0x00, 0x00 } };

        EXPECT_FALSE(midi::ci::discovery_reply_view::validate(sx));
    }

    // invalid size
    {
        midi::sysex7 sx_v1{ midi::manufacturer::universal_non_realtime,
                            { 0x7F, 0x0D, 0x70, 0x01, 0x44, 0x33, 0x22, 0x11, 0x77, 0x55, 0x33, 0x11, 0x00, 0x21,
                              0x09, 0x00, 0x18, 25,   0,    0,    0,    1,    0,    0x20, 0x00, 0x08, 0x00 } };

        EXPECT_FALSE(midi::ci::discovery_inquiry_view::validate(sx_v1));

        midi::sysex7 sx_v2{ midi::manufacturer::universal_non_realtime,
                            { 0x7F, 0x0D, 0x71, 0x02, 0x44, 0x33, 0x22, 0x11, 0x77, 0x55, 0x33, 0x11, 0x00, 0x21,
                              0x09, 0x00, 0x18, 25,   0,    0,    0,    1,    0,    0x20, 0x00, 0x08, 0x00, 0x00 } };

        EXPECT_FALSE(midi::ci::discovery_reply_view::validate(sx_v2));
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, make_discovery_message_v1)
{
    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto identity = midi::device_identity{ midi::manufacturer::native_instruments, 0x1730, 49, 0x00010005 };

        auto sx = midi::ci::discovery::make_discovery_message_v1(
          midi::ci::subtype::discovery_reply, 0x2435465, 0x12345, identity, 0x0C, 766);
        EXPECT_EQ(28u, sx.data.size());

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(midi::ci::discovery_reply_view::validate(sx));

        auto v = midi::ci::discovery_reply_view{ sx };

        EXPECT_EQ(0x7F, v.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, v.type());
        EXPECT_EQ(midi::ci::subtype::discovery_reply, v.subtype());

        EXPECT_EQ(midi::ci::message_version_1, v.message_version());
        EXPECT_EQ(0x2435465u, v.source_muid());
        EXPECT_EQ(0x12345u, v.destination_muid());

        const auto i = v.identity();

        EXPECT_EQ(0, memcmp(&identity, &i, sizeof(midi::device_identity)));

        EXPECT_EQ(0x0C, v.categories());
        EXPECT_EQ(766u, v.maximum_message_size());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, discovery_inquiry_view)
{
    using VUT = midi::ci::discovery_inquiry_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x70, 0x01, 0x44, 0x33, 0x22, 0x11, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0x21,
                           0x09, 0x00, 0x30, 25,   0,    0,    0,    4,    0,    0x20, 0x00, 0x08, 0x00, 0x00 } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::discovery_inquiry, m.subtype());

        EXPECT_EQ(1, m.message_version());
        EXPECT_EQ(0x22899C4u, m.source_muid());
        EXPECT_EQ(0xFFFFFFFu, m.destination_muid());

        const auto identity = m.identity();
        EXPECT_EQ(midi::manufacturer::native_instruments, identity.manufacturer);
        EXPECT_EQ(0x1800u, identity.family);
        EXPECT_EQ(25u, identity.model);
        EXPECT_EQ(0x00010000u, identity.revision);

        EXPECT_EQ(0x20, m.categories());
        EXPECT_EQ(0x400u, m.maximum_message_size());
        EXPECT_EQ(0x00u, m.output_path_id());
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x70, 0x02, 0x44, 0x33, 0x22, 0x11, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0x21, 0x09,
                           0x00, 0x30, 25,   0,    0,    0,    1,    0,    0x20, 0x00, 0x08, 0x00, 0x00, 0x11 } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(midi::ci::discovery_inquiry_view::validate(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::discovery_inquiry, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x22899C4u, m.source_muid());
        EXPECT_EQ(0xFFFFFFFu, m.destination_muid());

        const auto identity = m.identity();
        EXPECT_EQ(midi::manufacturer::native_instruments, identity.manufacturer);
        EXPECT_EQ(0x1800u, identity.family);
        EXPECT_EQ(25u, identity.model);
        EXPECT_EQ(0x00004000u, identity.revision);

        EXPECT_EQ(0x20, m.categories());
        EXPECT_EQ(0x400u, m.maximum_message_size());
        EXPECT_EQ(0x11u, m.output_path_id());
    }

    // invalid subtype
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x71, 0x01, 0x44, 0x33, 0x22, 0x11, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0x21,
                           0x09, 0x00, 0x18, 25,   0,    0,    0,    1,    0,    0x20, 0x00, 0x08, 0x00, 0x00 } };

        EXPECT_FALSE(midi::ci::discovery_inquiry_view::validate(sx));
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, discovery_inquiry)
{
    using VUT = midi::ci::discovery_inquiry_view;

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto identity = midi::device_identity{ midi::manufacturer::native_instruments, 0x1730, 49, 0x00010005 };

        auto sx = midi::ci::make_discovery_inquiry(0x2435465, identity, 0x0C, 766);
        EXPECT_EQ(29u, sx.data.size());

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::discovery_inquiry, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x2435465u, m.source_muid());
        EXPECT_EQ(0xFFFFFFFu, m.destination_muid());

        const auto i = m.identity();

        EXPECT_EQ(0, memcmp(&identity, &i, sizeof(midi::device_identity)));

        EXPECT_EQ(0x0C, m.categories());
        EXPECT_EQ(766u, m.maximum_message_size());
        EXPECT_EQ(0x00u, m.output_path_id());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, make_discovery_inquiry_v1)
{
    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto identity = midi::device_identity{ midi::manufacturer::native_instruments, 0x1730, 49, 0x00010005 };

        auto sx = midi::ci::make_discovery_inquiry_v1(0x2435465, identity, 0x0C, 766);
        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(midi::ci::discovery_inquiry_view::validate(sx));

        auto m = midi::ci::discovery_inquiry_view{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::discovery_inquiry, m.subtype());

        EXPECT_EQ(midi::ci::message_version_1, m.message_version());
        EXPECT_EQ(0x2435465u, m.source_muid());
        EXPECT_EQ(0xFFFFFFFu, m.destination_muid());

        const auto i = m.identity();

        EXPECT_EQ(0, memcmp(&identity, &i, sizeof(midi::device_identity)));

        EXPECT_EQ(0x0C, m.categories());
        EXPECT_EQ(766u, m.maximum_message_size());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, discovery_reply_view)
{
    using VUT = midi::ci::discovery_reply_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x71, 0x01, 0x77, 0x55, 0x33, 0x11, 0x44, 0x33, 0x22, 0x11, 0x00, 0x21,
                           0x09, 0x01, 0x18, 25,   4,    1,    2,    4,    8,    0x20, 0x00, 0x08, 0x00, 0x00 } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::discovery_reply, m.subtype());

        EXPECT_EQ(1, m.message_version());
        EXPECT_EQ(0x22CEAF7u, m.source_muid());
        EXPECT_EQ(0x22899C4u, m.destination_muid());

        const auto identity = m.identity();
        EXPECT_EQ(midi::manufacturer::native_instruments, identity.manufacturer);
        EXPECT_EQ(0x0C01u, identity.family);
        EXPECT_EQ(0x0219u, identity.model);
        EXPECT_EQ(0x01010101u, identity.revision);

        EXPECT_EQ(0x20, m.categories());
        EXPECT_EQ(0x400u, m.maximum_message_size());
        EXPECT_EQ(0x00u, m.output_path_id());
        EXPECT_EQ(0x7Fu, m.function_block());
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x71, 0x02, 0x77, 0x55, 0x33, 0x11, 0x44, 0x33, 0x22, 0x11, 0x00, 0x21, 0x09,
                           0x10, 0x30, 25,   0,    0,    0,    2,    0,    0x20, 0x00, 0x08, 0x00, 0x00, 0x77, 0x02 } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::discovery_reply, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x22CEAF7u, m.source_muid());
        EXPECT_EQ(0x22899C4u, m.destination_muid());

        const auto identity = m.identity();
        EXPECT_EQ(midi::manufacturer::native_instruments, identity.manufacturer);
        EXPECT_EQ(0x1810u, identity.family);
        EXPECT_EQ(25u, identity.model);
        EXPECT_EQ(0x00008000u, identity.revision);

        EXPECT_EQ(0x20, m.categories());
        EXPECT_EQ(0x400u, m.maximum_message_size());
        EXPECT_EQ(0x77u, m.output_path_id());
        EXPECT_EQ(0x02u, m.function_block());
    }

    // invalid subtype
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x70, 0x01, 0x77, 0x55, 0x33, 0x11, 0x44, 0x33, 0x22, 0x11, 0x00, 0x21, 0x09,
                           0x00, 0x18, 25,   0,    0,    0,    1,    0,    0x20, 0x00, 0x08, 0x00, 0x00, 0x01, 0x02 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, discovery_reply)
{
    using VUT = midi::ci::discovery_reply_view;

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto identity = midi::device_identity{ midi::manufacturer::native_instruments, 0x1730, 61, 0x00010005 };

        auto sx = midi::ci::make_discovery_reply(0x3445566, 0x2435465, identity, 0x01, 256, 0x54);
        EXPECT_EQ(30u, sx.data.size());

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::discovery_reply, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());

        const auto i = m.identity();

        EXPECT_EQ(0, memcmp(&identity, &i, sizeof(midi::device_identity)));

        EXPECT_EQ(0x3445566u, m.source_muid());
        EXPECT_EQ(0x2435465u, m.destination_muid());

        EXPECT_EQ(0x01, m.categories());
        EXPECT_EQ(256u, m.maximum_message_size());
        EXPECT_EQ(0x54u, m.output_path_id());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, make_discovery_reply_v1)
{
    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto identity = midi::device_identity{ midi::manufacturer::native_instruments, 0x1730, 61, 0x00010005 };

        auto sx = midi::ci::make_discovery_reply_v1(0x3445566, 0x2435465, identity, 0x01, 256);
        EXPECT_EQ(28u, sx.data.size());

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(midi::ci::discovery_reply_view::validate(sx));

        auto m = midi::ci::discovery_reply_view{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::discovery_reply, m.subtype());
        EXPECT_EQ(midi::ci::message_version_1, m.message_version());

        const auto i = m.identity();

        EXPECT_EQ(0, memcmp(&identity, &i, sizeof(midi::device_identity)));

        EXPECT_EQ(0x3445566u, m.source_muid());
        EXPECT_EQ(0x2435465u, m.destination_muid());

        EXPECT_EQ(0x01, m.categories());
        EXPECT_EQ(256u, m.maximum_message_size());
        EXPECT_EQ(0x00u, m.output_path_id());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, endpoint_information_inquiry_view)
{
    using VUT = midi::ci::endpoint_information_inquiry_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x72, 0x02, 0x77, 0x55, 0x33, 0x11, 0x44, 0x33, 0x22, 0x11, 0x00 } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::endpoint_information_inquiry, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x22CEAF7u, m.source_muid());
        EXPECT_EQ(0x22899C4u, m.destination_muid());

        EXPECT_EQ(0x00, m.status());
    }

    // invalid subtype
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x73, 0x02, 0x77, 0x55, 0x33, 0x11, 0x44, 0x33, 0x22, 0x11, 0x00 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    // invalid size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x72, 0x02, 0x77, 0x55, 0x33, 0x11, 0x44, 0x33, 0x22, 0x11 } };
        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, make_endpoint_information_inquiry)
{
    using VUT = midi::ci::endpoint_information_inquiry_view;

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_endpoint_information_inquiry(0x3445566, 0x2435465, 0x22);

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(midi::ci::endpoint_information_inquiry_view::validate(sx));
        EXPECT_EQ(13u, sx.data.size());

        auto m = midi::ci::endpoint_information_inquiry_view{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::endpoint_information_inquiry, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());

        EXPECT_EQ(0x3445566u, m.source_muid());
        EXPECT_EQ(0x2435465u, m.destination_muid());

        EXPECT_EQ(0x22u, m.status());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, endpoint_information_reply_view)
{
    using VUT = midi::ci::endpoint_information_reply_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x73, 0x02, 0x77, 0x55, 0x33, 0x11, 0x44, 0x33,
                           0x22, 0x11, 0x00, 0x05, 0x00, 0x71, 0x62, 0x53, 0x44, 0x35 } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::endpoint_information_reply, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x22CEAF7u, m.source_muid());
        EXPECT_EQ(0x22899C4u, m.destination_muid());

        EXPECT_EQ(0x00u, m.status());
        EXPECT_EQ(0x0005u, m.information_data_length());

        EXPECT_EQ(0x71u, m.information_data()[0]);
        EXPECT_EQ(0x62u, m.information_data()[1]);
        EXPECT_EQ(0x53u, m.information_data()[2]);
        EXPECT_EQ(0x44u, m.information_data()[3]);
        EXPECT_EQ(0x35u, m.information_data()[4]);
    }

    // invalid subtype
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x75, 0x02, 0x77, 0x55, 0x33, 0x11, 0x44, 0x33,
                           0x22, 0x11, 0x00, 0x05, 0x00, 0x71, 0x62, 0x53, 0x44, 0x35 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    // invalid size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F,
                           0x0D,
                           0x73,
                           0x02,
                           0x77,
                           0x55,
                           0x33,
                           0x11,
                           0x44,
                           0x33,
                           0x22,
                           0x11,
                           0x00,
                           0x05,
                           0x00,
                           0x71,
                           0x62,
                           0x53,
                           0x44 } };
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, make_endpoint_information_reply)
{
    using VUT = midi::ci::endpoint_information_reply_view;

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        std::vector<midi::uint7_t> d{ 9, 8, 7, 6, 5, 4, 3, 2, 1 };
        auto sx = midi::ci::make_endpoint_information_reply(0x3445566, 0x2435465, 0x08, d.data(), d.size());
        EXPECT_EQ(24u, sx.data.size());

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::endpoint_information_reply, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());

        EXPECT_EQ(0x3445566u, m.source_muid());
        EXPECT_EQ(0x2435465u, m.destination_muid());

        EXPECT_EQ(0x08u, m.status());
        EXPECT_EQ(9u, m.information_data_length());

        EXPECT_EQ(d,
                  std::vector<midi::uint7_t>(m.information_data(), m.information_data() + m.information_data_length()));

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, invalidate_muid)
{
    using VUT = midi::ci::invalidate_muid_view;

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_invalidate_muid_message(0x2435465);
        EXPECT_EQ(12u, sx.data.size());

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = midi::ci::invalidate_muid_view{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::invalidate_muid, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x2435465u, m.source_muid());
        EXPECT_EQ(0xFFFFFFFu, m.destination_muid());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, ack_view)
{
    using VUT = midi::ci::ack_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x09, 0x0D, 0x7D, 0x02, 0x45, 0x56, 0x47, 0x38, 0x44, 0x33, 0x22, 0x11, 0x45, 0x10, 10,
                           0x01, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 'T',  'i',  'm',  'e',  'o',  'u',  't' } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x09, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::ack, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x711EB45u, m.source_muid());
        EXPECT_EQ(0x22899C4u, m.destination_muid());

        EXPECT_EQ(0x45u, m.transaction());
        EXPECT_EQ(0x10u, m.status_code());
        EXPECT_EQ(10u, m.status_data());

        EXPECT_EQ(0x01u, m.details()[0]);
        EXPECT_EQ(0x01u, m.details()[1]);
        EXPECT_EQ(0x00u, m.details()[2]);
        EXPECT_EQ(0x00u, m.details()[3]);
        EXPECT_EQ(0x00u, m.details()[4]);

        EXPECT_EQ(7u, m.message_length());
        EXPECT_EQ(m.message(), "Timeout");
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7E, 0x0D, 0x7D, 0x02, 0x77, 0x55, 0x33, 0x11, 0x44, 0x33, 0x22,
                           0x11, 0x22, 0x00, 00,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7E, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::ack, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x22CEAF7u, m.source_muid());
        EXPECT_EQ(0x22899C4u, m.destination_muid());

        EXPECT_EQ(0x22u, m.transaction());
        EXPECT_EQ(0x00u, m.status_code());
        EXPECT_EQ(0u, m.status_data());

        EXPECT_EQ(0x00u, m.details()[0]);
        EXPECT_EQ(0x00u, m.details()[1]);
        EXPECT_EQ(0x00u, m.details()[2]);
        EXPECT_EQ(0x00u, m.details()[3]);
        EXPECT_EQ(0x00u, m.details()[4]);

        EXPECT_EQ(0u, m.message_length());
    }

    // invalid subtype
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7E, 0x0D, 0x7E, 0x02, 0x77, 0x55, 0x33, 0x11, 0x44, 0x33, 0x22,
                           0x11, 0x22, 0x00, 00,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    // invalid size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x09, 0x0D, 0x7D, 0x02, 0x45, 0x56, 0x47, 0x38, 0x44, 0x33, 0x22, 0x11, 0x45,
                           0x10, 10,   0x01, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 'O',  'o',  'p',  's' } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, make_ack_message)
{
    using VUT = midi::ci::ack_view;

    struct ack_details
    {
        midi::uint7_t data[5];
    } details = { { 5, 4, 3, 2, 1 } };

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_ack_message(0x2435465,
                                             0x1234567,
                                             0x4,
                                             midi::ci::subtype::set_property_data_inquiry,
                                             0x10,
                                             20,
                                             details.data,
                                             "A message text");

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x04u, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::ack, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x2435465u, m.source_muid());
        EXPECT_EQ(0x1234567u, m.destination_muid());

        EXPECT_EQ(midi::ci::subtype::set_property_data_inquiry, m.transaction());
        EXPECT_EQ(0x10u, m.status_code());
        EXPECT_EQ(20u, m.status_data());

        EXPECT_EQ(0, memcmp(&details.data, m.details(), 5));

        EXPECT_EQ(14u, m.message_length());
        EXPECT_EQ(m.message(), "A message text");

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, nak_view)
{
    using VUT = midi::ci::nak_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x09, 0x0D, 0x7F, 0x02, 0x45, 0x56, 0x47, 0x38, 0x44, 0x33, 0x22, 0x11, 0x45, 0x10, 10,
                           0x01, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 'T',  'i',  'm',  'e',  'o',  'u',  't' } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x09, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::nak, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x711EB45u, m.source_muid());
        EXPECT_EQ(0x22899C4u, m.destination_muid());

        EXPECT_EQ(0x45u, m.transaction());
        EXPECT_EQ(0x10u, m.status_code());
        EXPECT_EQ(10u, m.status_data());

        EXPECT_EQ(0x01u, m.details()[0]);
        EXPECT_EQ(0x01u, m.details()[1]);
        EXPECT_EQ(0x00u, m.details()[2]);
        EXPECT_EQ(0x00u, m.details()[3]);
        EXPECT_EQ(0x00u, m.details()[4]);

        EXPECT_EQ(7u, m.message_length());
        EXPECT_EQ(m.message(), "Timeout");
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7E, 0x0D, 0x7F, 0x02, 0x77, 0x55, 0x33, 0x11, 0x44, 0x33, 0x22,
                           0x11, 0x22, 0x00, 00,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7E, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::nak, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x22CEAF7u, m.source_muid());
        EXPECT_EQ(0x22899C4u, m.destination_muid());

        EXPECT_EQ(0x22u, m.transaction());
        EXPECT_EQ(0x00u, m.status_code());
        EXPECT_EQ(0u, m.status_data());

        EXPECT_EQ(0x00u, m.details()[0]);
        EXPECT_EQ(0x00u, m.details()[1]);
        EXPECT_EQ(0x00u, m.details()[2]);
        EXPECT_EQ(0x00u, m.details()[3]);
        EXPECT_EQ(0x00u, m.details()[4]);

        EXPECT_EQ(0u, m.message_length());
    }

    // version 1 NAK not supported
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7E, 0x0D, 0x7F, 0x01, 0x77, 0x55, 0x33, 0x11, 0x44, 0x33, 0x22, 0x11 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    // invalid subtype
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7E, 0x0D, 0x7E, 0x02, 0x77, 0x55, 0x33, 0x11, 0x44, 0x33, 0x22,
                           0x11, 0x22, 0x00, 00,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    // invalid size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x09, 0x0D, 0x7F, 0x02, 0x45, 0x56, 0x47, 0x38, 0x44, 0x33, 0x22, 0x11, 0x45,
                           0x10, 10,   0x01, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 'O',  'o',  'p',  's' } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, make_nak_message_v1)
{
    using VUT = midi::capability_inquiry_view;

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_nak_message_v1(0x2435465, 0x1234567, 8);

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        EXPECT_EQ(12u, sx.data.size());

        auto m = VUT{ sx };

        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::nak, m.subtype());

        EXPECT_EQ(midi::ci::message_version_1, m.message_version());
        EXPECT_EQ(0x2435465u, m.source_muid());
        EXPECT_EQ(0x1234567u, m.destination_muid());

        EXPECT_EQ(8, m.device_id());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto ridentity = midi::device_identity{ midi::manufacturer::native_instruments, 0x1730, 61, 0x00010005 };
        auto rsx       = midi::ci::make_discovery_reply_v1(0x3445566, 0x2435465, ridentity, 0x01, 256);
        auto r         = midi::ci::discovery_reply_view{ rsx };

        auto sx = midi::ci::make_nak_message_v1(r);

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        EXPECT_EQ(12u, sx.data.size());

        auto m = VUT{ sx };

        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::nak, m.subtype());

        EXPECT_EQ(midi::ci::message_version_1, m.message_version());
        EXPECT_EQ(0x2435465u, m.source_muid());
        EXPECT_EQ(0x3445566u, m.destination_muid());

        EXPECT_EQ(0x7Fu, m.device_id());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 2);
    }
}

//-----------------------------------------------

TEST_F(capability_inquiry, make_nak_message)
{
    using VUT = midi::ci::nak_view;

    struct nak_details
    {
        midi::uint7_t data[5];
    } details = { { 5, 4, 3, 2, 1 } };

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_nak_message(0x2435465,
                                             0x1234567,
                                             0x4,
                                             midi::ci::subtype::set_property_data_inquiry,
                                             0x10,
                                             20,
                                             details.data,
                                             "A message text");

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x04u, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::nak, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x2435465u, m.source_muid());
        EXPECT_EQ(0x1234567u, m.destination_muid());

        EXPECT_EQ(midi::ci::subtype::set_property_data_inquiry, m.transaction());
        EXPECT_EQ(0x10u, m.status_code());
        EXPECT_EQ(20u, m.status_data());

        EXPECT_EQ(0, memcmp(&details.data, m.details(), 5));

        EXPECT_EQ(14u, m.message_length());
        EXPECT_EQ(m.message(), "A message text");

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        const auto msg = midi::ci::make_notify_message(0x29D4832, 0x57EC42A, 42, 0, 9);
        const auto v   = midi::capability_inquiry_view{ msg };

        auto sx = midi::ci::make_nak_message(v, 88, 15, details.data, "What?!?");

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));
        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x09u, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::nak, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x57EC42Au, m.source_muid());
        EXPECT_EQ(0x29D4832u, m.destination_muid());

        EXPECT_EQ(midi::ci::subtype::notify, m.transaction());
        EXPECT_EQ(88u, m.status_code());
        EXPECT_EQ(15u, m.status_data());

        EXPECT_EQ(0, memcmp(&details.data, m.details(), 5));

        EXPECT_EQ(7u, m.message_length());
        EXPECT_EQ(m.message(), "What?!?");

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 2);
    }
}

//-----------------------------------------------
