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

#include <midi/data_message.h>
#include <midi/extended_data_message.h>
#include <midi/manufacturer.h>
#include <midi/types.h>

#include <cassert>
#include <vector>

#if NIMIDI2_PMR_SYSEX_DATA
#include <memory_resource>

#define NIMIDI2_PMR_SYSEX_DATA_ARG , std::pmr::memory_resource* mr = std::pmr::get_default_resource()
#define NIMIDI2_PMR_SYSEX_DATA_DATA_INITIALIZER mr
#define NIMIDI2_PMR_SYSEX_DATA_DATA_INITIALIZER_SUFFIX , NIMIDI2_PMR_SYSEX_DATA_DATA_INITIALIZER

#else

#define NIMIDI2_PMR_SYSEX_DATA_ARG
#define NIMIDI2_PMR_SYSEX_DATA_DATA_INITIALIZER
#define NIMIDI2_PMR_SYSEX_DATA_DATA_INITIALIZER_SUFFIX

#endif

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------
//! MIDI SysEx (7 bit)
struct sysex
{
#if NIMIDI2_CUSTOM_SYSEX_DATA_ALLOCATOR
    struct data_allocator
    {
        using value_type = uint8_t;
        using size_type  = size_t;

        using propagate_on_container_copy_assignment = std::true_type;
        using propagate_on_container_move_assignment = std::true_type;
        using propagate_on_container_swap            = std::true_type;
        using is_always_equal                        = std::true_type;

        //! deprecated, but still needed by some C++17 implementations
        template<typename U>
        struct rebind
        {
            using other = data_allocator;
        };

        constexpr data_allocator() noexcept { }
        constexpr data_allocator(data_allocator const&) noexcept = default;

        data_allocator& operator=(data_allocator const&) = default;
        data_allocator& operator=(data_allocator&&)      = default;

        bool operator==(data_allocator const&) const { return true; }
        bool operator!=(data_allocator const&) const { return false; }

        [[nodiscard]] value_type* allocate(std::size_t);    //!< user provided implementation required!
        void deallocate(value_type*, std::size_t) noexcept; //!< user provided implementation required!
    };
    using data_type = std::vector<uint8_t, data_allocator>;
#elif NIMIDI2_PMR_SYSEX_DATA
    using data_type = std::pmr::vector<uint8_t>;
#else
    using data_type = std::vector<uint8_t>;
#endif

    manufacturer_t manufacturerID{ 0 }; //!< manufacturer ID, \see midi::manufacturer
    data_type      data;                //!< SysEx data (without 0xF0 header, 0xF7 end byte and manufacturer ID)

    sysex() = default;

    /*! \param manufacturer manufacturer */
    explicit sysex(manufacturer_t manufacturer NIMIDI2_PMR_SYSEX_DATA_ARG)
      : manufacturerID(manufacturer)
      , data(NIMIDI2_PMR_SYSEX_DATA_DATA_INITIALIZER)
    {
    }

    //! initialize with capacity
    /*! \param manufacturer manufacturer
        \param capacity of data member
    */
    sysex(manufacturer_t manufacturer, size_t capacity NIMIDI2_PMR_SYSEX_DATA_ARG)
      : manufacturerID(manufacturer)
      , data(NIMIDI2_PMR_SYSEX_DATA_DATA_INITIALIZER)
    {
        data.reserve(capacity);
    }

    //! initialize with data buffer
    /*! \param manufacturer manufacturer
        \param buffer data buffer (without 0xF7 end byte)
        \param buffer_size number of bytes in\p buffer
    */
    sysex(manufacturer_t manufacturer, const uint8_t* buffer, size_t buffer_size NIMIDI2_PMR_SYSEX_DATA_ARG)
      : manufacturerID(manufacturer)
      , data(buffer, buffer + buffer_size NIMIDI2_PMR_SYSEX_DATA_DATA_INITIALIZER_SUFFIX)
    {
    }

    //! initialize with data vector
    /*! \param manufacturer manufacturer
        \param d data vector (without 0xF7 end byte)
    */
    sysex(manufacturer_t manufacturer, data_type d NIMIDI2_PMR_SYSEX_DATA_ARG)
      : manufacturerID(manufacturer)
      , data(std::move(d) NIMIDI2_PMR_SYSEX_DATA_DATA_INITIALIZER_SUFFIX)
    {
    }

