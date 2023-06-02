#pragma once

//--------------------------------------------------------------------------

#include <midi/types.h>
#include <midi/universal_packet.h>

//--------------------------------------------------------------------------

#include <cassert>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

struct extended_data_message : universal_packet
{
    constexpr extended_data_message();
    constexpr explicit extended_data_message(status_t);
    ~extended_data_message() = default;
};

//--------------------------------------------------------------------------

constexpr bool is_extended_data_message(const universal_packet&);

//--------------------------------------------------------------------------
// constexpr implementations
//--------------------------------------------------------------------------

constexpr extended_data_message::extended_data_message()
  : universal_packet(0x50000000)
{
}
constexpr extended_data_message::extended_data_message(status_t status)
  : universal_packet(0x50000000u | (status << 16))
{
}

//--------------------------------------------------------------------------

constexpr bool is_extended_data_message(const universal_packet& p)
{
    return p.type() == packet_type::extended_data;
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
