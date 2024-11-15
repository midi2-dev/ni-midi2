#include <midi/sysex_collector.h>

void sysex_collector_readme_code()
{
    using namespace midi;

    universal_packet p;

    {
        sysex7_collector c{ [](const sysex7& s) {
            // do something with message
            // ...
        } };

        if (is_sysex7_packet(p))
            c.feed(p);
    }

    {
        sysex8_collector c{ [](const sysex8& s, uint8_t stream_id) {
            // do something with message
            // ...
        } };

        if (is_sysex8_packet(p))
            c.feed(p);
    }
}

void run_sysex_collector_examples()
{
    sysex_collector_readme_code();
}
