#include <midi/manufacturer.h>
#include <midi/mixed_data_set.h>
#include <midi/mixed_data_set_collector.h>

#include <cassert>

void mixed_data_set_examples()
{
    using namespace midi;

    // a mixed data set with some payload data
    mixed_data_set mds{ manufacturer::native_instruments, 0, 0, 0 };
    mds.data = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };

    for (const auto& p : as_mixed_data_set_packets(mds, 0x0))
    {
        // send packet
        // ...
        (void)p;
    }

    // or send packets without allocating a vector
    send_mixed_data_set(mds, 0x0, 0, [](const universal_packet& p) {
        // send packet
        // ...
        (void)p;
    });
}

void mixed_data_set_collector_examples()
{
    using namespace midi;

    universal_packet p;

    mixed_data_set_collector c{ [](const mixed_data_set& mds, uint4_t mds_id) {
        // do something with message
        // ...
        (void)mds;
        (void)mds_id;
    } };

    if (is_mixed_data_set_packet(p))
        c.feed(p);
}

void run_mixed_data_set_examples()
{
    mixed_data_set_examples();
    mixed_data_set_collector_examples();
}
