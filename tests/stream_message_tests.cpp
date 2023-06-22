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

#include <midi/stream_message.h>
#include <midi/sysex.h> // manufacturer

//-----------------------------------------------

class stream_message : public ::testing::Test
{
  public:
};

//-----------------------------------------------

TEST_F(stream_message, constructors)
{
    using namespace midi;

    {
        constexpr midi::stream_message m;

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0000000u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::endpoint_discovery, m.status());
    }

    {
        const midi::stream_message m{ stream_status::endpoint_info };

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0010000u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::endpoint_info, m.status());
    }

    {
        constexpr midi::stream_message m{ stream_status::function_block_name, packet_format::cont };

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF8120000u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::cont, m.format());
        EXPECT_EQ(stream_status::function_block_name, m.status());
    }

    {
        const midi::stream_message m{ stream_status::product_instance_id, packet_format::start };

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF4040000u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::start, m.format());
        EXPECT_EQ(stream_status::product_instance_id, m.status());
    }

    {
        constexpr midi::stream_message m{ stream_status::endpoint_name, packet_format::end };

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xFC030000u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::end, m.format());
        EXPECT_EQ(stream_status::endpoint_name, m.status());
    }
}

//-----------------------------------------------

TEST_F(stream_message, set_format)
{
    using namespace midi;

    {
        midi::stream_message m;
        m.set_format(packet_format::end);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xFC000000u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::end, m.format());
        EXPECT_EQ(stream_status::endpoint_discovery, m.status());
    }
}

//-----------------------------------------------

TEST_F(stream_message, endpoint_discovery)
{
    using namespace midi;

    {
        constexpr midi::stream_message m = make_endpoint_discovery_message(0x0F);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0000101u, m.data[0]);
        EXPECT_EQ(0x0000000Fu, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::endpoint_discovery, m.status());

        EXPECT_TRUE(as_endpoint_discovery_view(m));

        auto v = endpoint_discovery_view{ m };
        EXPECT_EQ(1u, v.ump_version_major());
        EXPECT_EQ(1u, v.ump_version_minor());
        EXPECT_EQ(0x0101u, v.ump_version());
        EXPECT_EQ(0x0Fu, v.filter());
    }

    {
        const midi::stream_message m = make_endpoint_discovery_message(0x04, 2, 3);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0000203u, m.data[0]);
        EXPECT_EQ(0x00000004u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::endpoint_discovery, m.status());

        EXPECT_TRUE(as_endpoint_discovery_view(m));

        auto v = endpoint_discovery_view(m);
        EXPECT_EQ(2u, v.ump_version_major());
        EXPECT_EQ(3u, v.ump_version_minor());
        EXPECT_EQ(0x0203u, v.ump_version());
        EXPECT_EQ(0x04u, v.filter());
    }
}

//-----------------------------------------------

TEST_F(stream_message, endpoint_discovery_view)
{
    using namespace midi;

    {
        static constexpr midi::stream_message m = make_endpoint_discovery_message(0x04, 2, 3);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_endpoint_discovery_view(m));

        constexpr auto v = endpoint_discovery_view(m);
        EXPECT_EQ(2u, v.ump_version_major());
        EXPECT_EQ(3u, v.ump_version_minor());
        EXPECT_EQ(0x0203u, v.ump_version());
        EXPECT_EQ(0x04u, v.filter());
    }

    {
        static const midi::stream_message m = make_endpoint_discovery_message(0x03);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_endpoint_discovery_view(m));

        const auto v = endpoint_discovery_view(m);
        EXPECT_EQ(1u, v.ump_version_major());
        EXPECT_EQ(1u, v.ump_version_minor());
        EXPECT_EQ(0x0101u, v.ump_version());
        EXPECT_EQ(0x03u, v.filter());
    }

    {
        EXPECT_FALSE(as_endpoint_discovery_view(make_endpoint_info_message(3, false, 0x3, 0x0)));
    }
}

//-----------------------------------------------

