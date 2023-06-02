#include <gtest/gtest.h>

#include <midi/universal_packet.h>

#include <sstream>
#include <string>

//-----------------------------------------------

class universal_midi_packet : public ::testing::Test
{
  public:
};

//-----------------------------------------------

TEST_F(universal_midi_packet, constructors)
{
    using namespace midi;

    {
        universal_packet p;

        EXPECT_EQ(0u, p.data[0]);
        EXPECT_EQ(0u, p.data[1]);
        EXPECT_EQ(0u, p.data[2]);
        EXPECT_EQ(0u, p.data[3]);

        EXPECT_EQ(1u, p.size());
        EXPECT_EQ(packet_type::utility, p.type());
        EXPECT_EQ(0u, p.group());
        EXPECT_EQ(0u, p.status());
        EXPECT_EQ(utility_status::noop, p.status());
        EXPECT_EQ(0u, p.byte2());
        EXPECT_EQ(0u, p.byte3());
        EXPECT_EQ(0u, p.byte4());
    }

    {
        universal_packet p{ 0x12345678 };

        EXPECT_EQ(0x12345678u, p.data[0]);
        EXPECT_EQ(0x00000000u, p.data[1]);
        EXPECT_EQ(0x00000000u, p.data[2]);
        EXPECT_EQ(0x00000000u, p.data[3]);
    }

    {
        universal_packet p{ 1933467 };

        EXPECT_EQ(1933467u, p.data[0]);
        EXPECT_EQ(0u, p.data[1]);
        EXPECT_EQ(0u, p.data[2]);
        EXPECT_EQ(0u, p.data[3]);
    }

    {
        universal_packet p{ 0x21004567, 0x145938AA };

        EXPECT_EQ(0x21004567u, p.data[0]);
        EXPECT_EQ(0x145938AAu, p.data[1]);
        EXPECT_EQ(0x00000000u, p.data[2]);
        EXPECT_EQ(0x00000000u, p.data[3]);
    }

    {
        universal_packet p{ 0xBC352890, 0x9D2A445C, 77777 };

        EXPECT_EQ(0xBC352890u, p.data[0]);
        EXPECT_EQ(0x9D2A445Cu, p.data[1]);
        EXPECT_EQ(77777u, p.data[2]);
        EXPECT_EQ(0x00000000u, p.data[3]);
    }

    {
        universal_packet p{ 0x54ABCDEF, 0x12345678, 0xABADBABE, 0xCDCDCDCD };

        EXPECT_EQ(0x54ABCDEFu, p.data[0]);
        EXPECT_EQ(0x12345678u, p.data[1]);
        EXPECT_EQ(0xABADBABEu, p.data[2]);
        EXPECT_EQ(0xCDCDCDCDu, p.data[3]);
    }
}

//-----------------------------------------------

