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

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

using uint2_t  = std::uint8_t;
using uint4_t  = std::uint8_t;
using uint7_t  = std::uint8_t;
using uint8_t  = std::uint8_t;
using uint14_t = std::uint16_t;
using uint16_t = std::uint16_t;
using uint28_t = std::uint32_t;
using uint32_t = std::uint32_t;
using int32_t  = std::int32_t;
using size_t   = std::size_t;

//--------------------------------------------------------------------------

using group_t        = uint4_t;
using status_t       = uint8_t;
using channel_t      = uint4_t;
using note_nr_t      = uint7_t;
using controller_t   = uint7_t;
using program_t      = uint7_t;
using muid_t         = uint28_t;
using manufacturer_t = uint32_t;
using protocol_t     = uint8_t;
using extensions_t   = uint8_t;

//--------------------------------------------------------------------------

constexpr uint7_t  downsample_16_to_7bit(uint16_t);
constexpr uint7_t  downsample_32_to_7bit(uint32_t);
constexpr uint14_t downsample_32_to_14bit(uint32_t);

constexpr uint16_t upsample_7_to_16bit(uint7_t);
constexpr uint32_t upsample_7_to_32bit(uint7_t);
constexpr uint32_t upsample_14_to_32bit(uint14_t);

constexpr uint32_t upsample_x_to_ybit(uint32_t v, uint8_t x, uint8_t y);

//--------------------------------------------------------------------------

struct velocity
{
    uint16_t value{ 0x8000 };

    constexpr velocity() = default;
    constexpr explicit velocity(uint16_t);
    constexpr explicit velocity(uint7_t);
    constexpr explicit velocity(float);

    constexpr float   as_float() const; // [0..1]
    constexpr uint7_t as_uint7() const;

    constexpr bool operator==(const velocity&) const;
    constexpr bool operator!=(const velocity&) const;
};

//--------------------------------------------------------------------------

struct pitch_bend
{
    uint32_t value{ 0x80000000 };

    constexpr pitch_bend() = default;
    constexpr explicit pitch_bend(uint32_t);
    constexpr explicit pitch_bend(uint14_t);
    constexpr explicit pitch_bend(float);

    constexpr float    as_float() const; // [-1..1]
    constexpr uint14_t as_uint14() const;

    constexpr void reset();

    constexpr bool operator==(const pitch_bend&) const;
    constexpr bool operator!=(const pitch_bend&) const;
};

//--------------------------------------------------------------------------

struct pitch_increment
{
    int32_t value{ 0 }; // S6.25 fixed point format

    constexpr pitch_increment() = default;
    constexpr explicit pitch_increment(int32_t);
    constexpr explicit pitch_increment(float);

    constexpr void operator+=(const pitch_increment&);

    constexpr pitch_increment operator+(const pitch_increment&) const;

    constexpr bool operator==(const pitch_increment&) const;
    constexpr bool operator!=(const pitch_increment&) const;
};

//--------------------------------------------------------------------------

struct pitch_7_9
{
    uint16_t value{ 0 };

    constexpr pitch_7_9() = default;
    constexpr explicit pitch_7_9(uint16_t);
    constexpr explicit pitch_7_9(note_nr_t);
    constexpr explicit pitch_7_9(float);

    constexpr float     as_float() const; // [0..128)
    constexpr note_nr_t note_nr() const;

    constexpr bool operator==(const pitch_7_9&) const;
    constexpr bool operator!=(const pitch_7_9&) const;
};

//--------------------------------------------------------------------------

struct pitch_7_25
{
    uint32_t value{ 0 };

    constexpr pitch_7_25() = default;
    constexpr explicit pitch_7_25(uint32_t);
    constexpr explicit pitch_7_25(pitch_7_9);
    constexpr explicit pitch_7_25(note_nr_t);
    constexpr explicit pitch_7_25(float);

    constexpr float     as_float() const; // [0..128)
    constexpr note_nr_t note_nr() const;

    constexpr pitch_7_25& operator=(pitch_7_9);
    constexpr pitch_7_25  operator+(const pitch_increment&) const;
    constexpr pitch_7_25  operator+(float) const;

    constexpr void operator+=(const pitch_increment&);

    constexpr bool operator==(const pitch_7_25&) const;
    constexpr bool operator!=(const pitch_7_25&) const;
};

//--------------------------------------------------------------------------

struct pitch_bend_sensitivity : pitch_7_25
{
    constexpr pitch_bend_sensitivity();
    using pitch_7_25::pitch_7_25;
};

constexpr pitch_increment operator*(const pitch_bend&, const pitch_bend_sensitivity&);
constexpr pitch_increment operator*(const pitch_bend_sensitivity&, const pitch_bend&);

