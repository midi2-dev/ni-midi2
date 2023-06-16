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
