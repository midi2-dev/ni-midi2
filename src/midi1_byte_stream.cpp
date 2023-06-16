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

#include <midi/midi1_byte_stream.h>

#include <midi/data_message.h>
#include <midi/midi1_channel_voice_message.h>
#include <midi/system_message.h>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

void midi1_byte_stream_parser::feed(uint8_t byte)
{
    if (byte >= system_status::clock) // realtime messages
    {
        system_realtime(byte);
        return;
    }

    if (m_packet.type() == packet_type::data) // SysEx in progress?
    {
        if (byte < 0x80) // SysEx data
        {
            if (has_sysex_callback()) // SysEx callbacks enabled?
            {
                sysex_continue_callback(byte);
            }
            else
            {
                sysex_continue_packet(byte);
            }

            return;
        }
        else // end of SysEx
        {
            if (byte != 0xF7) // cancel invalid SysEx message
            {
                m_packet = universal_packet{};
            }
            else // end of SysEx
            {
                if (has_sysex_callback()) // SysEx callbacks enabled?
                {
                    sysex_end_callback();
                }
                else
                {
                    sysex_end_packet();
                }

                return;
            }
        }
    }

    if (byte == 0xF0) // SysEx start
    {
        sysex_start();
    }
    else if (byte > 0xF0) // System Common
    {
        system_common(byte);
    }
    else if (byte >= 0x80) // channel voice message
    {
        channel_voice(byte);
    }
    else if (m_packet.status() && m_num_missing_bytes && (m_packet_byte < 4))
    {
        m_packet.set_byte(m_packet_byte, byte);
        --m_num_missing_bytes;
        if (m_num_missing_bytes == 0)
        {
            // handle running status
            if (m_packet.status() < 0xF0)
            {
                m_num_missing_bytes = m_packet_byte - 1;
                m_packet_byte       = 2;
            }

            // post packet
            if (m_invoke_callbacks)
            {
                m_packet_callback(m_packet);
            }

            if (m_packet.status() >= 0xF0)
            {
                // cancel running status on SystemCommon messages
                m_packet = universal_packet{};
            }
        }
        else
        {
            ++m_packet_byte;
        }
    }
}

//--------------------------------------------------------------------------

void midi1_byte_stream_parser::feed(const uint8_t* begin, const uint8_t* end)
{
    for (const auto* it = begin; it < end; ++it)
    {
        feed(*it);
    }
}

//--------------------------------------------------------------------------
//! reset the parser state
void midi1_byte_stream_parser::reset()
{
    m_packet = universal_packet{};
    m_sysex.clear();

    m_packet_byte       = 1;
    m_num_missing_bytes = 0;
}

//--------------------------------------------------------------------------

void midi1_byte_stream_parser::system_realtime(uint8_t byte)
{
    if ((byte == 0xF9) || (byte == 0xFD))
        // undefined
        return;

    // post packet
    if (m_invoke_callbacks)
    {
        m_packet_callback(make_system_message(m_group, byte));
    }
}

//--------------------------------------------------------------------------

void midi1_byte_stream_parser::system_common(uint8_t byte)
{
    m_packet            = make_system_message(m_group, byte);
    m_packet_byte       = 2;
    m_num_missing_bytes = 0;

    switch (m_packet.status())
    {
    case system_status::mtc_quarter_frame:
    case system_status::song_select:
        m_num_missing_bytes = 1;
        break;
    case system_status::song_position:
        m_num_missing_bytes = 2;
        break;
    case system_status::tune_request:
        break;
    default:
        // undefined, skip, cancel running status
        m_packet = universal_packet{};
        return;
    }

    if (m_num_missing_bytes == 0)
    {
        // post packet
        if (m_invoke_callbacks)
        {
            m_packet_callback(m_packet);
        }

        // reset
        m_packet = universal_packet{};
    }
}

//--------------------------------------------------------------------------

void midi1_byte_stream_parser::channel_voice(uint8_t byte)
{
    m_packet            = midi1_channel_voice_message{ m_group, byte };
    m_packet_byte       = 2;
    m_num_missing_bytes = 0;

    switch (m_packet.status() & 0xF0)
    {
    case midi1_channel_voice_status::note_off:
    case midi1_channel_voice_status::note_on:
    case midi1_channel_voice_status::poly_pressure:
    case midi1_channel_voice_status::control_change:
    case midi1_channel_voice_status::pitch_bend:
        m_num_missing_bytes = 2;
        break;
    case midi1_channel_voice_status::program_change:
    case midi1_channel_voice_status::channel_pressure:
        m_num_missing_bytes = 1;
        break;
    default:
        break;
    }

    if (m_num_missing_bytes == 0)
    {
        // post packet
        if (m_invoke_callbacks)
        {
            m_packet_callback(m_packet);
        }
    }
}