//--------------------------------------------------------------------------

struct controller_increment
{
    int32_t value{ 0 };

    constexpr controller_increment() = default;
    constexpr explicit controller_increment(int32_t);

    constexpr bool operator==(const controller_increment&) const;
    constexpr bool operator!=(const controller_increment&) const;
};

//--------------------------------------------------------------------------

struct controller_value
{
    uint32_t value{ 0 };

    constexpr controller_value() = default;
    constexpr explicit controller_value(uint32_t);
    constexpr explicit controller_value(uint7_t);
    constexpr explicit controller_value(float);

    constexpr float   as_float() const; // [0..1]
    constexpr uint7_t as_uint7() const;

    constexpr void operator+=(controller_increment);

    constexpr controller_value operator+(controller_increment) const;

    constexpr bool operator==(const controller_value&) const;
    constexpr bool operator!=(const controller_value&) const;
};

//-----------------------------------------------
#pragma pack(push, 1)
struct device_identity
{
    manufacturer_t manufacturer; //!< Device Manufacturer (System Exclusive ID Number)
    uint14_t       family;       //!< Device Family
    uint14_t       model;        //!< Device Family Model Number
    uint28_t       revision;     //!< Device Revision Level
};
#pragma pack(pop)

//--------------------------------------------------------------------------
// constexpr implementations
//--------------------------------------------------------------------------

constexpr uint7_t downsample_16_to_7bit(uint16_t v)
{
    return uint7_t(v >> 9u);
}
constexpr uint7_t downsample_32_to_7bit(uint32_t v)
{
    return uint7_t(v >> 25u);
}
constexpr uint14_t downsample_32_to_14bit(uint32_t v)
{
    return uint14_t(v >> 18u);
}

constexpr velocity::velocity(uint16_t v)
  : value(v)
{
}
constexpr velocity::velocity(uint7_t v)
  : value(upsample_7_to_16bit(v))
{
}
constexpr uint7_t velocity::as_uint7() const
{
    return downsample_16_to_7bit(value);
}
constexpr bool velocity::operator==(const velocity& o) const
{
    return value == o.value;
}
constexpr bool velocity::operator!=(const velocity& o) const
{
    return value != o.value;
}

constexpr pitch_bend::pitch_bend(uint32_t v)
  : value(v)
{
}
constexpr pitch_bend::pitch_bend(uint14_t v)
  : value(upsample_14_to_32bit(v))
{
}
constexpr uint14_t pitch_bend::as_uint14() const
{
    return downsample_32_to_14bit(value);
}
constexpr void pitch_bend::reset()
{
    value = 0x80000000;
}
constexpr bool pitch_bend::operator==(const pitch_bend& o) const
{
    return value == o.value;
}
constexpr bool pitch_bend::operator!=(const pitch_bend& o) const
{
    return value != o.value;
}

constexpr pitch_increment::pitch_increment(int32_t v)
  : value(v)
{
}
constexpr bool pitch_increment::operator==(const pitch_increment& o) const
{
    return value == o.value;
}
constexpr bool pitch_increment::operator!=(const pitch_increment& o) const
{
    return value != o.value;
}

constexpr pitch_7_9::pitch_7_9(uint16_t v)
  : value(v)
{
}
constexpr pitch_7_9::pitch_7_9(note_nr_t n)
  : value(n << 9)
{
}
constexpr note_nr_t pitch_7_9::note_nr() const
{
    return note_nr_t(value >> 9);
}
constexpr bool pitch_7_9::operator==(const pitch_7_9& o) const
{
    return value == o.value;
}
constexpr bool pitch_7_9::operator!=(const pitch_7_9& o) const
{
    return value != o.value;
}

constexpr pitch_7_25::pitch_7_25(uint32_t v)
  : value(v)
{
}
constexpr pitch_7_25::pitch_7_25(note_nr_t n)
  : value(n << 25)
{
}
constexpr pitch_7_25::pitch_7_25(pitch_7_9 p)
{
    this->operator=(p);
}

constexpr note_nr_t pitch_7_25::note_nr() const
{
    return note_nr_t(value >> 25);
}
constexpr bool pitch_7_25::operator==(const pitch_7_25& o) const
{
    return value == o.value;
}
constexpr bool pitch_7_25::operator!=(const pitch_7_25& o) const
{
    return value != o.value;
}

constexpr pitch_bend_sensitivity::pitch_bend_sensitivity()
  : pitch_7_25{ note_nr_t{ 2 } }
{
}

