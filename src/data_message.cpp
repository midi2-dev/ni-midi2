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

} // namespace midi

//--------------------------------------------------------------------------