TEST_F(stream_message, endpoint_info)
{
    using namespace midi;

    {
        constexpr midi::stream_message m = make_endpoint_info_message(3, false, 0x3, 0x0);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0010101u, m.data[0]);
        EXPECT_EQ(0x03000300u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::endpoint_info, m.status());

        EXPECT_TRUE(as_endpoint_info_view(m));

        auto v = endpoint_info_view{ m };
        EXPECT_EQ(1u, v.ump_version_major());
        EXPECT_EQ(1u, v.ump_version_minor());
        EXPECT_EQ(0x0101u, v.ump_version());
        EXPECT_EQ(3u, v.num_function_blocks());
        EXPECT_FALSE(v.static_function_blocks());
        EXPECT_EQ(0x3u, v.protocols());
        EXPECT_EQ(0x0u, v.extensions());
    }

    {
        const midi::stream_message m = make_endpoint_info_message(1, true, 0x1, 0x2, 2, 3);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0010203u, m.data[0]);
        EXPECT_EQ(0x81000102u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::endpoint_info, m.status());

        EXPECT_TRUE(as_endpoint_info_view(m));

        auto v = endpoint_info_view{ m };
        EXPECT_EQ(2u, v.ump_version_major());
        EXPECT_EQ(3u, v.ump_version_minor());
        EXPECT_EQ(0x0203u, v.ump_version());
        EXPECT_EQ(1u, v.num_function_blocks());
        EXPECT_TRUE(v.static_function_blocks());
        EXPECT_EQ(1u, v.protocols());
        EXPECT_EQ(2u, v.extensions());
    }

    {
        const midi::stream_message m = make_endpoint_info_message(0, false, 0x2, 0x1);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0010101u, m.data[0]);
        EXPECT_EQ(0x00000201u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::endpoint_info, m.status());

        EXPECT_TRUE(as_endpoint_info_view(m));

        auto v = endpoint_info_view{ m };
        EXPECT_EQ(1u, v.ump_version_major());
        EXPECT_EQ(1u, v.ump_version_minor());
        EXPECT_EQ(0x0101u, v.ump_version());
        EXPECT_EQ(0u, v.num_function_blocks());
        EXPECT_FALSE(v.static_function_blocks());
        EXPECT_EQ(2u, v.protocols());
        EXPECT_EQ(1u, v.extensions());
    }
}

//-----------------------------------------------

TEST_F(stream_message, endpoint_info_view)
{
    using namespace midi;

    {
        static constexpr midi::stream_message m = make_endpoint_info_message(0, false, 0x2, 0x1);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_endpoint_info_view(m));

        constexpr auto v = endpoint_info_view{ m };
        EXPECT_EQ(1u, v.ump_version_major());
        EXPECT_EQ(1u, v.ump_version_minor());
        EXPECT_EQ(0x0101u, v.ump_version());
        EXPECT_EQ(0u, v.num_function_blocks());
        EXPECT_FALSE(v.static_function_blocks());
        EXPECT_EQ(2u, v.protocols());
        EXPECT_EQ(1u, v.extensions());
    }

    {
        static const midi::stream_message m = make_endpoint_info_message(4, true, 0x3, 0x3, 0x01, 0x02);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_endpoint_info_view(m));

        const auto v = endpoint_info_view{ m };
        EXPECT_EQ(1u, v.ump_version_major());
        EXPECT_EQ(2u, v.ump_version_minor());
        EXPECT_EQ(0x0102u, v.ump_version());
        EXPECT_EQ(4u, v.num_function_blocks());
        EXPECT_TRUE(v.static_function_blocks());
        EXPECT_EQ(3u, v.protocols());
        EXPECT_EQ(3u, v.extensions());
    }

    {
        EXPECT_FALSE(as_endpoint_info_view(make_endpoint_discovery_message(0x03)));
    }
}

//-----------------------------------------------

TEST_F(stream_message, device_identity)
{
    using namespace midi;

    {
        constexpr device_identity      identity{ midi::manufacturer::native_instruments, 0x1730, 49, 0x00010005 };
        constexpr midi::stream_message m = make_device_identity_message(identity);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0020000u, m.data[0]);
        EXPECT_EQ(0x00002109u, m.data[1]);
        EXPECT_EQ(0x17300031u, m.data[2]);
        EXPECT_EQ(0x00010005u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::device_identity, m.status());

        EXPECT_TRUE(as_device_identity_view(m));

        auto       v          = device_identity_view{ m };
        const auto v_identity = v.identity();
        EXPECT_EQ(identity.manufacturer, v_identity.manufacturer);
        EXPECT_EQ(identity.family, v_identity.family);
        EXPECT_EQ(identity.model, v_identity.model);
        EXPECT_EQ(identity.revision, v_identity.revision);
    }

    {
        constexpr device_identity  identity{ midi::manufacturer::native_instruments, 0x2200, 61, 0x00020006 };
        const midi::stream_message m = make_device_identity_message(identity);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0020000u, m.data[0]);
        EXPECT_EQ(0x00002109u, m.data[1]);
        EXPECT_EQ(0x2200003Du, m.data[2]);
        EXPECT_EQ(0x00020006u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::device_identity, m.status());

        EXPECT_TRUE(as_device_identity_view(m));

        auto       v          = device_identity_view{ m };
        const auto v_identity = v.identity();
        EXPECT_EQ(identity.manufacturer, v_identity.manufacturer);
        EXPECT_EQ(identity.family, v_identity.family);
        EXPECT_EQ(identity.model, v_identity.model);
        EXPECT_EQ(identity.revision, v_identity.revision);
    }
}

