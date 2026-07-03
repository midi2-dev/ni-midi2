//
// Copyright (c) 2026 Native Instruments
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

#include "mixed_data_set_test_data.h"

#include <midi/manufacturer.h>

//-----------------------------------------------

std::vector<midi::mixed_data_set_test_case> midi::mixed_data_set_test_cases{
    { "empty mixed data set (header only chunk)",
      { { 0x50840010, 0x00010001, 0x007E1234, 0x00050006 } },
      0x4,
      midi::mixed_data_set{ midi::manufacturer::universal_non_realtime, 0x1234, 0x0005, 0x0006 } },
    { "single payload packet",
      { { 0x50800017, 0x00010001, 0xA1090000, 0x00000000 }, { 0x50901122, 0x33445500, 0, 0 } },
      0x0,
      midi::mixed_data_set{ midi::manufacturer::native_instruments, 0, 0, 0, { 0x11, 0x22, 0x33, 0x44, 0x55 } } },
    { "two payload packets",
      { { 0x508F0028, 0x00010001, 0x0004FFFF, 0x01020304 },
        { 0x509F0102, 0x03040506, 0x0708090A, 0x0B0C0D0E },
        { 0x509F0F10, 0x11121314, 0, 0 } },
      0xF,
      midi::mixed_data_set{ midi::manufacturer::moog, 0xFFFF, 0x0102, 0x0304, { 0x01, 0x02, 0x03, 0x04, 0x05,
                                                                                0x06, 0x07, 0x08, 0x09, 0x0A,
                                                                                0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                                                                0x10, 0x11, 0x12, 0x13, 0x14 } } },
    { "two chunks",
      { { 0x50870020, 0x00020001, 0x007F0001, 0x00020003 },
        { 0x5097A0A1, 0xA2A3A4A5, 0xA6A7A8A9, 0xAAABACAD },
        { 0x50870015, 0x00020002, 0x007F0001, 0x00020003 },
        { 0x5097AEAF, 0xB0000000, 0, 0 } },
      0x7,
      midi::mixed_data_set{
        midi::manufacturer::universal_realtime,
        0x0001,
        0x0002,
        0x0003,
        { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0 } } },
    { "unknown number of chunks",
      { { 0x50810016, 0x00000001, 0xA11D0000, 0x00000000 },
        { 0x5091DEAD, 0xBEEF0000, 0, 0 },
        { 0x50810014, 0x00020002, 0xA11D0000, 0x00000000 },
        { 0x5091CAFE, 0, 0, 0 } },
      0x1,
      midi::mixed_data_set{ midi::manufacturer::ableton, 0, 0, 0, { 0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE } } }
};