TEST_F(universal_midi_packet, equality)
{
    using namespace midi;

    // simple cases
    {
        universal_packet p1;
        universal_packet p2{ 0x12345678 };
        universal_packet p3{ 1933467 };
        universal_packet p4{ 0x31004567, 0x145938AA };
        universal_packet p5{ 0x12345678 };

        EXPECT_EQ(p1, p1);
        EXPECT_NE(p1, p2);
        EXPECT_NE(p1, p3);
        EXPECT_NE(p1, p4);
        EXPECT_NE(p1, p5);

        EXPECT_NE(p2, p1);
        EXPECT_EQ(p2, p2);
        EXPECT_NE(p2, p3);
        EXPECT_NE(p2, p4);
        EXPECT_EQ(p2, p5);

        EXPECT_NE(p3, p1);
        EXPECT_NE(p3, p2);
        EXPECT_EQ(p3, p3);
        EXPECT_NE(p3, p4);
        EXPECT_NE(p3, p5);

        EXPECT_NE(p4, p1);
        EXPECT_NE(p4, p2);
        EXPECT_NE(p4, p3);
        EXPECT_EQ(p4, p4);
        EXPECT_NE(p4, p5);

        EXPECT_NE(p5, p1);
        EXPECT_EQ(p5, p2);
        EXPECT_NE(p5, p3);
        EXPECT_NE(p5, p4);
        EXPECT_EQ(p5, p5);
    }

    // noise in unused data words
    {
        universal_packet p1_1{ 0x12345678, 1000 };
        universal_packet p1_2{ 0x12345678, 1001 };
        universal_packet p1_x{ 0x12345677, 1000 };

        EXPECT_EQ(p1_1, p1_1);
        EXPECT_EQ(p1_1, p1_2);
        EXPECT_NE(p1_1, p1_x);

        EXPECT_EQ(p1_2, p1_1);
        EXPECT_EQ(p1_2, p1_2);
        EXPECT_NE(p1_2, p1_x);

        EXPECT_NE(p1_x, p1_1);
        EXPECT_EQ(p1_x, p1_x);
        EXPECT_NE(p1_x, p1_2);

        universal_packet p2_1{ 0x31004567, 0x145938AA, 2939 };
        universal_packet p2_2{ 0x31004567, 0x145938AA, 55 };
        universal_packet p2_x{ 0x31004568, 0x145938AA, 2939 };
        universal_packet p2_y{ 0x31004567, 0x5938AA12, 2939 };

        EXPECT_EQ(p2_1, p2_1);
        EXPECT_EQ(p2_1, p2_2);
        EXPECT_NE(p2_1, p2_x);
        EXPECT_NE(p2_1, p2_y);

        EXPECT_EQ(p2_2, p2_1);
        EXPECT_EQ(p2_2, p2_2);
        EXPECT_NE(p2_2, p2_x);
        EXPECT_NE(p2_2, p2_y);

        EXPECT_NE(p2_x, p2_1);
        EXPECT_NE(p2_x, p2_2);
        EXPECT_EQ(p2_x, p2_x);
        EXPECT_NE(p2_x, p2_y);

        EXPECT_NE(p2_y, p2_1);
        EXPECT_NE(p2_y, p2_2);
        EXPECT_NE(p2_y, p2_x);
        EXPECT_EQ(p2_y, p2_y);

        universal_packet p3_1{ 0xBA345678, 99, 55, 42 };
        universal_packet p3_2{ 0xBA345678, 99, 55, 43 };
        universal_packet p3_x{ 0xBA345679, 99, 55, 42 };
        universal_packet p3_y{ 0xBA345678, 98, 55, 42 };
        universal_packet p3_z{ 0xBA345678, 99, 56, 42 };

        EXPECT_EQ(p3_1, p3_1);
        EXPECT_EQ(p3_1, p3_2);
        EXPECT_NE(p3_1, p3_x);
        EXPECT_NE(p3_1, p3_y);
        EXPECT_NE(p3_1, p3_z);

        EXPECT_EQ(p3_2, p3_1);
        EXPECT_EQ(p3_2, p3_2);
        EXPECT_NE(p3_2, p3_x);
        EXPECT_NE(p3_2, p3_y);
        EXPECT_NE(p3_2, p3_z);

        EXPECT_NE(p3_x, p3_1);
        EXPECT_NE(p3_x, p3_2);
        EXPECT_EQ(p3_x, p3_x);
        EXPECT_NE(p3_x, p3_y);
        EXPECT_NE(p3_x, p3_z);

        EXPECT_NE(p3_y, p3_1);
        EXPECT_NE(p3_y, p3_2);
        EXPECT_NE(p3_y, p3_x);
        EXPECT_EQ(p3_y, p3_y);
        EXPECT_NE(p3_y, p3_z);

        EXPECT_NE(p3_z, p3_1);
        EXPECT_NE(p3_z, p3_2);
        EXPECT_NE(p3_z, p3_x);
        EXPECT_NE(p3_z, p3_y);
        EXPECT_EQ(p3_z, p3_z);
    }
}

//-----------------------------------------------

TEST_F(universal_midi_packet, type)
{
    using namespace midi;

    universal_packet p0{ 0x011F1234 };
    EXPECT_EQ(packet_type::utility, p0.type());

    universal_packet p1{ 0x15F12345 };
    EXPECT_EQ(packet_type::system, p1.type());

    universal_packet p2{ 0x2CBF8765 };
    EXPECT_EQ(packet_type::midi1_channel_voice, p2.type());

    universal_packet p3{ 0x34317F03, 0x12345678 };
    EXPECT_EQ(packet_type::data, p3.type());

    universal_packet p4{ 0x41937788, 0x81114080 };
    EXPECT_EQ(packet_type::midi2_channel_voice, p4.type());

    universal_packet p5{ 0x59937788, 0x11111111, 0x22222222, 0x22222222 };
    EXPECT_EQ(packet_type::extended_data, p5.type());

    p0.set_type(packet_type::midi1_channel_voice);
    EXPECT_EQ(packet_type::midi1_channel_voice, p0.type());

    p5.set_type(packet_type::midi2_channel_voice);
    EXPECT_EQ(packet_type::midi2_channel_voice, p5.type());
}