    sysex(manufacturer_t manufacturer, std::initializer_list<uint8_t> d NIMIDI2_PMR_SYSEX_DATA_ARG)
      : manufacturerID(manufacturer)
      , data(d.begin(), d.end() NIMIDI2_PMR_SYSEX_DATA_DATA_INITIALIZER_SUFFIX)
    {
    }

#if NIMIDI2_PMR_SYSEX_DATA
    sysex(manufacturer_t              manufacturer,
          const std::vector<uint8_t>& d,
          std::pmr::memory_resource*  mr = std::pmr::get_default_resource())
      : manufacturerID(manufacturer)
      , data(d.begin(), d.end(), mr)
    {
    }
#endif

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
    void add_data(const uint7_t* data, size_t data_size);

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

template<typename Sender>
void send_sysex7(const sysex7&, group_t, Sender&&);

std::vector<data_message> as_sysex7_packets(const sysex7&, group_t = 0);

//--------------------------------------------------------------------------
//! MIDI SysEx (8 bit)
struct sysex8 : sysex
{
    using sysex::sysex;

    bool operator==(const sysex8& other) const { return sysex::operator==(other); }
    bool operator!=(const sysex8& other) const { return sysex::operator!=(other); }
};

//--------------------------------------------------------------------------

template<typename Sender>
void send_sysex8(const sysex8&, uint8_t stream_id, group_t, Sender&&);

std::vector<extended_data_message> as_sysex8_packets(const sysex8&, uint8_t stream_id, group_t = 0);

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

inline void sysex7::add_data(const uint7_t* d, size_t data_size)
{
    assert(d);
    assert(data_size);

    data.insert(data.end(), d, d + data_size);
}

inline void sysex7::add_uint14(uint14_t value)
{
    assert(value <= uint14_max);

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
    assert(value <= uint28_max);

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

template<typename Sender>
void send_sysex7(const sysex7& sysex, group_t group, Sender&& sender)
{
    constexpr size_t max_payload_size = 6;

    // initialize first packet
    auto p = (sysex.total_data_size() <= max_payload_size) ? make_sysex7_complete_packet(group)
                                                           : make_sysex7_start_packet(group);

    // write manufacturerID bytes
    p.add_payload_byte((sysex.manufacturerID >> 16) & 0x7F);
    if (sysex.manufacturerID & 0x00FFFF)
    {
        p.add_payload_byte((sysex.manufacturerID >> 8) & 0x7F);
        p.add_payload_byte(sysex.manufacturerID & 0x7F);
    }

    // process data bytes
    size_t dataBytesLeft = sysex.data.size();
    for (auto b : sysex.data)
    {
        p.add_payload_byte(b);
        --dataBytesLeft;

        if (p.payload_size() == max_payload_size) // packet full?
        {
            sender(p);

            p =
              (dataBytesLeft <= max_payload_size) ? make_sysex7_end_packet(group) : make_sysex7_continue_packet(group);
        }
    }

    if (p.payload_size())
    {
        sender(p);
    }
}

//--------------------------------------------------------------------------

inline std::vector<data_message> as_sysex7_packets(const sysex7& sysex, group_t group)
{
    std::vector<data_message> result;
    result.reserve(sysex.total_data_size() / 6 + 1);

    send_sysex7(sysex, group, [&](const data_message& p) { result.push_back(p); });

    return result;
}

//--------------------------------------------------------------------------

template<typename Sender>
void send_sysex8(const sysex8& sysex, uint8_t stream_id, group_t group, Sender&& sender)
{
    constexpr size_t max_payload_size = 13;

    // initialize first packet
    auto p = (sysex.total_data_size() <= max_payload_size) ? make_sysex8_complete_packet(stream_id, group)
                                                           : make_sysex8_start_packet(stream_id, group);

    // write manufacturerID bytes
    if (sysex.manufacturerID & 0x00FFFF)
    {
        p.add_payload_byte(0x80 + ((sysex.manufacturerID >> 8) & 0x7F));
        p.add_payload_byte(sysex.manufacturerID & 0x7F);
    }
    else
    {
        p.add_payload_byte(0);
        p.add_payload_byte((sysex.manufacturerID >> 16) & 0x7F);
    }

    // process data bytes
    size_t dataBytesLeft = sysex.data.size();
    for (auto b : sysex.data)
    {
        p.add_payload_byte(b);
        --dataBytesLeft;

        if (p.payload_size() == max_payload_size) // packet full?
        {
            sender(p);

            p = (dataBytesLeft <= max_payload_size) ? make_sysex8_end_packet(stream_id, group)
                                                    : make_sysex8_continue_packet(stream_id, group);
        }
    }

    if (p.payload_size())
    {
        sender(p);
    }
}

inline std::vector<extended_data_message> as_sysex8_packets(const sysex8& sysex, uint8_t stream_id, group_t group)
{
    std::vector<extended_data_message> result;
    result.reserve((sysex.total_data_size() + 1) / 14 + 1);

    send_sysex8(sysex, stream_id, group, [&](const extended_data_message& p) { result.push_back(p); });

    return result;
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