//-----------------------------------------------

TEST_F(stream_message, device_identity_view)
{
    using namespace midi;

    {
        static constexpr device_identity identity{ midi::manufacturer::native_instruments, 0x1730, 49, 0x00010005 };
        static constexpr midi::stream_message m = make_device_identity_message(identity);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_device_identity_view(m));

        constexpr auto v          = device_identity_view{ m };
        constexpr auto v_identity = v.identity();
        EXPECT_EQ(identity.manufacturer, v_identity.manufacturer);
        EXPECT_EQ(identity.family, v_identity.family);
        EXPECT_EQ(identity.model, v_identity.model);
        EXPECT_EQ(identity.revision, v_identity.revision);
    }

    {
        EXPECT_FALSE(as_device_identity_view(make_endpoint_discovery_message(0x03)));
    }
}

//-----------------------------------------------

TEST_F(stream_message, endpoint_name)
{
    using namespace midi;

    {
        constexpr midi::stream_message m = make_endpoint_name_message(packet_format::complete, "A short name");

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0030000u + ('A' << 8) + ' ', m.data[0]);
        EXPECT_EQ(0u + ('s' << 24) + ('h' << 16) + ('o' << 8) + 'r', m.data[1]);
        EXPECT_EQ(0u + ('t' << 24) + (' ' << 16) + ('n' << 8) + 'a', m.data[2]);
        EXPECT_EQ(0u + ('m' << 24) + ('e' << 16), m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::endpoint_name, m.status());

        EXPECT_TRUE(as_endpoint_name_view(m));

        auto v = endpoint_name_view{ m };
        EXPECT_EQ(packet_format::complete, v.format());
        EXPECT_EQ(v.payload(), "A short name");
    }

    //  1             2             3             4
    // "A very long device name in three packets"
    {
        const midi::stream_message m = make_endpoint_name_message(packet_format::start, "A very long de");

        EXPECT_EQ(0xF4030000u + ('A' << 8) + ' ', m.data[0]);
        EXPECT_EQ(0u + ('v' << 24) + ('e' << 16) + ('r' << 8) + 'y', m.data[1]);
        EXPECT_EQ(0u + (' ' << 24) + ('l' << 16) + ('o' << 8) + 'n', m.data[2]);
        EXPECT_EQ(0u + ('g' << 24) + (' ' << 16) + ('d' << 8) + 'e', m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::start, m.format());
        EXPECT_EQ(stream_status::endpoint_name, m.status());

        EXPECT_TRUE(as_endpoint_name_view(m));

        auto v = endpoint_name_view{ m };
        EXPECT_EQ(packet_format::start, v.format());
        EXPECT_EQ(v.payload(), "A very long de");
    }

    {
        constexpr midi::stream_message m = make_endpoint_name_message(packet_format::cont, "vice name in t");

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF8030000u + ('v' << 8) + 'i', m.data[0]);
        EXPECT_EQ(0u + ('c' << 24) + ('e' << 16) + (' ' << 8) + 'n', m.data[1]);
        EXPECT_EQ(0u + ('a' << 24) + ('m' << 16) + ('e' << 8) + ' ', m.data[2]);
        EXPECT_EQ(0u + ('i' << 24) + ('n' << 16) + (' ' << 8) + 't', m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::cont, m.format());
        EXPECT_EQ(stream_status::endpoint_name, m.status());

        EXPECT_TRUE(as_endpoint_name_view(m));

        auto v = endpoint_name_view{ m };
        EXPECT_EQ(packet_format::cont, v.format());
        EXPECT_EQ(v.payload(), "vice name in t");
    }

    {
        const midi::stream_message m = make_endpoint_name_message(packet_format::end, "hree packets");

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xFC030000u + ('h' << 8) + 'r', m.data[0]);
        EXPECT_EQ(0u + ('e' << 24) + ('e' << 16) + (' ' << 8) + 'p', m.data[1]);
        EXPECT_EQ(0u + ('a' << 24) + ('c' << 16) + ('k' << 8) + 'e', m.data[2]);
        EXPECT_EQ(0u + ('t' << 24) + ('s' << 16), m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::end, m.format());
        EXPECT_EQ(stream_status::endpoint_name, m.status());

        EXPECT_TRUE(as_endpoint_name_view(m));

        auto v = endpoint_name_view{ m };
        EXPECT_EQ(packet_format::end, v.format());
        EXPECT_EQ(v.payload(), "hree packets");
    }
}

