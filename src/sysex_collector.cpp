#include <midi/data_message.h>
#include <midi/sysex_collector.h>

#include <algorithm>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

void sysex7_collector::feed(const universal_packet& p)
{
    if (!is_sysex7_packet(p))
        return;

    auto m = sysex7_packet_view{ p };

    switch (m.status())
    {
    case data_status::sysex7_complete:
    case data_status::sysex7_start:
        if (m_state != data_status::sysex7_start)
        {
            // invalid message sequence, reset
            reset();
        }
        break;
    default:
        if (m_state != data_status::sysex7_continue)
        {
            // invalid message sequence, reset
            reset();
            return;
        }
    }

    const auto numBytes = m.payload_size();
    if (m_sysex7.data.size() + numBytes > m_sysex7.data.capacity())
    {
        m_sysex7.data.reserve(std::max(size_t{ 128 }, 2 * m_sysex7.data.capacity()));
    }

    for (unsigned b = 0; b < numBytes; ++b)
    {
        const auto byte = m.payload_byte(b);
        switch (m_manufacturerIDBytesRead)
        {
        case 0:
            if (byte)
            {
                m_sysex7.manufacturerID   = (byte << 16);
                m_manufacturerIDBytesRead = 3;
            }
            else
            {
                m_manufacturerIDBytesRead = 1;
            }
            break;
        case 1:
            m_sysex7.manufacturerID   = (byte << 8);
            m_manufacturerIDBytesRead = 2;
            break;
        case 2:
            m_sysex7.manufacturerID |= byte;
            m_manufacturerIDBytesRead = 3;
            break;
        default:
            // collect data
            m_sysex7.data.push_back(byte);
            break;
        }
    }

    switch (m.status())
    {
    case data_status::sysex7_complete:
    case data_status::sysex7_end:
        if (m_cb)
        {
            m_cb(m_sysex7);
            m_sysex7.clear();
        }
        reset();
        break;
    default:
        m_state = data_status::sysex7_continue;
        break;
    }
}

//--------------------------------------------------------------------------

void sysex7_collector::reset()
{
    m_sysex7                  = sysex7{};
    m_state                   = data_status::sysex7_start;
    m_manufacturerIDBytesRead = 0;
}

//--------------------------------------------------------------------------
// TODO: implement sysex8_collector
//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
