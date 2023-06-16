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

#include <midi/data_message.h>
#include <midi/sysex.h>

#include <cassert>
#include <vector>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

void data_message::set_payload_size(size_t size)
{
    assert(size <= 6);
    set_byte(1, (status() & 0xF0) + (size & 0x0F));
}

//--------------------------------------------------------------------------

void data_message::add_payload_byte(uint8_t byte)
{
    const auto size = payload_size();
    assert(size < 6);
    set_byte_7bit(2 + size, byte);
    set_payload_size(size + 1);
}

//--------------------------------------------------------------------------

std::vector<data_message> as_sysex7_packets(const sysex7& sysex, group_t group)
{
    const size_t manufacturerIDBytes = (sysex.manufacturerID & 0x00FFFF) ? 3 : 1;
    const size_t payloadBytes        = sysex.data.size() + manufacturerIDBytes;

    // initialize first packet
    auto p = (payloadBytes <= 6) ? make_sysex7_complete_packet(group) : make_sysex7_start_packet(group);

    // write manufacturerID bytes
    p.add_payload_byte((sysex.manufacturerID >> 16) & 0xFF);
    if (manufacturerIDBytes > 1)
    {
        p.add_payload_byte((sysex.manufacturerID >> 8) & 0xFF);
        p.add_payload_byte(sysex.manufacturerID & 0xFF);
    }

    // process data bytes
    std::vector<data_message> result;
    result.reserve(payloadBytes / 6 + 1);

    size_t dataBytesLeft = sysex.data.size();
    for (auto b : sysex.data)
    {
        p.add_payload_byte(b);
        --dataBytesLeft;

        if (p.payload_size() == 6) // packet full?
        {
            result.push_back(p);

            p = (dataBytesLeft <= 6) ? make_sysex7_end_packet(group) : make_sysex7_continue_packet(group);
        }
    }

    if (p.payload_size())
    {
        result.push_back(p);
    }

    return result;
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
