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

class ci_profile_configuration : public ::testing::Test
{ };

//-----------------------------------------------

TEST_F(ci_profile_configuration, profile_id)
{
    midi::ci::profile_id a;
    EXPECT_EQ(0x7E, a.byte1);
    EXPECT_EQ(0x00, a.byte2);
    EXPECT_EQ(0x00, a.byte3);
    EXPECT_EQ(0x00, a.byte4);
    EXPECT_EQ(0x00, a.byte5);

    midi::ci::profile_id b{ 0x7E, 0x02, 0x03, 0x04, 0x05 };
    EXPECT_EQ(0x7E, b.byte1);
    EXPECT_EQ(0x02, b.byte2);
    EXPECT_EQ(0x03, b.byte3);
    EXPECT_EQ(0x04, b.byte4);
    EXPECT_EQ(0x05, b.byte5);

    midi::ci::profile_id c{ 0x00, 0x21, 0x09, 0x7F, 0x01 };
    EXPECT_EQ(0x00, c.byte1);
    EXPECT_EQ(0x21, c.byte2);
    EXPECT_EQ(0x09, c.byte3);
    EXPECT_EQ(0x7F, c.byte4);
    EXPECT_EQ(0x01, c.byte5);

    midi::ci::profile_id d{ b };

    EXPECT_TRUE(a == a);
    EXPECT_FALSE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_FALSE(a == d);

    EXPECT_FALSE(b == a);
    EXPECT_TRUE(b == b);
    EXPECT_FALSE(b == c);
    EXPECT_TRUE(b == d);

    EXPECT_FALSE(c == a);
    EXPECT_FALSE(c == b);
    EXPECT_TRUE(c == c);
    EXPECT_FALSE(c == d);

    EXPECT_FALSE(d == a);
    EXPECT_TRUE(d == b);
    EXPECT_FALSE(d == c);
    EXPECT_TRUE(d == d);

    midi::ci::profile_id e{ 0x7E, 0x02, 0x03, 0x05, 0x05 };
    midi::ci::profile_id f{ 0x7E, 0x02, 0x03, 0x04, 0x06 };

    EXPECT_FALSE(b == e);
    EXPECT_FALSE(b == f);
    EXPECT_FALSE(e == f);
    EXPECT_FALSE(f == e);
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, profile_inquiry)
{
    using VUT = midi::ci::profile_inquiry_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x20, 0x02, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x20, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x24D2B78u, m.source_muid());
        EXPECT_EQ(0x22CEAF7u, m.destination_muid());
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x20, 0x02, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));
    }

    // forward compatibility - accept larger message size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x20, 0x02, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x04 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));
    }

    // invalid subtype
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x21, 0x00, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x04 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    // invalid size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x20, 0x02, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    // not a universal sysex message
    {
        midi::sysex7 sx{ midi::manufacturer::native_instruments, { 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0xF7 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_inquiry_message(0x7665544, 0x24D2B78, 0x08);

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(12u, m.data_size());

        EXPECT_EQ(0x08, m.device_id());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7665544u, m.source_muid());
        EXPECT_EQ(0x24D2B78u, m.destination_muid());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_inquiry_message(0x7263544, 5);

        auto m = VUT{ mut };

        EXPECT_EQ(12u, m.data_size());

        EXPECT_EQ(0x7F, m.device_id());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7263544u, m.source_muid());
        EXPECT_EQ(5u, m.destination_muid());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, profile_inquiry_reply_view_make_profiles)
{
    using VUT = midi::ci::profile_inquiry_reply_view;

    midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                     { /* 0  */ 0x7F, 0x0D, 0x21, 0x00, 0x78, 0x56, 0x34, 0x12, 0x44, 0x33, 0x22, 0x11,
                       /* 12 */ 0x02, 0x00, 0x00, 0x21, 0x09, 42,   7,    0x7E, 0x03, 0x02, 0x01, 0x00,
                       /* 24 */ 0x01, 0x00, 0x7E, 0x01, 0x02, 0x03, 0x04,
                       /* 31 */ 0x00, 0x00 } };

    EXPECT_TRUE(VUT::validate(sx));

    const auto pc = VUT{ sx };

    {
        const auto profiles = pc.enabled_profiles();

        EXPECT_EQ(2u, profiles.size());

        EXPECT_EQ(0x00, profiles[0].byte1);
        EXPECT_EQ(0x21, profiles[0].byte2);
        EXPECT_EQ(0x09, profiles[0].byte3);
        EXPECT_EQ(42, profiles[0].byte4);
        EXPECT_EQ(7, profiles[0].byte5);

        EXPECT_EQ(0x7E, profiles[1].byte1);
        EXPECT_EQ(0x03, profiles[1].byte2);
        EXPECT_EQ(0x02, profiles[1].byte3);
        EXPECT_EQ(0x01, profiles[1].byte4);
        EXPECT_EQ(0x00, profiles[1].byte5);
    }

    {
        const auto profiles = pc.disabled_profiles();

        EXPECT_EQ(1u, profiles.size());

        EXPECT_EQ(0x7E, profiles[0].byte1);
        EXPECT_EQ(0x01, profiles[0].byte2);
        EXPECT_EQ(0x02, profiles[0].byte3);
        EXPECT_EQ(0x03, profiles[0].byte4);
    }
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, profile_inquiry_reply_view)
{
    using VUT = midi::ci::profile_inquiry_reply_view;

    {
        midi::sysex7 sx{
            midi::manufacturer::universal_non_realtime,
            { 0x0A, 0x0D, 0x21, 0x02, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x00, 0x00, 0x00, 0x00 }
        };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x21, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x24D2B78u, m.source_muid());
        EXPECT_EQ(0x22CEAF7u, m.destination_muid());

        EXPECT_FALSE(m.has_enabled_profiles());
        EXPECT_FALSE(m.has_disabled_profiles());

        EXPECT_EQ(0u, m.num_enabled_profiles());
        EXPECT_EQ(0u, m.num_disabled_profiles());

        EXPECT_TRUE(m.enabled_profiles().empty());
        EXPECT_TRUE(m.disabled_profiles().empty());
    }

    // forward compatibility - accept larger message size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A,
                           0x0D,
                           0x21,
                           0x02,
                           0x78,
                           0x56,
                           0x34,
                           0x12,
                           0x77,
                           0x55,
                           0x33,
                           0x11,
                           0x00,
                           0x00,
                           0x00,
                           0x00,
                           0x00,
                           0x00,
                           0x00 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x21, 0x02, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x00,
                           0x00, 0x02, 0x00, 0x7E, 0x01, 0x02, 0x03, 0x04, 0x00, 0x21, 0x09, 11,   77 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x21, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x24D2B78u, m.source_muid());
        EXPECT_EQ(0x22CEAF7u, m.destination_muid());

        EXPECT_FALSE(m.has_enabled_profiles());
        EXPECT_TRUE(m.enabled_profiles().empty());

        EXPECT_TRUE(m.has_disabled_profiles());
        EXPECT_EQ(2u, m.num_disabled_profiles());
        const auto disabled = m.disabled_profiles();
        EXPECT_EQ(2u, disabled.size());

        const uint8_t* p = sx.data.data() + 16;
        for (const auto& profile : disabled)
        {
            EXPECT_EQ(0, memcmp(p, &profile, 5));
            p += 5;
        }
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x02, 0x0D, 0x21, 0x02, 0x77, 0x55, 0x33, 0x11, 0x78, 0x56, 0x34, 0x12, 0x02,
                           0x00, 0x41, 0x00, 0x00, 8,    9,    0x7E, 0x03, 0x02, 0x01, 0x00, 0x00, 0x00 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x02, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x21, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x22CEAF7u, m.source_muid());
        EXPECT_EQ(0x24D2B78u, m.destination_muid());

        EXPECT_TRUE(m.has_enabled_profiles());
        EXPECT_EQ(2u, m.num_enabled_profiles());
        const auto enabled = m.enabled_profiles();
        EXPECT_EQ(2u, enabled.size());

        const uint8_t* p = sx.data.data() + 14;
        for (const auto& profile : enabled)
        {
            EXPECT_EQ(0, memcmp(p, &profile, 5));
            p += 5;
        }

        EXPECT_FALSE(m.has_disabled_profiles());
        EXPECT_EQ(0u, m.num_disabled_profiles());
        EXPECT_TRUE(m.disabled_profiles().empty());
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x21, 0x02, 0x12, 0x34, 0x56, 0x78, 0x78, 0x56, 0x34, 0x12,
                           0x01, 0x00, 0x00, 0x21, 0x09, 42,   7,    0x03, 0x00, 0x7E, 0x01, 0x02,
                           0x03, 0x04, 0x00, 0x21, 0x09, 11,   77,   0x7E, 0x04, 0x03, 0x02, 0x01 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x21, m.subtype());

        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0xF159A12u, m.source_muid());
        EXPECT_EQ(0x24D2B78u, m.destination_muid());

        EXPECT_TRUE(m.has_enabled_profiles());
        EXPECT_EQ(1u, m.num_enabled_profiles());
        const auto enabled = m.enabled_profiles();
        EXPECT_EQ(1u, enabled.size());

        const uint8_t* p = sx.data.data() + 14u;
        for (const auto& profile : enabled)
        {
            EXPECT_EQ(0, memcmp(p, &profile, 5));
            p += 5;
        }

        EXPECT_TRUE(m.has_disabled_profiles());
        EXPECT_EQ(3u, m.num_disabled_profiles());
        const auto disabled = m.disabled_profiles();
        EXPECT_EQ(3u, disabled.size());

        p += 2;
        for (const auto& profile : disabled)
        {
            EXPECT_EQ(0, memcmp(p, &profile, 5));
            p += 5;
        }
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x14, 0x02, 0x78, 0x56, 0x34, 0x12, 0x12, 0x34, 0x56, 0x78, 0x00,
                           0x00, 0x02, 0x00, 0x7E, 0x01, 0x02, 0x03, 0x04, 0x00, 0x21, 0x09, 0,    11 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x21, 0x02, 0x78, 0x56, 0x34, 0x12, 0x12, 0x34, 0x56, 0x78, 0x00,
                           0x01, 0x02, 0x00, 0x7E, 0x01, 0x02, 0x03, 0x04, 0x00, 0x21, 0x09, 0,    11 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x21, 0x02, 0x12, 0x34, 0x56, 0x78, 0x78, 0x56, 0x34, 0x12,
                           0x01, 0x00, 0x00, 0x21, 0x09, 42,   7,    12,   0x03, 0x00, 0x7E, 0x01,
                           0x02, 0x03, 0x04, 0x00, 0x21, 0x09, 11,   77,   0x7E, 0x01, 0x02, 0x03 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::native_instruments, { 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0xF7 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, make_profile_inquiry_reply_from_carray)
{
    using VUT = midi::ci::profile_inquiry_reply_view;

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_inquiry_reply(0x7665544, 0x4711, nullptr, 0, nullptr, 0, 0x08);
        EXPECT_EQ(16u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x08, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x21, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7665544u, m.source_muid());
        EXPECT_EQ(0x4711u, m.destination_muid());

        EXPECT_FALSE(m.has_enabled_profiles());
        EXPECT_EQ(0u, m.num_enabled_profiles());
        const auto enabled = m.enabled_profiles();
        EXPECT_EQ(0u, enabled.size());

        EXPECT_FALSE(m.has_disabled_profiles());
        EXPECT_EQ(0u, m.num_disabled_profiles());
        const auto disabled = m.disabled_profiles();
        EXPECT_EQ(0u, disabled.size());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        constexpr midi::uint14_t    num_enabled_profiles = 0;
        const midi::ci::profile_id* enabled_profiles     = nullptr;

        constexpr midi::uint14_t   num_disabled_profiles = 2;
        const midi::ci::profile_id disabled_profiles[]   = { { 0x7E, 0x01, 0x02, 0x03, 0x04 },
                                                             { 0x00, 0x21, 0x09, 11, 77 } };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_inquiry_reply(
          0x1234567, 0xCCCCCCC, enabled_profiles, num_enabled_profiles, disabled_profiles, num_disabled_profiles, 0x0A);
        EXPECT_EQ(26u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x21, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(0xCCCCCCCu, m.destination_muid());

        EXPECT_FALSE(m.has_enabled_profiles());
        EXPECT_TRUE(m.enabled_profiles().empty());

        EXPECT_TRUE(m.has_disabled_profiles());
        EXPECT_EQ(2u, m.num_disabled_profiles());
        const auto disabled = m.disabled_profiles();
        EXPECT_EQ(2u, disabled.size());

        auto p = 0;
        for (const auto& profile : disabled)
        {
            EXPECT_EQ(0, memcmp(disabled_profiles + p, &profile, 5));
            ++p;
        }

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        constexpr midi::uint14_t   num_enabled_profiles = 2;
        const midi::ci::profile_id enabled_profiles[]   = { { 0x41, 0x00, 0x00, 8, 0 }, { 0x00, 0x21, 0x09, 11, 77 } };

        constexpr midi::uint14_t    num_disabled_profiles = 0;
        const midi::ci::profile_id* disabled_profiles     = nullptr;

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_inquiry_reply(
          0x1133557, 0x2244668, enabled_profiles, num_enabled_profiles, disabled_profiles, num_disabled_profiles, 0x02);
        EXPECT_EQ(26u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x02, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x21, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1133557u, m.source_muid());
        EXPECT_EQ(0x2244668u, m.destination_muid());

        EXPECT_TRUE(m.has_enabled_profiles());
        EXPECT_EQ(2u, m.num_enabled_profiles());
        const auto enabled = m.enabled_profiles();
        EXPECT_EQ(2u, enabled.size());

        auto p = 0;
        for (const auto& profile : enabled)
        {
            EXPECT_EQ(0, memcmp(enabled_profiles + p, &profile, 5));
            ++p;
        }

        EXPECT_FALSE(m.has_disabled_profiles());
        EXPECT_EQ(0u, m.num_disabled_profiles());
        EXPECT_TRUE(m.disabled_profiles().empty());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        constexpr midi::uint14_t   num_enabled_profiles = 1;
        const midi::ci::profile_id enabled_profiles[]   = { { 0x00, 0x21, 0x09, 42, 7 } };

        constexpr midi::uint14_t   num_disabled_profiles = 3;
        const midi::ci::profile_id disabled_profiles[]   = { { 0x7E, 0x01, 0x02, 0x03, 0x04 },
                                                             { 0x00, 0x21, 0x09, 11, 77 },
                                                             { 0x7E, 0x03, 0x02, 0x01, 0x00 } };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_inquiry_reply(
          0x8563412, 0x4927364, enabled_profiles, num_enabled_profiles, disabled_profiles, num_disabled_profiles);
        EXPECT_EQ(36u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x21, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x8563412u, m.source_muid());
        EXPECT_EQ(0x4927364u, m.destination_muid());

        EXPECT_TRUE(m.has_enabled_profiles());
        EXPECT_EQ(1u, m.num_enabled_profiles());
        const auto enabled = m.enabled_profiles();
        EXPECT_EQ(1u, enabled.size());

        auto p = 0;
        for (const auto& profile : enabled)
        {
            EXPECT_EQ(0, memcmp(enabled_profiles + p, &profile, 5));
            ++p;
        }

        EXPECT_TRUE(m.has_disabled_profiles());
        EXPECT_EQ(3u, m.num_disabled_profiles());
        const auto disabled = m.disabled_profiles();
        EXPECT_EQ(3u, disabled.size());

        p = 0;
        for (const auto& profile : disabled)
        {
            EXPECT_EQ(0, memcmp(disabled_profiles + p, &profile, 5));
            ++p;
        }

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        constexpr midi::uint14_t   num_enabled_profiles = 256;
        const midi::ci::profile_id enabled_profiles[num_enabled_profiles];

        constexpr midi::uint14_t   num_disabled_profiles = 322;
        const midi::ci::profile_id disabled_profiles[num_disabled_profiles];

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_inquiry_reply(
          0x7856341, 0x1234567, enabled_profiles, num_enabled_profiles, disabled_profiles, num_disabled_profiles);
        EXPECT_EQ(2906u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x21, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7856341u, m.source_muid());
        EXPECT_EQ(0x1234567u, m.destination_muid());

        EXPECT_TRUE(m.has_enabled_profiles());
        EXPECT_EQ(num_enabled_profiles, m.num_enabled_profiles());
        const auto enabled = m.enabled_profiles();
        EXPECT_EQ(num_enabled_profiles, enabled.size());

        auto p = 0;
        for (const auto& profile : enabled)
        {
            EXPECT_EQ(0, memcmp(enabled_profiles + p, &profile, 5));
            ++p;
        }

        EXPECT_TRUE(m.has_disabled_profiles());
        EXPECT_EQ(num_disabled_profiles, m.num_disabled_profiles());
        const auto disabled = m.disabled_profiles();
        EXPECT_EQ(num_disabled_profiles, disabled.size());

        p = 0;
        for (const auto& profile : disabled)
        {
            EXPECT_EQ(0, memcmp(disabled_profiles + p, &profile, 5));
            ++p;
        }

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, make_profile_inquiry_reply_from_vector)
{
    using VUT = midi::ci::profile_inquiry_reply_view;

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_inquiry_reply(
          0xF665544, 0x7777777, std::vector<midi::ci::profile_id>{}, std::vector<midi::ci::profile_id>{}, 0x08);
        EXPECT_EQ(16u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x08, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x21, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0xF665544u, m.source_muid());
        EXPECT_EQ(0x7777777u, m.destination_muid());

        EXPECT_FALSE(m.has_enabled_profiles());
        EXPECT_EQ(0u, m.num_enabled_profiles());
        const auto enabled = m.enabled_profiles();
        EXPECT_EQ(0u, enabled.size());

        EXPECT_FALSE(m.has_disabled_profiles());
        EXPECT_EQ(0u, m.num_disabled_profiles());
        const auto disabled = m.disabled_profiles();
        EXPECT_EQ(0u, disabled.size());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const std::vector<midi::ci::profile_id> enabled_profiles;
        const std::vector<midi::ci::profile_id> disabled_profiles{ { 0x7E, 0x01, 0x02, 0x03, 0x04 },
                                                                   { 0x00, 0x21, 0x09, 11, 77 } };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_inquiry_reply(0x2345678, 5, enabled_profiles, disabled_profiles, 0x0A);
        EXPECT_EQ(26u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x21, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x2345678u, m.source_muid());
        EXPECT_EQ(5u, m.destination_muid());

        EXPECT_FALSE(m.has_enabled_profiles());
        EXPECT_TRUE(m.enabled_profiles().empty());

        EXPECT_TRUE(m.has_disabled_profiles());
        EXPECT_EQ(2u, m.num_disabled_profiles());
        const auto disabled = m.disabled_profiles();
        EXPECT_EQ(2u, disabled.size());

        auto p = 0;
        for (const auto& profile : disabled)
        {
            EXPECT_EQ(0, memcmp(disabled_profiles.data() + p, &profile, 5));
            ++p;
        }

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const std::vector<midi::ci::profile_id> enabled_profiles{ { 0x41, 0x00, 0x00, 8, 1 },
                                                                  { 0x00, 0x21, 0x09, 11, 77 } };

        const std::vector<midi::ci::profile_id> disabled_profiles;

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut =
          midi::ci::make_profile_inquiry_reply(0xC335577, 0xACACACA, enabled_profiles, disabled_profiles, 0x02);
        EXPECT_EQ(26u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x02, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x21, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0xC335577u, m.source_muid());
        EXPECT_EQ(0xACACACAu, m.destination_muid());

        EXPECT_TRUE(m.has_enabled_profiles());
        EXPECT_EQ(2u, m.num_enabled_profiles());
        const auto enabled = m.enabled_profiles();
        EXPECT_EQ(2u, enabled.size());

        auto p = 0;
        for (const auto& profile : enabled)
        {
            EXPECT_EQ(0, memcmp(enabled_profiles.data() + p, &profile, 5));
            ++p;
        }

        EXPECT_FALSE(m.has_disabled_profiles());
        EXPECT_EQ(0u, m.num_disabled_profiles());
        EXPECT_TRUE(m.disabled_profiles().empty());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const std::vector<midi::ci::profile_id> enabled_profiles{ { 0x00, 0x21, 0x09, 42, 7 } };

        const std::vector<midi::ci::profile_id> disabled_profiles{ { 0x7E, 0x01, 0x02, 0x03, 0x04 },
                                                                   { 0x00, 0x21, 0x09, 11, 77 },
                                                                   { 0x7E, 0x04, 0x03, 0x02, 0x01 } };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_inquiry_reply(0x8563412, 0x1918172, enabled_profiles, disabled_profiles);
        EXPECT_EQ(36u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x21, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x8563412u, m.source_muid());
        EXPECT_EQ(0x1918172u, m.destination_muid());

        EXPECT_TRUE(m.has_enabled_profiles());
        EXPECT_EQ(1u, m.num_enabled_profiles());
        const auto enabled = m.enabled_profiles();
        EXPECT_EQ(1u, enabled.size());

        auto p = 0;
        for (const auto& profile : enabled)
        {
            EXPECT_EQ(0, memcmp(enabled_profiles.data() + p, &profile, 5));
            ++p;
        }

        EXPECT_TRUE(m.has_disabled_profiles());
        EXPECT_EQ(3u, m.num_disabled_profiles());
        const auto disabled = m.disabled_profiles();
        EXPECT_EQ(3u, disabled.size());

        p = 0;
        for (const auto& profile : disabled)
        {
            EXPECT_EQ(0, memcmp(disabled_profiles.data() + p, &profile, 5));
            ++p;
        }

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, profile_id_view)
{
    using VUT = midi::ci::profile_id_view;

    {
        const midi::ci::profile_id profile{ 0x01, 0x02, 0x03, 0x04, 0x05 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_on_request(0x1234568, 0x2345689, profile, 0x0D);
        EXPECT_EQ(17u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x0D, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x22, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234568u, m.source_muid());
        EXPECT_EQ(0x2345689u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const midi::ci::profile_id profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_disabled_notification(0x7856312, 0x2345689, profile);
        EXPECT_EQ(17u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x25, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7856312u, m.source_muid());
        EXPECT_EQ(0x2345689u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const midi::ci::profile_id profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_added_notification(0x7863412, 0x2345689, profile);
        EXPECT_EQ(17u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x26, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7863412u, m.source_muid());
        EXPECT_EQ(0x2345689u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    // invalid type
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F,
                           0x08,
                           0x26,
                           0x02,
                           0x78,
                           0x56,
                           0x34,
                           0x12,
                           0x77,
                           0x55,
                           0x33,
                           0x11,
                           0x44,
                           0x55,
                           0x66,
                           0x12,
                           0x34,
                           0x56 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    // invalid subtype
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F,
                           0x0D,
                           0x28,
                           0x02,
                           0x78,
                           0x56,
                           0x34,
                           0x12,
                           0x77,
                           0x55,
                           0x33,
                           0x11,
                           0x44,
                           0x55,
                           0x66,
                           0x12,
                           0x34,
                           0x56 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    // invalid size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x26, 0x02, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    // forward compatibility - accept larger message size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F,
                           0x0D,
                           0x26,
                           0x02,
                           0x78,
                           0x56,
                           0x34,
                           0x12,
                           0x77,
                           0x55,
                           0x33,
                           0x11,
                           0x44,
                           0x55,
                           0x66,
                           0x12,
                           0x34,
                           0x56 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));
    }
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, set_profile_on)
{
    using VUT = midi::ci::profile_id_view;

    {
        const midi::ci::profile_id profile{ 0x01, 0x02, 0x03, 0x04, 0x05 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_on_request(0x1234567, 0, profile, 0x0D);
        EXPECT_EQ(17u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x0D, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x22, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(0u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const midi::ci::profile_id profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_on_request(0x8563412, 0x1FFFFFF, profile);
        EXPECT_EQ(17u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x22, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x8563412u, m.source_muid());
        EXPECT_EQ(0x1FFFFFFu, m.destination_muid());
        EXPECT_EQ(profile, m.profile());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const midi::ci::profile_id profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };

        auto mut    = midi::ci::make_profile_on_request(0x8563412, 0x1234567, profile);
        mut.data[2] = 0x31;

        EXPECT_FALSE(VUT::validate(mut));
        EXPECT_FALSE(midi::ci::as<VUT>(mut));
    }
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, set_profile_off)
{
    using VUT = midi::ci::profile_id_view;

    {
        const midi::ci::profile_id profile{ 0x01, 0x02, 0x03, 0x04, 0x05 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_off_request(0x1234567, 4711, profile, 0x0D);
        EXPECT_EQ(17u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x0D, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x23, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(4711u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const midi::ci::profile_id profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_off_request(0x7563412, 42, profile);
        EXPECT_EQ(17u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x23, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7563412u, m.source_muid());
        EXPECT_EQ(42u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const midi::ci::profile_id profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };

        auto mut    = midi::ci::make_profile_off_request(0x7856341, 0x292A2B2, profile);
        mut.data[2] = 0x31;

        EXPECT_FALSE(VUT::validate(mut));
        EXPECT_FALSE(midi::ci::as<VUT>(mut));
    }
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, profile_enabled)
{
    using VUT = midi::ci::profile_id_view;

    {
        const midi::ci::profile_id profile{ 0x01, 0x02, 0x03, 0x04, 0x05 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_enabled_notification(0x2345678, 0x0101010, profile, 0x0D);
        EXPECT_EQ(17u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x0D, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x24, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x2345678u, m.source_muid());
        EXPECT_EQ(0x0101010u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const midi::ci::profile_id profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_enabled_notification(0x7863412, 0x5647382, profile);
        EXPECT_EQ(17u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x24, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7863412u, m.source_muid());
        EXPECT_EQ(0x5647382u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const midi::ci::profile_id profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };

        auto mut    = midi::ci::make_profile_enabled_notification(0x8563412, 0x0192837, profile);
        mut.data[2] = 0x31;

        EXPECT_FALSE(VUT::validate(mut));
        EXPECT_FALSE(midi::ci::as<VUT>(mut));
    }
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, profile_disabled)
{
    using VUT = midi::ci::profile_id_view;

    {
        const midi::ci::profile_id profile{ 0x01, 0x02, 0x03, 0x04, 0x05 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_disabled_notification(0x1234567, 5, profile, 0x0D);
        EXPECT_EQ(17u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x0D, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x25, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(5u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const midi::ci::profile_id profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_disabled_notification(0x7856341, 0x9999999, profile);
        EXPECT_EQ(17u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x25, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7856341u, m.source_muid());
        EXPECT_EQ(0x9999999u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const midi::ci::profile_id profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };

        auto mut    = midi::ci::make_profile_disabled_notification(0x7856312, 13, profile);
        mut.data[2] = 0x31;

        EXPECT_FALSE(VUT::validate(mut));
        EXPECT_FALSE(midi::ci::as<VUT>(mut));
    }
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, profile_added)
{
    using VUT = midi::ci::profile_id_view;

    {
        const midi::ci::profile_id profile{ 0x01, 0x02, 0x03, 0x04, 0x05 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_added_notification(0x1234567, 5, profile, 0x0D);
        EXPECT_EQ(17u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x0D, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::profile_added, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(5u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const midi::ci::profile_id profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_added_notification(0x7856341, 0x9999999, profile);
        EXPECT_EQ(17u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::profile_added, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7856341u, m.source_muid());
        EXPECT_EQ(0x9999999u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const midi::ci::profile_id profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };

        auto mut    = midi::ci::make_profile_added_notification(0x7856312, 13, profile);
        mut.data[2] = 0x31;

        EXPECT_FALSE(VUT::validate(mut));
        EXPECT_FALSE(midi::ci::as<VUT>(mut));
    }
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, profile_removed)
{
    using VUT = midi::ci::profile_id_view;

    {
        const midi::ci::profile_id profile{ 0x01, 0x02, 0x03, 0x04, 0x05 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_removed_notification(0x1234567, 5, profile, 0x0D);
        EXPECT_EQ(17u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x0D, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::profile_removed, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(5u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());
    }

    {
        const midi::ci::profile_id profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto mut = midi::ci::make_profile_removed_notification(0x7856341, 0x9999999, profile);
        EXPECT_EQ(17u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::profile_removed, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7856341u, m.source_muid());
        EXPECT_EQ(0x9999999u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const midi::ci::profile_id profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };

        auto mut    = midi::ci::make_profile_removed_notification(0x7856312, 13, profile);
        mut.data[2] = 0x31;

        EXPECT_FALSE(VUT::validate(mut));
        EXPECT_FALSE(midi::ci::as<VUT>(mut));
    }
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, profile_details_inquiry_view)
{
    using VUT = midi::ci::profile_details_inquiry_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x09,
                           0x0D,
                           0x28,
                           0x02,
                           0x45,
                           0x56,
                           0x47,
                           0x38,
                           0x44,
                           0x33,
                           0x22,
                           0x11,
                           0x01,
                           0x02,
                           0x03,
                           0x04,
                           0x05,
                           0x34 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x09, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::profile_details_inquiry, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x711EB45u, m.source_muid());
        EXPECT_EQ(0x22899C4u, m.destination_muid());

        const midi::ci::profile_id profile{ 0x01, 0x02, 0x03, 0x04, 0x05 };
        EXPECT_EQ(profile, m.profile());
        EXPECT_EQ(0x34u, m.target());
    }

    // invalid subtype
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x09,
                           0x0D,
                           0x22,
                           0x02,
                           0x45,
                           0x56,
                           0x47,
                           0x38,
                           0x44,
                           0x33,
                           0x22,
                           0x11,
                           0x01,
                           0x02,
                           0x03,
                           0x04,
                           0x05,
                           0x34 } };

        EXPECT_FALSE(VUT::validate(sx));
    }

    // invalid size
    {
        midi::sysex7 sx{
            midi::manufacturer::universal_non_realtime,
            { 0x09, 0x0D, 0x28, 0x02, 0x45, 0x56, 0x47, 0x38, 0x44, 0x33, 0x22, 0x11, 0x01, 0x02, 0x03, 0x04, 0x05 }
        };

        EXPECT_FALSE(VUT::validate(sx));
    }
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, make_profile_details_inquiry)
{
    using VUT = midi::ci::profile_details_inquiry_view;

    {
        const midi::ci::profile_id profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_profile_details_inquiry(0x2345678, 0x0101010, profile, 0x32, 0x7E);
        EXPECT_EQ(18u, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7Eu, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::profile_details_inquiry, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x2345678u, m.source_muid());
        EXPECT_EQ(0x0101010u, m.destination_muid());

        EXPECT_EQ(profile, m.profile());
        EXPECT_EQ(0x32u, m.target());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, profile_details_reply_view)
{
    using VUT = midi::ci::profile_details_reply_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x09, 0x0D, 0x29, 0x02, 0x45, 0x56, 0x47, 0x38, 0x44, 0x33, 0x22, 0x11, 0x01, 0x02, 0x03,
                           0x04, 0x05, 0x11, 0x09, 0x00, 9,    8,    7,    6,    5,    4,    3,    2,    1 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x09, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::profile_details_reply, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x711EB45u, m.source_muid());
        EXPECT_EQ(0x22899C4u, m.destination_muid());

        EXPECT_EQ(0x11u, m.target());

        EXPECT_EQ(9u, m.target_data_length());
        EXPECT_EQ(9u, m.target_data()[0]);
        EXPECT_EQ(8u, m.target_data()[1]);
        EXPECT_EQ(7u, m.target_data()[2]);
        EXPECT_EQ(6u, m.target_data()[3]);
        EXPECT_EQ(5u, m.target_data()[4]);
        EXPECT_EQ(4u, m.target_data()[5]);
        EXPECT_EQ(3u, m.target_data()[6]);
        EXPECT_EQ(2u, m.target_data()[7]);
        EXPECT_EQ(1u, m.target_data()[8]);
    }

    // invalid subtype
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x09, 0x0D, 0x2E, 0x02, 0x45, 0x56, 0x47, 0x38, 0x44, 0x33, 0x22, 0x11, 0x01, 0x02, 0x03,
                           0x04, 0x05, 0x11, 0x09, 0x00, 9,    8,    7,    6,    5,    4,    3,    2,    1 } };

        EXPECT_FALSE(VUT::validate(sx));
    }

    // invalid size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x09, 0x0D, 0x29, 0x02, 0x45, 0x56, 0x47, 0x38, 0x44, 0x33, 0x22, 0x11, 0x01, 0x02, 0x03,
                           0x04, 0x05, 0x11, 0x00, 0x01, 9,    8,    7,    6,    5,    4,    3,    2,    1 } };

        EXPECT_FALSE(VUT::validate(sx));
    }
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, make_profile_details_reply)
{
    using VUT = midi::ci::profile_details_reply_view;

    {
        const midi::ci::profile_id       profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };
        const std::vector<midi::uint7_t> data{ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_profile_details_reply(0x2345678, 0x0101010, profile, 0x32, data.data(), data.size());
        EXPECT_EQ(31u, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7Fu, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::profile_details_reply, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x2345678u, m.source_muid());
        EXPECT_EQ(0x0101010u, m.destination_muid());

        EXPECT_EQ(profile, m.profile());
        EXPECT_EQ(0x32u, m.target());

        EXPECT_EQ(11u, m.target_data_length());
        EXPECT_EQ(0, memcmp(data.data(), m.target_data(), 11));

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(ci_profile_configuration, profile_specific_data)
{
    using VUT = midi::ci::profile_specific_data_view;

    {
        const midi::ci::profile_id profile{ 0x01, 0x02, 0x03, 0x04, 0x05 };

        auto mut = make_profile_specific_data_message(0x1234567, 5, profile, nullptr, 0, 0x0D);
        EXPECT_EQ(21u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x0D, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x2F, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(5u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());
        EXPECT_EQ(0u, m.data_size());
        EXPECT_EQ(m.data_end(), m.data_begin());
    }

    {
        const midi::ci::profile_id profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };
        const midi::uint7_t        data[] = { 0x01, 0x02, 0x03, 0x04 };

        auto mut = make_profile_specific_data_message(0x7856341, 0x9999999, profile, data, sizeof(data));
        EXPECT_EQ(25u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x2F, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7856341u, m.source_muid());
        EXPECT_EQ(0x9999999u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());
        EXPECT_EQ(4u, m.data_size());
        EXPECT_EQ(m.data_end(), m.data_begin() + 4);
        EXPECT_EQ(0, memcmp(data, m.data_begin(), 4));
    }

    {
        const midi::ci::profile_id       profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };
        const std::vector<midi::uint7_t> data{ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

        auto mut = make_profile_specific_data_message(0x7856312, 13, profile, data);
        EXPECT_EQ(32u, mut.data.size());

        EXPECT_TRUE(VUT::validate(mut));
        EXPECT_TRUE(midi::ci::as<VUT>(mut));

        auto m = VUT{ mut };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x2F, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x7856312u, m.source_muid());
        EXPECT_EQ(13u, m.destination_muid());
        EXPECT_EQ(profile, m.profile());
        EXPECT_EQ(11u, m.data_size());
        EXPECT_EQ(m.data_end(), m.data_begin() + 11);
        EXPECT_EQ(0, memcmp(data.data(), m.data_begin(), 11));
    }

    {
        const midi::ci::profile_id       profile{ 0x7E, 0x44, 0x33, 0x22, 0x11 };
        const std::vector<midi::uint7_t> data{ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

        auto mut     = make_profile_specific_data_message(0x7856312, 13, profile, data);
        mut.data[18] = 7; // fuzz data size

        EXPECT_FALSE(VUT::validate(mut));
    }
}

//-----------------------------------------------