//-----------------------------------------------

TEST_F(stream_message, endpoint_name_view)
{
    using namespace midi;

    {
        static constexpr midi::stream_message m = make_endpoint_name_message(packet_format::cont, "vice name in t");

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_endpoint_name_view(m));

        constexpr auto v = endpoint_name_view{ m };
        EXPECT_EQ(packet_format::cont, v.format());
        EXPECT_EQ(v.payload(), "vice name in t");
    }

    {
        static const midi::stream_message m = make_endpoint_name_message(packet_format::complete, "A Name");

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_endpoint_name_view(m));

        const auto v = endpoint_name_view{ m };
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(v.payload(), "A Name");
    }

    {
        EXPECT_FALSE(as_endpoint_name_view(make_endpoint_info_message(4, true, 0x3, 0x3, 0x01, 0x02)));
    }
}

//-----------------------------------------------

TEST_F(stream_message, send_endpoint_name)
{
    using namespace midi;

    auto run_test = [](std::string_view name) {
        size_t      number_of_packets = 0;
        std::string resulting_name;
        send_endpoint_name(name, [&](const midi::universal_packet& p) {
            auto msg = as_endpoint_name_view(p);
            EXPECT_TRUE(msg);
            if (msg)
            {
                ++number_of_packets;
                resulting_name.append(msg->payload());
            }
        });

        EXPECT_EQ(resulting_name, name);
        return number_of_packets;
    };

    EXPECT_EQ(1u, run_test("short name"));
    EXPECT_EQ(2u, run_test("A reasonably long name"));
    EXPECT_EQ(3u, run_test("A pretty long name that needs 3 packets"));
    EXPECT_EQ(4u, run_test("An even longer name that needs four packets"));
}

//-----------------------------------------------

TEST_F(stream_message, product_instance_id)
{
    using namespace midi;

    {
        constexpr midi::stream_message m = make_product_instance_id_message(packet_format::complete, "14AD4C5HE5EA9F");

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0040000u + ('1' << 8) + '4', m.data[0]);
        EXPECT_EQ(0u + ('A' << 24) + ('D' << 16) + ('4' << 8) + 'C', m.data[1]);
        EXPECT_EQ(0u + ('5' << 24) + ('H' << 16) + ('E' << 8) + '5', m.data[2]);
        EXPECT_EQ(0u + ('E' << 24) + ('A' << 16) + ('9' << 8) + 'F', m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::product_instance_id, m.status());

        EXPECT_TRUE(as_product_instance_id_view(m));

        auto v = product_instance_id_view{ m };
        EXPECT_EQ(packet_format::complete, v.format());
        EXPECT_EQ(v.payload(), "14AD4C5HE5EA9F");
    }

    {
        const midi::stream_message m = make_product_instance_id_message(packet_format::complete, "14AD4C5HE5EA9F");

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0040000u + ('1' << 8) + '4', m.data[0]);
        EXPECT_EQ(0u + ('A' << 24) + ('D' << 16) + ('4' << 8) + 'C', m.data[1]);
        EXPECT_EQ(0u + ('5' << 24) + ('H' << 16) + ('E' << 8) + '5', m.data[2]);
        EXPECT_EQ(0u + ('E' << 24) + ('A' << 16) + ('9' << 8) + 'F', m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::product_instance_id, m.status());
    }
}

//-----------------------------------------------

TEST_F(stream_message, product_instance_id_view)
{
    using namespace midi;

    {
        static constexpr midi::stream_message m =
          make_product_instance_id_message(packet_format::complete, "14AD4C5HE5EA9F");

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_product_instance_id_view(m));

        constexpr auto v = product_instance_id_view{ m };
        EXPECT_EQ(packet_format::complete, v.format());
        EXPECT_EQ(v.payload(), "14AD4C5HE5EA9F");
    }

    {
        static const midi::stream_message m = make_product_instance_id_message(packet_format::complete, "ABCDE");

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_product_instance_id_view(m));

        const auto v = product_instance_id_view{ m };
        EXPECT_EQ(packet_format::complete, v.format());
        EXPECT_EQ(v.payload(), "ABCDE");
    }

    {
        EXPECT_FALSE(as_product_instance_id_view(make_endpoint_name_message(packet_format::cont, "vice name in t")));
    }
}

