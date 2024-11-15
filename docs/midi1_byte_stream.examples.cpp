#include <midi/midi1_byte_stream.h>

void midi1_byte_stream_parser_examples()
{
    // from channel_voice_message.examples.cpp
    extern void channel_voice_message_processor(const midi::universal_packet& p);

    using namespace midi;

    auto process_packet = [](midi::universal_packet p) { channel_voice_message_processor(p); };
    auto process_sysex7 = [](const midi::sysex7&) {};

    midi1_byte_stream_parser p(process_packet, process_sysex7);

    const std::vector<uint8_t> midi1{ 0x90, 0x12, 0x34, 0xB0, 0x44, 0x75, 0xE0, 0x33,
                                      0x44, 0xA0, 0x12, 0x60, 0x80, 0x12, 0x40 };
    p.feed(midi1.data(), midi1.size());
}

void from_to_midi1_byte_stream_examples()
{
    using namespace midi;

    auto m1 = from_midi1_byte_stream(0x90, 0x12, 0x34);
    auto m2 = from_midi1_byte_stream(0xB0, 0x44, 0x75);

    auto send_bytes = [](uint8_t*, size_t) {};

    uint8_t bytes[8];
    auto    cnt = to_midi1_byte_stream(universal_packet{ 0x20E03344 }, bytes);
    send_bytes(bytes, cnt);

    cnt = to_midi1_byte_stream(universal_packet{ 0x20801240 }, bytes);
    send_bytes(bytes, cnt);
}

void run_midi1_byte_stream_examples()
{
    midi1_byte_stream_parser_examples();
    from_to_midi1_byte_stream_examples();
}
