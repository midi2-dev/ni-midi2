#include <midi/prop073_tunnel.h>

//--------------------------------------------------------------------------

#include <cassert>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

sysex7 to_prop073_tunnel_message(const universal_packet& msg)
{
    const auto num_words = msg.size();

    sysex7 result{ manufacturer::educational };
    result.data.reserve(num_words * 5);

    for (auto w = 0u; w < num_words; ++w)
    {
        const auto    word = msg.data[w];
        const uint8_t top_bits =
          ((word >> 28u) & 0x8) | ((word >> 21u) & 0x4) | ((word >> 14u) & 0x2) | ((word >> 7u) & 0x1);

        result.data.push_back(top_bits | static_cast<uint8_t>(w ? 0 : ((num_words - 1) << 4)));
        result.data.push_back((word >> 24u) & 0x7F);
        result.data.push_back((word >> 16u) & 0x7F);
        result.data.push_back((word >> 8u) & 0x7F);
        result.data.push_back(word & 0x7F);
    }

    return result;
}

//--------------------------------------------------------------------------

universal_packet from_prop073_tunnel_message(const sysex7& sx)
{
    universal_packet result;
    if ((sx.manufacturerID == manufacturer::educational) && !sx.data.empty())
    {
        size_t num_words = ((sx.data[0] & 0x70) >> 4) + 1;
        if (sx.data.size() >= num_words * 5)
        {
            const auto* bytes = sx.data.data();
            for (auto w = 0u; w < num_words; ++w, bytes += 5)
            {
                result.data[w] = ((bytes[0] & 0x8) << 28u) | ((bytes[0] & 0x4) << 21u) | ((bytes[0] & 0x2) << 14u) |
                                 ((bytes[0] & 0x1) << 7u) | (bytes[1] << 24u) | (bytes[2] << 16u) | (bytes[3] << 8u) |
                                 bytes[4];
            }
        }

        assert(result.size() == num_words);
    }

    return result;
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
