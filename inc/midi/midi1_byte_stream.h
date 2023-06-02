#pragma once

//--------------------------------------------------------------------------

#include <midi/sysex.h>
#include <midi/types.h>
#include <midi/universal_packet.h>

#include <cstdint>
#include <functional>
#include <utility>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

class midi1_byte_stream_parser
{
  public:
    using packet_callback = std::function<void(universal_packet)>;
    using sysex_callback  = std::function<void(const midi::sysex7&)>;

    explicit midi1_byte_stream_parser(packet_callback, sysex_callback = {}, bool enable_callbacks = true);
    midi1_byte_stream_parser(group_t, packet_callback, sysex_callback = {}, bool enable_callbacks = true);

    bool callbacks_enabled() const { return m_invoke_callbacks; }
    void enable_callbacks(bool enable) { m_invoke_callbacks = enable; }

    group_t group() const;
    void    set_group(group_t);

    void feed(uint8_t);
    void feed(const uint8_t* data, size_t num_bytes);
    void feed(const uint8_t* begin, const uint8_t* end);

    void reset();

  protected:
    void system_realtime(uint8_t);
    void system_common(uint8_t);
    void channel_voice(uint8_t);

    bool has_sysex_callback() const { return static_cast<bool>(m_sysex_callback); }

    void sysex_start();
    void sysex_continue_callback(uint8_t);
    void sysex_end_callback();
    void sysex_continue_packet(uint8_t);
    void sysex_end_packet();

  private:
    uint8_t         m_group{ 0 };
    packet_callback m_packet_callback;
    sysex_callback  m_sysex_callback;
    bool            m_invoke_callbacks{ true };

    universal_packet m_packet;
    sysex7           m_sysex;

    uint8_t m_packet_byte{ 0 };
    uint8_t m_num_missing_bytes{ 0 };
};

//--------------------------------------------------------------------------

constexpr universal_packet from_midi1_byte_stream(uint8_t status, uint7_t d1, uint7_t d2);

//--------------------------------------------------------------------------

constexpr size_t midi1_byte_stream_size(const universal_packet&);

//--------------------------------------------------------------------------

size_t to_midi1_byte_stream(const universal_packet&, uint8_t bytes[8]);

//--------------------------------------------------------------------------

inline midi1_byte_stream_parser::midi1_byte_stream_parser(packet_callback pcb,
                                                          sysex_callback  sxcb,
                                                          bool            enable_callbacks)
  : m_packet_callback(std::move(pcb))
  , m_sysex_callback(std::move(sxcb))
  , m_invoke_callbacks(enable_callbacks)
{
}

inline midi1_byte_stream_parser::midi1_byte_stream_parser(group_t         group,
                                                          packet_callback pcb,
                                                          sysex_callback  sxcb,
                                                          bool            enable_callbacks)
  : m_group(group)
  , m_packet_callback(std::move(pcb))
  , m_sysex_callback(std::move(sxcb))
  , m_invoke_callbacks(enable_callbacks)
{
}

//--------------------------------------------------------------------------

inline group_t midi1_byte_stream_parser::group() const
{
    return m_group;
}

//--------------------------------------------------------------------------

inline void midi1_byte_stream_parser::set_group(group_t group)
{
    m_group = group;
}

//--------------------------------------------------------------------------

inline void midi1_byte_stream_parser::feed(const uint8_t* data, size_t num_bytes)
{
    feed(data, data + num_bytes);
}

//--------------------------------------------------------------------------

constexpr size_t midi1_byte_stream_size(const universal_packet& p)
{
    switch (p.type())
    {
    case packet_type::system:
        switch (p.status())
        {
        case system_status::song_position:
            return 3u;
        case system_status::mtc_quarter_frame:
        case system_status::song_select:
            return 2u;
        case system_status::tune_request:
        case system_status::clock:
        case system_status::start:
        case system_status::cont:
        case system_status::stop:
        case system_status::active_sense:
        case system_status::reset:
            return 1u;
        }
        break;
    case packet_type::midi1_channel_voice:
        switch (p.status() & 0xF0)
        {
        case midi1_channel_voice_status::note_off:
        case midi1_channel_voice_status::note_on:
        case midi1_channel_voice_status::poly_pressure:
        case midi1_channel_voice_status::control_change:
        case midi1_channel_voice_status::pitch_bend:
        case system_status::song_position:
            return 3u;
        case midi1_channel_voice_status::program_change:
        case midi1_channel_voice_status::channel_pressure:
            return 2u;
        }
        break;
    case packet_type::data:
        switch (p.status() & 0xF0)
        {
        case data_status::sysex7_complete:
        case data_status::sysex7_start:
        case data_status::sysex7_end:
        case data_status::sysex7_continue:
            if ((p.status() & 0x0F) <= 6)
                return p.status() & 0x0F;
            break;
        }
        break;
    default:
        break;
    }

    return 0u;
}

//--------------------------------------------------------------------------

constexpr universal_packet from_midi1_byte_stream(uint8_t status, uint7_t d1, uint7_t d2)
{
    uint8_t type = static_cast<uint8_t>(packet_type::midi1_channel_voice);

    if ((status & 0xF0) == 0xF0) // System message?
    {
        type = static_cast<uint8_t>(packet_type::system);

        switch (status)
        {
        case 0xF0:
        case 0xF7:
            // assert((status!=0xF0) && (status!=0xF7)); // use sysex() instead!
            //  fall through
        case 0xF4:
        case 0xF5:
        case 0xF9:
        case 0xFD:
            // reserved
            return {};
        default:
            break;
        }
    }
    else if (status < 0x80)
    {
        return {};
    }

    return universal_packet{ static_cast<uint32_t>((type << 28) | (status << 16u) | (d1 << 8u) | d2) };
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
