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

class ci_property_exchange : public ::testing::Test
{ };

//-----------------------------------------------

TEST_F(ci_property_exchange, property_exchange_capabilities_view)
{
    using VUT = midi::ci::property_exchange_capabilities_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x30, 0x01, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 2 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x30, m.subtype());
        EXPECT_EQ(1, m.message_version());
        EXPECT_EQ(0x24D2B78u, m.source_muid());
        EXPECT_EQ(0x22CEAF7u, m.destination_muid());

        EXPECT_EQ(2, m.maximum_number_of_requests());
        EXPECT_EQ(0, m.pe_version_major());
        EXPECT_EQ(0, m.pe_version_minor());
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x31, 0x02, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x03, 1, 2 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x31, m.subtype());
        EXPECT_EQ(2, m.message_version());
        EXPECT_EQ(0x24D2B78u, m.source_muid());
        EXPECT_EQ(0x22CEAF7u, m.destination_muid());

        EXPECT_EQ(3, m.maximum_number_of_requests());
        EXPECT_EQ(1, m.pe_version_major());
        EXPECT_EQ(2, m.pe_version_minor());
    }

    // message version 2 with missing pe version fields
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x31, 0x02, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 2 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    // invalid type
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0E, 0x30, 0x01, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 2 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    // invalid subtype
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x21, 0x00, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 2 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    // invalid size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x30, 0x01, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x31, 0x01, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    // forward compatibility - accept larger message size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x30, 0x01, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 2, 1, 0 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x31, 0x01, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x03, 0x04, 0x05 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));
    }
}

//-----------------------------------------------

