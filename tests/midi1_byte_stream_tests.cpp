#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <midi/midi1_byte_stream.h>

#include <midi/data_message.h>
#include <midi/midi1_channel_voice_message.h>
#include <midi/system_message.h>

#include <functional>

//-----------------------------------------------

class midi1_byte_stream : public ::testing::Test
{
  public:
    class parser_client
    {
      public:
        MOCK_METHOD1(on_packet, void(midi::universal_packet));
        MOCK_METHOD1(on_sysex, void(midi::sysex7));
    };
};

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_constructors)
{
    {
        midi::midi1_byte_stream_parser p([](midi::universal_packet) {});
        EXPECT_TRUE(p.callbacks_enabled());
    }

    {
        midi::midi1_byte_stream_parser p([](midi::universal_packet) {}, [](midi::sysex7) {});
        EXPECT_TRUE(p.callbacks_enabled());
    }

    {
        midi::midi1_byte_stream_parser p([](midi::universal_packet) {}, [](midi::sysex7) {}, false);
        EXPECT_FALSE(p.callbacks_enabled());
    }

    {
        unsigned num_callbacks = 0;

        midi::midi1_byte_stream_parser p(
          5,
          [&num_callbacks](midi::universal_packet p) {
              ++num_callbacks;
              EXPECT_EQ(5u, p.group());
          },
          {},
          true);
        EXPECT_TRUE(p.callbacks_enabled());
        p.feed(0x91);
        p.feed(0x39);
        p.feed(100);
        EXPECT_EQ(1u, num_callbacks);
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_group)
{
    unsigned num_callbacks = 0;

    midi::midi1_byte_stream_parser p(
      [&num_callbacks](midi::universal_packet p) {
          ++num_callbacks;
          EXPECT_EQ(9u, p.group());
      },
      {},
      true);

    EXPECT_EQ(0u, p.group());

    p.set_group(9);
    EXPECT_EQ(9u, p.group());

    p.feed(0xFA);
    EXPECT_EQ(1u, num_callbacks);
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_enable_callbacks)
{
    bool callback_invoked = false;

    midi::midi1_byte_stream_parser p([&callback_invoked](midi::universal_packet) { callback_invoked = true; });
    EXPECT_TRUE(p.callbacks_enabled());

    p.feed(0xF8);
    EXPECT_TRUE(callback_invoked);

    p.enable_callbacks(false);
    EXPECT_FALSE(p.callbacks_enabled());

    callback_invoked = false;
    p.feed(0xF9);
    EXPECT_FALSE(callback_invoked);

    p.enable_callbacks(true);
    EXPECT_TRUE(p.callbacks_enabled());

    p.feed(0xFA);
    EXPECT_TRUE(callback_invoked);
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_system_messages)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1),
                                     std::bind(&parser_client::on_sysex, &c, std::placeholders::_1));

    constexpr std::uint8_t byte_stream[] = { 0xF8, 0xF9, 0xF1, 0x09, 0xFA, 0xF2, 0x11, 0x44, 0xFB, 0xF3, 0x75, 0xFC,
                                             0xF4, 0x02, 0xFD, 0xF5, 0x27, 0xFE, 0xF6, 0xFF, 0x33, 0xFA, 0xFE };

    const midi::universal_packet packets[] = {
        midi::make_system_message(0, midi::system_status::clock),
        midi::make_system_message(0, midi::system_status::mtc_quarter_frame, 0x09),
        midi::make_system_message(0, midi::system_status::start),
        midi::make_system_message(0, midi::system_status::song_position, 0x11, 0x44),
        midi::make_system_message(0, midi::system_status::cont),
        midi::make_system_message(0, midi::system_status::song_select, 0x75),
        midi::make_system_message(0, midi::system_status::stop),
        midi::make_system_message(0, midi::system_status::active_sense),
        midi::make_system_message(0, midi::system_status::tune_request),
        midi::make_system_message(0, midi::system_status::reset),
        midi::make_system_message(0, midi::system_status::start),
        midi::make_system_message(0, midi::system_status::active_sense)
    };

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        for (const auto b : byte_stream)
        {
            p.feed(b);
        }
    }

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        p.feed(byte_stream, byte_stream + sizeof(byte_stream));
    }

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        p.feed(byte_stream, sizeof(byte_stream));
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_channel_voice_messages)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1),
                                     std::bind(&parser_client::on_sysex, &c, std::placeholders::_1));

    constexpr std::uint8_t byte_stream[] = { 0x83, 0x45, 0x6E, 0x9E, 0x30, 0x7F, 0xAA, 0x44, 0x03, 0x77,
                                             0xB0, 0x07, 0x70, 0xC5, 0x11, 0xDB, 0x14, 0xE5, 0x03, 0x72 };

    const midi::universal_packet packets[] = {
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::note_off, 3, 0x45, 0x6E),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::note_on, 14, 0x30, 0x7F),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::poly_pressure, 10, 0x44, 0x03),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::control_change, 0, 0x07, 0x70),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::program_change, 5, 0x11),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::channel_pressure, 11, 0x14),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::pitch_bend, 5, 0x03, 0x72)
    };

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        for (const auto b : byte_stream)
        {
            p.feed(b);
        }
    }

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        p.feed(byte_stream, byte_stream + sizeof(byte_stream));
    }

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        p.feed(byte_stream, sizeof(byte_stream));
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_running_status)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1),
                                     std::bind(&parser_client::on_sysex, &c, std::placeholders::_1));

    constexpr std::uint8_t byte_stream[] = { 0xA5, 0x44, 0x03, 0x44, 0x77, 0xB8, 0x07, 0x70, 0x72, 0x79, 0x7A,
                                             0xC6, 0x11, 0x12, 0xD2, 0x14, 0x64, 0xE5, 0x03, 0x72, 0x08, 0x74 };

    const midi::universal_packet packets[] = {
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::poly_pressure, 5, 0x44, 0x03),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::poly_pressure, 5, 0x44, 0x77),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::control_change, 8, 0x07, 0x70),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::control_change, 8, 0x72, 0x79),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::program_change, 6, 0x11),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::program_change, 6, 0x12),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::channel_pressure, 2, 0x14),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::channel_pressure, 2, 0x64),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::pitch_bend, 5, 0x03, 0x72),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::pitch_bend, 5, 0x08, 0x74),
    };

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        for (const auto b : byte_stream)
        {
            p.feed(b);
        }
    }

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        p.feed(byte_stream, byte_stream + sizeof(byte_stream));
    }

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        p.feed(byte_stream, sizeof(byte_stream));
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_system_common_cancels_running_status)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1),
                                     std::bind(&parser_client::on_sysex, &c, std::placeholders::_1));

    constexpr std::uint8_t byte_stream[] = { 0xC6, 0x11, 0x12, 0xD2, 0x14, 0x64, 0xF6, 0x00, 0xE5, 0x03, 0x72 };

    const midi::universal_packet packets[] = {
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::program_change, 6, 0x11),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::program_change, 6, 0x12),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::channel_pressure, 2, 0x14),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::channel_pressure, 2, 0x64),
        midi::make_system_message(0, midi::system_status::tune_request),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::pitch_bend, 5, 0x03, 0x72)
    };

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        for (const auto b : byte_stream)
        {
            p.feed(b);
        }
    }

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        p.feed(byte_stream, byte_stream + sizeof(byte_stream));
    }

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        p.feed(byte_stream, sizeof(byte_stream));
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_system_realtime_intersperes_running_status)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1),
                                     std::bind(&parser_client::on_sysex, &c, std::placeholders::_1));

    constexpr std::uint8_t byte_stream[] = { 0xA5, 0x44, 0x03, 0x44, 0xFA, 0x77, 0xB8, 0x07, 0x70,
                                             0xFB, 0x72, 0x79, 0x7A, 0xC6, 0x11, 0x12, 0xD2, 0xFE,
                                             0x14, 0x64, 0xE5, 0x03, 0xFF, 0x72, 0x08, 0x74 };

    const midi::universal_packet packets[] = {
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::poly_pressure, 5, 0x44, 0x03),
        midi::make_system_message(0, midi::system_status::start),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::poly_pressure, 5, 0x44, 0x77),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::control_change, 8, 0x07, 0x70),
        midi::make_system_message(0, midi::system_status::cont),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::control_change, 8, 0x72, 0x79),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::program_change, 6, 0x11),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::program_change, 6, 0x12),
        midi::make_system_message(0, midi::system_status::active_sense),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::channel_pressure, 2, 0x14),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::channel_pressure, 2, 0x64),
        midi::make_system_message(0, midi::system_status::reset),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::pitch_bend, 5, 0x03, 0x72),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::pitch_bend, 5, 0x08, 0x74),
    };

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        for (const auto b : byte_stream)
        {
            p.feed(b);
        }
    }

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        p.feed(byte_stream, byte_stream + sizeof(byte_stream));
    }

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        p.feed(byte_stream, sizeof(byte_stream));
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_sysex_callback)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1),
                                     std::bind(&parser_client::on_sysex, &c, std::placeholders::_1));

    constexpr std::uint8_t byte_stream[] = { 0xF0, 0x7D, 0x25, 0x50, 0x44, 0xF7, 0xF0, 0x00, 0x21, 0x09, 0x44,
                                             0x03, 0x44, 0x77, 0x07, 0x70, 0x72, 0x79, 0xF7, 0xF0, 0x00, 0x02,
                                             0x0B, 0x11, 0x12, 0x14, 0x64, 0x03, 0x72, 0x08, 0x74, 0x22, 0xF7 };

    const midi::sysex7 sysex[] = { { midi::manufacturer::educational, byte_stream + 2, 3u },
                                   { midi::manufacturer::native_instruments, byte_stream + 10, 8u },
                                   { midi::manufacturer::denon, byte_stream + 23, 9u } };

    {
        testing::Sequence s;

        for (auto& sx : sysex)
        {
            EXPECT_CALL(c, on_sysex(sx)).Times(1).InSequence(s);
        }

        for (const auto b : byte_stream)
        {
            p.feed(b);
        }
    }

    {
        testing::Sequence s;

        for (auto& sx : sysex)
        {
            EXPECT_CALL(c, on_sysex(sx)).Times(1).InSequence(s);
        }

        p.feed(byte_stream, byte_stream + sizeof(byte_stream));
    }

    {
        testing::Sequence s;

        for (auto& sx : sysex)
        {
            EXPECT_CALL(c, on_sysex(sx)).Times(1).InSequence(s);
        }

        p.feed(byte_stream, sizeof(byte_stream));
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_sysex_packets)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1));

    constexpr std::uint8_t byte_stream[] = { 0xF0, 0x7D, 0x25, 0x50, 0x44, 0xF7, 0xF0, 0x00, 0x21, 0x09, 0x44, 0x03,
                                             0x44, 0x77, 0x07, 0x70, 0x72, 0x79, 0xF7, 0xF0, 0x00, 0x02, 0x0B, 0x11,
                                             0x12, 0x14, 0x64, 0x03, 0x72, 0x08, 0x74, 0x22, 0x37, 0x7D, 0x3A, 0xF7 };

    const midi::universal_packet packets[] = { { 0x30047D25, 0x50440000 },

                                               { 0x30160021, 0x09440344 }, { 0x30357707, 0x70727900 },

                                               { 0x30160002, 0x0B111214 }, { 0x30266403, 0x72087422 },
                                               { 0x3033377D, 0x3A000000 } };

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        for (const auto b : byte_stream)
        {
            p.feed(b);
        }
    }

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        p.feed(byte_stream, byte_stream + sizeof(byte_stream));
    }

    {
        testing::Sequence s;

        for (auto& r : packets)
        {
            EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
        }

        p.feed(byte_stream, sizeof(byte_stream));
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_sysex_intersperse_system_realtime_callback)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1),
                                     std::bind(&parser_client::on_sysex, &c, std::placeholders::_1));

    constexpr std::uint8_t byte_stream[] = { 0xF0, 0x7D, 0xF8, 0x25, 0x50, 0x44, 0xF7, 0xF0, 0x00, 0xFA,
                                             0x21, 0x09, 0x44, 0x03, 0xFB, 0x44, 0x77, 0x07, 0x70, 0x72,
                                             0x79, 0xF7, 0xF0, 0x00, 0x02, 0x0B, 0xFC, 0x11, 0x12, 0x14,
                                             0xFE, 0x64, 0x03, 0x72, 0x08, 0x74, 0x22, 0xF7, 0xFF };

    const midi::sysex7 sysex[] = {
        { midi::manufacturer::educational, std::vector<std::uint8_t>{ 0x25, 0x50, 0x44 } },
        { midi::manufacturer::native_instruments,
          std::vector<std::uint8_t>{ 0x44, 0x03, 0x44, 0x77, 0x07, 0x70, 0x72, 0x79 } },
        { midi::manufacturer::denon, std::vector<std::uint8_t>{ 0x11, 0x12, 0x14, 0x64, 0x03, 0x72, 0x08, 0x74, 0x22 } }
    };

    const midi::universal_packet rt[] = { midi::make_system_message(0, midi::system_status::clock),
                                          midi::make_system_message(0, midi::system_status::start),
                                          midi::make_system_message(0, midi::system_status::cont),
                                          midi::make_system_message(0, midi::system_status::stop),
                                          midi::make_system_message(0, midi::system_status::active_sense),
                                          midi::make_system_message(0, midi::system_status::reset) };

    testing::Sequence s;
    EXPECT_CALL(c, on_packet(rt[0])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_sysex(sysex[0])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_packet(rt[1])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_packet(rt[2])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_sysex(sysex[1])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_packet(rt[3])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_packet(rt[4])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_sysex(sysex[2])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_packet(rt[5])).Times(1).InSequence(s);

    p.feed(byte_stream, byte_stream + sizeof(byte_stream));
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_sysex_intersperse_system_realtime_packets)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1));

    constexpr std::uint8_t byte_stream[] = { 0xF0, 0x7D, 0xF8, 0x25, 0x50, 0x44, 0xF7, 0xF0, 0x00, 0xFA,
                                             0x21, 0x09, 0x44, 0x03, 0xFB, 0x44, 0x77, 0x07, 0x70, 0x72,
                                             0x79, 0xF7, 0xF0, 0x00, 0x02, 0x0B, 0xFC, 0x11, 0x12, 0x14,
                                             0xFE, 0x64, 0x03, 0x72, 0x08, 0x74, 0x22, 0xF7, 0xFF };

    const midi::sysex7 sysex[] = {
        { midi::manufacturer::native_instruments,
          std::vector<std::uint8_t>{ 0x44, 0x03, 0x44, 0x77, 0x07, 0x70, 0x72, 0x79 } },
        { midi::manufacturer::denon, std::vector<std::uint8_t>{ 0x11, 0x12, 0x14, 0x64, 0x03, 0x72, 0x08, 0x74, 0x22 } }
    };

    const midi::universal_packet packets[] = { midi::make_system_message(0, midi::system_status::clock),
                                               { 0x30047D25, 0x50440000 },
                                               midi::make_system_message(0, midi::system_status::start),
                                               midi::make_system_message(0, midi::system_status::cont),
                                               { 0x30160021, 0x09440344 },
                                               { 0x30357707, 0x70727900 },
                                               midi::make_system_message(0, midi::system_status::stop),
                                               { 0x30160002, 0x0B111214 },
                                               midi::make_system_message(0, midi::system_status::active_sense),
                                               { 0x30266403, 0x72087422 },
                                               { 0x30300000, 0x00000000 },
                                               midi::make_system_message(0, midi::system_status::reset) };

    testing::Sequence s;

    for (auto& r : packets)
    {
        EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
    }

    for (const auto b : byte_stream)
    {
        p.feed(b);
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_sysex_intersperse_other_callback)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1),
                                     std::bind(&parser_client::on_sysex, &c, std::placeholders::_1));

    constexpr std::uint8_t byte_stream[] = { 0xF0, 0x7D, 0x25, 0x50, 0xF2, 0x44, 0x11, 0xF7, 0xF0, 0x00, 0x21, 0x09,
                                             0x44, 0x03, 0x44, 0x77, 0x9C, 0x07, 0x70, 0x72, 0x79, 0xF7, 0xF0, 0x00,
                                             0x02, 0x0B, 0x11, 0x12, 0x14, 0x64, 0x03, 0x72, 0x08, 0x74, 0x22, 0xF7 };

    const midi::sysex7 sysex[] = {
        { midi::manufacturer::denon, std::vector<std::uint8_t>{ 0x11, 0x12, 0x14, 0x64, 0x03, 0x72, 0x08, 0x74, 0x22 } }
    };

    const midi::universal_packet packets[] = {
        midi::make_system_message(0, midi::system_status::song_position, 0x44, 0x11),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::note_on, 12, 0x07, 0x70),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::note_on, 12, 0x72, 0x79),
    };

    testing::Sequence s;
    EXPECT_CALL(c, on_packet(packets[0])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_packet(packets[1])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_packet(packets[2])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_sysex(sysex[0])).Times(1).InSequence(s);

    p.feed(byte_stream, sizeof(byte_stream));
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_sysex_intersperse_other_packets)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1));

    constexpr std::uint8_t byte_stream[] = { 0xF0, 0x7D, 0x25, 0x50, 0xF2, 0x44, 0x11, 0xF7, 0xF0, 0x00, 0x21, 0x09,
                                             0x44, 0x03, 0x44, 0x77, 0x9C, 0x07, 0x70, 0x72, 0x79, 0xF7, 0xF0, 0x00,
                                             0x02, 0x0B, 0x11, 0x12, 0x14, 0x64, 0x03, 0x72, 0x08, 0x74, 0x22, 0xF7 };

    const midi::universal_packet packets[] = {
        midi::make_system_message(0, midi::system_status::song_position, 0x44, 0x11),
        { 0x30160021, 0x09440344 },
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::note_on, 12, 0x07, 0x70),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::note_on, 12, 0x72, 0x79),
        { 0x30160002, 0x0B111214 },
        { 0x30266403, 0x72087422 },
        { 0x30300000, 0x00000000 }
    };

    testing::Sequence s;

    for (auto& r : packets)
    {
        EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
    }

    p.feed(byte_stream, byte_stream + sizeof(byte_stream));
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_real_world_stream_callback)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1),
                                     std::bind(&parser_client::on_sysex, &c, std::placeholders::_1));

    constexpr std::uint8_t byte_stream[] = { 0xF0, 0x7D, 0xF8, 0x25, 0x50, 0x44, 0xF7, 0x84, 0x33, 0x74,
                                             0x22, 0x56, 0xF0, 0x00, 0xFA, 0x21, 0x09, 0x44, 0x03, 0xFB,
                                             0x44, 0x77, 0x07, 0x70, 0x72, 0x79, 0xF7, 0xF1, 0x6C, 0xF0,
                                             0x00, 0x02, 0x0B, 0xFC, 0x11, 0x12, 0x14, 0xFE, 0x64, 0x03,
                                             0x72, 0x08, 0x74, 0x22, 0xF7, 0xFF, 0xB4, 0x07, 0x10 };

    const midi::sysex7 sysex[] = {
        { midi::manufacturer::educational, std::vector<std::uint8_t>{ 0x25, 0x50, 0x44 } },
        { midi::manufacturer::native_instruments,
          std::vector<std::uint8_t>{ 0x44, 0x03, 0x44, 0x77, 0x07, 0x70, 0x72, 0x79 } },
        { midi::manufacturer::denon, std::vector<std::uint8_t>{ 0x11, 0x12, 0x14, 0x64, 0x03, 0x72, 0x08, 0x74, 0x22 } }
    };

    const midi::universal_packet packets[] = {
        midi::make_system_message(0, midi::system_status::clock),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::note_off, 4, 0x33, 0x74),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::note_off, 4, 0x22, 0x56),
        midi::make_system_message(0, midi::system_status::start),
        midi::make_system_message(0, midi::system_status::cont),
        midi::make_system_message(0, midi::system_status::mtc_quarter_frame, 0x6C),
        midi::make_system_message(0, midi::system_status::stop),
        midi::make_system_message(0, midi::system_status::active_sense),
        midi::make_system_message(0, midi::system_status::reset),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::control_change, 4, 0x07, 0x10),
    };

    testing::Sequence s;
    EXPECT_CALL(c, on_packet(packets[0])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_sysex(sysex[0])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_packet(packets[1])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_packet(packets[2])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_packet(packets[3])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_packet(packets[4])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_sysex(sysex[1])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_packet(packets[5])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_packet(packets[6])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_packet(packets[7])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_sysex(sysex[2])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_packet(packets[8])).Times(1).InSequence(s);
    EXPECT_CALL(c, on_packet(packets[9])).Times(1).InSequence(s);

    p.feed(byte_stream, sizeof(byte_stream));
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_real_world_stream_packets)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1));

    constexpr std::uint8_t byte_stream[] = { 0xF0, 0x7D, 0xF8, 0x25, 0x50, 0x44, 0xF7, 0x84, 0x33, 0x74,
                                             0x22, 0x56, 0xF0, 0x00, 0xFA, 0x21, 0x09, 0x44, 0x03, 0xFB,
                                             0x44, 0x77, 0x07, 0x70, 0x72, 0x79, 0xF7, 0xF1, 0x6C, 0xF0,
                                             0x00, 0x02, 0x0B, 0xFC, 0x11, 0x12, 0x14, 0xFE, 0x64, 0x03,
                                             0x72, 0x08, 0x74, 0x22, 0x33, 0xF7, 0xFF, 0xB4, 0x07, 0x10 };

    const midi::sysex7 sysex[] = {
        { midi::manufacturer::educational, std::vector<std::uint8_t>{ 0x25, 0x50, 0x44 } },
        { midi::manufacturer::native_instruments,
          std::vector<std::uint8_t>{ 0x44, 0x03, 0x44, 0x77, 0x07, 0x70, 0x72, 0x79 } },
        { midi::manufacturer::denon, std::vector<std::uint8_t>{ 0x11, 0x12, 0x14, 0x64, 0x03, 0x72, 0x08, 0x74, 0x22 } }
    };

    const midi::universal_packet packets[] = {
        midi::make_system_message(0, midi::system_status::clock),
        { 0x30047D25, 0x50440000 },
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::note_off, 4, 0x33, 0x74),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::note_off, 4, 0x22, 0x56),
        midi::make_system_message(0, midi::system_status::start),
        midi::make_system_message(0, midi::system_status::cont),
        { 0x30160021, 0x09440344 },
        { 0x30357707, 0x70727900 },
        midi::make_system_message(0, midi::system_status::mtc_quarter_frame, 0x6C),
        midi::make_system_message(0, midi::system_status::stop),
        { 0x30160002, 0x0B111214 },
        midi::make_system_message(0, midi::system_status::active_sense),
        { 0x30266403, 0x72087422 },
        { 0x30313300, 0x00000000 },
        midi::make_system_message(0, midi::system_status::reset),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::control_change, 4, 0x07, 0x10)
    };

    testing::Sequence s;

    for (auto& r : packets)
    {
        EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
    }

    p.feed(byte_stream, sizeof(byte_stream));
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_invalid_sysex)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1),
                                     std::bind(&parser_client::on_sysex, &c, std::placeholders::_1));

    constexpr std::uint8_t byte_stream[] = { 0xF0, 0xF7, 0xF0, 0x00, 0xF7, 0xF0, 0x00, 0x11, 0xF7,
                                             0xF0, 0x7D, 0xF7, 0xF0, 0x00, 0x21, 0x09, 0xF7 };

    const midi::sysex7 sysex[] = {
        midi::sysex7{ midi::manufacturer::educational },
        midi::sysex7{ midi::manufacturer::native_instruments },
    };

    testing::Sequence s;

    for (auto& sx : sysex)
    {
        EXPECT_CALL(c, on_sysex(sx)).Times(1).InSequence(s);
    }

    p.feed(byte_stream, byte_stream + sizeof(byte_stream));
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_reset)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1),
                                     std::bind(&parser_client::on_sysex, &c, std::placeholders::_1));

    constexpr std::uint8_t byte_stream1[] = { 0xF0, 0x00, 0x11 };
    constexpr std::uint8_t byte_stream2[] = { 0x11, 0xF7 };

    p.feed(byte_stream1, byte_stream1 + sizeof(byte_stream1));
    p.reset();
    p.feed(byte_stream2, byte_stream2 + sizeof(byte_stream2));
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_no_sysex_callback)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1));

    constexpr std::uint8_t byte_stream[] = { 0x83, 0x45, 0x6E, 0xF0, 0x01, 0x02, 0xF7, 0x9E, 0x30, 0x7F, 0xF0, 0xF7 };

    const midi::universal_packet packets[] = {
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::note_off, 3, 0x45, 0x6E),
        []() {
            auto p = midi::make_sysex7_complete_packet(0);
            p.add_payload_byte(0x01);
            p.add_payload_byte(0x02);
            return p;
        }(),
        midi::make_midi1_channel_voice_message(0, midi::midi1_channel_voice_status::note_on, 14, 0x30, 0x7F)
    };

    testing::Sequence s;

    for (auto& r : packets)
    {
        EXPECT_CALL(c, on_packet(r)).Times(1).InSequence(s);
    }

    for (const auto b : byte_stream)
    {
        p.feed(b);
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, parser_large_sysex)
{
    testing::StrictMock<parser_client> c;

    midi::midi1_byte_stream_parser p(std::bind(&parser_client::on_packet, &c, std::placeholders::_1),
                                     std::bind(&parser_client::on_sysex, &c, std::placeholders::_1));

    midi::sysex7 sx{ 0x100000u };
    sx.data.reserve(1234);
    for (auto i = 0u; i < 1234; ++i)
    {
        sx.data.push_back(i % 128);
    }

    EXPECT_CALL(c, on_sysex(sx)).Times(1);

    p.feed(0xF0);
    p.feed(0x10);
    p.feed(sx.data.data(), sx.data.data() + sx.data.size());
    p.feed(0xF7);
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, size_utility_messages)
{
    for (std::uint32_t d = 0x0000; d < 0x0100; ++d)
    {
        midi::universal_packet m{ d << 16 };
        EXPECT_EQ(0u, midi::midi1_byte_stream_size(m));
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, size_system_messages)
{
    constexpr std::pair<unsigned, size_t> test_cases[]{
        { 0x10012345, 0 }, { 0x13112345, 0 }, { 0x1A254621, 0 }, { 0x16312345, 0 }, { 0x17412345, 0 },
        { 0x19512345, 0 }, { 0x1C654621, 0 }, { 0x12712345, 0 }, { 0x1D854621, 0 }, { 0x12912345, 0 },
        { 0x14A54621, 0 }, { 0x1FB12345, 0 }, { 0x17C54621, 0 }, { 0x14D12345, 0 }, { 0x10E54621, 0 },

        { 0x11F01234, 0 }, { 0x1CF14321, 2 }, { 0x16F21234, 3 }, { 0x19F34321, 2 }, { 0x1AF41234, 0 },
        { 0x11F54321, 0 }, { 0x12F61234, 1 }, { 0x18F74321, 0 }, { 0x1EF81234, 1 }, { 0x13F94321, 0 },
        { 0x14FA1234, 1 }, { 0x1BFB4321, 1 }, { 0x19FC1234, 1 }, { 0x18FD4321, 0 }, { 0x17FE1234, 1 },
        { 0x1FFF4321, 1 }
    };

    for (const auto& c : test_cases)
    {
        midi::universal_packet m{ c.first };
        EXPECT_EQ(c.second, midi::midi1_byte_stream_size(m));
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, size_channel_voice_messages)
{
    constexpr std::pair<unsigned, size_t> test_cases[]{ { 0x20012345, 0 }, { 0x23112345, 0 }, { 0x2A254621, 0 },
                                                        { 0x26312345, 0 }, { 0x27412345, 0 }, { 0x29512345, 0 },
                                                        { 0x2C654621, 0 }, { 0x22712345, 0 },

                                                        { 0x2D854621, 3 }, { 0x22912345, 3 }, { 0x24A54621, 3 },
                                                        { 0x2FB12345, 3 }, { 0x27C54621, 2 }, { 0x24D12345, 2 },
                                                        { 0x2AE41234, 3 },

                                                        { 0x2FFF4321, 0 } };

    for (const auto& c : test_cases)
    {
        midi::universal_packet m{ c.first };
        EXPECT_EQ(c.second, midi::midi1_byte_stream_size(m));
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, size_data_messages)
{
    constexpr std::pair<unsigned, size_t> test_cases[]{
        { 0x30002345, 0 }, { 0x33012345, 1 }, { 0x3A024621, 2 }, { 0x36032345, 3 }, { 0x37042345, 4 },
        { 0x39052345, 5 }, { 0x3C064621, 6 },

        { 0x32072345, 0 }, { 0x3D084621, 0 }, { 0x32092345, 0 }, { 0x340A4621, 0 }, { 0x3F0B2345, 0 },
        { 0x370C4621, 0 }, { 0x340D2345, 0 }, { 0x3A0E1234, 0 }, { 0x3F0F4321, 0 },

        { 0x30102345, 0 }, { 0x33112345, 1 }, { 0x3A124621, 2 }, { 0x36132345, 3 }, { 0x37142345, 4 },
        { 0x39152345, 5 }, { 0x3C164621, 6 },

        { 0x32172345, 0 }, { 0x3D184621, 0 }, { 0x32192345, 0 }, { 0x341A4621, 0 }, { 0x3F1B2345, 0 },
        { 0x371C4621, 0 }, { 0x341D2345, 0 }, { 0x3A1E1234, 0 }, { 0x3F1F4321, 0 },

        { 0x30202345, 0 }, { 0x33212345, 1 }, { 0x3A224621, 2 }, { 0x36232345, 3 }, { 0x37242345, 4 },
        { 0x39252345, 5 }, { 0x3C264621, 6 },

        { 0x32272345, 0 }, { 0x3D284621, 0 }, { 0x32292345, 0 }, { 0x342A4621, 0 }, { 0x3F2B2345, 0 },
        { 0x372C4621, 0 }, { 0x342D2345, 0 }, { 0x3A2E1234, 0 }, { 0x3F2F4321, 0 },

        { 0x30302345, 0 }, { 0x33312345, 1 }, { 0x3A324621, 2 }, { 0x36332345, 3 }, { 0x37342345, 4 },
        { 0x39352345, 5 }, { 0x3C364621, 6 },

        { 0x32372345, 0 }, { 0x3D384621, 0 }, { 0x32392345, 0 }, { 0x343A4621, 0 }, { 0x3F3B2345, 0 },
        { 0x373C4621, 0 }, { 0x343D2345, 0 }, { 0x3A3E1234, 0 }, { 0x3F3F4321, 0 },
    };

    for (const auto& c : test_cases)
    {
        midi::universal_packet m{ c.first };
        EXPECT_EQ(c.second, midi::midi1_byte_stream_size(m));
    }

    for (std::uint32_t d = 0x3040; d < 0x3100; ++d)
    {
        midi::universal_packet m{ d << 16 };
        EXPECT_EQ(0u, midi::midi1_byte_stream_size(m));
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, size_other_messages)
{
    for (std::uint32_t t = 0x4; t < 0xf; ++t)
    {
        for (std::uint32_t s = 0x00; s < 0x100; ++s)
        {
            midi::universal_packet m{ (t << 28) || (s << 16) };
            EXPECT_EQ(0u, midi::midi1_byte_stream_size(m));
        }
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, from_system_messages)
{
    using namespace midi;

    const std::pair<unsigned, universal_packet> test_cases[]{
        { 0x6876F0, universal_packet{} },
        { 0x1234F1, universal_packet{ 0x10F13412 } },
        { 0x4321F2, universal_packet{ 0x10F22143 } },
        { 0x2B3DF3, universal_packet{ 0x10F33D2B } },
        { 0x7C6AF4, universal_packet{} },
        { 0x7117F5, universal_packet{} },
        { 0x4231F6, universal_packet{ 0x10F63142 } },
        { 0x6759F7, universal_packet{} },
        { 0x6C7BF8, universal_packet{ 0x10F87B6C } },
        { 0x2748F9, universal_packet{} },
        { 0x1728FA, universal_packet{ 0x10FA2817 } },
        { 0x2817FB, universal_packet{ 0x10FB1728 } },
        { 0x1425FC, universal_packet{ 0x10FC2514 } },
        { 0x5C4DFD, universal_packet{} },
        { 0x141DFE, universal_packet{ 0x10FE1D14 } },
        { 0x105EFF, universal_packet{ 0x10FF5E10 } },
    };

    for (const auto& c : test_cases)
    {
        const auto v = c.first;

        const auto m = from_midi1_byte_stream((v & 0xFF), ((v >> 8) & 0xFF), ((v >> 16) & 0xFF));
        EXPECT_EQ(c.second, m);
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, from_channel_voice_messages)
{
    using namespace midi;

    const std::pair<unsigned, universal_packet> test_cases[]{
        { 0x687683, universal_packet{ 0x20837668 } }, { 0x12349E, universal_packet{ 0x209E3412 } },
        { 0x4321A4, universal_packet{ 0x20A42143 } }, { 0x2B3DB9, universal_packet{ 0x20B93D2B } },
        { 0x7C6ACC, universal_packet{ 0x20CC6A7C } }, { 0x7117D5, universal_packet{ 0x20D51771 } },
        { 0x4231E2, universal_packet{ 0x20E23142 } },
    };

    for (const auto& c : test_cases)
    {
        const auto v = c.first;

        const auto m = from_midi1_byte_stream((v & 0xFF), ((v >> 8) & 0xFF), ((v >> 16) & 0xFF));
        EXPECT_EQ(c.second, m);
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, from_invalid_data)
{
    using namespace midi;

    const midi::universal_packet r{ 0 };
    for (std::uint32_t v = 0x00; v < 0x80; ++v)
    {
        const auto m = from_midi1_byte_stream((v & 0xFF), ((v >> 8) & 0xFF), ((v >> 16) & 0xFF));
        EXPECT_EQ(r, m);
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, to_midi1_byte_stream_system)
{
    using namespace midi;

    uint8_t bs_buffer[8];

    EXPECT_EQ(0u, to_midi1_byte_stream(make_system_message(0, 0xF0), bs_buffer));

    {
        const uint8_t result[] = { 0xF1, 0x35 };
        const auto    bytes =
          to_midi1_byte_stream(make_system_message(0, system_status::mtc_quarter_frame, 0x35), bs_buffer);

        EXPECT_EQ(bytes, 2);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 0xF2, 0x37, 0x6A };
        const auto    bytes =
          to_midi1_byte_stream(make_system_message(0, system_status::song_position, 0x37, 0x6A), bs_buffer);

        EXPECT_EQ(bytes, 3);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 0xF3, 0x36 };
        const auto    bytes = to_midi1_byte_stream(make_system_message(0, system_status::song_select, 0x36), bs_buffer);

        EXPECT_EQ(bytes, 2);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    EXPECT_EQ(0u, to_midi1_byte_stream(make_system_message(0, 0xF4), bs_buffer));
    EXPECT_EQ(0u, to_midi1_byte_stream(make_system_message(0, 0xF5), bs_buffer));

    {
        const uint8_t result[] = { 0xF6 };
        const auto    bytes    = to_midi1_byte_stream(make_system_message(0, system_status::tune_request), bs_buffer);

        EXPECT_EQ(bytes, 1);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    EXPECT_EQ(0u, to_midi1_byte_stream(make_system_message(0, 0xF7), bs_buffer));

    {
        const uint8_t result[] = { 0xF8 };
        const auto    bytes    = to_midi1_byte_stream(make_system_message(0, system_status::clock), bs_buffer);

        EXPECT_EQ(bytes, 1);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    EXPECT_EQ(0u, to_midi1_byte_stream(make_system_message(0, 0xF9), bs_buffer));

    {
        const uint8_t result[] = { 0xFA };
        const auto    bytes    = to_midi1_byte_stream(make_system_message(0, system_status::start), bs_buffer);

        EXPECT_EQ(bytes, 1);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 0xFB };
        const auto    bytes    = to_midi1_byte_stream(make_system_message(0, system_status::cont), bs_buffer);

        EXPECT_EQ(bytes, 1);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 0xFC };
        const auto    bytes    = to_midi1_byte_stream(make_system_message(0, system_status::stop), bs_buffer);

        EXPECT_EQ(bytes, 1);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    EXPECT_EQ(0u, to_midi1_byte_stream(make_system_message(0, 0xFD), bs_buffer));

    {
        const uint8_t result[] = { 0xFE };
        const auto    bytes    = to_midi1_byte_stream(make_system_message(0, system_status::active_sense), bs_buffer);

        EXPECT_EQ(bytes, 1);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 0xFF };
        const auto    bytes    = to_midi1_byte_stream(make_system_message(0, system_status::reset), bs_buffer);

        EXPECT_EQ(bytes, 1);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, to_midi1_byte_stream_midi1_channel_voice)
{
    using namespace midi;

    uint8_t bs_buffer[8];

    // undefined MIDI 1 statuses
    for (status_t s = 0x00; s < 0x80; s += 0x10)
    {
        EXPECT_EQ(0u, to_midi1_byte_stream(make_midi1_channel_voice_message(0, s, 1, 2), bs_buffer));
    }

    {
        const uint8_t result[] = { 0x8A, 110, 100 };
        const auto    bytes =
          to_midi1_byte_stream(make_midi1_note_off_message(0, 10, 110, velocity{ uint7_t{ 100 } }), bs_buffer);

        EXPECT_EQ(bytes, 3);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 0x98, 88, 37 };
        const auto    bytes =
          to_midi1_byte_stream(make_midi1_note_on_message(0, 8, 88, velocity{ uint7_t{ 37 } }), bs_buffer);

        EXPECT_EQ(bytes, 3);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 0xAA, 18, 73 };
        const auto    bytes    = to_midi1_byte_stream(
          make_midi1_poly_pressure_message(7, 0xA, note_nr_t{ 18 }, controller_value{ uint7_t{ 73 } }), bs_buffer);

        EXPECT_EQ(bytes, 3);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 0xC5, 6 };
        const auto    bytes = to_midi1_byte_stream(make_midi1_program_change_message(4, 5, program_t{ 6 }), bs_buffer);

        EXPECT_EQ(bytes, 2);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 0xDC, 0x0A };
        const auto    bytes    = to_midi1_byte_stream(
          make_midi1_channel_pressure_message(0xE, 0xC, controller_value{ uint7_t{ 0xA } }), bs_buffer);

        EXPECT_EQ(bytes, 2);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 0xE6, 0x00, 0x40 };
        const auto    bytes    = to_midi1_byte_stream(make_midi1_pitch_bend_message(4, 6, pitch_bend{}), bs_buffer);

        EXPECT_EQ(bytes, 3);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    // more undefined MIDI 1 statuses
    for (status_t s = 0xF0; s < 0xFF; ++s)
    {
        EXPECT_EQ(0u, to_midi1_byte_stream(make_midi1_channel_voice_message(0, s, 1, 2), bs_buffer));
    }
}

//-----------------------------------------------

TEST_F(midi1_byte_stream, to_midi1_byte_stream_data)
{
    using namespace midi;

    uint8_t bs_buffer[8];

    // sysex 7 complete

    {
        const uint8_t result[] = { 0xF0, 1, 2, 3, 4, 5, 0xF7 };
        const auto    bytes    = to_midi1_byte_stream(universal_packet{ 0x34050102, 0x03040506 }, bs_buffer);

        EXPECT_EQ(bytes, 7);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 0xF0, 1, 2, 3, 4, 5, 6, 0xF7 };
        const auto    bytes    = to_midi1_byte_stream(universal_packet{ 0x34060102, 0x03040506 }, bs_buffer);

        EXPECT_EQ(bytes, 8);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 0xF0, 0xF7 };
        const auto    bytes    = to_midi1_byte_stream(universal_packet{ 0x34000102, 0x03040506 }, bs_buffer);

        EXPECT_EQ(bytes, 2);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    for (uint32_t s = 0x07; s < 0x10; ++s)
    {
        EXPECT_EQ(0u, to_midi1_byte_stream(universal_packet{ 0x39000102 + (s << 16) }, bs_buffer));
    }

    // sysex 7 start

    {
        const uint8_t result[] = { 0xF0, 1 };
        const auto    bytes    = to_midi1_byte_stream(universal_packet{ 0x34110102, 0x03040506 }, bs_buffer);

        EXPECT_EQ(bytes, 2);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 0xF0, 1, 2, 3, 4, 5, 6 };
        const auto    bytes    = to_midi1_byte_stream(universal_packet{ 0x34160102, 0x03040506 }, bs_buffer);

        EXPECT_EQ(bytes, 7);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 0xF0 };
        const auto    bytes    = to_midi1_byte_stream(universal_packet{ 0x34100102, 0x03040506 }, bs_buffer);

        EXPECT_EQ(bytes, 1);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    for (uint32_t s = 0x17; s < 0x20; ++s)
    {
        EXPECT_EQ(0u, to_midi1_byte_stream(universal_packet{ 0x39000102 + (s << 16) }, bs_buffer));
    }

    // sysex 7 continue

    {
        const uint8_t result[] = { 1, 2, 3, 4 };
        const auto    bytes    = to_midi1_byte_stream(universal_packet{ 0x34240102, 0x03040506 }, bs_buffer);

        EXPECT_EQ(bytes, 4);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 1, 2, 3, 4, 5, 6 };
        const auto    bytes    = to_midi1_byte_stream(universal_packet{ 0x34260102, 0x03040506 }, bs_buffer);

        EXPECT_EQ(bytes, 6);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    EXPECT_EQ(0u, to_midi1_byte_stream(universal_packet{ 0x34200102, 0x03040506 }, bs_buffer));

    for (uint32_t s = 0x27; s < 0x30; ++s)
    {
        EXPECT_EQ(0u, to_midi1_byte_stream(universal_packet{ 0x39000102 + (s << 16) }, bs_buffer));
    }

    // sysex 7 end

    {
        const uint8_t result[] = { 1, 2, 3, 0xF7 };
        const auto    bytes    = to_midi1_byte_stream(universal_packet{ 0x34330102, 0x03040506 }, bs_buffer);

        EXPECT_EQ(bytes, 4);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 1, 2, 3, 4, 5, 6, 0xF7 };
        const auto    bytes    = to_midi1_byte_stream(universal_packet{ 0x34360102, 0x03040506 }, bs_buffer);

        EXPECT_EQ(bytes, 7);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    {
        const uint8_t result[] = { 0xF7 };
        const auto    bytes    = to_midi1_byte_stream(universal_packet{ 0x34300102, 0x03040506 }, bs_buffer);

        EXPECT_EQ(bytes, 1);
        EXPECT_EQ(0, memcmp(result, bs_buffer, bytes));
    }

    // invalid or non-MIDI1 statuses
    for (uint32_t s = 0x37; s < 0xFF; ++s)
    {
        EXPECT_EQ(0u, to_midi1_byte_stream(universal_packet{ 0x39000102 + (s << 16) }, bs_buffer));
    }
}

//-----------------------------------------------