//-----------------------------------------------

TEST_F(stream_message, send_product_instance_id)
{
    using namespace midi;

    auto run_test = [](std::string_view pid) {
        size_t      number_of_packets = 0;
        std::string resulting_pid;
        send_product_instance_id(pid, [&](const midi::universal_packet& p) {
            auto msg = as_product_instance_id_view(p);
            EXPECT_TRUE(msg);
            if (msg)
            {
                ++number_of_packets;
                resulting_pid.append(msg->payload());
            }
        });

        EXPECT_EQ(resulting_pid, pid);
        return number_of_packets;
    };

    EXPECT_EQ(1u, run_test("ABCDE"));
    EXPECT_EQ(1u, run_test("14AD4C5HE5EA9F"));
    EXPECT_EQ(2u, run_test("14AD4C5HE5EA9F0"));
    EXPECT_EQ(2u, run_test("14AD4C5HE5EA9F01"));
}

//-----------------------------------------------

TEST_F(stream_message, stream_configuration_request)
{
    using namespace midi;

    {
        constexpr midi::stream_message m = make_stream_configuration_request(protocol::midi2);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0050200u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(stream_status::stream_configuration_request, m.status());

        EXPECT_TRUE(as_stream_configuration_view(m));

        auto v = stream_configuration_view{ m };
        EXPECT_EQ(protocol::midi2, v.protocol());
        EXPECT_EQ(0u, v.extensions());
    }

    {
        const midi::stream_message m =
          make_stream_configuration_request(protocol::midi1, extensions::jitter_reduction_receive);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0050102u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(stream_status::stream_configuration_request, m.status());

        EXPECT_TRUE(as_stream_configuration_view(m));

        auto v = stream_configuration_view{ m };
        EXPECT_EQ(protocol::midi1, v.protocol());
        EXPECT_EQ(extensions::jitter_reduction_receive, v.extensions());
    }

    {
        constexpr midi::stream_message m = make_stream_configuration_request(
          protocol::midi2, extensions::jitter_reduction_transmit + extensions::jitter_reduction_receive);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0050203u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(stream_status::stream_configuration_request, m.status());

        EXPECT_TRUE(as_stream_configuration_view(m));

        auto v = stream_configuration_view{ m };
        EXPECT_EQ(protocol::midi2, v.protocol());
        EXPECT_EQ(extensions::jitter_reduction_transmit + extensions::jitter_reduction_receive, v.extensions());
    }

    {
        EXPECT_FALSE(as_endpoint_info_view(make_endpoint_name_message(packet_format::cont, "vice name in t")));
    }
}

//-----------------------------------------------

TEST_F(stream_message, stream_configuration_request_view)
{
    using namespace midi;

    {
        static constexpr midi::stream_message m =
          make_stream_configuration_request(protocol::midi1, extensions::jitter_reduction_receive);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_stream_configuration_view(m));

        constexpr auto v = stream_configuration_view{ m };
        EXPECT_EQ(protocol::midi1, v.protocol());
        EXPECT_EQ(extensions::jitter_reduction_receive, v.extensions());
    }

    {
        static const midi::stream_message m =
          make_stream_configuration_request(protocol::midi2, extensions::jitter_reduction_transmit);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_stream_configuration_view(m));

        const auto v = stream_configuration_view{ m };
        EXPECT_EQ(protocol::midi2, v.protocol());
        EXPECT_EQ(extensions::jitter_reduction_transmit, v.extensions());
    }

    {
        EXPECT_FALSE(as_stream_configuration_view(make_endpoint_name_message(packet_format::cont, "vice name in t")));
    }
}

//-----------------------------------------------

TEST_F(stream_message, make_stream_configuration_notification)
{
    using namespace midi;

    {
        constexpr midi::stream_message m = make_stream_configuration_notification(protocol::midi1);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0060100u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(stream_status::stream_configuration_notify, m.status());

        EXPECT_TRUE(as_stream_configuration_view(m));

        auto v = stream_configuration_view{ m };
        EXPECT_EQ(protocol::midi1, v.protocol());
        EXPECT_EQ(0u, v.extensions());
    }

    {
        const midi::stream_message m =
          make_stream_configuration_notification(protocol::midi2, extensions::jitter_reduction_transmit);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0060201u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(stream_status::stream_configuration_notify, m.status());

        EXPECT_TRUE(as_stream_configuration_view(m));

        auto v = stream_configuration_view{ m };
        EXPECT_EQ(protocol::midi2, v.protocol());
        EXPECT_EQ(extensions::jitter_reduction_transmit, v.extensions());
    }
}