//-----------------------------------------------

TEST_F(universal_midi_packet, size)
{
    using namespace midi;

    // defined mesage types
    EXPECT_EQ(1u, universal_packet(0x011F1234).size());
    EXPECT_EQ(1u, universal_packet(0x15F12345).size());
    EXPECT_EQ(1u, universal_packet(0x2CBF8765).size());
    EXPECT_EQ(2u, universal_packet(0x34317F03, 0x12345678).size());
    EXPECT_EQ(2u, universal_packet(0x41937788, 0x81114080).size());
    EXPECT_EQ(4u, universal_packet(0x59937788, 0x11111111, 0x22222222, 0x22222222).size());

    // reserved (future) message types
    EXPECT_EQ(1u, universal_packet(0x60000000).size());
    EXPECT_EQ(1u, universal_packet(0x70000000).size());
    EXPECT_EQ(2u, universal_packet(0x80000000).size());
    EXPECT_EQ(2u, universal_packet(0x90000000).size());
    EXPECT_EQ(2u, universal_packet(0xA0000000).size());
    EXPECT_EQ(3u, universal_packet(0xB0000000).size());
    EXPECT_EQ(3u, universal_packet(0xC0000000).size());
    EXPECT_EQ(4u, universal_packet(0xD0000000).size());
    EXPECT_EQ(4u, universal_packet(0xE0000000).size());
    EXPECT_EQ(4u, universal_packet(0xF0000000).size());
}

//-----------------------------------------------

TEST_F(universal_midi_packet, group)
{
    using namespace midi;

    universal_packet p0{ 0x011F1234 };
    EXPECT_EQ(1u, p0.group());

    universal_packet p1{ 0x15F12345 };
    EXPECT_EQ(5u, p1.group());

    universal_packet p2{ 0x2CBF8765 };
    EXPECT_EQ(12u, p2.group());

    universal_packet p3{ 0x34317F03, 0x12345678 };
    EXPECT_EQ(4u, p3.group());

    universal_packet p4{ 0x41937788, 0x81114080 };
    EXPECT_EQ(1u, p4.group());

    universal_packet p5{ 0x59937788, 0x11111111, 0x22222222, 0x22222222 };
    EXPECT_EQ(9u, p5.group());

    p1.set_group(4);
    EXPECT_EQ(4u, p1.group());

    p3.set_group(0xF);
    EXPECT_EQ(15u, p3.group());
}

//-----------------------------------------------

TEST_F(universal_midi_packet, status)
{
    using namespace midi;

    universal_packet p0{ 0x011F1234 };
    EXPECT_EQ(0x1Fu, p0.status());

    universal_packet p1{ 0x15F12345 };
    EXPECT_EQ(0xF1u, p1.status());

    universal_packet p2{ 0x2CBF8765 };
    EXPECT_EQ(0xBFu, p2.status());

    universal_packet p3{ 0x34317F03, 0x12345678 };
    EXPECT_EQ(0x31u, p3.status());

    universal_packet p4{ 0x41937788, 0x81114080 };
    EXPECT_EQ(0x93u, p4.status());

    universal_packet p5{ 0x599A7788, 0x11111111, 0x22222222, 0x22222222 };
    EXPECT_EQ(0x9Au, p5.status());

    p2.set_byte(1, 0xAB);
    EXPECT_EQ(0xABu, p2.status());

    p4.set_byte(1, 0x01);
    EXPECT_EQ(0x01u, p4.status());
}

//-----------------------------------------------