TEST_F(ci_property_exchange, make_property_exchange_capabilities_inquiry)
{
    using VUT = midi::ci::property_exchange_capabilities_view;

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_property_exchange_capabilities_inquiry(0x1234567, 0xFEDCBA9, 3, 0x05);
        EXPECT_EQ(15u, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x05, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x30, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(0xFEDCBA9u, m.destination_muid());

        EXPECT_EQ(3, m.maximum_number_of_requests());
        EXPECT_EQ(0, m.pe_version_major());
        EXPECT_EQ(0, m.pe_version_minor());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_property_exchange_capabilities_inquiry(0x856312, 0x123456);
        EXPECT_EQ(15u, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x30, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x856312u, m.source_muid());
        EXPECT_EQ(0x123456u, m.destination_muid());

        EXPECT_EQ(1, m.maximum_number_of_requests());
        EXPECT_EQ(0, m.pe_version_major());
        EXPECT_EQ(0, m.pe_version_minor());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_property_exchange_capabilities_inquiry(0x1234567, 0x9856312, 7);
        EXPECT_EQ(15u, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x30, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(0x9856312u, m.destination_muid());

        EXPECT_EQ(7, m.maximum_number_of_requests());
        EXPECT_EQ(0, m.pe_version_major());
        EXPECT_EQ(0, m.pe_version_minor());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(ci_property_exchange, reply_capabilities)
{
    using VUT = midi::ci::property_exchange_capabilities_view;

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_property_exchange_capabilities_reply(0x1234567, 0xABCDEF0, 12, 0x08);
        EXPECT_EQ(15u, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x08, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x31, m.subtype());
        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(0xABCDEF0u, m.destination_muid());

        EXPECT_EQ(12, m.maximum_number_of_requests());
        EXPECT_EQ(0, m.pe_version_major());
        EXPECT_EQ(0, m.pe_version_minor());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_property_exchange_capabilities_reply(0x756341, 0x392817);
        EXPECT_EQ(15u, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x31, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x756341u, m.source_muid());
        EXPECT_EQ(0x392817u, m.destination_muid());

        EXPECT_EQ(1, m.maximum_number_of_requests());
        EXPECT_EQ(0, m.pe_version_major());
        EXPECT_EQ(0, m.pe_version_minor());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_property_exchange_capabilities_reply(0x1234567, 0x392817F, 7);
        EXPECT_EQ(15u, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x31, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(0x392817Fu, m.destination_muid());

        EXPECT_EQ(7, m.maximum_number_of_requests());
        EXPECT_EQ(0, m.pe_version_major());
        EXPECT_EQ(0, m.pe_version_minor());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(ci_property_exchange, property_data_message_view)
{
    using VUT = midi::ci::property_exchange::property_data_message_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x34, 0x05, 0x78, 0x56, 0x34, 0x12, 0x12, 0x34, 0x56, 0x78, 0x00, 0x08, 0x00,
                           0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x34, m.subtype());

        EXPECT_EQ(5, m.message_version());
        EXPECT_EQ(0x24D2B78u, m.source_muid());
        EXPECT_EQ(0xF159A12u, m.destination_muid());

        EXPECT_EQ(0x00, m.request_id());

        EXPECT_EQ(8u, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), sx.data.data() + 15, m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin() + m.chunk_size());
    }

    // forward compatibility - accept larger message size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x34, 0x05, 0x78, 0x56, 0x34, 0x12, 0x12, 0x34, 0x56,
                           0x78, 0x00, 0x08, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x37, 0x01, 0x77, 0x55, 0x33, 0x11, 0x12, 0x34, 0x56, 0x78, 0x04,
                           0x10, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x10, 0x20,
                           0x30, 0x40, 0x50, 0x60, 0x70, 0x10, 0x00, 0x01, 0x00, 0x01, 0x00, 0x33 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x37, m.subtype());

        EXPECT_EQ(1u, m.message_version());
        EXPECT_EQ(0x22CEAF7u, m.source_muid());
        EXPECT_EQ(0xF159A12u, m.destination_muid());

        EXPECT_EQ(0x04, m.request_id());

        EXPECT_EQ(16u, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), sx.data.data() + 15, m.header_size()));

        EXPECT_EQ(16u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        EXPECT_EQ(1u, m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin() + m.chunk_size());
        EXPECT_EQ(0x33, *m.chunk_begin());
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x3F, 0x11, 0x77, 0x55, 0x33, 0x11, 0x12, 0x34, 0x56, 0x78, 0x1E,
                           0x10, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x10, 0x20,
                           0x30, 0x40, 0x50, 0x60, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x3F, m.subtype());

        EXPECT_EQ(0x11, m.message_version());
        EXPECT_EQ(0x22CEAF7u, m.source_muid());
        EXPECT_EQ(0xF159A12u, m.destination_muid());

        EXPECT_EQ(0x1E, m.request_id());

        EXPECT_EQ(16u, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), sx.data.data() + 15, m.header_size()));

        EXPECT_EQ(0u, m.number_of_chunks());
        EXPECT_EQ(0u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin() + m.chunk_size());
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x36, 0x05, 0x78, 0x56, 0x34, 0x12, 0x12, 0x34, 0x56, 0x78, 0x04, 0x00, 0x01,
                           0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x37, 0x00, 0x77, 0x55, 0x34, 0x11, 0x12, 0x34, 0x56, 0x78, 0x04,
                           0x12, 0x34, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x10, 0x20,
                           0x30, 0x40, 0x50, 0x60, 0x70, 0x12, 0x34, 0x01, 0x23, 0x56, 0x04 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x7F, 0x0D, 0x3C, 0x11, 0x77, 0x55, 0x38, 0x11, 0x12, 0x34, 0x56, 0x78, 0x04,
                           0x10, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x10, 0x20,
                           0x30, 0x40, 0x50, 0x60, 0x70, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }
}

//-----------------------------------------------

