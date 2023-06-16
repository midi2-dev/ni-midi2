//
// Copyright (c) 2023 Native Instruments
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

//--------------------------------------------------------------------------

#include <midi/types.h>
#include <midi/universal_packet.h>

//--------------------------------------------------------------------------

#include <cassert>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

struct utility_message : universal_packet
{
    constexpr utility_message() = default;
    constexpr explicit utility_message(status_t, uint16_t payload = 0u);
    ~utility_message() = default;

    // utility messages are groupless since UMP 1.1
    constexpr group_t group() const      = delete;
    constexpr void    set_group(group_t) = delete;
};

//--------------------------------------------------------------------------

struct utility_message_view
{
    constexpr explicit utility_message_view(const universal_packet& ump)
      : p(ump)
    {
        assert(p.type() == packet_type::utility);
    }

    constexpr status_t status() const { return p.status(); }
    constexpr uint16_t payload() const { return static_cast<uint16_t>(p.byte3() << 8) | p.byte4(); }

  private:
    const universal_packet& p;
};

//--------------------------------------------------------------------------

constexpr utility_message make_utility_message(status_t, uint16_t data);

//--------------------------------------------------------------------------
// constexpr implementations
//--------------------------------------------------------------------------

constexpr utility_message::utility_message(status_t status, uint16_t payload)
  : universal_packet(static_cast<uint32_t>(status << 16) | payload)
{
}

//--------------------------------------------------------------------------

constexpr utility_message make_utility_message(status_t status, uint16_t data)
{
    return utility_message{ status, data };
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