TEST_F(universal_midi_packet, bytes)
{
    using namespace midi;

    universal_packet p{ 0x599A7788, 0x4F95A278, 0x34317F03, 0x011F1234 };

    EXPECT_EQ(0x59u, p.get_byte(0));
    EXPECT_EQ(0x9Au, p.get_byte(1));
    EXPECT_EQ(0x9Au, p.byte2());
    EXPECT_EQ(0x77u, p.get_byte(2));
    EXPECT_EQ(0x77u, p.byte3());
    EXPECT_EQ(0x88u, p.get_byte(3));
    EXPECT_EQ(0x88u, p.byte4());

    EXPECT_EQ(0x4Fu, p.get_byte(4));
    EXPECT_EQ(0x95u, p.get_byte(5));
    EXPECT_EQ(0xA2u, p.get_byte(6));
    EXPECT_EQ(0x78u, p.get_byte(7));

    EXPECT_EQ(0x34u, p.get_byte(8));
    EXPECT_EQ(0x31u, p.get_byte(9));
    EXPECT_EQ(0x7Fu, p.get_byte(10));
    EXPECT_EQ(0x03u, p.get_byte(11));

    EXPECT_EQ(0x01u, p.get_byte(12));
    EXPECT_EQ(0x1Fu, p.get_byte(13));
    EXPECT_EQ(0x12u, p.get_byte(14));
    EXPECT_EQ(0x34u, p.get_byte(15));

    for (std::uint8_t i = 0; i < 16; ++i)
    {
        const std::uint8_t sevenbits = p.get_byte(i) & 0x7F;
        EXPECT_EQ(sevenbits, p.get_byte_7bit(i));

        p.set_byte(i, i + 1);
        EXPECT_EQ(i + 1, p.get_byte(i));

        p.set_byte_7bit(i, 0x80 + i);
        EXPECT_EQ(i, p.get_byte(i));
    }
}

//-----------------------------------------------

TEST_F(universal_midi_packet, channel)
{
    using namespace midi;

    // defined message types
    universal_packet p0{ 0x011F1234 };
    EXPECT_FALSE(p0.has_channel());

    universal_packet p1{ 0x15F12345 };
    EXPECT_FALSE(p1.has_channel());

    universal_packet p2{ 0x2CBF8765 };
    EXPECT_TRUE(p2.has_channel());
    EXPECT_EQ(15u, p2.channel());

    universal_packet p3{ 0x34317F03, 0x12345678 };
    EXPECT_FALSE(p3.has_channel());

    universal_packet p4{ 0x41937788, 0x81114080 };
    EXPECT_TRUE(p4.has_channel());
    EXPECT_EQ(3u, p4.channel());

    universal_packet p5{ 0x599A7788, 0x11111111, 0x22222222, 0x22222222 };
    EXPECT_FALSE(p5.has_channel());

    p2.set_byte(1, 0x11);
    EXPECT_EQ(1u, p2.channel());

    p4.set_byte(1, 0xDE);
    EXPECT_EQ(14u, p4.channel());

    // reserved (future) message types
    EXPECT_FALSE(universal_packet(0x60000000).has_channel());
    EXPECT_FALSE(universal_packet(0x70000000).has_channel());
    EXPECT_FALSE(universal_packet(0x80000000).has_channel());
    EXPECT_FALSE(universal_packet(0x90000000).has_channel());
    EXPECT_FALSE(universal_packet(0xA0000000).has_channel());
    EXPECT_FALSE(universal_packet(0xB0000000).has_channel());
    EXPECT_FALSE(universal_packet(0xC0000000).has_channel());
    EXPECT_TRUE(universal_packet(0xD0000000).has_channel());
    EXPECT_FALSE(universal_packet(0xE0000000).has_channel());
    EXPECT_FALSE(universal_packet(0xF0000000).has_channel());
}

//-----------------------------------------------

TEST_F(universal_midi_packet, reset)
{
    using namespace midi;

    universal_packet p(0x50123456, 0x789ABCDE, 0xFEDCBA98, 0x76543210);

    p.reset();

    EXPECT_EQ(0u, p.data[0]);
    EXPECT_EQ(0u, p.data[1]);
    EXPECT_EQ(0u, p.data[2]);
    EXPECT_EQ(0u, p.data[3]);
}

//-----------------------------------------------

