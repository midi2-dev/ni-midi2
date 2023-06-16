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

#include <gtest/gtest.h>

#include <midi/manufacturer.h>
#include <midi/types.h>

//-----------------------------------------------

TEST(velocity, default_constructor)
{
    midi::velocity v;

    EXPECT_EQ(0x8000u, v.value);
}

//-----------------------------------------------

TEST(velocity, construct_from_uint16)
{
    using namespace midi;

    {
        velocity v{ uint16_t{ 0 } };

        EXPECT_EQ(0u, v.value);
    }

    {
        velocity v{ uint16_t{ 0x1234 } };

        EXPECT_EQ(0x1234u, v.value);
    }

    {
        velocity v{ uint16_t{ 0x4823 } };

        EXPECT_EQ(0x4823u, v.value);
    }

    {
        velocity v{ uint16_t{ 0x8000 } };

        EXPECT_EQ(0x8000u, v.value);
    }

    {
        velocity v{ uint16_t{ 0xCABE } };

        EXPECT_EQ(0xCABEu, v.value);
    }

    {
        velocity v{ uint16_t{ 0xFFFF } };

        EXPECT_EQ(0xFFFFu, v.value);
    }
}

//-----------------------------------------------

TEST(velocity, construct_from_uint7)
{
    using namespace midi;

    {
        velocity v{ uint7_t{ 0 } };

        EXPECT_EQ(0u, v.value);
    }

    {
        velocity v{ uint7_t{ 0x12 } };

        EXPECT_EQ(0x2400u, v.value);
    }

    {
        velocity v{ uint7_t{ 0x34 } };

        EXPECT_EQ(0x6800u, v.value);
    }

    {
        velocity v{ uint7_t{ 0x40 } };

        EXPECT_EQ(0x8000u, v.value);
    }

    {
        velocity v{ uint7_t{ 0b101'0011 } };

        EXPECT_EQ(0b1010'0110'1001'1010u, v.value);
    }

    {
        velocity v{ uint7_t{ 0x7F } };

        EXPECT_EQ(0xFFFFu, v.value);
    }
}

//-----------------------------------------------

TEST(velocity, construct_from_float)
{
    using namespace midi;

    {
        velocity v{ 0.f };

        EXPECT_EQ(0u, v.value);
    }

    {
        velocity v{ 0.00001f };

        EXPECT_EQ(0u, v.value);
    }

    {
        velocity v{ 0.00002f };

        EXPECT_LT(0u, v.value);
    }

    {
        velocity v{ 0.125f };

        EXPECT_EQ(0x2000u, v.value);
    }

    {
        velocity v{ 0.2f };

        EXPECT_LT(0x3000u, v.value);
        EXPECT_GT(0x3800u, v.value);
    }

    {
        velocity v{ 0.25f };

        EXPECT_EQ(0x4000u, v.value);
    }

    {
        velocity v{ 0.5f };

        EXPECT_EQ(0x8000u, v.value);
    }

    {
        velocity v{ 0.75f };

        EXPECT_EQ(0xBFFFu, v.value);
    }

    {
        velocity v{ 0.9999f };

        EXPECT_GT(0xFFFFu, v.value);
    }

    {
        velocity v{ 1.f };

        EXPECT_EQ(0xFFFFu, v.value);
    }

    {
        velocity v{ -0.01f };

        EXPECT_EQ(0, v.value);
    }

    {
        velocity v{ -0.8f };

        EXPECT_EQ(0, v.value);
    }

    {
        velocity v{ -99.f };

        EXPECT_EQ(0, v.value);
    }

    {
        velocity v{ 1.01f };

        EXPECT_EQ(0xFFFFu, v.value);
    }

    {
        velocity v{ 42.f };

        EXPECT_EQ(0xFFFFu, v.value);
    }
}

//-----------------------------------------------

TEST(velocity, equality)
{
    using namespace midi;

    EXPECT_EQ(velocity{}, velocity{});
    EXPECT_EQ(velocity{ uint7_t{ 0 } }, velocity{ uint16_t{ 0 } });
    EXPECT_EQ(velocity{ uint7_t{ 64 } }, velocity{ uint16_t{ 0x8000 } });
    EXPECT_EQ(velocity{ uint7_t{ 96 } }, velocity{ uint16_t{ 0b1100000100000100 } });
    EXPECT_EQ(velocity{ uint7_t{ 127 } }, velocity{ uint16_t{ 0xFFFF } });

    EXPECT_NE(velocity{ uint7_t{ 0 } }, velocity{ uint7_t{ 64 } });
    EXPECT_NE(velocity{ uint7_t{ 64 } }, velocity{ uint16_t{ 0xFFFF } });
    EXPECT_NE(velocity{ uint16_t{ 0xFFFF } }, velocity{ uint16_t{ 0x8000 } });
}

//-----------------------------------------------

TEST(velocity, as_float)
{
    using namespace midi;

    EXPECT_FLOAT_EQ(0.f, velocity{ uint7_t{ 0 } }.as_float());
    EXPECT_FLOAT_EQ(0.25f, velocity{ uint7_t{ 32 } }.as_float());
    EXPECT_FLOAT_EQ(0.5f, velocity{ uint16_t{ 0x8000 } }.as_float());
    EXPECT_FLOAT_EQ(1.f, velocity{ uint16_t{ 0xFFFF } }.as_float());
}

//-----------------------------------------------

TEST(velocity, as_uint7)
{
    using namespace midi;

    EXPECT_EQ(0u, velocity{ uint16_t{ 0x0000 } }.as_uint7());
    EXPECT_EQ(16u, velocity{ uint16_t{ 0x2000 } }.as_uint7());
    EXPECT_EQ(32u, velocity{ uint16_t{ 0x4000 } }.as_uint7());
    EXPECT_EQ(64u, velocity{ uint16_t{ 0x8000 } }.as_uint7());
    EXPECT_EQ(96u, velocity{ uint16_t{ 0xC000 } }.as_uint7());
    EXPECT_EQ(127u, velocity{ uint16_t{ 0xFFFF } }.as_uint7());
}

//-----------------------------------------------

TEST(velocity, symmetric_float_conversions)
{
    using namespace midi;

    for (uint7_t n = 0; n < 0x80; ++n)
    {
        velocity a{ n };
        velocity b{ a.as_float() };
        EXPECT_EQ(a, b);
    }

    for (uint16_t v = 0; v < 0xFFFF; ++v)
    {
        velocity a{ v };
        velocity b{ a.as_float() };

        EXPECT_EQ(a.value, b.value);
    }
}

//-----------------------------------------------

TEST(pitch_bend, default_constructor)
{
    using namespace midi;

    pitch_bend pb;

    EXPECT_EQ(0x80000000, pb.value);
}

//-----------------------------------------------

TEST(pitch_bend, construct_from_uint32)
{
    using namespace midi;

    {
        pitch_bend pb{ uint32_t{ 0u } };

        EXPECT_EQ(0x00000000u, pb.value);
    }

    {
        pitch_bend pb{ uint32_t{ 0x12345678 } };

        EXPECT_EQ(0x12345678u, pb.value);
    }

    {
        pitch_bend pb{ uint32_t{ 0x80000000 } };

        EXPECT_EQ(0x80000000u, pb.value);
    }

    {
        pitch_bend pb{ uint32_t{ 0xCDCDCDCD } };

        EXPECT_EQ(0xCDCDCDCDu, pb.value);
    }

    {
        pitch_bend pb{ uint32_t{ 0xFFFFFFFF } };

        EXPECT_EQ(0xFFFFFFFFu, pb.value);
    }
}

//-----------------------------------------------

TEST(pitch_bend, construct_from_uint14)
{
    using namespace midi;

    {
        pitch_bend pb{ uint14_t{ 0u } };

        EXPECT_EQ(0x00000000u, pb.value);
    }

    {
        pitch_bend pb{ uint14_t{ 0x1234 } };

        EXPECT_EQ(0x48D00000u, pb.value);
    }

    {
        pitch_bend pb{ uint14_t{ 0x2000 } };

        EXPECT_EQ(0x80000000, pb.value);
    }

    {
        pitch_bend pb{ uint14_t{ 0b10'1010'0011'1011 } };

        EXPECT_EQ(0b1010'1000'1110'1101'0100'0111'0110'1010u, pb.value);
    }

    {
        pitch_bend pb{ uint14_t{ 0x3FFF } };

        EXPECT_EQ(0xFFFFFFFFu, pb.value);
    }
}

//-----------------------------------------------

TEST(pitch_bend, construct_from_float)
{
    using namespace midi;

    {
        pitch_bend pb{ -1.f };

        EXPECT_EQ(0x00000000u, pb.value);
    }

    {
        pitch_bend pb{ -0.5f };

        EXPECT_EQ(0x40000000u, pb.value);
    }

    {
        pitch_bend pb{ 0.f };

        EXPECT_EQ(0x80000000, pb.value);
    }

    {
        pitch_bend pb{ 0.5f };

        EXPECT_EQ(0xBFFFFFFF, pb.value);
    }

    {
        pitch_bend pb{ 1.f };

        EXPECT_EQ(0xFFFFFFFFu, pb.value);
    }

    {
        pitch_bend pb{ -1.02f };

        EXPECT_EQ(0x00000000u, pb.value);
    }

    {
        pitch_bend pb{ -1.6f };

        EXPECT_EQ(0x00000000u, pb.value);
    }

    {
        pitch_bend pb{ -73.f };

        EXPECT_EQ(0x00000000u, pb.value);
    }

    {
        pitch_bend pb{ 1.04f };

        EXPECT_EQ(0xFFFFFFFFu, pb.value);
    }

    {
        pitch_bend pb{ 99.f };

        EXPECT_EQ(0xFFFFFFFFu, pb.value);
    }
}

//-----------------------------------------------

TEST(pitch_bend, equality)
{
    using namespace midi;

    EXPECT_EQ(pitch_bend{}, pitch_bend{});
    EXPECT_EQ(pitch_bend{ uint14_t{ 0 } }, pitch_bend{ uint32_t{ 0 } });
    EXPECT_EQ(pitch_bend{ uint14_t{ 0x2000 } }, pitch_bend{ uint32_t{ 0x80000000 } });
    EXPECT_EQ(pitch_bend{ uint14_t{ 0x3000 } }, pitch_bend{ uint32_t{ 0b1100'0000'0000'0010'0000'0000'0001'0000 } });
    EXPECT_EQ(pitch_bend{ uint14_t{ 0x3FFF } }, pitch_bend{ uint32_t{ 0xFFFFFFFF } });

    EXPECT_NE(pitch_bend{ uint14_t{ 0 } }, pitch_bend{ uint14_t{ 0x2000 } });
    EXPECT_NE(pitch_bend{ uint14_t{ 0x2000 } }, pitch_bend{ uint32_t{ 0xFFFF } });
    EXPECT_NE(pitch_bend{ uint32_t{ 0xFFFF } }, pitch_bend{ uint32_t{ 0x8000 } });
}

//-----------------------------------------------

TEST(pitch_bend, as_float)
{
    using namespace midi;

    EXPECT_FLOAT_EQ(-1.0f, pitch_bend{ uint32_t{ 0x00000000 } }.as_float());
    EXPECT_FLOAT_EQ(-0.5f, pitch_bend{ uint32_t{ 0x40000000 } }.as_float());
    EXPECT_FLOAT_EQ(0.0f, pitch_bend{ uint32_t{ 0x80000000 } }.as_float());
    EXPECT_FLOAT_EQ(0.5f, pitch_bend{ uint32_t{ 0xBFFFFFFF } }.as_float());
    EXPECT_FLOAT_EQ(1.0f, pitch_bend{ uint32_t{ 0xFFFFFFFF } }.as_float());
}

//-----------------------------------------------

TEST(pitch_bend, as_uint14)
{
    using namespace midi;

    EXPECT_EQ(0x0000u, pitch_bend{ uint32_t{ 0x00000000 } }.as_uint14());
    EXPECT_EQ(0x048Du, pitch_bend{ uint32_t{ 0x12345678 } }.as_uint14());
    EXPECT_EQ(0x2000u, pitch_bend{ uint32_t{ 0x80000000 } }.as_uint14());
    EXPECT_EQ(0x3373u, pitch_bend{ uint32_t{ 0xCDCDCDCD } }.as_uint14());
    EXPECT_EQ(0x3FFFu, pitch_bend{ uint32_t{ 0xFFFFFFFF } }.as_uint14());
}

//-----------------------------------------------

TEST(pitch_bend, reset)
{
    using namespace midi;

    pitch_bend pb{ -0.44f };

    pb.reset();

    EXPECT_EQ(0x80000000, pb.value);
}

//-----------------------------------------------

TEST(pitch_increment, default_constructor)
{
    midi::pitch_increment inc;
    EXPECT_EQ(0, inc.value);
}

//-----------------------------------------------

TEST(pitch_increment, construct_from_int32)
{
    using namespace midi;

    {
        pitch_increment inc{ 99 };

        EXPECT_EQ(99, inc.value);
    }

    {
        pitch_increment inc{ 0x12345678 };

        EXPECT_EQ(0x12345678, inc.value);
    }

    {
        pitch_increment inc{ 0x48888888 };

        EXPECT_EQ(0x48888888, inc.value);
    }

    {
        pitch_increment inc{ 0x7FFFFFFF };

        EXPECT_EQ(0x7FFFFFFF, inc.value);
    }

    {
        pitch_increment inc{ -778899 };

        EXPECT_EQ(-778899, inc.value);
    }
}

//-----------------------------------------------

TEST(pitch_increment, construct_from_float)
{
    using namespace midi;

    constexpr int32_t one = 0x2000000;

    {
        pitch_increment inc{ 0.f };

        EXPECT_EQ(0, inc.value);
    }

    {
        pitch_increment inc{ 1.f };

        EXPECT_EQ(one, inc.value);
    }

    {
        pitch_increment inc{ -1.f };

        EXPECT_EQ(-one, inc.value);
    }

    {
        pitch_increment inc{ 0.5f };

        EXPECT_EQ(0x1000000, inc.value);
    }

    {
        pitch_increment inc{ -4.f };

        EXPECT_EQ(-4 * one, inc.value);
    }

    {
        pitch_increment inc{ 17.25f };

        EXPECT_EQ(17 * one + 0x800000, inc.value);
    }

    {
        pitch_increment inc{ -47.75f };

        EXPECT_EQ(-47 * one - 0x1800000, inc.value);
    }

    {
        pitch_increment inc{ 63.999999f };

        EXPECT_EQ(0x7FFFFFFF, inc.value);
    }

    {
        pitch_increment inc{ 67.f };

        EXPECT_EQ(std::numeric_limits<int32_t>::max(), inc.value);
    }

    {
        pitch_increment inc{ -99.23f };

        EXPECT_EQ(std::numeric_limits<int32_t>::min(), inc.value);
    }
}

//-----------------------------------------------

TEST(pitch_increment, operator_plus_assign)
{
    using namespace midi;

    constexpr int32_t one  = 0x2000000;
    constexpr int32_t pt5  = 0x1000000;
    constexpr int32_t pt25 = 0x0800000;
    constexpr int32_t pt75 = 0x1800000;

    {
        pitch_increment a{ pt5 };

        a += pitch_increment{ pt25 };

        EXPECT_EQ(pt75, a.value);
    }

    {
        pitch_increment a{ pt25 };

        a += pitch_increment{ pt75 };

        EXPECT_EQ(one, a.value);
    }

    {
        pitch_increment a{ one };

        a += pitch_increment{ -pt25 };

        EXPECT_EQ(pt75, a.value);
    }

    {
        pitch_increment a{ pt25 };

        a += pitch_increment{ -one };

        EXPECT_EQ(-pt75, a.value);
    }

    {
        pitch_increment a{ 8.75f };

        a += pitch_increment{ 2.25f };

        EXPECT_EQ(11 * one, a.value);
    }

    {
        pitch_increment a{ 8.75f };

        a += pitch_increment{ -2.25f };

        EXPECT_EQ(6 * one + pt5, a.value);
    }

    {
        pitch_increment a{ 12.5f };

        a += pitch_increment{ -25.f };

        EXPECT_EQ(-12 * one - pt5, a.value);
    }
}

//-----------------------------------------------

TEST(pitch_increment, operator_plus)
{
    using namespace midi;

    constexpr int32_t one  = 0x2000000;
    constexpr int32_t pt5  = 0x1000000;
    constexpr int32_t pt25 = 0x0800000;
    constexpr int32_t pt75 = 0x1800000;

    EXPECT_EQ(0, (pitch_increment{ -pt5 } + pitch_increment{ pt5 }).value);
    EXPECT_EQ(one, (pitch_increment{ pt25 } + pitch_increment{ pt75 }).value);
    EXPECT_EQ(pt75, (pitch_increment{ pt25 } + pitch_increment{ pt25 } + pitch_increment{ pt25 }).value);
    EXPECT_EQ(-pt75, (pitch_increment{ pt25 } + pitch_increment{ -one }).value);

    EXPECT_EQ(pitch_increment{ 15.f }.value, (pitch_increment{ 12.5f } + pitch_increment{ 2.5f }).value);
    EXPECT_EQ(pitch_increment{ -9.5f }.value, (pitch_increment{ 8.f } + pitch_increment{ -17.5f }).value);
    EXPECT_EQ(pitch_increment{ 3 * one }.value, (pitch_increment{ one } + pitch_increment{ 2.f }).value);
}

//-----------------------------------------------

TEST(pitch_increment, equality)
{
    using namespace midi;

    pitch_increment a{ 0x73D46BA4 };
    pitch_increment b{ 0xC48F54C };
    pitch_increment c{ -562234 };

    EXPECT_EQ(pitch_increment{}, pitch_increment{ 0 });
    EXPECT_EQ(a, pitch_increment{ 0x73D46BA4 });
    EXPECT_EQ(b, pitch_increment{ 0xC48F54C });
    EXPECT_EQ(c, pitch_increment{ -562234 });

    EXPECT_NE(a, b);
    EXPECT_NE(b, c);
    EXPECT_NE(pitch_increment{ 0 }, pitch_increment{ 0x20002000 });
    EXPECT_NE(pitch_increment{ 0x73D46BA4 }, pitch_increment{ 0xC48F54C });
    EXPECT_NE(pitch_increment{ -562234 }, pitch_increment{ 562234 });
}

//-----------------------------------------------

TEST(pitch_7_9, default_constructor)
{
    midi::pitch_7_9 p;
    EXPECT_EQ(0x0000u, p.value);
}

//-----------------------------------------------

TEST(pitch_7_9, construct_from_uint16)
{
    using namespace midi;

    {
        pitch_7_9 p{ uint16_t{ 0u } };

        EXPECT_EQ(0x0000u, p.value);
    }

    {
        pitch_7_9 p{ uint16_t{ 0x1234 } };

        EXPECT_EQ(0x1234u, p.value);
    }

    {
        pitch_7_9 p{ uint16_t{ 0x5678 } };

        EXPECT_EQ(0x5678u, p.value);
    }

    {
        pitch_7_9 p{ uint16_t{ 0x8000 } };

        EXPECT_EQ(0x8000u, p.value);
    }

    {
        pitch_7_9 p{ uint16_t{ 0x9ABC } };

        EXPECT_EQ(0x9ABCu, p.value);
    }

    {
        pitch_7_9 p{ uint16_t{ 0xFFFF } };

        EXPECT_EQ(0xFFFFu, p.value);
    }
}

//-----------------------------------------------

TEST(pitch_7_9, construct_from_note_nr)
{
    using namespace midi;

    {
        pitch_7_9 p{ note_nr_t{ 0 } };

        EXPECT_EQ(0x0000u, p.value);
    }

    {
        pitch_7_9 p{ note_nr_t{ 64 } };

        EXPECT_EQ(0x8000u, p.value);
    }

    {
        pitch_7_9 p{ note_nr_t{ 17 } };

        EXPECT_EQ(0x2200u, p.value);
    }

    {
        pitch_7_9 p{ note_nr_t{ 99 } };

        EXPECT_EQ(0xC600u, p.value);
    }

    {
        pitch_7_9 p{ note_nr_t{ 127 } };

        EXPECT_EQ(0xFE00u, p.value);
    }
}

//-----------------------------------------------

TEST(pitch_7_9, construct_from_float)
{
    using namespace midi;

    {
        pitch_7_9 p{ 0.f };

        EXPECT_EQ(0x0000u, p.value);
    }

    {
        pitch_7_9 p{ 64.5f };

        EXPECT_EQ(0x8100u, p.value);
    }

    {
        pitch_7_9 p{ 17.25f };

        EXPECT_EQ(0x2280u, p.value);
    }

    {
        pitch_7_9 p{ 99.75f };

        EXPECT_EQ(0xC780u, p.value);
    }

    {
        pitch_7_9 p{ 127.9999f };

        EXPECT_EQ(0xFFFFu, p.value);
    }

    {
        pitch_7_9 p{ -0.5f };

        EXPECT_EQ(0x0000u, p.value);
    }

    {
        pitch_7_9 p{ -99.23f };

        EXPECT_EQ(0x0000u, p.value);
    }

    {
        pitch_7_9 p{ 128.f };

        EXPECT_EQ(0xFFFFu, p.value);
    }

    {
        pitch_7_9 p{ 1000.f };

        EXPECT_EQ(0xFFFFu, p.value);
    }
}

//-----------------------------------------------

TEST(pitch_7_9, equality)
{
    using namespace midi;

    EXPECT_EQ(pitch_7_9{ uint16_t{ 0 } }, pitch_7_9{ note_nr_t{ 0 } });
    EXPECT_EQ(pitch_7_9{ uint16_t{ 0x8000 } }, pitch_7_9{ note_nr_t{ 64 } });
    EXPECT_EQ(pitch_7_9{ uint16_t{ 0x3000 } }, pitch_7_9{ 24.f });
    EXPECT_EQ(pitch_7_9{ uint16_t{ 0xFE00 } }, pitch_7_9{ note_nr_t{ 127 } });

    EXPECT_NE(pitch_7_9{ uint16_t{ 0 } }, pitch_7_9{ uint16_t{ 0x2000 } });
    EXPECT_NE(pitch_7_9{ uint16_t{ 0x2000 } }, pitch_7_9{ uint16_t{ 0xFFFF } });
    EXPECT_NE(pitch_7_9{ uint16_t{ 0xFFFF } }, pitch_7_9{ uint16_t{ 0x8000 } });
}

//-----------------------------------------------

TEST(pitch_7_9, as_float)
{
    using namespace midi;

    EXPECT_FLOAT_EQ(0.0f, pitch_7_9{ uint16_t{ 0x0000 } }.as_float());
    EXPECT_FLOAT_EQ(32.f, pitch_7_9{ uint16_t{ 0x4000 } }.as_float());
    EXPECT_FLOAT_EQ(64.25f, pitch_7_9{ uint16_t{ 0x8080 } }.as_float());
    EXPECT_FLOAT_EQ(96.125f, pitch_7_9{ uint16_t{ 0xC040 } }.as_float());
    EXPECT_FLOAT_EQ(127.5f, pitch_7_9{ uint16_t{ 0xFF00 } }.as_float());
}

//-----------------------------------------------

TEST(pitch_7_9, note_nr)
{
    using namespace midi;

    EXPECT_EQ(0u, pitch_7_9{ uint16_t{ 0x0000 } }.note_nr());
    EXPECT_EQ(9u, pitch_7_9{ uint16_t{ 0x1234 } }.note_nr());
    EXPECT_EQ(64u, pitch_7_9{ uint16_t{ 0x8000 } }.note_nr());
    EXPECT_EQ(99u, pitch_7_9{ note_nr_t{ 99 } }.note_nr());
    EXPECT_EQ(117u, pitch_7_9{ 117.56f }.note_nr());
    EXPECT_EQ(127u, pitch_7_9{ uint16_t{ 0xFFFF } }.note_nr());
}

//-----------------------------------------------

TEST(pitch_7_9, symmetric_float_conversions)
{
    using namespace midi;

    for (note_nr_t n = 0; n < 0x80; ++n)
    {
        pitch_7_9 a{ n };
        pitch_7_9 b{ a.as_float() };
        EXPECT_EQ(a, b);
    }

    for (uint16_t v = 0; v < 0xFFFF; ++v)
    {
        pitch_7_9 a{ v };
        pitch_7_9 b{ a.as_float() };

        EXPECT_EQ(a.value, b.value);
    }
}

//-----------------------------------------------

TEST(pitch_7_25, default_constructor)
{
    midi::pitch_7_25 p;
    EXPECT_EQ(0x00000000u, p.value);
}

//-----------------------------------------------

TEST(pitch_7_25, construct_from_uint32)
{
    using namespace midi;

    {
        pitch_7_25 p{ uint32_t{ 0u } };

        EXPECT_EQ(0x00000000u, p.value);
    }

    {
        pitch_7_25 p{ uint32_t{ 0x12345678 } };

        EXPECT_EQ(0x12345678u, p.value);
    }

    {
        pitch_7_25 p{ uint32_t{ 0x80000000 } };

        EXPECT_EQ(0x80000000u, p.value);
    }

    {
        pitch_7_25 p{ uint32_t{ 0xCDCDCDCD } };

        EXPECT_EQ(0xCDCDCDCDu, p.value);
    }

    {
        pitch_7_25 p{ uint32_t{ 0xFFFFFFFF } };

        EXPECT_EQ(0xFFFFFFFFu, p.value);
    }
}

//-----------------------------------------------

TEST(pitch_7_25, construct_from_note_nr)
{
    using namespace midi;

    {
        pitch_7_25 p{ note_nr_t{ 0 } };

        EXPECT_EQ(0x00000000u, p.value);
    }

    {
        pitch_7_25 p{ note_nr_t{ 64 } };

        EXPECT_EQ(0x80000000u, p.value);
    }

    {
        pitch_7_25 p{ note_nr_t{ 17 } };

        EXPECT_EQ(0x22000000u, p.value);
    }

    {
        pitch_7_25 p{ note_nr_t{ 99 } };

        EXPECT_EQ(0xC6000000u, p.value);
    }

    {
        pitch_7_25 p{ note_nr_t{ 127 } };

        EXPECT_EQ(0xFE000000u, p.value);
    }
}

//-----------------------------------------------

TEST(pitch_7_25, construct_from_pitch7_9)
{
    using namespace midi;

    {
        pitch_7_25 p{ pitch_7_9{ uint16_t{ 0x1234u } } };

        EXPECT_EQ(0x12340000u, p.value);
    }

    {
        pitch_7_25 p{ pitch_7_9{ note_nr_t{ 64 } } };

        EXPECT_EQ(0x80000000u, p.value);
    }

    {
        pitch_7_25 p{ pitch_7_9{ uint16_t{ 0xABCDu } } };

        EXPECT_EQ(0xABCD0000u, p.value);
    }

    {
        pitch_7_25 p{ pitch_7_9{ 69.75f } };

        EXPECT_FLOAT_EQ(69.75f, p.as_float());
    }
}

//-----------------------------------------------

TEST(pitch_7_25, construct_from_float)
{
    using namespace midi;

    {
        pitch_7_25 p{ 0.f };

        EXPECT_EQ(0x00000000u, p.value);
    }

    {
        pitch_7_25 p{ 64.5f };

        EXPECT_EQ(0x81000000u, p.value);
    }

    {
        pitch_7_25 p{ 17.25f };

        EXPECT_EQ(0x22800000u, p.value);
    }

    {
        pitch_7_25 p{ 99.75f };

        EXPECT_EQ(0xC7800000u, p.value);
    }

    {
        pitch_7_25 p{ 127.9999f };

        EXPECT_EQ(0xFFFFFFFFu, (p.value | 0xFFF)); // uint32_t precision > float precision
    }

    {
        pitch_7_25 p{ -0.5f };

        EXPECT_EQ(0x00000000u, p.value);
    }

    {
        pitch_7_25 p{ -99.23f };

        EXPECT_EQ(0x00000000u, p.value);
    }

    {
        pitch_7_25 p{ 128.f };

        EXPECT_EQ(0xFFFFFFFFu, p.value);
    }

    {
        pitch_7_25 p{ 1000.f };

        EXPECT_EQ(0xFFFFFFFFu, p.value);
    }
}

//-----------------------------------------------

TEST(pitch_7_25, equality)
{
    using namespace midi;

    EXPECT_EQ(pitch_7_25{ uint32_t{ 0 } }, pitch_7_25{ note_nr_t{ 0 } });
    EXPECT_EQ(pitch_7_25{ uint32_t{ 0x80000000 } }, pitch_7_25{ note_nr_t{ 64 } });
    EXPECT_EQ(pitch_7_25{ uint32_t{ 0x30000000 } }, pitch_7_25{ 24.f });
    EXPECT_EQ(pitch_7_25{ uint32_t{ 0xFE000000 } }, pitch_7_25{ note_nr_t{ 127 } });

    EXPECT_NE(pitch_7_25{ uint32_t{ 0 } }, pitch_7_25{ uint32_t{ 0x2000 } });
    EXPECT_NE(pitch_7_25{ uint32_t{ 0x2000 } }, pitch_7_25{ uint32_t{ 0xFFFF } });
    EXPECT_NE(pitch_7_25{ uint32_t{ 0xFFFF } }, pitch_7_25{ uint32_t{ 0x8000 } });
}

//-----------------------------------------------

TEST(pitch_7_25, as_float)
{
    using namespace midi;

    EXPECT_EQ(0.0f, pitch_7_25{ uint32_t{ 0x00000000 } }.as_float());
    EXPECT_EQ(32.f, pitch_7_25{ uint32_t{ 0x40000000 } }.as_float());
    EXPECT_EQ(64.25f, pitch_7_25{ uint32_t{ 0x80800000 } }.as_float());
    EXPECT_EQ(96.125f, pitch_7_25{ uint32_t{ 0xC0400000 } }.as_float());
    EXPECT_EQ(127.5f, pitch_7_25{ uint32_t{ 0xFF000000 } }.as_float());
}

//-----------------------------------------------

TEST(pitch_7_25, note_nr)
{
    using namespace midi;

    EXPECT_EQ(0u, pitch_7_25{ uint32_t{ 0x00000000 } }.note_nr());
    EXPECT_EQ(9u, pitch_7_25{ uint32_t{ 0x12345678 } }.note_nr());
    EXPECT_EQ(64u, pitch_7_25{ uint32_t{ 0x80000000 } }.note_nr());
    EXPECT_EQ(99u, pitch_7_25{ note_nr_t{ 99 } }.note_nr());
    EXPECT_EQ(117u, pitch_7_25{ 117.56f }.note_nr());
    EXPECT_EQ(127u, pitch_7_25{ uint32_t{ 0xFFFFFFFF } }.note_nr());
}

//-----------------------------------------------

TEST(pitch_7_25, assign_pitch_7_9)
{
    using namespace midi;
    pitch_7_25 p{ note_nr_t{ 64 } };

    {
        p = pitch_7_9{ uint16_t{ 0x0000 } };

        EXPECT_EQ(0x00000000u, p.value);
    }

    {
        p = pitch_7_9{ uint16_t{ 0xA0CD } };

        EXPECT_EQ(0xA0CD0000u, p.value);
    }

    {
        p = pitch_7_9{ 99.5f };

        EXPECT_EQ(0xC7000000u, p.value);
    }

    {
        p = pitch_7_9{ uint16_t{ 0b00110001'01101001 } };

        EXPECT_EQ(0b00110001'01101001'00000000'00000000u, p.value);
    }

    {
        p = pitch_7_9{ uint16_t{ 0xFFFF } };

        EXPECT_EQ(0xFFFF0000u, p.value);
    }
}

//-----------------------------------------------

TEST(pitch_7_25, operator_plus_pitch_increment)
{
    using namespace midi;

    constexpr auto pt5  = pitch_increment{ 0x1000000 };
    constexpr auto pt75 = pitch_increment{ 0x1800000 };

    const auto minus_one  = pitch_increment{ -1.f };
    const auto minus_pt5  = pitch_increment{ -.5f };
    const auto minus_pt75 = pitch_increment{ -.75f };

    EXPECT_FLOAT_EQ(19.5f, (pitch_7_25{ 19.f } + pt5).as_float());
    EXPECT_FLOAT_EQ(64.75f, (pitch_7_25{ note_nr_t{ 64 } } + pt75).as_float());

    EXPECT_FLOAT_EQ(98.f, (pitch_7_25{ note_nr_t{ 99 } } + minus_one).as_float());
    EXPECT_FLOAT_EQ(118.f, (pitch_7_25{ 117.25f } + pt75).as_float());

    EXPECT_FLOAT_EQ(18.5f, (pitch_7_25{ 19.f } + minus_pt5).as_float());
    EXPECT_FLOAT_EQ(63.25f, (pitch_7_25{ note_nr_t{ 64 } } + minus_pt75).as_float());

    EXPECT_FLOAT_EQ(98.9f, (pitch_7_25{ note_nr_t{ 99 } } + pitch_increment{ -0.1f }).as_float());
    EXPECT_FLOAT_EQ(117.12f, (pitch_7_25{ 117.56f } + pitch_increment{ -.44f }).as_float());
}

//-----------------------------------------------

TEST(pitch_7_25, operator_plus_float)
{
    using namespace midi;

    EXPECT_FLOAT_EQ(19.5f, (pitch_7_25{ 19.f } + 0.5f).as_float());
    EXPECT_FLOAT_EQ(64.75f, (pitch_7_25{ note_nr_t{ 64 } } + 0.75f).as_float());

    EXPECT_FLOAT_EQ(99.1f, (pitch_7_25{ note_nr_t{ 99 } } + 0.1f).as_float());
    EXPECT_FLOAT_EQ(118.f, (pitch_7_25{ 117.56f } + .44f).as_float());

    EXPECT_EQ(0xFFFFFFFFu, (pitch_7_25{ 127.77f } + 1.23f).value);

    EXPECT_FLOAT_EQ(18.5f, (pitch_7_25{ 19.f } + -0.5f).as_float());
    EXPECT_FLOAT_EQ(63.25f, (pitch_7_25{ note_nr_t{ 64 } } + -0.75f).as_float());

    EXPECT_FLOAT_EQ(98.9f, (pitch_7_25{ note_nr_t{ 99 } } + -0.1f).as_float());
    EXPECT_FLOAT_EQ(117.12f, (pitch_7_25{ 117.56f } + -.44f).as_float());

    EXPECT_EQ(0x00000000u, (pitch_7_25{ 1.f } + -1.23f).value);
}

//-----------------------------------------------

TEST(pitch_7_25, operator_plus_assign_pitch_increment)
{
    using namespace midi;

    constexpr auto pt5  = pitch_increment{ 0x1000000 };
    constexpr auto pt75 = pitch_increment{ 0x1800000 };

    const auto minus_one = pitch_increment{ -1.f };
    const auto minus_pt5 = pitch_increment{ -.5f };

    {
        pitch_7_25 p{ note_nr_t{ 60 } };

        p += pt5;

        EXPECT_EQ(pitch_7_25{ 60.5f }, p);
    }

    {
        pitch_7_25 p{ 77.25f };

        p += pt75;

        EXPECT_EQ(pitch_7_25{ note_nr_t{ 78 } }, p);
    }

    {
        pitch_7_25 p{ note_nr_t{ 50 } };

        p += minus_one;

        EXPECT_EQ(pitch_7_25{ note_nr_t{ 49 } }, p);

        p += minus_pt5;

        EXPECT_EQ(pitch_7_25{ 48.5f }, p);
    }
}

//-----------------------------------------------

TEST(pitch_bend_sensitivity, default_constructor)
{
    midi::pitch_bend_sensitivity sens;
    EXPECT_EQ(0x04000000u, sens.value);
}

//-----------------------------------------------

TEST(pitch_bend_sensitivity, operator_times)
{
    using namespace midi;

    constexpr auto n = pitch_7_25{ note_nr_t{ 10 } };

    {
        constexpr auto a = pitch_bend{} * pitch_bend_sensitivity{ note_nr_t{ 1 } };

        EXPECT_EQ(a, pitch_increment{ 0 });
    }

    {
        constexpr auto a = pitch_bend{ 0.5f } * pitch_bend_sensitivity{ note_nr_t{ 1 } };

        EXPECT_EQ(a, pitch_bend_sensitivity{ note_nr_t{ 1 } } * pitch_bend{ 0.5f });

        EXPECT_FLOAT_EQ(n.as_float() + .5f, (n + a).as_float());
    }

    {
        constexpr auto a = pitch_bend{ -0.5f } * pitch_bend_sensitivity{ note_nr_t{ 2 } };

        EXPECT_EQ(a, pitch_bend_sensitivity{ note_nr_t{ 2 } } * pitch_bend{ -0.5f });

        EXPECT_FLOAT_EQ(n.as_float() - 1.0f, (n + a).as_float());
    }

    {
        const auto a = pitch_bend{ 1.f } * pitch_bend_sensitivity{ 2.5f };

        EXPECT_EQ(a, pitch_bend_sensitivity{ 2.5f } * pitch_bend{ 1.f });

        EXPECT_FLOAT_EQ(n.as_float() + 2.5f, (n + a).as_float());
    }

    {
        const auto a = pitch_bend{ -1.f } * pitch_bend_sensitivity{ 4.f };

        EXPECT_EQ(a, pitch_bend_sensitivity{ 4.f } * pitch_bend{ -1.f });

        EXPECT_FLOAT_EQ(n.as_float() - 4.f, (n + a).as_float());
    }
}

//-----------------------------------------------

TEST(controller_increment, default_constructor)
{
    midi::controller_increment inc;
    EXPECT_EQ(0, inc.value);
}

//-----------------------------------------------

TEST(controller_increment, construct_from_int32)
{
    using namespace midi;

    {
        controller_increment inc{ 99 };

        EXPECT_EQ(99, inc.value);
    }

    {
        controller_increment inc{ 0x12345678 };

        EXPECT_EQ(0x12345678, inc.value);
    }

    {
        controller_increment inc{ 0x48888888 };

        EXPECT_EQ(0x48888888, inc.value);
    }

    {
        controller_increment inc{ 0x7FFFFFFF };

        EXPECT_EQ(0x7FFFFFFF, inc.value);
    }

    {
        controller_increment inc{ -778899 };

        EXPECT_EQ(-778899, inc.value);
    }
}

//-----------------------------------------------

TEST(controller_increment, equality)
{
    using namespace midi;

    controller_increment a{ 0x73D46BA4 };
    controller_increment b{ 0xC48F54C };
    controller_increment c{ -562234 };

    EXPECT_EQ(controller_increment{}, controller_increment{ 0 });
    EXPECT_EQ(a, controller_increment{ 0x73D46BA4 });
    EXPECT_EQ(b, controller_increment{ 0xC48F54C });
    EXPECT_EQ(c, controller_increment{ -562234 });

    EXPECT_NE(a, b);
    EXPECT_NE(b, c);
    EXPECT_NE(controller_increment{ 0 }, controller_increment{ 0x20002000 });
    EXPECT_NE(controller_increment{ 0x73D46BA4 }, controller_increment{ 0xC48F54C });
    EXPECT_NE(controller_increment{ -562234 }, controller_increment{ 562234 });
}

//-----------------------------------------------

TEST(controller_value, default_constructor)
{
    midi::controller_value v;
    EXPECT_EQ(0x00000000u, v.value);
}

//-----------------------------------------------

TEST(controller_value, construct_from_uint32)
{
    using namespace midi;

    {
        controller_value v{ uint32_t{ 0u } };

        EXPECT_EQ(0x00000000u, v.value);
    }

    {
        controller_value v{ uint32_t{ 0x12345678 } };

        EXPECT_EQ(0x12345678u, v.value);
    }

    {
        controller_value v{ uint32_t{ 0x80000000 } };

        EXPECT_EQ(0x80000000u, v.value);
    }

    {
        controller_value v{ uint32_t{ 0xCDCDCDCD } };

        EXPECT_EQ(0xCDCDCDCDu, v.value);
    }

    {
        controller_value v{ uint32_t{ 0xFFFFFFFF } };

        EXPECT_EQ(0xFFFFFFFFu, v.value);
    }
}

//-----------------------------------------------

TEST(controller_value, construct_from_uint7)
{
    using namespace midi;

    {
        controller_value v{ uint7_t{ 0u } };

        EXPECT_EQ(0x00000000u, v.value);
    }

    {
        controller_value v{ uint7_t{ 0x3C } };

        EXPECT_EQ(0x78000000u, v.value);
    }

    {
        controller_value v{ uint7_t{ 64 } };

        EXPECT_EQ(0x80000000u, v.value);
    }

    {
        controller_value v{ uint7_t{ 0b101'1001 } };

        EXPECT_EQ(0b1011'0010'1100'1011'0010'1100'1011'0010u, v.value);
    }

    {
        controller_value v{ uint7_t{ 127 } };

        EXPECT_EQ(0xFFFFFFFFu, v.value);
    }
}

//-----------------------------------------------

TEST(controller_value, construct_from_float)
{
    using namespace midi;

    {
        controller_value v{ 0.f };

        EXPECT_EQ(0x00000000u, v.value);
    }

    {
        controller_value v{ 0.25f };

        EXPECT_EQ(0x40000000u, v.value);
    }

    {
        controller_value v{ 0.5f };

        EXPECT_EQ(0x80000000, v.value);
    }

    {
        controller_value v{ 0.75f };

        EXPECT_EQ(0xBFFFFFFF, v.value);
    }

    {
        controller_value v{ 1.f };

        EXPECT_EQ(0xFFFFFFFFu, v.value);
    }

    {
        controller_value v{ -0.01f };

        EXPECT_EQ(0x00000000u, v.value);
    }

    {
        controller_value v{ -0.5f };

        EXPECT_EQ(0x00000000u, v.value);
    }

    {
        controller_value v{ -108.f };

        EXPECT_EQ(0x00000000u, v.value);
    }

    {
        controller_value v{ 1.4f };

        EXPECT_EQ(0xFFFFFFFFu, v.value);
    }

    {
        controller_value v{ 99.f };

        EXPECT_EQ(0xFFFFFFFFu, v.value);
    }
}

//-----------------------------------------------

TEST(controller_value, equality)
{
    using namespace midi;

    EXPECT_EQ(controller_value{ uint7_t{ 0 } }, controller_value{ uint32_t{ 0 } });
    EXPECT_EQ(controller_value{ uint7_t{ 64 } }, controller_value{ uint32_t{ 0x80000000 } });
    EXPECT_EQ(controller_value{ uint7_t{ 77 } },
              controller_value{ uint32_t{ 0b1001'1010'0110'1001'1010'0110'1001'1010 } });
    EXPECT_EQ(controller_value{ uint7_t{ 127 } }, controller_value{ uint32_t{ 0xFFFFFFFF } });

    EXPECT_NE(controller_value{ uint7_t{ 0 } }, controller_value{ uint32_t{ 0x20002000 } });
    EXPECT_NE(controller_value{ uint7_t{ 0x40 } }, controller_value{ uint32_t{ 0xFFFFFFFF } });
    EXPECT_NE(controller_value{ uint32_t{ 0xFFFFFFFF } }, controller_value{ uint32_t{ 0x80000000 } });
}

//-----------------------------------------------

TEST(controller_value, as_float)
{
    using namespace midi;

    EXPECT_FLOAT_EQ(0.0f, controller_value{ uint32_t{ 0x00000000 } }.as_float());
    EXPECT_FLOAT_EQ(0.25f, controller_value{ uint32_t{ 0x40000000 } }.as_float());
    EXPECT_FLOAT_EQ(0.5f, controller_value{ uint32_t{ 0x80000000 } }.as_float());
    EXPECT_FLOAT_EQ(0.75f, controller_value{ uint32_t{ 0xBFFFFFFF } }.as_float());
    EXPECT_FLOAT_EQ(1.0f, controller_value{ uint32_t{ 0xFFFFFFFF } }.as_float());
}

//-----------------------------------------------

TEST(controller_value, as_uint7)
{
    using namespace midi;

    EXPECT_EQ(0u, controller_value{ uint32_t{ 0x00000000 } }.as_uint7());
    EXPECT_EQ(0u, controller_value{ uint32_t{ 0x00123456 } }.as_uint7());
    EXPECT_EQ(10u, controller_value{ uint32_t{ 0x15738382 } }.as_uint7());
    EXPECT_EQ(16u, controller_value{ uint32_t{ 0x20000000 } }.as_uint7());
    EXPECT_EQ(27u, controller_value{ uint32_t{ 0x379AFF43 } }.as_uint7());
    EXPECT_EQ(32u, controller_value{ uint32_t{ 0x40000000 } }.as_uint7());
    EXPECT_EQ(64u, controller_value{ uint32_t{ 0x80000000 } }.as_uint7());
    EXPECT_EQ(85u, controller_value{ uint32_t{ 0xABCDEF12 } }.as_uint7());
    EXPECT_EQ(96u, controller_value{ uint32_t{ 0xC0000000 } }.as_uint7());
    EXPECT_EQ(127u, controller_value{ uint32_t{ 0xFFFFFFFF } }.as_uint7());
}

//-----------------------------------------------

TEST(controller_value, plus_controller_increment)
{
    using namespace midi;

    {
        const controller_value v{ uint32_t{ 0 } };

        EXPECT_EQ(0u, (v + controller_increment{ 0 }).value);
        EXPECT_EQ(44u, (v + controller_increment{ 44 }).value);
        EXPECT_EQ(0u, (v + controller_increment{ -5 }).value);
    }

    {
        const controller_value v{ uint32_t{ 0x379AFF43 } };

        EXPECT_EQ(0x379AFF43u, (v + controller_increment{ 0 }).value);
        EXPECT_EQ(0x379AFF6Fu, (v + controller_increment{ 44 }).value);
        EXPECT_EQ(0x379AFF3Eu, (v + controller_increment{ -5 }).value);
        EXPECT_EQ(0x00000000u, (v + controller_increment{ -932904815 }).value);
    }

    {
        const controller_value v{ uint32_t{ 0xABCDEF12 } };

        EXPECT_EQ(0xABCDEF12u, (v + controller_increment{ 0 }).value);
        EXPECT_EQ(0xABCDEF3Eu, (v + controller_increment{ 44 }).value);
        EXPECT_EQ(0xABCDEF0Du, (v + controller_increment{ -5 }).value);
        EXPECT_EQ(0xFFFFFFFFu, (v + controller_increment{ 0x7FFFFFFF }).value);
    }

    {
        const controller_value v{ uint32_t{ 0xEF43A670 } };

        EXPECT_EQ(0xEF43A670u, (v + controller_increment{ 0 }).value);
        EXPECT_EQ(0xEF43A69Cu, (v + controller_increment{ 44 }).value);
        EXPECT_EQ(0xEF43A66Bu, (v + controller_increment{ -5 }).value);
        EXPECT_EQ(0xFFFFFFFFu, (v + controller_increment{ 0x20000000 }).value);
    }

    {
        const controller_value v{ uint32_t{ 0xFFFFFFFF } };

        EXPECT_EQ(0xFFFFFFFFu, (v + controller_increment{ 0 }).value);
        EXPECT_EQ(0xFFFFFFFFu, (v + controller_increment{ 44 }).value);
        EXPECT_EQ(0xFFFFFFFAu, (v + controller_increment{ -5 }).value);
    }
}

//-----------------------------------------------

TEST(device_identity, constructor)
{
    using namespace midi;

    {
        constexpr auto identity = device_identity{ manufacturer::native_instruments, 0x1730, 49, 0x00010005 };

        EXPECT_EQ(0x002109u, identity.manufacturer);
        EXPECT_EQ(0x1730u, identity.family);
        EXPECT_EQ(49u, identity.model);
        EXPECT_EQ(0x00010005u, identity.revision);
    }

    {
        auto identity = device_identity{ manufacturer::roland, 0x0808, 0x11, 0x00050102 };

        EXPECT_EQ(0x410000u, identity.manufacturer);
        EXPECT_EQ(0x0808u, identity.family);
        EXPECT_EQ(0x11u, identity.model);
        EXPECT_EQ(0x00050102u, identity.revision);
    }
}

//-----------------------------------------------