//-----------------------------------------------

TEST_F(stream_message, make_stream_configuration_notification_view)
{
    using namespace midi;

    {
        static constexpr midi::stream_message m =
          make_stream_configuration_notification(protocol::midi2, extensions::jitter_reduction_transmit);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_stream_configuration_view(m));

        constexpr auto v = stream_configuration_view{ m };
        EXPECT_EQ(protocol::midi2, v.protocol());
        EXPECT_EQ(extensions::jitter_reduction_transmit, v.extensions());
    }

    {
        static const midi::stream_message m = make_stream_configuration_notification(protocol::midi1);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_stream_configuration_view(m));

        const auto v = stream_configuration_view{ m };
        EXPECT_EQ(protocol::midi1, v.protocol());
        EXPECT_EQ(0u, v.extensions());
    }

    {
        EXPECT_FALSE(as_stream_configuration_view(universal_packet(0x41000000)));
    }
}

//-----------------------------------------------

TEST_F(stream_message, function_block_discovery)
{
    using namespace midi;

    {
        constexpr midi::stream_message m = make_function_block_discovery_message(0xFF, 0x3);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF010FF03u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::function_block_discovery, m.status());

        EXPECT_TRUE(as_function_block_discovery_view(m));

        auto v = function_block_discovery_view{ m };
        EXPECT_EQ(0xFFu, v.function_block());
        EXPECT_EQ(3u, v.filter());
    }

    {
        const midi::stream_message m = make_function_block_discovery_message(5, 0x2);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0100502u, m.data[0]);
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::function_block_discovery, m.status());

        EXPECT_TRUE(as_function_block_discovery_view(m));

        auto v = function_block_discovery_view{ m };
        EXPECT_EQ(5, v.function_block());
        EXPECT_EQ(2u, v.filter());
    }
}

//-----------------------------------------------

TEST_F(stream_message, function_block_discovery_view)
{
    using namespace midi;

    {
        static constexpr midi::stream_message m = make_function_block_discovery_message(5, 0x2);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_function_block_discovery_view(m));

        constexpr auto v = function_block_discovery_view{ m };
        EXPECT_EQ(5, v.function_block());
        EXPECT_EQ(2u, v.filter());
    }

    {
        static const midi::stream_message m = make_function_block_discovery_message(0xE, 4);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_function_block_discovery_view(m));

        const auto v = function_block_discovery_view{ m };
        EXPECT_EQ(0xE, v.function_block());
        EXPECT_EQ(4u, v.filter());
    }

    {
        EXPECT_FALSE(as_function_block_discovery_view(make_stream_configuration_request(protocol::midi2)));
    }
}

//-----------------------------------------------

