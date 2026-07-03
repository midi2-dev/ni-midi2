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

#pragma once

//--------------------------------------------------------------------------

#include <midi/extended_data_message.h>
#include <midi/mixed_data_set.h>
#include <midi/universal_packet.h>

#include <array>
#include <functional>
#include <utility>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

class mixed_data_set_collector
{
  public:
    using callback = std::function<void(const mixed_data_set&, uint4_t mds_id)>;

    explicit mixed_data_set_collector(callback);

    void set_callback(callback);
    void set_max_data_size(size_t); //!< limit maximum size of accepted mixed data set data

    void feed(const universal_packet&);
    void reset();

  private:
    //! collection state, one per mds_id
    //! (up to 16 mixed data sets can be transmitted simultaneously per group)
    struct assembly
    {
        mixed_data_set data;
        uint16_t       nr_of_chunks{ 0 }; //!< declared by most recent chunk header, 0 means unknown
        uint16_t       chunks_collected{ 0 };
        size_t         chunk_bytes_left{ 0 }; //!< payload data bytes still expected in current chunk
        bool           collecting{ false };

        void reset();
    };

    void process_header(const mixed_data_set_header_packet_view&);
    void process_payload(const mixed_data_set_payload_packet_view&);
    void complete_chunk(assembly&, uint4_t mds_id);

    std::array<assembly, 16> m_assemblies;
    size_t                   m_max_data_size{ 0 };
    callback                 m_cb;
};

//--------------------------------------------------------------------------

inline mixed_data_set_collector::mixed_data_set_collector(callback cb)
  : m_cb(std::move(cb))
{
}
inline void mixed_data_set_collector::set_callback(callback cb)
{
    m_cb = std::move(cb);
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
