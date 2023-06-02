#pragma once

//--------------------------------------------------------------------------

#include <midi/sysex.h>
#include <midi/universal_packet.h>

#include <functional>
#include <utility>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

class sysex7_collector
{
  public:
    using callback = std::function<void(const sysex7&)>;

    explicit sysex7_collector(callback);

    void set_callback(callback);

    void feed(const universal_packet&);
    void reset();

  private:
    sysex7   m_sysex7;
    status_t m_state{ data_status::sysex7_start };
    uint8_t  m_manufacturerIDBytesRead{ 0 };
    callback m_cb;
};

//--------------------------------------------------------------------------
/*
class sysex8_collector
{
public:
  using callback = std::function<void (sysex8)>;

  void feed(const universal_packet&);
  void reset();
};
*/
//--------------------------------------------------------------------------

inline sysex7_collector::sysex7_collector(callback cb)
  : m_cb(std::move(cb))
{
}
inline void sysex7_collector::set_callback(callback cb)
{
    m_cb = std::move(cb);
}

} // namespace midi

//--------------------------------------------------------------------------
