//
// Copyright (c) 2026 Native Instruments
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

#include <midi/mixed_data_set_collector.h>

#include <algorithm>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

void mixed_data_set_collector::set_max_data_size(size_t s)
{
    m_max_data_size = s;
}

//--------------------------------------------------------------------------

void mixed_data_set_collector::feed(const universal_packet& p)
{
    if (is_mixed_data_set_header_packet(p))
    {
        process_header(mixed_data_set_header_packet_view{ p });
    }
    else if (is_mixed_data_set_payload_packet(p))
    {
        process_payload(mixed_data_set_payload_packet_view{ p });
    }
}

//--------------------------------------------------------------------------

void mixed_data_set_collector::reset()
{
    for (auto& a : m_assemblies)
    {
        a.reset();
    }
}

//--------------------------------------------------------------------------

void mixed_data_set_collector::assembly::reset()
{
    data.clear();
    nr_of_chunks     = 0;
    chunks_collected = 0;
    chunk_bytes_left = 0;
    collecting       = false;
}

//--------------------------------------------------------------------------

void mixed_data_set_collector::process_header(const mixed_data_set_header_packet_view& h)
{
    auto& a = m_assemblies[h.mds_id()];

    const auto chunk_nr = h.chunk_nr();
    if (chunk_nr == 0)
    {
        // mixed data set aborted by sender
        a.reset();
        return;
    }

    if (chunk_nr == 1)
    {
        // start of a new mixed data set
        a.reset();
        a.collecting          = true;
        a.data.manufacturerID = manufacturer_from_16bit_id(h.manufacturer_id());
        a.data.deviceID       = h.device_id();
        a.data.subID1         = h.sub_id_1();
        a.data.subID2         = h.sub_id_2();
    }
    else if (!a.collecting || (chunk_nr != a.chunks_collected + 1))
    {
        // invalid chunk sequence, reset
        a.reset();
        return;
    }

    a.nr_of_chunks = h.nr_of_chunks();
    if (a.nr_of_chunks && (chunk_nr > a.nr_of_chunks))
    {
        // invalid chunk header, reset
        a.reset();
        return;
    }

    // derive the number of chunk data bytes from the valid bytes in chunk
    // (16 header bytes plus two status bytes per payload packet)
    const auto valid_bytes = h.valid_bytes_in_chunk();
    if (valid_bytes < 16)
    {
        // invalid chunk header, reset
        a.reset();
        return;
    }
    const size_t valid_payload_bytes = valid_bytes - 16u;
    const size_t nr_of_packets       = (valid_payload_bytes + 15u) / 16u;
    if (valid_payload_bytes < 2 * nr_of_packets)
    {
        // invalid chunk header, reset
        a.reset();
        return;
    }
    a.chunk_bytes_left = valid_payload_bytes - 2 * nr_of_packets;

    if (m_max_data_size && (a.data.data.size() + a.chunk_bytes_left > m_max_data_size))
    {
        // data size exceeds m_max_data_size, wait for start of new mixed data set
        a.reset();
        return;
    }
    a.data.data.reserve(a.data.data.size() + a.chunk_bytes_left);

    if (a.chunk_bytes_left == 0)
    {
        // header only chunk
        complete_chunk(a, h.mds_id());
    }
}

//--------------------------------------------------------------------------

void mixed_data_set_collector::process_payload(const mixed_data_set_payload_packet_view& p)
{
    auto& a = m_assemblies[p.mds_id()];

    if (!a.collecting || (a.chunk_bytes_left == 0))
    {
        // unexpected payload packet, ignore
        return;
    }

    const auto numBytes = std::min(mixed_data_set_payload_packet::payload_size, a.chunk_bytes_left);
    for (size_t b = 0; b < numBytes; ++b)
    {
        a.data.data.push_back(p.payload_byte(b));
    }
    a.chunk_bytes_left -= numBytes;

    if (a.chunk_bytes_left == 0)
    {
        complete_chunk(a, p.mds_id());
    }
}

//--------------------------------------------------------------------------

void mixed_data_set_collector::complete_chunk(assembly& a, uint4_t mds_id)
{
    ++a.chunks_collected;

    if (a.nr_of_chunks == a.chunks_collected)
    {
        // final chunk collected, mixed data set is complete
        if (m_cb)
        {
            m_cb(a.data, mds_id);
        }
        a.reset();
    }
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
