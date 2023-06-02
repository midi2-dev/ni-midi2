#pragma once

//--------------------------------------------------------------------------

#include <midi/types.h>
#include <midi/universal_packet.h>

//--------------------------------------------------------------------------

#include <chrono>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

constexpr uint16_t jr_clock_frequency = 31250;

//--------------------------------------------------------------------------

using jr_ticks = std::chrono::duration<long long, std::ratio<1, jr_clock_frequency>>;

//--------------------------------------------------------------------------

struct jr_timestamp_t
{
    uint16_t value;

    jr_timestamp_t()
      : value(0)
    {
    }
    explicit jr_timestamp_t(uint16_t v)
      : value(v)
    {
    }

    bool operator==(jr_timestamp_t other) const { return (value == other.value); }
    bool operator!=(jr_timestamp_t other) const { return (value != other.value); }

    jr_ticks operator-(jr_timestamp_t other) const
    {
        int diff = static_cast<int>(value) - static_cast<int>(other.value);
        if (diff < 0)
            diff += 0x10000;
        return jr_ticks{ static_cast<uint16_t>(diff) };
    }
};

//--------------------------------------------------------------------------

class jr_clock
{
  public:
    typedef jr_ticks         duration;
    typedef duration::rep    rep;
    typedef duration::period period;
    typedef uint16_t         time_point;

    static jr_timestamp_t now();
};

//--------------------------------------------------------------------------
//! base class for Jitter Reduction messages
struct jr_message : universal_packet
{
    inline jr_timestamp_t timestamp() const
    {
        return jr_timestamp_t(static_cast<uint16_t>(get_byte(2) << 8) | get_byte(3));
    }
    inline void set_timestamp(jr_timestamp_t ts)
    {
        set_byte(2, static_cast<uint8_t>(ts.value >> 8));
        set_byte(3, static_cast<uint8_t>(ts.value & 0xFF));
    };

  protected:
    inline explicit jr_message(uint8_t status)
    {
        set_type(packet_type::utility);
        set_byte(1, status);
    }
    inline jr_message(uint8_t status, jr_timestamp_t ts)
      : jr_message(status)
    {
        set_timestamp(ts);
    }
};

//--------------------------------------------------------------------------
//! Jitter Reduction Clock message
struct jr_clock_message : jr_message
{
    inline jr_clock_message()
      : jr_message(utility_status::jr_clock, {})
    {
    }
    inline explicit jr_clock_message(jr_timestamp_t ts)
      : jr_message(utility_status::jr_clock, ts)
    {
    }
};

//--------------------------------------------------------------------------
//! Jitter Reduction Timestamp message
struct jr_timestamp_message : jr_message
{
    inline jr_timestamp_message()
      : jr_message(utility_status::jr_timestamp, {})
    {
    }
    inline explicit jr_timestamp_message(jr_timestamp_t ts)
      : jr_message(utility_status::jr_timestamp, ts)
    {
    }
};

//--------------------------------------------------------------------------

class jr_clock_follower
{
  public:
    jr_clock_follower() = default;

    void reset();

    using system_clock = std::chrono::high_resolution_clock;
    using time_point   = system_clock::time_point;
    using duration     = system_clock::duration;

    void       process_clock(time_point, jr_timestamp_t);
    time_point schedule_message(time_point, jr_timestamp_t);

    inline duration security_offset() const { return m_security_offset; }
    inline duration jitter() const { return m_jitter; }

    void set_security_offset(duration);

  protected:
    void update_stats(duration jitter);
    void recalc_security_offset();

  private:
    const system_clock::time_point zero = system_clock::now();

    jr_timestamp_t m_clock_ts{ 0 };        //!< last jr clock timestamp
    time_point     m_clock_time{ zero };   //!< jitter-reduced send time of last jr clock
    time_point     m_message_time{ zero }; //!< schedule time of last message

    duration m_jitter{ 0 };                                     //!< current jitter
    duration m_security_offset{ std::chrono::milliseconds(2) }; //!< security offset, default two ms
};

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
