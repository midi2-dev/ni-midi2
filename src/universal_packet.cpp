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
