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

#include <midi/types.h>

//--------------------------------------------------------------------------

namespace midi {

//--------------------------------------------------------------------------

using manufacturer_t = uint32_t;

//--------------------------------------------------------------------------

namespace manufacturer {
    constexpr manufacturer_t sequential_circuits = 0x010000;
    constexpr manufacturer_t moog                = 0x040000;
    constexpr manufacturer_t lexicon             = 0x060000;
    constexpr manufacturer_t kurzweil            = 0x070000;
    constexpr manufacturer_t fender              = 0x080000;
    constexpr manufacturer_t oberheim            = 0x100000;
    constexpr manufacturer_t midi9               = 0x090000;
    constexpr manufacturer_t simmons             = 0x120000;
    constexpr manufacturer_t digidesign          = 0x130000;
    constexpr manufacturer_t fairlight           = 0x140000;
    constexpr manufacturer_t emu                 = 0x180000;
    constexpr manufacturer_t hohner              = 0x240000;
    constexpr manufacturer_t ppg                 = 0x290000;
    constexpr manufacturer_t wersi               = 0x3B0000;
    constexpr manufacturer_t waldorf             = 0x3E0000;
    constexpr manufacturer_t kawai               = 0x400000;
    constexpr manufacturer_t roland              = 0x410000;
    constexpr manufacturer_t korg                = 0x420000;
    constexpr manufacturer_t yamaha              = 0x430000;
    constexpr manufacturer_t casio               = 0x440000;
    constexpr manufacturer_t akai                = 0x470000;
    constexpr manufacturer_t fujitsu             = 0x4B0000;
    constexpr manufacturer_t sony                = 0x4C0000;
    constexpr manufacturer_t teac                = 0x4E0000;
    constexpr manufacturer_t fostex              = 0x510000;
    constexpr manufacturer_t zoom                = 0x520000;
    constexpr manufacturer_t suzuki              = 0x550000;

    constexpr manufacturer_t educational            = 0x7D0000;
    constexpr manufacturer_t universal_non_realtime = 0x7E0000;
    constexpr manufacturer_t universal_realtime     = 0x7F0000;

    constexpr manufacturer_t rane        = 0x000017;
    constexpr manufacturer_t allen_heath = 0x00001A;
    constexpr manufacturer_t motu        = 0x00003B;
    constexpr manufacturer_t atari       = 0x000058;
    constexpr manufacturer_t mackie      = 0x000066;
    constexpr manufacturer_t amd         = 0x00006F;

    constexpr manufacturer_t m_audio   = 0x000105;
    constexpr manufacturer_t microsoft = 0x00010A;
    constexpr manufacturer_t line6     = 0x00010C;
    constexpr manufacturer_t cakewalk  = 0x000121;
    constexpr manufacturer_t numark    = 0x00013F;

    constexpr manufacturer_t denon           = 0x00020B;
    constexpr manufacturer_t google          = 0x00020D;
    constexpr manufacturer_t imitone         = 0x000213;
    constexpr manufacturer_t universal_audio = 0x000218;
    constexpr manufacturer_t sensel          = 0x00021D;
    constexpr manufacturer_t mk2_image       = 0x000222;

    constexpr manufacturer_t bon_tempi          = 0x00200B;
    constexpr manufacturer_t fatar              = 0x00201A;
    constexpr manufacturer_t pinnacle           = 0x00201E;
    constexpr manufacturer_t tc_electronics     = 0x00201F;
    constexpr manufacturer_t doepfer            = 0x002020;
    constexpr manufacturer_t focusrite          = 0x002029;
    constexpr manufacturer_t novation           = 0x002029;
    constexpr manufacturer_t emagic             = 0x002031;
    constexpr manufacturer_t behringer          = 0x002032;
    constexpr manufacturer_t access             = 0x002033;
    constexpr manufacturer_t terratec           = 0x002036;
    constexpr manufacturer_t propellerhead      = 0x00203A;
    constexpr manufacturer_t klavis             = 0x002047;
    constexpr manufacturer_t vermona            = 0x00204D;
    constexpr manufacturer_t nokia              = 0x00204E;
    constexpr manufacturer_t hartmann           = 0x002050;
    constexpr manufacturer_t waves              = 0x002066;
    constexpr manufacturer_t arturia            = 0x00206B;
    constexpr manufacturer_t hanpin             = 0x002079;
    constexpr manufacturer_t serato             = 0x00207F;
    constexpr manufacturer_t presonus           = 0x002103;
    constexpr manufacturer_t native_instruments = 0x002109;
    constexpr manufacturer_t ploytec            = 0x00210D;
    constexpr manufacturer_t roli               = 0x002110;
    constexpr manufacturer_t ik_multimedia      = 0x00211A;
    constexpr manufacturer_t ableton            = 0x00211D;
    constexpr manufacturer_t bome               = 0x002132;
    constexpr manufacturer_t touchkeys          = 0x002136;
} // namespace manufacturer

//--------------------------------------------------------------------------

} // namespace midi

//--------------------------------------------------------------------------
