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

#include <midi/sysex.h>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

size_t sysex::total_data_size() const
{
    if (manufacturerID)
    {
        return data.size() + ((manufacturerID & 0xFF0000) ? 1u : 3u);
    }

    return data.size();
}

//--------------------------------------------------------------------------

bool sysex::empty() const
{
    return (manufacturerID == 0) && data.empty();
}

//--------------------------------------------------------------------------

bool sysex::is_7bit() const
{
    for (const auto b : data)
        if (b & 0x80)
            return false;

    return true;
}

//--------------------------------------------------------------------------

bool sysex::is_8bit() const
{
    for (const auto b : data)
        if (b & 0x80)
            return true;

    return false;
}

//--------------------------------------------------------------------------

void sysex::clear()
{
    manufacturerID = 0;
    data.clear();

    if (data.capacity() > 16384)
        data.shrink_to_fit();
}

//-----------------------------------------------

void sysex7::add_device_identity(const device_identity& identity)
{
    assert((identity.manufacturer & 0xFF808080) == 0);
    assert((identity.family & 0x8080) == 0);
    assert((identity.model & 0x8080) == 0);
    assert((identity.revision & 0x80808080) == 0);

    data.push_back((identity.manufacturer >> 16) & 0x7F);
    data.push_back((identity.manufacturer >> 8) & 0x7F);
    data.push_back(identity.manufacturer & 0x7F);

    data.push_back(identity.family & 0x7F);
    data.push_back((identity.family >> 8) & 0x7F);

    data.push_back(identity.model & 0x7F);
    data.push_back((identity.model >> 8) & 0x7F);

    data.push_back(identity.revision & 0x7F);
    data.push_back((identity.revision >> 8) & 0x7F);
    data.push_back((identity.revision >> 16) & 0x7F);
    data.push_back((identity.revision >> 24) & 0x7F);
}

//-----------------------------------------------

device_identity sysex7::make_device_identity(size_t data_pos) const
{
    assert(data_pos + 10 < data.size());
    const auto* const d = data.data() + data_pos;

    device_identity result;
    result.manufacturer = (d[0] << 16) | (d[1] << 8) | d[2];
    result.family       = d[3] | (d[4] << 8);
    result.model        = d[5] | (d[6] << 8);
    result.revision     = d[7] | (d[8] << 8) | (d[9] << 16) | (d[10] << 24);
    return result;
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
