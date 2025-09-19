#include <midi/flex_data_message.h>

void flex_data_message_examples()
{
    using namespace midi;

    auto a = make_flex_data_message(0, packet_format::complete, packet_address::channel, 5, 0x20, 0x01, 0x123456);
    assert(is_flex_data_message(a));

    if (auto m = as_flex_data_message_view(a))
    {
        // inspect message fields
        assert(m->group() == 0);
        assert(m->format() == packet_format::complete);
        assert(m->address() == packet_address::channel);
        assert(m->channel() == 5);
        assert(m->status_bank() == 0x20);
        assert(m->status() == 0x01);
        assert(m->data1() == 0x123456);
        assert(m->data2() == 0);
        assert(m->data3() == 0);
    }

    auto t =
      make_flex_data_text_message(0, packet_format::complete, packet_address::channel, 5, 0x20, 0x01, "Hello World!");
    if (auto m = as_flex_data_message_view(t))
    {
        assert(m->payload_as_string() == "Hello World!");
    }

    auto tmpo = make_set_tempo_message(0, 5000000);
    if (auto m = as_flex_data_message_view(tmpo))
    {
        assert(m->status_bank() == 0x00);
        assert(m->status() == 0x00);
        assert(m->data1() == 5000000);
    }

    constexpr uint8_t numerator     = 4;
    constexpr uint8_t denominator   = 2; // 4 (2^2) denominator
    constexpr uint8_t nr_32rd_notes = 8; // 4/4 time signature

    auto tsig = make_set_time_signature_message(0, numerator, denominator, nr_32rd_notes);
    if (auto m = as_flex_data_message_view(tsig))
    {
        assert(m->status_bank() == 0x00);
        assert(m->status() == 0x01);
        assert(m->data_byte1() == numerator);
        assert(m->data_byte2() == denominator);
        assert(m->data_byte3() == nr_32rd_notes);
    }

    constexpr uint8_t num_clocks_per_primary_click = 24;
    constexpr uint8_t bar_accent_part1             = 3;
    constexpr uint8_t bar_accent_part2             = 2;
    constexpr uint8_t bar_accent_part3             = 0;
    constexpr uint8_t num_subdivision_clicks1      = 2;
    constexpr uint8_t num_subdivision_clicks2      = 0;

    auto metro = make_set_metronome_message(0,
                                            num_clocks_per_primary_click,
                                            bar_accent_part1,
                                            bar_accent_part2,
                                            bar_accent_part3,
                                            num_subdivision_clicks1,
                                            num_subdivision_clicks2);
    if (auto m = as_flex_data_message_view(metro))
    {
        assert(m->status_bank() == 0x00);
        assert(m->status() == 0x02);
        assert(m->data_byte1() == num_clocks_per_primary_click);
        assert(m->data_byte2() == bar_accent_part1);
        assert(m->data_byte3() == bar_accent_part2);
        assert(m->data_byte4() == bar_accent_part3);
        assert(m->data_byte5() == num_subdivision_clicks1);
        assert(m->data_byte6() == num_subdivision_clicks2);
    }

    constexpr uint4_t sharps_or_flats = 5;   // 5 sharps
    constexpr uint4_t tonic_note      = 0x4; // tonic D

    auto ksig = make_set_key_signature_message(0, packet_address::channel, 5, sharps_or_flats, tonic_note); // D major
    if (auto m = as_flex_data_message_view(ksig))
    {
        assert(m->status_bank() == 0x00);
        assert(m->status() == 0x05);
        assert((m->data_byte1() >> 4) == sharps_or_flats);
        assert((m->data_byte1() & 0x0F) == tonic_note);
    }
}

void run_flex_data_message_examples()
{
    flex_data_message_examples();
}
