#include <midi/data_message.h>

#include <cassert>

void sysex7_message_examples()
{
    using namespace midi;

    auto p1 = make_sysex7_complete_packet(group_t{ 4 });
    p1.add_payload_byte(0x7D);
    p1.add_payload_byte(0x01);
    p1.add_payload_byte(0x02);

    assert(is_data_message(p1));

    if (auto m1 = as_sysex7_packet_view(p1))
    {
        assert(m1->payload_size() == 3);
        assert(m1->payload_byte(0) == 0x7D);
        assert(m1->payload_byte(1) == 1);
        assert(m1->payload_byte(2) == 2);
    }

    sysex7_packet p2[] = { make_sysex7_start_packet(group_t{ 7 }),
                           make_sysex7_continue_packet(group_t{ 7 }),
                           make_sysex7_end_packet(group_t{ 7 }) };

    const char     payload[] = { 0x7D, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    sysex7_packet* dest      = p2;
    for (auto b : payload)
    {
        dest->add_payload_byte(b);
        if (dest->payload_size() == 6)
            ++dest;
    }

    assert(is_sysex7_packet(p2[0]));
    auto m21 = sysex7_packet_view{ p2[0] };
    assert(m21.payload_size() == 6);
    assert(m21.payload_byte(5) == 5);

    assert(is_sysex7_packet(p2[1]));
    auto m22 = sysex7_packet_view{ p2[1] };
    assert(m22.payload_size() == 6);
    assert(m22.payload_byte(3) == 9);

    assert(is_sysex7_packet(p2[1]));
    auto m23 = sysex7_packet_view{ p2[2] };
    assert(m23.payload_size() == 4);
    assert(m23.payload_byte(3) == 15);
}

void run_data_message_examples()
{
    sysex7_message_examples();
}