TEST_F(stream_message, function_block_info)
{
    using namespace midi;

    {
        constexpr midi::stream_message m = make_function_block_info_message(1, 3, 4, 5);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0118133u, m.data[0]);
        EXPECT_EQ(0x04050000u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::function_block_info, m.status());

        EXPECT_TRUE(as_function_block_info_view(m));

        auto v = function_block_info_view{ m };
        EXPECT_TRUE(v.active());
        EXPECT_EQ(1u, v.function_block());
        EXPECT_EQ(0b11, v.direction());
        EXPECT_EQ(0u, v.midi1());
        EXPECT_EQ(0b11, v.ui_hint());
        EXPECT_EQ(4u, v.first_group());
        EXPECT_EQ(5u, v.num_groups_spanned());
        EXPECT_EQ(0u, v.ci_message_version());
        EXPECT_EQ(0u, v.max_num_sysex8_streams());
    }

    {
        const midi::stream_message m = make_function_block_info_message(8, 1, 6, 4);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0118811u, m.data[0]);
        EXPECT_EQ(0x06040000u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::function_block_info, m.status());

        EXPECT_TRUE(as_function_block_info_view(m));

        auto v = function_block_info_view{ m };
        EXPECT_TRUE(v.active());
        EXPECT_EQ(8u, v.function_block());
        EXPECT_EQ(0b01, v.direction());
        EXPECT_EQ(0u, v.midi1());
        EXPECT_EQ(0b01, v.ui_hint());
        EXPECT_EQ(6u, v.first_group());
        EXPECT_EQ(4u, v.num_groups_spanned());
        EXPECT_EQ(0u, v.ci_message_version());
        EXPECT_EQ(0u, v.max_num_sysex8_streams());
    }

    {
        constexpr function_block_options options = {
            true,                                     // active
            function_block_options::direction_input,  // direction
            function_block_options::midi1_31250,      // midi1
            function_block_options::ui_hint_receiver, // ui_hint
            0x02,                                     // ci_message_version
            4                                         // max_num_sysex8_streams
        };
        constexpr midi::stream_message m = make_function_block_info_message(4, options, 3, 1);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0118419u, m.data[0]);
        EXPECT_EQ(0x03010204u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::function_block_info, m.status());

        EXPECT_TRUE(as_function_block_info_view(m));

        auto v = function_block_info_view{ m };
        EXPECT_TRUE(v.active());
        EXPECT_EQ(4u, v.function_block());
        EXPECT_EQ(0b01, v.direction());
        EXPECT_EQ(2u, v.midi1());
        EXPECT_EQ(3u, v.first_group());
        EXPECT_EQ(1u, v.num_groups_spanned());
        EXPECT_EQ(0x02u, v.ci_message_version());
        EXPECT_EQ(4u, v.max_num_sysex8_streams());
    }

    {
        constexpr function_block_options options = {
            false,                                        // active
            function_block_options::direction_output,     // direction
            function_block_options::midi1_unrestricted,   // midi1
            function_block_options::ui_hint_as_direction, // ui_hint
            0x01,                                         // ci_message_version
            2                                             // max_num_sysex8_streams
        };
        const midi::stream_message m = make_function_block_info_message(7, options, 8, 3);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0110726u, m.data[0]);
        EXPECT_EQ(0x08030102u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::function_block_info, m.status());

        EXPECT_TRUE(as_function_block_info_view(m));

        auto v = function_block_info_view{ m };
        EXPECT_FALSE(v.active());
        EXPECT_EQ(7u, v.function_block());
        EXPECT_EQ(0b10, v.direction());
        EXPECT_EQ(1u, v.midi1());
        EXPECT_EQ(8u, v.first_group());
        EXPECT_EQ(3u, v.num_groups_spanned());
        EXPECT_EQ(0x01u, v.ci_message_version());
        EXPECT_EQ(2u, v.max_num_sysex8_streams());
    }

    {
        constexpr function_block_options options = {
            true,                                   // active
            function_block_options::bidirectional,  // direction
            function_block_options::not_midi1,      // midi1
            function_block_options::ui_hint_sender, // ui_hint
            0x03,                                   // ci_message_version
            0                                       // max_num_sysex8_streams
        };
        constexpr midi::stream_message m = make_function_block_info_message(31, options, 0xE, 2);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0119F23u, m.data[0]);
        EXPECT_EQ(0x0E020300u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::function_block_info, m.status());

        EXPECT_TRUE(as_function_block_info_view(m));

        auto v = function_block_info_view{ m };
        EXPECT_TRUE(v.active());
        EXPECT_EQ(31u, v.function_block());
        EXPECT_EQ(0b11, v.direction());
        EXPECT_EQ(0u, v.midi1());
        EXPECT_EQ(0xEu, v.first_group());
        EXPECT_EQ(2u, v.num_groups_spanned());
        EXPECT_EQ(0x03u, v.ci_message_version());
        EXPECT_EQ(0u, v.max_num_sysex8_streams());
    }
}

//-----------------------------------------------

TEST_F(stream_message, function_block_info_view)
{
    using namespace midi;

    {
        static constexpr function_block_options options = {
            false,                                        // active
            function_block_options::direction_output,     // direction
            function_block_options::midi1_unrestricted,   // midi1
            function_block_options::ui_hint_as_direction, // ui_hint
            0x01,                                         // ci_message_version
            2                                             // max_num_sysex8_streams
        };
        static constexpr midi::stream_message m = make_function_block_info_message(7, options, 8, 3);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_function_block_info_view(m));

        constexpr auto v = function_block_info_view{ m };
        EXPECT_FALSE(v.active());
        EXPECT_EQ(7u, v.function_block());
        EXPECT_EQ(0b10, v.direction());
        EXPECT_EQ(1u, v.midi1());
        EXPECT_EQ(8u, v.first_group());
        EXPECT_EQ(3u, v.num_groups_spanned());
        EXPECT_EQ(0x01u, v.ci_message_version());
        EXPECT_EQ(2u, v.max_num_sysex8_streams());
    }

    {
        static constexpr function_block_options options = {
            true,                                         // active
            function_block_options::direction_input,      // direction
            function_block_options::midi1_31250,          // midi1
            function_block_options::ui_hint_as_direction, // ui_hint
            0x02,                                         // ci_message_version
            4                                             // max_num_sysex8_streams
        };
        static const midi::stream_message m = make_function_block_info_message(4, options, 0xE, 2);

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_function_block_info_view(m));

        const auto v = function_block_info_view{ m };
        EXPECT_TRUE(v.active());
        EXPECT_EQ(4u, v.function_block());
        EXPECT_EQ(0b01, v.direction());
        EXPECT_EQ(2u, v.midi1());
        EXPECT_EQ(0xEu, v.first_group());
        EXPECT_EQ(2u, v.num_groups_spanned());
        EXPECT_EQ(0x02u, v.ci_message_version());
        EXPECT_EQ(4u, v.max_num_sysex8_streams());
    }

    {
        EXPECT_FALSE(as_function_block_info_view(make_stream_configuration_request(protocol::midi2)));
    }
}