constexpr pitch_increment operator*(const pitch_bend_sensitivity& s, const pitch_bend& p)
{
    return operator*(p, s);
}

constexpr controller_increment::controller_increment(int32_t v)
  : value(v)
{
}
constexpr bool controller_increment::operator==(const controller_increment& o) const
{
    return value == o.value;
}
constexpr bool controller_increment::operator!=(const controller_increment& o) const
{
    return value != o.value;
}

constexpr controller_value::controller_value(uint32_t v)
  : value(v)
{
}
constexpr controller_value::controller_value(uint7_t v)
  : value(upsample_7_to_32bit(v))
{
}
constexpr uint7_t controller_value::as_uint7() const
{
    return downsample_32_to_7bit(value);
}
constexpr bool controller_value::operator==(const controller_value& o) const
{
    return value == o.value;
}
constexpr bool controller_value::operator!=(const controller_value& o) const
{
    return value != o.value;
}

//--------------------------------------------------------------------------

constexpr uint16_t upsample_7_to_16bit(uint7_t v)
{
    uint16_t result = (v << 9u);
    if (v > 64)
    {
        auto bits = v & 0x3F; // 6 bits
        result |= (bits << 3u) | (bits >> 3u);
    }

    return result;
}

//--------------------------------------------------------------------------

constexpr uint32_t upsample_7_to_32bit(uint7_t v)
{
    uint32_t result = (uint32_t(v) << 25u);
    if (v > 64)
    {
        uint32_t bits = (v & 0x3F);
        bits |= (bits << 6); // 12 bits
        result |= (bits << 13u) | (bits << 1) | (bits >> 11u);
    }

    return result;
}

//--------------------------------------------------------------------------

constexpr uint32_t upsample_14_to_32bit(uint14_t v)
{
    uint32_t result = (v << 18u);

    if (v > 8192)
    {
        auto bits = v & 0x1FFF; // 13 bits
        result |= (bits << 5u) | (bits >> 8u);
    }

    return result;
}

//--------------------------------------------------------------------------

constexpr uint32_t upsample_x_to_ybit(uint32_t v, uint8_t x, uint8_t y)
{
    assert((x > 1) && (y <= 32) && (x < y));

    const uint8_t  scale_bits = (y - x);      // number of bits to upscale
    const uint32_t x_center   = 1 << (x - 1); // center value for x, e.g.
                                              // 0x40 (64) for 7 bits
                                              // 0x2000 (8192) for 14 bits
                                              // simple bit shift
    uint32_t result = v << scale_bits;
    if (v <= x_center)
    {
        return result;
    }

    // expanded bit repeat scheme
    uint8_t  repeat_bits  = x - 1; // we must repeat all but the highest bit
    uint32_t repeat_mask  = (1 << repeat_bits) - 1;
    uint32_t repeat_value = v & repeat_mask; // repeat bit sequence
    if (scale_bits > repeat_bits)
    { // need to repeat multiple times
        repeat_value <<= (scale_bits - repeat_bits);
    }
    else
    {
        repeat_value >>= (repeat_bits - scale_bits);
    }
    while (repeat_value != 0)
    {
        result |= repeat_value;       // fill lower bits with repeat_value
        repeat_value >>= repeat_bits; // move repeat bit sequence to next position
    }
    return result;
}

//--------------------------------------------------------------------------

namespace impl {
    template<typename T>
    constexpr T from_float_0_1(float f)
    {
        if (f <= 0.f)
            return 0;

        if (f >= 1.f)
            return std::numeric_limits<T>::max();

        constexpr auto max = std::numeric_limits<T>::max();

        if (f <= .5f)
        {
            constexpr auto scale = static_cast<double>(max) + 1.;
            const double   d     = f * scale;
            return static_cast<T>(d);
        }

        constexpr auto mid   = (max >> 1) + 1;
        constexpr auto scale = static_cast<double>(max);

        const double d = (f - 0.5) * scale;
        return mid + static_cast<T>(d);
    }
} // namespace impl

//--------------------------------------------------------------------------

constexpr velocity::velocity(float f)
  : value(impl::from_float_0_1<decltype(value)>(f))
{
}

constexpr float velocity::as_float() const
{
    if (value <= 0x8000)
        return static_cast<float>(value / static_cast<double>(0x8000) / 2.);
    else
        return static_cast<float>(value / static_cast<double>(0xFFFF));
}

//--------------------------------------------------------------------------

constexpr pitch_bend::pitch_bend(float f)
  : value(impl::from_float_0_1<decltype(value)>((f + 1.f) / 2.f))
{
}

