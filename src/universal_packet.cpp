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

#include <midi/universal_packet.h>

//--------------------------------------------------------------------------

#include <cassert>
#include <iomanip>
#include <iostream>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

namespace {

    struct iobase_restore_flags
    {
        explicit iobase_restore_flags(std::ios_base& strm)
          : strm(strm)
        {
        }

        ~iobase_restore_flags() { strm.flags(flags); }

        std::ios_base&          strm;
        std::ios_base::fmtflags flags = strm.flags();
    };

} // namespace

//--------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const universal_packet& p)
{
    iobase_restore_flags flag_restorer(out);

    for (auto w = 0u; w < p.size(); ++w)
    {
        if (w)
            out << ' ';
        out << std::hex << std::setfill('0') << std::setw(8) << p.data[w];
    }
    return out;
}

//--------------------------------------------------------------------------

std::istream& operator>>(std::istream& in, universal_packet& p)
{
    iobase_restore_flags flag_restorer(in);
    in >> std::hex >> p.data[0];
    if (in.good())
    {
        const auto num_words = p.size();
        for (auto w = 1u; w < num_words; ++w)
        {
            in >> std::hex >> p.data[w];
            if (!in.good())
                break;
        }
    }

    return in;
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
