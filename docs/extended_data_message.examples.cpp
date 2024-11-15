#include <midi/extended_data_message.h>

#include <cassert>
#include <numeric>
#include <vector>

void sysex8_message_examples()
{
    using namespace midi;

    auto p1 = make_sysex8_complete_packet(uint8_t { 1 }, group_t { 4 });
    p1.add_payload_byte(0xFF);
    p1.add_payload_byte(0xF1);
    p1.add_payload_byte(0xF2);

    assert(is_extended_data_message(p1));

    if (auto m1 = as_sysex8_packet_view(p1))
    {
        assert(m1->payload_size() == 3);
        assert(m1->payload_byte(0) == 0xFF);
        assert(m1->payload_byte(1) == 0xF1);
        assert(m1->payload_byte(2) == 0xF2);
    }

    sysex8_packet p2[] = {
        make_sysex8_start_packet(uint8_t { 2 }, group_t { 7 }),
        make_sysex8_continue_packet(uint8_t { 2 }, group_t { 7 }),
        make_sysex8_end_packet(uint8_t { 2 }, group_t { 7 })
    };

    std::vector<uint8_t> payload(30);
    std::iota(payload.begin(), payload.end(), 200);

    sysex8_packet *dest = p2;
    for (auto b : payload)
    {
        dest->add_payload_byte(b);
        if (dest->payload_size() == 13) ++dest;
    }

    assert(is_sysex8_packet(p2[0]));
    auto m21 = sysex8_packet_view{ p2[0] };
    assert(m21.payload_size() == 13);
    assert(m21.payload_byte(12) == 212);

    assert(is_sysex8_packet(p2[1]));
    auto m22 = sysex8_packet_view{ p2[1] };
    assert(m22.payload_size() == 13);
    assert(m22.payload_byte(7) == 220);

    assert(is_sysex8_packet(p2[1]));
    auto m23 = sysex8_packet_view{ p2[2] };
    assert(m23.payload_size() == 4);
    assert(m23.payload_byte(3) == 229);
}

void run_extended_data_message_examples()
{
    sysex8_message_examples();
}