TEST_F(ci_property_exchange, property_data_message_from_carray)
{
    using VUT = midi::ci::property_exchange::property_data_message_view;

    {
        const midi::uint7_t header[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };

        auto sx = midi::ci::property_exchange::make_property_data_message(
          0x34, 0x1234567, 0xFEED, { header, sizeof(header) }, 1, 1, {}, 5, 0x0A);
        EXPECT_EQ(29u, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x34, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(0xFEEDu, m.destination_muid());

        EXPECT_EQ(0x05, m.request_id());

        EXPECT_EQ(sizeof(header), m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), header, m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin() + m.chunk_size());
    }

    {
        const midi::uint7_t chunk[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                        0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70 };

        auto sx = midi::ci::property_exchange::make_property_data_message(
          0x37, 0x1335577, 0xF159A12, {}, 16, 4, { chunk, sizeof(chunk) }, 0x0A);
        EXPECT_EQ(37u, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x37, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1335577u, m.source_muid());
        EXPECT_EQ(0xF159A12u, m.destination_muid());

        EXPECT_EQ(0x0A, m.request_id());

        EXPECT_EQ(0u, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(16u, m.number_of_chunks());
        EXPECT_EQ(4u, m.number_of_this_chunk());

        EXPECT_EQ(sizeof(chunk), m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin() + m.chunk_size());

        EXPECT_EQ(0, memcmp(m.chunk_begin(), chunk, m.chunk_size()));
    }

    {
        const midi::uint7_t chunk[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x10, 0x20,
                                        0x30, 0x40, 0x50, 0x60, 0x70, 0x77, 0x55, 0x33, 0x11, 0x19 };

        auto sx = midi::ci::property_exchange::make_property_data_message(
          0x3F, 0x1335577, 0xF159A12, {}, 256, 128, { chunk, sizeof(chunk) });
        EXPECT_EQ(42u, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x3F, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1335577u, m.source_muid());
        EXPECT_EQ(0xF159A12u, m.destination_muid());

        EXPECT_EQ(0x00, m.request_id());

        EXPECT_EQ(0u, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(256u, m.number_of_chunks());
        EXPECT_EQ(128u, m.number_of_this_chunk());

        EXPECT_EQ(sizeof(chunk), m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin() + m.chunk_size());

        EXPECT_EQ(0, memcmp(m.chunk_begin(), chunk, m.chunk_size()));
    }

    {
        auto sx = midi::ci::property_exchange::make_property_data_message(0x34, 0x1335577, 0xF159A12, {}, 0, 0, {});
        EXPECT_EQ(21u, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(0x34, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1335577u, m.source_muid());
        EXPECT_EQ(0xF159A12u, m.destination_muid());

        EXPECT_EQ(0x00, m.request_id());

        EXPECT_EQ(0u, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin());

        EXPECT_EQ(0u, m.number_of_chunks());
        EXPECT_EQ(0u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin());
    }

    {
        const midi::uint7_t property[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                           0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70 };

        auto sx = midi::ci::property_exchange::make_property_data_message(
          0x3C, 0x1133557, 0x3355711, { property, sizeof(property) }, 16, 4, {}, 0x0A);

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }
}

//-----------------------------------------------

TEST_F(ci_property_exchange, make_rjson)
{
    using namespace midi::ci::property_exchange;

    {
        auto rjson = make_rjson("a", "b");

        EXPECT_EQ(rjson, "{\"a\":\"b\"}");
    }

    {
        auto rjson = make_rjson("number", 5);

        EXPECT_EQ(rjson, "{\"number\":5}");
    }

    {
        auto rjson = make_rjson("resource", "ProgramList");

        EXPECT_EQ(rjson, "{\"resource\":\"ProgramList\"}");
    }

    {
        auto rjson = make_rjson(std::string{ "ssjjee" }, std::string{ "j37sjf" });

        EXPECT_EQ(rjson, "{\"ssjjee\":\"j37sjf\"}");
    }

    {
        auto rjson = make_rjson(tags::status, 12345);

        EXPECT_EQ(rjson, "{\"status\":12345}");
    }

    {
        const auto options = header_options{ { "a", "b" }, { "c", "d" }, { "00", "11" } };
        auto       rjson   = make_rjson(tags::resource, std::string{ "ProgramList" }, options);

        EXPECT_EQ(rjson, "{\"resource\":\"ProgramList\",\"a\":\"b\",\"c\":\"d\",\"00\":11}");
    }

    {
        const auto options = header_options{ { "delta", "-10" } };
        auto       rjson   = make_rjson(tags::resource, std::string{ "VendorResource" }, options);

        EXPECT_EQ(rjson, "{\"resource\":\"VendorResource\",\"delta\":-10}");
    }
}

//-----------------------------------------------

TEST_F(ci_property_exchange, get_property_data_inquiry)
{
    using VUT = midi::ci::get_property_data_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x34, 0x05, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x03, 0x08, 0x00,
                           0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::get_property_data_inquiry, m.subtype());

        EXPECT_EQ(5, m.message_version());
        EXPECT_EQ(0x24D2B78u, m.source_muid());
        EXPECT_EQ(0x22CEAF7u, m.destination_muid());

        EXPECT_EQ(0x03, m.request_id());

        EXPECT_EQ(8u, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), sx.data.data() + 15, m.header_size()));

        EXPECT_EQ(0u, m.number_of_chunks());
        EXPECT_EQ(0u, m.number_of_this_chunk());
    }

    // forward compatibility - accept larger message size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x34, 0x05, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33,
                           0x11, 0x03, 0x08, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x35, 0x00, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x03, 0x00, 0x08,
                           0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    {
        const char*  resource = "ResourceList";
        const char*  request  = "{\"resource\":\"ResourceList\"}";
        const size_t len      = strlen(request);

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_get_property_data_inquiry(0x1234567, 0x4332211, resource, 0x11, 0x0A);
        EXPECT_EQ(21u + len, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::get_property_data_inquiry, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(0x4332211u, m.destination_muid());

        EXPECT_EQ(0x11, m.request_id());

        EXPECT_EQ(len, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        std::string s{ m.header_begin(), m.header_end() };
        EXPECT_EQ(0, memcmp(m.header_begin(), request, m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        std::string resource{ "ProgramList" };
        std::string request{ "{\"resource\":\"ProgramList\"}" };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_get_property_data_inquiry(0x1135577, 0x4242, resource);
        EXPECT_EQ(21 + request.length(), sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::get_property_data_inquiry, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1135577u, m.source_muid());
        EXPECT_EQ(0x4242u, m.destination_muid());

        EXPECT_EQ(0x00, m.request_id());

        EXPECT_EQ(request.length(), m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        std::string s{ m.header_begin(), m.header_end() };
        EXPECT_EQ(0, memcmp(m.header_begin(), request.c_str(), m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(ci_property_exchange, get_property_data_reply)
{
    using VUT = midi::ci::get_property_data_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x35, 0x05, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x79,
                           0x08, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x00, 0x01,
                           0x00, 0x0A, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x1F, 0x2E } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::get_property_data_reply, m.subtype());

        EXPECT_EQ(5, m.message_version());
        EXPECT_EQ(0x24D2B78u, m.source_muid());
        EXPECT_EQ(0x22CEAF7u, m.destination_muid());

        EXPECT_EQ(0x79, m.request_id());

        EXPECT_EQ(8u, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), sx.data.data() + 15, m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        EXPECT_EQ(10u, m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin() + m.chunk_size());

        EXPECT_EQ(0, memcmp(m.chunk_begin(), sx.data.data() + 29, m.chunk_size()));
    }

    // forward compatibility - accept larger message size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x35, 0x05, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x79, 0x08,
                           0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x00, 0x01, 0x00, 0x0A,
                           0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x1F, 0x2E, 0x11 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x36, 0x00, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x79, 0x08, 0x00,
                           0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    {
        const char*         status     = "{\"status\":200}";
        const size_t        status_len = strlen(status);
        const midi::uint7_t chunk[]    = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx =
          midi::ci::make_get_property_data_reply(0x1234567, 0x1234568, 200, 1, 1, { chunk, sizeof(chunk) }, 12, 0x0A);
        EXPECT_EQ(21 + status_len + sizeof(chunk), sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::get_property_data_reply, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(0x1234568u, m.destination_muid());

        EXPECT_EQ(12, m.request_id());

        EXPECT_EQ(status_len, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), status, m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        EXPECT_EQ(sizeof(chunk), m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin() + m.chunk_size());

        EXPECT_EQ(0, memcmp(m.chunk_begin(), chunk, m.chunk_size()));

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const std::vector<midi::uint7_t> chunk{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x10, 0x20,
                                                0x30, 0x40, 0x50, 0x60, 0x70, 0x77, 0x55, 0x33, 0x11, 0x19 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_get_property_data_reply(0x1133577,
                                                         0xFFAABB0,
                                                         0,
                                                         128,
                                                         midi::ci::property_exchange::chunk{
                                                           chunk,
                                                         });
        EXPECT_EQ(21 + chunk.size(), sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::get_property_data_reply, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1133577u, m.source_muid());
        EXPECT_EQ(0xFFAABB0u, m.destination_muid());

        EXPECT_EQ(0x00, m.request_id());

        EXPECT_EQ(0u, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0u, m.number_of_chunks());
        EXPECT_EQ(128u, m.number_of_this_chunk());

        EXPECT_EQ(chunk.size(), m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin() + m.chunk_size());

        EXPECT_EQ(0, memcmp(m.chunk_begin(), chunk.data(), m.chunk_size()));

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const char*  status     = "{\"status\":404}";
        const size_t status_len = strlen(status);

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_get_property_data_reply(0x1133557, 7, 404, 0, 0, {}, 0);
        EXPECT_EQ(21 + status_len, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::get_property_data_reply, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1133557u, m.source_muid());
        EXPECT_EQ(7u, m.destination_muid());

        EXPECT_EQ(0x00, m.request_id());

        EXPECT_EQ(status_len, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + status_len);

        EXPECT_EQ(0, memcmp(m.header_begin(), status, m.header_size()));

        EXPECT_EQ(0u, m.number_of_chunks());
        EXPECT_EQ(0u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin() + m.chunk_size());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const char*  header     = "{\"status\":200,\"totalCount\":11}";
        const size_t header_len = strlen(header);

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_get_property_data_reply(
          0x1133557,
          7,
          midi::ci::property_exchange::header{
            midi::ci::property_exchange::make_rjson(
              "status", 200, midi::ci::property_exchange::header_options({ { "totalCount", "11" } })),
          },
          0,
          0,
          {},
          0);
        EXPECT_EQ(21 + header_len, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::get_property_data_reply, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1133557u, m.source_muid());
        EXPECT_EQ(7u, m.destination_muid());

        EXPECT_EQ(0x00, m.request_id());

        EXPECT_EQ(header_len, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + header_len);

        EXPECT_EQ(0, memcmp(m.header_begin(), header, m.header_size()));

        EXPECT_EQ(0u, m.number_of_chunks());
        EXPECT_EQ(0u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin() + m.chunk_size());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(ci_property_exchange, set_property_data_inquiry)
{
    using VUT = midi::ci::set_property_data_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x36, 0x05, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x7F, 0x08, 0x00,
                           0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::set_property_data_inquiry, m.subtype());

        EXPECT_EQ(5, m.message_version());
        EXPECT_EQ(0x24D2B78u, m.source_muid());
        EXPECT_EQ(0x22CEAF7u, m.destination_muid());

        EXPECT_EQ(0x7F, m.request_id());

        EXPECT_EQ(8u, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), sx.data.data() + 15, m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin() + m.chunk_size());
    }

    // forward compatibility - accept larger message size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x36, 0x05, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x7F,
                           0x08, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x00, 0x01,
                           0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x35, 0x00, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33,
                           0x11, 0x7F, 0x08, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    {
        const midi::uint7_t property[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
        const std::string   request{ "{\"resource\":\"rawData\"}" };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_set_property_data_inquiry(
          0x1234567, 0xAAAAAAA, "rawData", 1, 1, { property, sizeof(property) }, 9, 0x0A);
        EXPECT_EQ(21u + request.length() + sizeof(property), sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::set_property_data_inquiry, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(0xAAAAAAAu, m.destination_muid());

        EXPECT_EQ(0x09, m.request_id());

        EXPECT_EQ(request.length(), m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), request.c_str(), m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        EXPECT_EQ(sizeof(property), m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin() + m.chunk_size());

        EXPECT_EQ(0, memcmp(m.chunk_begin(), property, m.chunk_size()));

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const std::vector<midi::uint7_t> property{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x10, 0x20,
                                                   0x30, 0x40, 0x50, 0x60, 0x70, 0x77, 0x55, 0x33, 0x11, 0x19 };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_set_property_data_inquiry(0x1135577,
                                                           0x9674353,
                                                           256,
                                                           128,
                                                           midi::ci::property_exchange::chunk{
                                                             property,
                                                           });
        EXPECT_EQ(21 + property.size(), sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::set_property_data_inquiry, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1135577u, m.source_muid());
        EXPECT_EQ(0x9674353u, m.destination_muid());

        EXPECT_EQ(0x00, m.request_id());

        EXPECT_EQ(0u, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin());

        EXPECT_EQ(256u, m.number_of_chunks());
        EXPECT_EQ(128u, m.number_of_this_chunk());

        EXPECT_EQ(property.size(), m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin() + m.chunk_size());

        EXPECT_EQ(0, memcmp(m.chunk_begin(), property.data(), m.chunk_size()));

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_set_property_data_inquiry(0x1133577, 0x7487643, 0, 0, {});
        EXPECT_EQ(21u, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::set_property_data_inquiry, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1133577u, m.source_muid());
        EXPECT_EQ(0x7487643u, m.destination_muid());

        EXPECT_EQ(0x00, m.request_id());

        EXPECT_EQ(0u, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin());

        EXPECT_EQ(0u, m.number_of_chunks());
        EXPECT_EQ(0u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(ci_property_exchange, set_property_data_reply)
{
    using VUT = midi::ci::set_property_data_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x37, 0x05, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x05, 0x08, 0x00,
                           0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00 } };

        EXPECT_TRUE(midi::is_capability_inquiry_message(sx));

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::set_property_data_reply, m.subtype());

        EXPECT_EQ(5, m.message_version());
        EXPECT_EQ(0x24D2B78u, m.source_muid());
        EXPECT_EQ(0x22CEAF7u, m.destination_muid());

        EXPECT_EQ(0x05, m.request_id());

        EXPECT_EQ(8u, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), sx.data.data() + 15, m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin());
    }

    // forward compatibility - accept larger message size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x37, 0x05, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33,
                           0x11, 0x05, 0x08, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x77, 0x66 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x3F, 0x00, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x05, 0x08, 0x00,
                           0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    {
        const char*  status     = "{\"status\":200}";
        const size_t status_len = strlen(status);

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_set_property_data_reply(0x1234567, 0x3344556, 200, 5, 0x0A);
        EXPECT_EQ(21 + status_len, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::set_property_data_reply, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(0x3344556u, m.destination_muid());

        EXPECT_EQ(5, m.request_id());

        EXPECT_EQ(status_len, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), status, m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const char*  status     = "{\"status\":404,\"message\":\"Invalid Request.\"}";
        const size_t status_len = strlen(status);

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_set_property_data_reply(0x1135577, 0x2244668, 404, "Invalid Request.");
        EXPECT_EQ(21 + status_len, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::set_property_data_reply, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1135577u, m.source_muid());
        EXPECT_EQ(0x2244668u, m.destination_muid());

        EXPECT_EQ(0x00, m.request_id());

        EXPECT_EQ(status_len, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), status, m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(ci_property_exchange, subscription)
{
    using VUT = midi::ci::subscription_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x04, 0x0D, 0x38, 0x05, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x14, 0x08, 0x00,
                           0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x04, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::subscription_inquiry, m.subtype());

        EXPECT_EQ(5, m.message_version());
        EXPECT_EQ(0x24D2B78u, m.source_muid());
        EXPECT_EQ(0x22CEAF7u, m.destination_muid());

        EXPECT_EQ(0x14, m.request_id());

        EXPECT_EQ(8u, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), sx.data.data() + 15, m.header_size()));

        EXPECT_EQ(0u, m.number_of_chunks());
        EXPECT_EQ(0u, m.number_of_this_chunk());
    }

    // forward compatibility - accept larger message size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x04, 0x0D, 0x38, 0x05, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x14, 0x08, 0x00,
                           0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x39, 0x00, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x03, 0x00, 0x08,
                           0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    {
        const char*  resource = "ChannelList";
        const char*  command  = "start";
        const char*  subId    = "aabbcc34";
        const char*  request  = "{\"resource\":\"ChannelList\",\"command\":\"start\",\"subscribeId\":\"aabbcc34\"}";
        const size_t len      = strlen(request);

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_subscription_inquiry(0x1234567, 0x4332211, resource, command, subId, 0x11, 0x0A);
        EXPECT_EQ(21u + len, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::subscription_inquiry, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(0x4332211u, m.destination_muid());

        EXPECT_EQ(0x11, m.request_id());

        EXPECT_EQ(len, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        std::string s{ m.header_begin(), m.header_end() };
        EXPECT_EQ(0, memcmp(m.header_begin(), request, m.header_size()));

        EXPECT_EQ(0u, m.number_of_chunks());
        EXPECT_EQ(0u, m.number_of_this_chunk());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        std::string resource{ "ProgramList" };
        std::string command{ "full" };
        std::string subId{ "987xx" };
        std::string data{ "this is not valid JSON data" };
        std::string request{ "{\"resource\":\"ProgramList\",\"command\":\"full\",\"subscribeId\":\"987xx\"}" };

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_subscription_inquiry(0x1135577,
                                                      0x4242,
                                                      resource,
                                                      command,
                                                      subId,
                                                      5,
                                                      1,
                                                      midi::ci::property_exchange::chunk{
                                                        data,
                                                      });
        EXPECT_EQ(21 + request.length() + data.length(), sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::subscription_inquiry, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1135577u, m.source_muid());
        EXPECT_EQ(0x4242u, m.destination_muid());

        EXPECT_EQ(0x00, m.request_id());

        EXPECT_EQ(request.length(), m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        std::string s{ m.header_begin(), m.header_end() };
        EXPECT_EQ(0, memcmp(m.header_begin(), request.c_str(), m.header_size()));

        EXPECT_EQ(5u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(ci_property_exchange, subscription_reply)
{
    using VUT = midi::ci::subscription_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x39, 0x05, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x79, 0x08, 0x00,
                           0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::subscription_reply, m.subtype());

        EXPECT_EQ(5, m.message_version());
        EXPECT_EQ(0x24D2B78u, m.source_muid());
        EXPECT_EQ(0x22CEAF7u, m.destination_muid());

        EXPECT_EQ(0x79, m.request_id());

        EXPECT_EQ(8u, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), sx.data.data() + 15, m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());
    }

    // forward compatibility - accept larger message size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x39, 0x05, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x79,
                           0x08, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x00, 0x01,
                           0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x05, 0x0D, 0x36, 0x00, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x79, 0x08, 0x00,
                           0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    {
        const char*  status     = "{\"status\":200}";
        const size_t status_len = strlen(status);

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_subscription_reply(0x1234567, 0x1234568, 200, 12, 0x07);
        EXPECT_EQ(21 + status_len, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x07, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::subscription_reply, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(0x1234568u, m.destination_muid());

        EXPECT_EQ(12, m.request_id());

        EXPECT_EQ(status_len, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), status, m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const char*  status     = "{\"status\":404}";
        const size_t status_len = strlen(status);

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_subscription_reply(0x1133557, 7, 404, 0);
        EXPECT_EQ(21 + status_len, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::subscription_reply, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1133557u, m.source_muid());
        EXPECT_EQ(7u, m.destination_muid());

        EXPECT_EQ(0x00, m.request_id());

        EXPECT_EQ(status_len, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + status_len);

        EXPECT_EQ(0, memcmp(m.header_begin(), status, m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin() + m.chunk_size());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------

TEST_F(ci_property_exchange, notify)
{
    using VUT = midi::ci::notify_view;

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x3F, 0x05, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x79, 0x08, 0x00,
                           0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00 } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x0A, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::notify, m.subtype());

        EXPECT_EQ(5, m.message_version());
        EXPECT_EQ(0x24D2B78u, m.source_muid());
        EXPECT_EQ(0x22CEAF7u, m.destination_muid());

        EXPECT_EQ(0x79, m.request_id());

        EXPECT_EQ(8u, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), sx.data.data() + 15, m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());
    }

    // forward compatibility - accept larger message size
    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x0A, 0x0D, 0x3F, 0x05, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33,
                           0x11, 0x79, 0x08, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x7A, 0x6B } };

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));
    }

    {
        midi::sysex7 sx{ midi::manufacturer::universal_non_realtime,
                         { 0x05, 0x0D, 0x36, 0x00, 0x78, 0x56, 0x34, 0x12, 0x77, 0x55, 0x33, 0x11, 0x79, 0x08, 0x00,
                           0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00 } };

        EXPECT_FALSE(VUT::validate(sx));
        EXPECT_FALSE(midi::ci::as<VUT>(sx));
    }

    {
        const char*  status     = "{\"status\":100}";
        const size_t status_len = strlen(status);

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_notify_message(0x1234567, 0x1234568, 100, 12, 0x07);
        EXPECT_EQ(21 + status_len, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x07, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::notify, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1234567u, m.source_muid());
        EXPECT_EQ(0x1234568u, m.destination_muid());

        EXPECT_EQ(12, m.request_id());

        EXPECT_EQ(status_len, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + m.header_size());

        EXPECT_EQ(0, memcmp(m.header_begin(), status, m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }

    {
        const char*  status     = "{\"status\":408}";
        const size_t status_len = strlen(status);

        SYSEX_ALLOCATOR_CAPTURE_COUNT(c);

        auto sx = midi::ci::make_notify_message(0x1133557, 7, 408, 0);
        EXPECT_EQ(21 + status_len, sx.data.size());

        EXPECT_TRUE(VUT::validate(sx));
        EXPECT_TRUE(midi::ci::as<VUT>(sx));

        auto m = VUT{ sx };

        EXPECT_EQ(0x7F, m.device_id());
        EXPECT_EQ(midi::universal_sysex::type::capability_inquiry, m.type());
        EXPECT_EQ(midi::ci::subtype::notify, m.subtype());

        EXPECT_EQ(midi::ci::version, m.message_version());
        EXPECT_EQ(0x1133557u, m.source_muid());
        EXPECT_EQ(7u, m.destination_muid());

        EXPECT_EQ(0x00, m.request_id());

        EXPECT_EQ(status_len, m.header_size());
        EXPECT_EQ(m.header_end(), m.header_begin() + status_len);

        EXPECT_EQ(0, memcmp(m.header_begin(), status, m.header_size()));

        EXPECT_EQ(1u, m.number_of_chunks());
        EXPECT_EQ(1u, m.number_of_this_chunk());

        EXPECT_EQ(0u, m.chunk_size());
        EXPECT_EQ(m.chunk_end(), m.chunk_begin() + m.chunk_size());

        SYSEX_ALLOCATOR_VERIFY_DIFF(c, 1);
    }
}

//-----------------------------------------------