//--------------------------------------------------------------------------

void midi1_byte_stream_parser::sysex_start()
{
    m_packet            = make_sysex7_start_packet(m_group);
    m_packet_byte       = 2;
    m_num_missing_bytes = 0;

    if (has_sysex_callback())
    {
        m_sysex.clear();
        if (m_sysex.data.capacity() < 1024)
            m_sysex.data.reserve(1024);

        if (!m_invoke_callbacks)
            m_packet_byte = 0; // do not collect any data
    }
}

//--------------------------------------------------------------------------

void midi1_byte_stream_parser::sysex_continue_callback(uint8_t byte)
{
    switch (m_packet_byte)
    {
    case 0:
    case 1:
        // skip SysEx data
        return;
    case 2: // Manufacturer
        m_sysex.manufacturerID = (byte << 16);
        ++m_packet_byte;
        return;
    case 3:
    case 4:
        if ((m_sysex.manufacturerID & 0xFF0000) == 0) // three byte manufacturer ?
        {
            m_sysex.manufacturerID += (byte << ((4 - m_packet_byte) * 8));
            ++m_packet_byte;
            return;
        }
        break;
    default:
        break;
    }

    // resize necessary?
    if (m_sysex.data.size() == m_sysex.data.capacity())
    {
        m_sysex.data.reserve(m_sysex.data.capacity() * 2);
    }

    // collect the data
    m_sysex.data.push_back(byte);
}

//--------------------------------------------------------------------------

void midi1_byte_stream_parser::sysex_end_callback()
{
    m_packet = universal_packet{};

    if (((m_sysex.manufacturerID & 0xFF0000) == 0) && (m_packet_byte < 5))
    {
        // incomplete three byte manufacturer, invalid
        return;
    }

    // notify SysEx
    if (m_invoke_callbacks)
    {
        m_sysex_callback(m_sysex);
    }
    m_sysex.clear();
}

//--------------------------------------------------------------------------

void midi1_byte_stream_parser::sysex_continue_packet(uint8_t byte)
{
    // SysEx as packets
    m_packet.set_byte(m_packet_byte, byte);
    if (++m_packet_byte == 8)
    {
        if (m_invoke_callbacks)
        {
            m_packet.set_byte(1, (m_packet.status() & 0xF0) + 6);
            m_packet_callback(m_packet);
        }

        m_packet      = make_sysex7_continue_packet(m_group);
        m_packet_byte = 2;
    }
}

//--------------------------------------------------------------------------

void midi1_byte_stream_parser::sysex_end_packet()
{
    const auto cur_sysex_status = uint8_t(m_packet.status() & 0xF0);
    const auto cur_packet_size  = uint8_t((m_packet_byte - 2) & 0x0F);

    if (cur_sysex_status == data_status::sysex7_start)
    {
        if (m_packet_byte < 3)
        {
            // no manufacturer, useless
            m_packet = universal_packet{};
            return;
        }

        m_packet.set_byte(1, data_status::sysex7_complete + cur_packet_size);
    }
    else
    {
        m_packet.set_byte(1, data_status::sysex7_end + cur_packet_size);
    }

    if (m_invoke_callbacks)
    {
        m_packet_callback(m_packet);
    }

    m_packet      = universal_packet{};
    m_packet_byte = 2;
}

//--------------------------------------------------------------------------

size_t to_midi1_byte_stream(const universal_packet& p, uint8_t result[8])
{
    const auto payload_bytes = midi1_byte_stream_size(p);

    size_t result_bytes = 0;

    switch (p.type())
    {
    case packet_type::system:
    case packet_type::midi1_channel_voice:
        if (payload_bytes > 0)
        {
            for (; result_bytes < payload_bytes; ++result_bytes)
                result[result_bytes] = p.get_byte(1 + result_bytes);
        }
        break;
    case packet_type::data:
        if (is_sysex7_packet(p))
        {
            const auto status = p.status() & 0xF0;

            if ((status == data_status::sysex7_complete) || (status == data_status::sysex7_start))
                result[result_bytes++] = 0xF0;

            for (size_t b = 0; b < payload_bytes; ++b)
                result[result_bytes++] = p.get_byte(2 + b);

            if ((status == data_status::sysex7_complete) || (status == data_status::sysex7_end))
                result[result_bytes++] = 0xF7;
        }
        break;
    default:
        return 0;
    }

    return result_bytes;
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
