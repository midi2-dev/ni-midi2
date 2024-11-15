#include <midi/system_message.h>

void system_message_examples()
{
    using namespace midi;

    auto a = make_system_message(0, system_status::clock);
    if (is_system_message(a))
    {
        auto m = system_message_view{ a };

        group_t g = m.group();
        status_t s = m.status();
    }

    auto b = make_system_message(0, system_status::song_select, 44);
    if (auto m = as_system_message_view(b))
    {
        group_t g = m->group();
        status_t s = m->status();
        uint7_t d1 = m->data_byte_1();
    }

    auto c = make_song_position_message(0, 0x1234);
    if (auto m = as_system_message_view(c))
    {
        group_t g = m->group();
        status_t s = m->status();
        uint7_t d1 = m->data_byte_1();
        uint7_t d2 = m->data_byte_2();
        uint14_t pos = m->get_song_position();
    }
}

void system_message_readme_code()
{
    using namespace midi;

    auto packet = make_system_message(0, system_status::song_select, 4);

    if (packet.type() == packet_type::system)
    {
        auto m = system_message_view{ packet };

        // access message data
        if (m.status() == system_status::song_select)
        {
            auto song = m.data_byte_1();
        }
    }

    if (auto m = as_system_message_view(packet))
    {
        // access message data
        if (m->status() == system_status::song_position)
        {
            auto pos = m->get_song_position();
        }
    }
}

void run_system_message_examples()
{
    system_message_examples();
    system_message_readme_code();
}
