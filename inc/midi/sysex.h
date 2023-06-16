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

#pragma once

//--------------------------------------------------------------------------

#include <midi/manufacturer.h>
#include <midi/types.h>

#include <cassert>
#include <vector>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------
//! MIDI SysEx (7 bit)
struct sysex
{
    manufacturer_t       manufacturerID{ 0 }; //!< manufacturer ID, \see midi::manufacturer
    std::vector<uint8_t> data;                //!< SysEx data (without 0xF0 header, 0xF7 end byte and manufacturer ID)

    sysex() = default;

    /*! \param manufacturer manufacturer */
    explicit sysex(manufacturer_t manufacturer)
      : manufacturerID(manufacturer)
    {
    }

    //! initialize with capacity
    /*! \param manufacturer manufacturer
        \param capacity of data member
    */
    sysex(manufacturer_t manufacturer, size_t capacity)
      : manufacturerID(manufacturer)
    {
        data.reserve(capacity);
    }

    //! initialize with data buffer
    /*! \param manufacturer manufacturer
        \param buffer data buffer (without 0xF7 end byte)
        \param buffer_size number of bytes in\p buffer
    */
    sysex(manufacturer_t manufacturer, const uint8_t* buffer, size_t buffer_size)
      : manufacturerID(manufacturer)
      , data(buffer, buffer + buffer_size)
    {
    }

    //! initialize with data vector
    /*! \param manufacturer manufacturer
        \param d data vector (without 0xF7 end byte)
    */
    sysex(manufacturer_t manufacturer, std::vector<uint8_t> d)
      : manufacturerID(manufacturer)
      , data(std::move(d))
    {
    }

    size_t total_data_size() const;

    bool empty() const;

    bool is_7bit() const;
    bool is_8bit() const;

    sysex(const sysex& other) = default;
    sysex(sysex&& other)      = default;

    ~sysex() = default;

    sysex& operator=(const sysex&) = default;
    sysex& operator=(sysex&&)      = default;

    bool operator==(const sysex&) const;
    bool operator!=(const sysex&) const;

    void clear();
};

//--------------------------------------------------------------------------
//! MIDI SysEx (7 bit)
struct sysex7 : sysex
{
    using sysex::sysex;

    bool is_valid() const { return ((manufacturerID & 0xFF808080) == 0) && is_7bit(); }

    bool operator==(const sysex7& other) const { return sysex::operator==(other); }
    bool operator!=(const sysex7& other) const { return sysex::operator!=(other); }

    // helpers
    void add_uint7(uint7_t);

    void     add_uint14(uint14_t);
    uint14_t make_uint14(size_t data_pos) const;

    void     add_uint28(uint28_t);
    uint28_t make_uint28(size_t data_pos) const;

    void            add_device_identity(const device_identity&);
    device_identity make_device_identity(size_t data_pos) const;

    static constexpr size_t uint7_max  = (1 << 7) - 1;
    static constexpr size_t uint14_max = (1 << 14) - 1;
    static constexpr size_t uint28_max = (1 << 28) - 1;
};

//--------------------------------------------------------------------------
//! MIDI SysEx (8 bit)
struct sysex8 : sysex
{
    using sysex::sysex;

    bool operator==(const sysex8& other) const { return sysex::operator==(other); }
    bool operator!=(const sysex8& other) const { return sysex::operator!=(other); }
};

//----------------------------------------------- inline implementations

inline bool sysex::operator==(const sysex& other) const
{
    return (manufacturerID == other.manufacturerID) && (data == other.data);
}

inline bool sysex::operator!=(const sysex& other) const
{
    return (manufacturerID != other.manufacturerID) || (data != other.data);
}

inline void sysex7::add_uint7(uint7_t value)
{
    assert(value <= uint7_max);

    const uint7_t d[] = { uint7_t(value & 0x7Fu) };
    data.insert(data.end(), d, d + sizeof(d));
}

inline void sysex7::add_uint14(uint14_t value)
{
    assert(value < uint14_max);

    const uint7_t d[] = { uint7_t(value & 0x7Fu), uint7_t((value >> 7) & 0x7Fu) };
    data.insert(data.end(), d, d + sizeof(d));
}

inline uint14_t sysex7::make_uint14(size_t data_pos) const
{
    assert(data_pos + 1 < data.size());
    return data[data_pos] | (data[data_pos + 1] << 7);
}

inline void sysex7::add_uint28(uint28_t value)
{
    assert(value < uint28_max);

    const uint7_t d[] = { uint7_t(value & 0x7Fu),
                          uint7_t((value >> 7) & 0x7Fu),
                          uint7_t((value >> 14) & 0x7Fu),
                          uint7_t((value >> 21) & 0x7Fu) };
    data.insert(data.end(), d, d + sizeof(d));
}

inline uint28_t sysex7::make_uint28(size_t data_pos) const
{
    assert(data_pos + 3 < data.size());
    return data[data_pos] | (data[data_pos + 1] << 7) | (data[data_pos + 2] << 14) | (data[data_pos + 3] << 21);
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