TEST_F(universal_midi_packet, is_utility_message)
{
    using namespace midi;

    for (unsigned t = 0x0; t < 0x10; ++t)
    {
        universal_packet p{ t << 28 };

        switch (t)
        {
        case 0:
            EXPECT_TRUE(p.is_utility_message());
            break;
        default:
            EXPECT_FALSE(p.is_utility_message());
            break;
        }
    }
}

//-----------------------------------------------

TEST_F(universal_midi_packet, is_system_message)
{
    using namespace midi;

    for (unsigned t = 0x0; t < 0x10; ++t)
    {
        universal_packet p{ t << 28 };

        switch (t)
        {
        case 1:
            EXPECT_TRUE(p.is_system_message());
            break;
        default:
            EXPECT_FALSE(p.is_system_message());
            break;
        }
    }
}

//-----------------------------------------------

TEST_F(universal_midi_packet, is_channel_voice_message)
{
    using namespace midi;

    for (unsigned t = 0x0; t < 0x10; ++t)
    {
        universal_packet p{ t << 28 };

        switch (t)
        {
        case 2:
        case 4:
            EXPECT_TRUE(p.is_channel_voice_message());
            break;
        default:
            EXPECT_FALSE(p.is_channel_voice_message());
            break;
        }
    }
}

//-----------------------------------------------

TEST_F(universal_midi_packet, is_data_message)
{
    using namespace midi;

    for (unsigned t = 0x0; t < 0x10; ++t)
    {
        universal_packet p{ t << 28 };

        switch (t)
        {
        case 3:
        case 5:
            EXPECT_TRUE(p.is_data_message());
            break;
        default:
            EXPECT_FALSE(p.is_data_message());
            break;
        }
    }
}

//-----------------------------------------------

TEST_F(universal_midi_packet, is_midi1_protocol_message)
{
    using namespace midi;

    for (unsigned t = 0x0; t < 0x10; ++t)
    {
        for (unsigned s = 0x0; s < 0xFF; ++s)
        {
            universal_packet p{ (t << 28) | (s << 16) };

            switch (t)
            {
            case 1:
                if (p.status() > 0xF0)
                {
                    switch (p.status())
                    {
                    case 0xF4:
                    case 0xF5:
                    case 0xF7:
                    case 0xF9:
                    case 0xFD:
                        EXPECT_FALSE(p.is_midi1_protocol_message());
                        break;
                    default:
                        EXPECT_TRUE(p.is_midi1_protocol_message());
                        break;
                    }
                }
                else
                {
                    EXPECT_FALSE(p.is_midi1_protocol_message());
                }
                break;
            case 2:
                if ((p.status() >= 0x80) && (p.status() < 0xF0))
                {
                    EXPECT_TRUE(p.is_midi1_protocol_message());
                }
                else
                {
                    EXPECT_FALSE(p.is_midi1_protocol_message());
                }
                break;
            default:
                EXPECT_FALSE(p.is_midi1_protocol_message());
                break;
            }
        }
    }
}

//-----------------------------------------------

TEST_F(universal_midi_packet, stream_operators)
{
    using namespace midi;

    universal_packet p(0x50123456, 0x789ABCDE, 0xFEDCBA98, 0x76543210);

    {
        std::stringstream s;
        s << p << std::endl;

        universal_packet o;
        s >> o;
        EXPECT_EQ(o, p);
        EXPECT_TRUE(s.good());
    }

    {
        std::stringstream s;
        s << p;

        std::string p_as_string1, p_as_string2, p_as_string3, p_as_string4;
        s >> p_as_string1 >> p_as_string2 >> p_as_string3 >> p_as_string4;
        EXPECT_EQ(p_as_string1, "50123456");
        EXPECT_EQ(p_as_string2, "789abcde");
        EXPECT_EQ(p_as_string3, "fedcba98");
        EXPECT_EQ(p_as_string4, "76543210");
    }

    {
        std::stringstream s;
        s << "invaliddata";

        universal_packet o;
        s >> o;
        EXPECT_FALSE(s.good());
        EXPECT_EQ(o, universal_packet{});
    }

    {
        std::stringstream s;
        s << "50123456p1";

        universal_packet o;
        s >> o;
        EXPECT_FALSE(s.good());
        EXPECT_EQ(o, universal_packet{ 0x50123456 });
    }
}

//-----------------------------------------------
