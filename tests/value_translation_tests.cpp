
#include <gtest/gtest.h>

#include <midi/types.h>

//-----------------------------------------------
/*! Unit test class for MIDI 1 <-> MIDI 2 value translation implementations

This class checks the following MIDI 2 translation requirements:

* the minumum value of the source range translates to the minumum value of the destination range
* the maximum value of the source range translates to the maximum value of the destination range
* the center value of the source range translates to the center value of the destination range
* any translation MIDI 1 -> MIDI 2 -> MIDI 1 preserves the source value

*/
class value_translation : public ::testing::Test
{
  public:
};

//-----------------------------------------------

TEST_F(value_translation, downscale_min)
{
    EXPECT_EQ(0u, midi::downsample_16_to_7bit(0)) << "min 16 -> 7 is 0";
    EXPECT_EQ(0u, midi::downsample_32_to_7bit(0)) << "min 32 -> 7 is 0";
    EXPECT_EQ(0u, midi::downsample_32_to_14bit(0)) << "min 32 -> 14 is 0";
}

//-----------------------------------------------

TEST_F(value_translation, downscale_center)
{
    EXPECT_EQ(0x40, midi::downsample_16_to_7bit(0x8000)) << "center 16 -> 7 is 0x40";
    EXPECT_EQ(0x40, midi::downsample_32_to_7bit(0x80000000)) << "center 32 -> 7 is 0x40";
    EXPECT_EQ(0x2000, midi::downsample_32_to_14bit(0x80000000)) << "center 32 -> 14 is 0x2000";
}

//-----------------------------------------------

TEST_F(value_translation, downscale_max)
{
    EXPECT_EQ(0x7F, midi::downsample_16_to_7bit(0xFFFF)) << "max 16 -> 7 is 0x7F";
    EXPECT_EQ(0x7F, midi::downsample_32_to_7bit(0xFFFFFFFF)) << "max 32 -> 7 is 0x7F";
    EXPECT_EQ(0x3FFF, midi::downsample_32_to_14bit(0xFFFFFFFF)) << "max 32 -> 14 is 0x3FFF";
}

//-----------------------------------------------

TEST_F(value_translation, upscale_min)
{
    EXPECT_EQ(0u, midi::upsample_7_to_16bit(0)) << "min 7 -> 16 is 0";
    EXPECT_EQ(0u, midi::upsample_7_to_32bit(0)) << "min 7 -> 32 is 0";
    EXPECT_EQ(0u, midi::upsample_14_to_32bit(0)) << "min 14 -> 32 is 0";
}

//-----------------------------------------------

TEST_F(value_translation, upscale_center)
{
    EXPECT_EQ(0x8000, midi::upsample_7_to_16bit(0x40)) << "center 7 -> 16 is 0x8000";
    EXPECT_EQ(0x80000000, midi::upsample_7_to_32bit(0x40)) << "center 7 -> 32 is 0x80000000";
    EXPECT_EQ(0x80000000, midi::upsample_14_to_32bit(0x2000)) << "center 14 -> 32 is 0x80000000";
}

//-----------------------------------------------

TEST_F(value_translation, upscale_max)
{
    EXPECT_EQ(0xFFFF, midi::upsample_7_to_16bit(0x7F)) << "max 7 -> 16 is 0xFFFF";
    EXPECT_EQ(0xFFFFFFFF, midi::upsample_7_to_32bit(0x7F)) << "max 7 -> 32 is 0xFFFFFFFF";
    EXPECT_EQ(0xFFFFFFFF, midi::upsample_14_to_32bit(0x3FFF)) << "max 14 -> 32 is 0xFFFFFFFF";
}

//-----------------------------------------------

TEST_F(value_translation, preserve_7_16_7)
{
    for (midi::uint7_t v = 0u; v < 0x80; ++v)
    {
        EXPECT_EQ(v, midi::downsample_16_to_7bit(midi::upsample_7_to_16bit(v))) << "preserve_7_16_7(" << v << ")";
    }
}

//-----------------------------------------------

TEST_F(value_translation, preserve_7_32_7)
{
    for (midi::uint7_t v = 0u; v < 0x80; ++v)
    {
        EXPECT_EQ(v, midi::downsample_32_to_7bit(midi::upsample_7_to_32bit(v))) << "preserve_7_32_7(" << v << ")";
    }
}

//-----------------------------------------------

TEST_F(value_translation, preserve_14_32_14)
{
    for (midi::uint14_t v = 0u; v < 0x4000; ++v)
    {
        EXPECT_EQ(v, midi::downsample_32_to_14bit(midi::upsample_14_to_32bit(v))) << "preserve_14_32_14(" << v << ")";
    }
}
