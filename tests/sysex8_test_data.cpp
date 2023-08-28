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

#include "sysex8_test_data.h"

//-----------------------------------------------

std::vector<midi::sysex8_test_case> midi::sysex8_test_cases{
    { "empty sysex", { { 0x50034400, 0x00000000 } }, 0x44, {} },
    { "one byte manufacturer only", { { 0x51033400, 0x04000000 } }, 0x34, midi::sysex8{ midi::manufacturer::moog } },
    { "three byte manufacturer only",
      { { 0x550300A1, 0x09000000 } },
      0x00,
      midi::sysex8{ midi::manufacturer::native_instruments } },
    { "three byte manufacturer, complete packet",
      { { 0x5106AA82, 0x0D85A600 } },
      0xAA,
      midi::sysex8{ midi::manufacturer::google, { 0x85, 0xA6, 0x00 } } },
    { "one byte manufacturer, complete packet",
      { { 0x5F089300, 0x09F5E4D3, 0xC2B10000 } },
      0x93,
      midi::sysex8{ midi::manufacturer::midi9, { 0xF5, 0xE4, 0xD3, 0xC2, 0xB1 } } },
    {
      "two packets",
      { { 0x5A1E3900, 0x4E090807, 0x06050403, 0x020100FF }, { 0x5A3639EE, 0xDDCCBBAA } },
      0x39,
      midi::sysex8{ midi::manufacturer::teac, { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA } },
    },
    { "three packets",
      { { 0x561EC3A1, 0x1D112233, 0x44556677, 0x18293A4B },
        { 0x562EC35C, 0x6D7C8B9A, 0xA9B8C7D6, 0xE5F40312 },
        { 0x5632C331, 0 } },
      0xC3,
      midi::sysex8{ midi::manufacturer::ableton,
                    { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x18, 0x29, 0x3A, 0x4B, 0x5C, 0x6D,
                      0x7C, 0x8B, 0x9A, 0xA9, 0xB8, 0xC7, 0xD6, 0xE5, 0xF4, 0x03, 0x12, 0x31 } } },
    { "lots of data",
      { { 0x511E83A1, 0x09112233, 0x44556677, 0x8899AABB },
        { 0x512E83CC, 0xDDEEFF18, 0x293A4B5C, 0x6D7E8F90 },
        { 0x512E83A1, 0xB2C3D4E5, 0xF6E7D8F0, 0xE1D2C3B4 },
        { 0x512E83A5, 0x96877869, 0x5A4B3C2D, 0x1E0F420F },
        { 0x51328310, 0x00000000 } },
      0x83,
      midi::sysex8{ midi::manufacturer::native_instruments,
                    { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD,
                      0xEE, 0xFF, 0x18, 0x29, 0x3A, 0x4B, 0x5C, 0x6D, 0x7E, 0x8F, 0x90, 0xA1, 0xB2,
                      0xC3, 0xD4, 0xE5, 0xF6, 0xE7, 0xD8, 0xF0, 0xE1, 0xD2, 0xC3, 0xB4, 0xA5, 0x96,
                      0x87, 0x78, 0x69, 0x5A, 0x4B, 0x3C, 0x2D, 0x1E, 0x0F, 0x42, 0x0F, 0x10 } } }
};
