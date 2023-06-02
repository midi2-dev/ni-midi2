#include <midi/jitter_reduction_timestamps.h>

#include <cassert>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

jr_timestamp_t jr_clock::now()
{
    using system_clock     = std::chrono::high_resolution_clock;
    static const auto zero = system_clock::now();

    const auto jr_timestamp_now = std::chrono::duration_cast<jr_ticks>(system_clock::now() - zero);

    jr_timestamp_t result(jr_timestamp_now.count() & 0xFFFF); // 16 bit

    return result;
}

//--------------------------------------------------------------------------

void jr_clock_follower::reset()
{
    m_clock_ts        = {};
    m_clock_time      = zero;
    m_message_time    = zero;
    m_jitter          = std::chrono::milliseconds(0);
    m_security_offset = std::chrono::milliseconds(2);
}

//--------------------------------------------------------------------------

void jr_clock_follower::set_security_offset(duration offset)
{
    assert(offset >= duration{ 0 });
    m_security_offset = offset;
}

//--------------------------------------------------------------------------
//! update clock statistics with received jr clock timestamp
void jr_clock_follower::process_clock(time_point received, jr_timestamp_t jr_timestamp)
{
    if (m_clock_time == zero)
    {
        m_clock_time   = received;
        m_clock_ts     = jr_timestamp;
        m_message_time = received;
        return;
    }

    const auto diff_ts = jr_timestamp - m_clock_ts;

    m_clock_ts = jr_timestamp;

    const auto diff_ts_duration      = std::chrono::duration_cast<system_clock::duration>(diff_ts);
    const auto expected_receive_time = m_clock_time + diff_ts_duration;
    const auto jitter                = received - expected_receive_time;

    if (received < expected_receive_time)
    // we are either in the startup phase or sender clock is slower than we
    {
        // use received as new reference time
        m_clock_time = received;

        if (received > m_message_time) // ensure that message order is preserved
        {
            m_message_time = received;
        }
    }
    else
    {
        m_clock_time   = expected_receive_time;
        m_message_time = received;
    }

    update_stats(jitter);
}

//--------------------------------------------------------------------------
//! calculate the scheduled process time for a jr timestamp
jr_clock_follower::time_point jr_clock_follower::schedule_message(time_point received, jr_timestamp_t jr_timestamp)
{
    if (m_clock_time != zero)
    {
        const auto diff_ts               = jr_timestamp - m_clock_ts;
        const auto system_clock_duration = std::chrono::duration_cast<system_clock::duration>(diff_ts);
        const auto message_time          = m_clock_time + system_clock_duration;

        // ensure that messages stay in correct order
        if (message_time > m_message_time)
        {
            m_message_time = message_time;
        }
    }
    else
    {
        m_message_time = received;
    }

    const auto result = m_message_time + security_offset();
    return result;
}

//--------------------------------------------------------------------------

void jr_clock_follower::update_stats(duration jitter)
{
    if (jitter < duration{ 0 })
    {
        m_jitter -= jitter;
        recalc_security_offset();
    }
    else if (jitter > m_jitter)
    {
        m_jitter = jitter;
        recalc_security_offset();
    }
}

//--------------------------------------------------------------------------

void jr_clock_follower::recalc_security_offset()
{
    const auto value = m_jitter * 12 / 10;
    if (value > m_security_offset)
        m_security_offset = value;
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
