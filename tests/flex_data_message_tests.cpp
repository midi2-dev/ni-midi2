#include <gtest/gtest.h>

#include <midi/flex_data_message.h>

//-----------------------------------------------

class flex_data_message : public ::testing::Test
{ };

//-----------------------------------------------

TEST_F(flex_data_message, constructors)
{
    {
        constexpr midi::flex_data_message m;

        EXPECT_EQ(midi::packet_type::flex_data, m.type());
        EXPECT_EQ(0u, m.group());
        EXPECT_EQ(0u, m.format());
        EXPECT_EQ(0u, m.address());
        EXPECT_EQ(0u, m.channel());
        EXPECT_EQ(0u, m.status_bank());
        EXPECT_EQ(0u, m.status());
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }

    {
        constexpr midi::flex_data_message m{ 4 };

        EXPECT_EQ(midi::packet_type::flex_data, m.type());
        EXPECT_EQ(4u, m.group());
        EXPECT_EQ(0u, m.format());
        EXPECT_EQ(0u, m.address());
        EXPECT_EQ(0u, m.channel());
        EXPECT_EQ(0u, m.status_bank());
        EXPECT_EQ(0u, m.status());
        EXPECT_EQ(0u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }

    {
        constexpr midi::flex_data_message m{
            9u, midi::packet_format::cont, 0u, 10u, 2u, 6, 0x12345678u, 0x9ABCDEF0u, 0xA9876543u
        };

        EXPECT_EQ(midi::packet_type::flex_data, m.type());
        EXPECT_EQ(9u, m.group());
        EXPECT_EQ(midi::packet_format::cont, m.format());
        EXPECT_EQ(0u, m.address());
        EXPECT_EQ(10u, m.channel());
        EXPECT_EQ(2u, m.status_bank());
        EXPECT_EQ(6u, m.status());
        EXPECT_EQ(0x12345678u, m.data[1]);
        EXPECT_EQ(0x9ABCDEF0u, m.data[2]);
        EXPECT_EQ(0xA9876543u, m.data[3]);
    }

    {
        constexpr midi::flex_data_message m{ 0xF, midi::packet_format::end, 1, 0, 9, 8, 7, 6, 5 };

        EXPECT_EQ(midi::packet_type::flex_data, m.type());
        EXPECT_EQ(0xFu, m.group());
        EXPECT_EQ(midi::packet_format::end, m.format());
        EXPECT_EQ(1u, m.address());
        EXPECT_EQ(0u, m.channel());
        EXPECT_EQ(9u, m.status_bank());
        EXPECT_EQ(8u, m.status());
        EXPECT_EQ(7u, m.data[1]);
        EXPECT_EQ(6u, m.data[2]);
        EXPECT_EQ(5u, m.data[3]);
    }
}

//-----------------------------------------------

TEST_F(flex_data_message, is_flex_data_message)
{
    using namespace midi;

    EXPECT_TRUE(is_flex_data_message(universal_packet{ 0xD1010000, 1, 2, 3 }));
    EXPECT_TRUE(is_flex_data_message(universal_packet{ 0xDF140000, 4, 5, 6 }));
    EXPECT_TRUE(is_flex_data_message(universal_packet{ 0xD3260000, 7, 8, 9 }));
    EXPECT_TRUE(is_flex_data_message(universal_packet{ 0xD3880000, 10, 11, 12 }));

    EXPECT_FALSE(is_flex_data_message(universal_packet{ 0x01000000 }));
    EXPECT_FALSE(is_flex_data_message(universal_packet{ 0x19321200 }));
    EXPECT_FALSE(is_flex_data_message(universal_packet{ 0x2691447F }));
    EXPECT_FALSE(is_flex_data_message(universal_packet{ 0x40885060, 0x12340000 }));
    EXPECT_FALSE(is_flex_data_message(universal_packet{ 0x3F0D0000, 1, 2, 3 }));
    EXPECT_FALSE(is_flex_data_message(universal_packet{ 0x53260000, 7, 8, 9 }));
}

//-----------------------------------------------

TEST_F(flex_data_message, flex_data_message_view)
{
    {
        static constexpr midi::flex_data_message m{
            9u, midi::packet_format::cont, 0, 10u, 2u, 6, 0x12345678u, 0x9ABCDEF0u, 0xA9876543u
        };

        constexpr auto v = midi::flex_data_message_view{ m };
        EXPECT_TRUE(as_flex_data_message_view(m));

        EXPECT_EQ(9u, v.group());
        EXPECT_EQ(midi::packet_format::cont, v.format());
        EXPECT_EQ(0u, v.address());
        EXPECT_EQ(10u, v.channel());
        EXPECT_EQ(2u, v.status_bank());
        EXPECT_EQ(6u, v.status());
        EXPECT_EQ(0x12345678u, v.data1());
        EXPECT_EQ(0x9ABCDEF0u, v.data2());
        EXPECT_EQ(0xA9876543u, v.data3());
    }

    {
        static constexpr midi::flex_data_message m{ 4, midi::packet_format::end, 1, 0, 2, 3, 0xA, 0xB, 0xC };

        const auto v = midi::flex_data_message_view{ m };
        EXPECT_TRUE(midi::as_flex_data_message_view(m));

        EXPECT_EQ(midi::packet_format::end, v.format());
        EXPECT_EQ(1u, v.address());
        EXPECT_EQ(0u, v.channel());
        EXPECT_EQ(2u, v.status_bank());
        EXPECT_EQ(3u, v.status());
        EXPECT_EQ(0xAu, v.data1());
        EXPECT_EQ(0xBu, v.data2());
        EXPECT_EQ(0xCu, v.data3());
    }

    {
        static constexpr midi::flex_data_message m{ 1, 2, 0, 4, 5, 6, 7, 8, 9 };

        constexpr auto v = midi::flex_data_message_view{ m };
        EXPECT_TRUE(midi::as_flex_data_message_view(m));

        EXPECT_EQ(2u, v.format());
        EXPECT_EQ(0u, v.address());
        EXPECT_EQ(4u, v.channel());
        EXPECT_EQ(5u, v.status_bank());
        EXPECT_EQ(6u, v.status());
        EXPECT_EQ(7u, v.data1());
        EXPECT_EQ(8u, v.data2());
        EXPECT_EQ(9u, v.data3());
    }

    {
        static constexpr midi::flex_data_message m{ 1,          midi::packet_format::complete, 0, 12, 1, 7, 0x74657374u,
                                                    0x74657874u };

        const auto v = midi::flex_data_message_view{ m };
        EXPECT_TRUE(midi::as_flex_data_message_view(m));

        EXPECT_EQ(midi::packet_format::complete, v.format());
        EXPECT_EQ(0u, v.address());
        EXPECT_EQ(12u, v.channel());
        EXPECT_EQ(1u, v.status_bank());
        EXPECT_EQ(7u, v.status());
        EXPECT_EQ(0x74657374u, v.data1());
        EXPECT_EQ(0x74657874u, v.data2());
        EXPECT_EQ(0u, v.data3());
        EXPECT_EQ(v.payload_as_string(), "testtext");
    }

    {
        static constexpr midi::universal_packet m;

        EXPECT_FALSE(midi::as_flex_data_message_view(m));
    }
}

//-----------------------------------------------

TEST_F(flex_data_message, make_flex_data_message)
{
    using namespace midi;

    {
        constexpr auto m = make_flex_data_message(4, packet_format::end, 0, 6, 2, 3, 0xA, 0xB, 0xC);

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(4u, m.group());
        EXPECT_EQ(midi::packet_format::end, m.format());
        EXPECT_EQ(0u, m.address());
        EXPECT_EQ(6u, m.channel());
        EXPECT_EQ(2u, m.status_bank());
        EXPECT_EQ(3u, m.status());
        EXPECT_EQ(0xAu, m.data[1]);
        EXPECT_EQ(0xBu, m.data[2]);
        EXPECT_EQ(0xCu, m.data[3]);
    }

    {
        auto m = make_flex_data_message(1, 2, 0, 4, 5, 6, 7, 8, 9);

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(1u, m.group());
        EXPECT_EQ(2u, m.format());
        EXPECT_EQ(0u, m.address());
        EXPECT_EQ(4u, m.channel());
        EXPECT_EQ(5u, m.status_bank());
        EXPECT_EQ(6u, m.status());
        EXPECT_EQ(7u, m.data[1]);
        EXPECT_EQ(8u, m.data[2]);
        EXPECT_EQ(9u, m.data[3]);
    }
}

//-----------------------------------------------

TEST_F(flex_data_message, make_flex_data_text_message)
{
    using namespace midi;

    {
        constexpr auto m = make_flex_data_text_message(1, packet_format::complete, 0, 12, 1, 7, "testtext");

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(1u, m.group());
        EXPECT_EQ(midi::packet_format::complete, m.format());
        EXPECT_EQ(0u, m.address());
        EXPECT_EQ(12u, m.channel());
        EXPECT_EQ(1u, m.status_bank());
        EXPECT_EQ(7u, m.status());
        EXPECT_EQ(0x74657374u, m.data[1]);
        EXPECT_EQ(0x74657874u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
        EXPECT_EQ(m.payload_as_string(), "testtext");
    }

    std::string copyright_text;

    {
        const auto m = make_flex_data_text_message(5, packet_format::start, 1, 0, 2, 4, "This is a mu");

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(5u, m.group());
        EXPECT_EQ(midi::packet_format::start, m.format());
        EXPECT_EQ(1u, m.address());
        EXPECT_EQ(0u, m.channel());
        EXPECT_EQ(2u, m.status_bank());
        EXPECT_EQ(4u, m.status());
        EXPECT_EQ(0x54686973u, m.data[1]);
        EXPECT_EQ(0x20697320u, m.data[2]);
        EXPECT_EQ(0x61206D75u, m.data[3]);
        EXPECT_EQ(m.payload_as_string(), "This is a mu");
        copyright_text.append(m.payload_as_string());
    }

    {
        constexpr auto m = make_flex_data_text_message(5, packet_format::cont, 1, 0, 2, 4, "lti-packet c");

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(5u, m.group());
        EXPECT_EQ(midi::packet_format::cont, m.format());
        EXPECT_EQ(1u, m.address());
        EXPECT_EQ(0u, m.channel());
        EXPECT_EQ(2u, m.status_bank());
        EXPECT_EQ(4u, m.status());
        EXPECT_EQ(0x6C74692Du, m.data[1]);
        EXPECT_EQ(0x7061636Bu, m.data[2]);
        EXPECT_EQ(0x65742063u, m.data[3]);
        EXPECT_EQ(m.payload_as_string(), "lti-packet c");
        copyright_text.append(m.payload_as_string());
    }

    {
        const auto m = make_flex_data_text_message(5, packet_format::cont, 1, 0, 2, 4, "opyright not");

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(5u, m.group());
        EXPECT_EQ(midi::packet_format::cont, m.format());
        EXPECT_EQ(1u, m.address());
        EXPECT_EQ(0u, m.channel());
        EXPECT_EQ(2u, m.status_bank());
        EXPECT_EQ(4u, m.status());
        EXPECT_EQ(0x6F707972u, m.data[1]);
        EXPECT_EQ(0x69676874u, m.data[2]);
        EXPECT_EQ(0x206E6F74u, m.data[3]);
        EXPECT_EQ(m.payload_as_string(), "opyright not");
        copyright_text.append(m.payload_as_string());
    }

    {
        constexpr auto m = make_flex_data_text_message(5, packet_format::end, 1, 0, 2, 4, "ice");

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(5u, m.group());
        EXPECT_EQ(midi::packet_format::end, m.format());
        EXPECT_EQ(1u, m.address());
        EXPECT_EQ(0u, m.channel());
        EXPECT_EQ(2u, m.status_bank());
        EXPECT_EQ(4u, m.status());
        EXPECT_EQ(0x69636500u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
        EXPECT_EQ(m.payload_as_string(), "ice");
        copyright_text.append(m.payload_as_string());
    }

    EXPECT_EQ(copyright_text, "This is a multi-packet copyright notice");
}

//-----------------------------------------------

TEST_F(flex_data_message, make_set_tempo_message)
{
    using namespace midi;

    {
        constexpr auto m = make_set_tempo_message(0xD, 0x12345678);

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(0xDu, m.group());
        EXPECT_EQ(midi::packet_format::complete, m.format());
        EXPECT_EQ(1u, m.address());
        EXPECT_EQ(0u, m.channel());
        EXPECT_EQ(0x00u, m.status_bank());
        EXPECT_EQ(0x00u, m.status());
        EXPECT_EQ(0x12345678u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }

    {
        const auto m = make_set_tempo_message(2, 500 /* ms */ * 100 * 1000); // 120bpm

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(2u, m.group());
        EXPECT_EQ(midi::packet_format::complete, m.format());
        EXPECT_EQ(1u, m.address());
        EXPECT_EQ(0u, m.channel());
        EXPECT_EQ(0x00u, m.status_bank());
        EXPECT_EQ(0x00u, m.status());
        EXPECT_EQ(0x2FAF080u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }
}

//-----------------------------------------------

TEST_F(flex_data_message, make_set_time_signature_message)
{
    using namespace midi;

    {
        constexpr auto m = make_set_time_signature_message(7, 3, 4, 8);

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(07u, m.group());
        EXPECT_EQ(midi::packet_format::complete, m.format());
        EXPECT_EQ(1u, m.address());
        EXPECT_EQ(0u, m.channel());
        EXPECT_EQ(0x00u, m.status_bank());
        EXPECT_EQ(0x01u, m.status());
        EXPECT_EQ(0x03040800u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }

    {
        const auto m = make_set_time_signature_message(4, 16, 8, 32);

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(4u, m.group());
        EXPECT_EQ(midi::packet_format::complete, m.format());
        EXPECT_EQ(1u, m.address());
        EXPECT_EQ(0u, m.channel());
        EXPECT_EQ(0x00u, m.status_bank());
        EXPECT_EQ(0x01u, m.status());
        EXPECT_EQ(0x10082000u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }
}

//-----------------------------------------------

TEST_F(flex_data_message, make_set_metronome_message)
{
    using namespace midi;

    {
        constexpr auto m = make_set_metronome_message(1, 24, 4, 0, 0, 0, 0);

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(1u, m.group());
        EXPECT_EQ(midi::packet_format::complete, m.format());
        EXPECT_EQ(1u, m.address());
        EXPECT_EQ(0u, m.channel());
        EXPECT_EQ(0x00u, m.status_bank());
        EXPECT_EQ(0x02u, m.status());
        EXPECT_EQ(0x18040000u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }

    {
        const auto m = make_set_metronome_message(0xA, 1, 2, 3, 4, 5, 6);

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(0xAu, m.group());
        EXPECT_EQ(midi::packet_format::complete, m.format());
        EXPECT_EQ(1u, m.address());
        EXPECT_EQ(0u, m.channel());
        EXPECT_EQ(0x00u, m.status_bank());
        EXPECT_EQ(0x02u, m.status());
        EXPECT_EQ(0x01020304u, m.data[1]);
        EXPECT_EQ(0x05060000u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }
}

//-----------------------------------------------

TEST_F(flex_data_message, make_set_key_signature_message)
{
    using namespace midi;

    {
        constexpr auto m = make_set_key_signature_message(5, 0, 5, -5, 3);

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(5u, m.group());
        EXPECT_EQ(midi::packet_format::complete, m.format());
        EXPECT_EQ(0u, m.address());
        EXPECT_EQ(5u, m.channel());
        EXPECT_EQ(0x00u, m.status_bank());
        EXPECT_EQ(0x05u, m.status());
        EXPECT_EQ(0xB3000000u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }

    {
        const auto m = make_set_key_signature_message(0xC, 1, 0, 7, 2);

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(0xCu, m.group());
        EXPECT_EQ(midi::packet_format::complete, m.format());
        EXPECT_EQ(1u, m.address());
        EXPECT_EQ(0u, m.channel());
        EXPECT_EQ(0x00u, m.status_bank());
        EXPECT_EQ(0x05u, m.status());
        EXPECT_EQ(0x72000000u, m.data[1]);
        EXPECT_EQ(0u, m.data[2]);
        EXPECT_EQ(0u, m.data[3]);
    }
}

//-----------------------------------------------

TEST_F(flex_data_message, make_set_chord_message)
{
    using namespace midi;

    {
        constexpr auto m = make_set_chord_message(6, 0, 12, 0x12345678, 0x9ABCDEF0, 0x21436587);

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(6u, m.group());
        EXPECT_EQ(midi::packet_format::complete, m.format());
        EXPECT_EQ(0u, m.address());
        EXPECT_EQ(12u, m.channel());
        EXPECT_EQ(0x00u, m.status_bank());
        EXPECT_EQ(0x06u, m.status());
        EXPECT_EQ(0x12345678u, m.data[1]);
        EXPECT_EQ(0x9ABCDEF0u, m.data[2]);
        EXPECT_EQ(0x21436587u, m.data[3]);
    }

    {
        const auto m = make_set_chord_message(6, 0, 12, 0x21436587, 0x9ABCDEF0, 0x12345678);

        EXPECT_TRUE(is_flex_data_message(m));
        EXPECT_EQ(6u, m.group());
        EXPECT_EQ(midi::packet_format::complete, m.format());
        EXPECT_EQ(0u, m.address());
        EXPECT_EQ(12u, m.channel());
        EXPECT_EQ(0x00u, m.status_bank());
        EXPECT_EQ(0x06u, m.status());
        EXPECT_EQ(0x21436587u, m.data[1]);
        EXPECT_EQ(0x9ABCDEF0u, m.data[2]);
        EXPECT_EQ(0x12345678u, m.data[3]);
    }
}

//-----------------------------------------------