//-----------------------------------------------

TEST_F(stream_message, function_block_name)
{
    using namespace midi;

    {
        constexpr midi::stream_message m = make_function_block_name_message(packet_format::complete, 5, "Main");

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF0120500u + 'M', m.data[0]);
        EXPECT_EQ(0u + ('a' << 24) + ('i' << 16) + ('n' << 8), m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::complete, m.format());
        EXPECT_EQ(stream_status::function_block_name, m.status());

        EXPECT_TRUE(as_function_block_name_view(m));

        auto v = function_block_name_view{ m };
        EXPECT_EQ(v.payload(), "Main");
    }

    {
        const midi::stream_message m = make_function_block_name_message(packet_format::start, 0xA, "Start of Name");

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_EQ(0xF4120A00u + 'S', m.data[0]);
        EXPECT_EQ(0u + ('t' << 24) + ('a' << 16) + ('r' << 8) + 't', m.data[1]);
        EXPECT_EQ(0u + (' ' << 24) + ('o' << 16) + ('f' << 8) + ' ', m.data[2]);
        EXPECT_EQ(0u + ('N' << 24) + ('a' << 16) + ('m' << 8) + 'e', m.data[3]);

        EXPECT_EQ(4u, m.size());
        EXPECT_EQ(packet_type::stream, m.type());
        EXPECT_EQ(packet_format::start, m.format());
        EXPECT_EQ(stream_status::function_block_name, m.status());

        EXPECT_TRUE(as_function_block_name_view(m));

        auto v = function_block_name_view{ m };
        EXPECT_EQ(v.payload(), "Start of Name");
    }
}

//-----------------------------------------------

TEST_F(stream_message, function_block_name_view)
{
    using namespace midi;

    {
        static constexpr midi::stream_message m = make_function_block_name_message(packet_format::complete, 5, "Main");

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_function_block_name_view(m));

        constexpr auto v = function_block_name_view{ m };
        EXPECT_EQ(packet_format::complete, v.format());
        EXPECT_EQ(5u, v.function_block());
        EXPECT_EQ(v.payload(), "Main");
    }

    {
        const midi::stream_message m = make_function_block_name_message(packet_format::start, 0xA, "Start of Name");

        EXPECT_TRUE(is_stream_message(m));

        EXPECT_TRUE(as_function_block_name_view(m));

        const auto v = function_block_name_view{ m };
        EXPECT_EQ(packet_format::start, v.format());
        EXPECT_EQ(0xAu, v.function_block());
        EXPECT_EQ(v.payload(), "Start of Name");
    }

    {
        EXPECT_FALSE(as_function_block_name_view(make_function_block_discovery_message(0xE, 4)));
    }
}

//-----------------------------------------------

TEST_F(stream_message, send_function_block_name)
{
    using namespace midi;

    auto run_test = [](uint7_t block, std::string_view name) {
        size_t      number_of_packets = 0;
        std::string resulting_name;
        send_function_block_name(block, name, [&](const midi::universal_packet& p) {
            auto msg = as_function_block_name_view(p);
            EXPECT_TRUE(msg);
            if (msg)
            {
                EXPECT_EQ(msg->function_block(), block);

                ++number_of_packets;
                resulting_name.append(msg->payload());
            }
        });

        EXPECT_EQ(resulting_name, name);
        return number_of_packets;
    };

    EXPECT_EQ(1u, run_test(0, "short name"));
    EXPECT_EQ(1u, run_test(4, "short name ?!"));
    EXPECT_EQ(2u, run_test(7, "short name ?!?"));
    EXPECT_EQ(2u, run_test(31, "A reasonably long name"));
    EXPECT_EQ(3u, run_test(9, "A pretty long name that needs 3 packets"));
    EXPECT_EQ(4u, run_test(14, "An even longer name that needs four packets"));
}
