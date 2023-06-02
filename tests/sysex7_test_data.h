#pragma once

#include <midi/data_message.h>
#include <midi/sysex.h>

#include <string>
#include <vector>

namespace midi {

struct sysex7_test_case
{
    std::string                   description;
    std::vector<universal_packet> packets;
    sysex7                        sysex;
};

extern std::vector<sysex7_test_case> sysex7_test_cases;

} // namespace midi
