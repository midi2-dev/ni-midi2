#include <midi/sysex.h>

void sysex_examples()
{
    using namespace midi;

    {
        sysex7 sx{ manufacturer::native_instruments, { 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

        assert(sx.is_7bit());
        assert(sx.is_valid());
        assert(sx.data.size() == 9);
    }

    {
        sysex8 sx{ manufacturer::native_instruments, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0x88 } };

        assert(!sx.is_7bit());
        assert(sx.is_8bit());
        assert(sx.data.size() == 10);
    }

    {
        sysex sx{ manufacturer::native_instruments, { 1, 2 } };

        assert(sx.is_7bit());
        assert(!sx.is_8bit());
        assert(sx.data.size() == 2);
    }

    {
        sysex7 sx;
        sx.manufacturerID = manufacturer::educational;

        sx.add_uint7(42);
        sx.add_uint28(10);
        const uint8_t some_data[] = { 1, 2, 3, 4 };
        sx.add_data(some_data, sizeof(some_data));

        assert(sx.is_7bit());
        assert(sx.data.size() == 9);

        const auto identity = device_identity{ midi::manufacturer::native_instruments, 0x1730, 49, 0x00010005 };
        sx.add_device_identity(identity);

        assert(sx.is_7bit());
        assert(sx.data.size() == 20);

        // read data
        auto v = sx.make_uint28(1);
        assert(v == 10);

        auto i = sx.make_device_identity(9);
        assert(i.manufacturer == identity.manufacturer);
        assert(i.family == identity.family);
        assert(i.model == identity.model);
        assert(i.revision == identity.revision);
    }
}

void send_sysex_examples()
{
    using namespace midi;

    auto send_packet = [](const universal_packet&) {};

    sysex7  sx7{ manufacturer::native_instruments, { 1, 2, 3, 4, 5, 6, 7, 8, 9 } };
    group_t group = 4;
    send_sysex7(sx7, group, send_packet);

    sysex8  sx8{ manufacturer::native_instruments, { 1, 2, 3, 4, 5, 6, 7, 8, 9 } };
    uint8_t stream_id = 0;
    send_sysex8(sx8, stream_id, group, send_packet);
}

void sysex_as_packets_examples()
{
    using namespace midi;

    sysex7 sx7{ manufacturer::native_instruments, { 1, 2, 3, 4, 5, 6, 7, 8, 9 } };
    auto   packets1 = as_sysex7_packets(sx7, 9);

    sysex8 sx8{ manufacturer::native_instruments, { 1, 2, 3, 4, 5, 6, 7, 8, 9 } };
    auto   packets2 = as_sysex8_packets(sx8, 0);
}

void run_sysex_examples()
{
    sysex_examples();
    send_sysex_examples();
    sysex_as_packets_examples();
}