constexpr float pitch_bend::as_float() const
{
    if (value >= 0x80000000)
        return static_cast<float>((value - 0x80000000) / static_cast<double>(0x7FFFFFFF));
    else
        return static_cast<float>((0x80000000 - value) / (-static_cast<double>(0x80000000)));
}

//--------------------------------------------------------------------------

constexpr pitch_increment::pitch_increment(float f)
{
    if (f >= 64.f)
    {
        value = std::numeric_limits<int32_t>::max();
    }
    else if (f <= -64.f)
    {
        value = std::numeric_limits<int32_t>::min();
    }
    else if (f >= 0.f)
    {
        value = static_cast<int32_t>(pitch_7_25(f).value);
    }
    else
    {
        value = -static_cast<int32_t>(pitch_7_25(-f).value);
    }
}

constexpr void pitch_increment::operator+=(const pitch_increment& inc)
{
    value += inc.value;
}

constexpr pitch_increment pitch_increment::operator+(const pitch_increment& inc) const
{
    return pitch_increment{ value + inc.value };
}

//--------------------------------------------------------------------------

constexpr pitch_7_9::pitch_7_9(float f)
{
    constexpr int fractional_bits = 9;
    if (f <= 0.f)
    {
        value = 0;
    }
    else if (f >= 128.f)
    {
        value = 0xFFFF;
    }
    else
    {
        auto result = std::round(f * (1 << fractional_bits));
        value       = (result < 0x10000) ? static_cast<uint16_t>(result) : 0xFFFF;
    }
}

constexpr float pitch_7_9::as_float() const
{
    constexpr int fractional_bits = 9;
    return static_cast<float>(value) / static_cast<float>(1 << fractional_bits);
}

//--------------------------------------------------------------------------

constexpr pitch_7_25::pitch_7_25(float f)
{
    constexpr int fractional_bits = 25;
    if (f <= 0.f)
    {
        value = 0;
    }
    else if (f >= 128.f)
    {
        value = 0xFFFFFFFF;
    }
    else
    {
        auto result = std::round(f * float(1 << fractional_bits));
        value       = (result < 4294967296.) ? static_cast<uint32_t>(result) : 0xFFFFFFFF;
    }
}

constexpr float pitch_7_25::as_float() const
{
    constexpr int fractional_bits = 25;
    return static_cast<float>(static_cast<double>(value) / static_cast<double>(1 << fractional_bits));
}

constexpr pitch_7_25& pitch_7_25::operator=(pitch_7_9 pitch)
{
    value = static_cast<uint32_t>(pitch.value) << 16;
    return *this;
}

constexpr pitch_7_25 pitch_7_25::operator+(const pitch_increment& inc) const
{
    return pitch_7_25{ static_cast<uint32_t>(value) + inc.value };
}

constexpr pitch_7_25 pitch_7_25::operator+(float detune) const
{
    int64_t r = value;

    if (detune >= 0.f)
    {
        pitch_7_25 inc(detune);
        r += inc.value;
        if (r > 0xFFFFFFFF)
            r = 0xFFFFFFFF;
    }
    else
    {
        pitch_7_25 dec(-detune);
        r -= dec.value;
        if (r < 0)
            r = 0;
    }

    return pitch_7_25{ static_cast<uint32_t>(r) };
}

//--------------------------------------------------------------------------

constexpr void pitch_7_25::operator+=(const pitch_increment& inc)
{
    value = static_cast<uint32_t>(value) + inc.value;
}

//--------------------------------------------------------------------------

constexpr pitch_increment operator*(const pitch_bend& pb, const pitch_bend_sensitivity& sens)
{
    if (auto result = std::int64_t(pb.value) - std::int64_t(0x80000000))
    {
        result *= sens.value;

        return pitch_increment{ static_cast<int32_t>(result >> 31) };
    }

    return pitch_increment{ int32_t{ 0 } };
}

//--------------------------------------------------------------------------

constexpr controller_value::controller_value(float f)
  : value(impl::from_float_0_1<decltype(value)>(f))
{
}

constexpr float controller_value::as_float() const
{
    if (value <= 0x80000000)
        return static_cast<float>(value / static_cast<double>(0x80000000) / 2.);
    else
        return static_cast<float>(value / static_cast<double>(0xFFFFFFFF));
}

constexpr controller_value controller_value::operator+(controller_increment inc) const
{
    int64_t r = static_cast<int64_t>(value) + static_cast<int64_t>(inc.value);

    if (r <= 0)
        return controller_value{ uint32_t{ 0 } };

    if (r > 0xFFFFFFFF)
        return controller_value{ uint32_t{ 0xFFFFFFFF } };

    return controller_value{ static_cast<uint32_t>(r) };
}

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
