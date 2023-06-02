#include "sysex7_test_data.h"

//-----------------------------------------------

std::vector<midi::sysex7_test_case> midi::sysex7_test_cases{
    { "empty sysex", { { 0x30010000, 0x00000000 } }, {} },
    { "one byte manufacturer only", { { 0x31010400, 0x00000000 } }, midi::sysex7{ midi::manufacturer::moog } },
    { "three byte manufacturer only",
      { { 0x35030021, 0x09000000 } },
      midi::sysex7{ midi::manufacturer::native_instruments } },
    { "three byte manufacturer, complete message",
      { { 0x31050002, 0x0D152600 } },
      midi::sysex7{ midi::manufacturer::google, { 0x15, 0x26 } } },
    { "one byte manufacturer, complete message",
      { { 0x3F060905, 0x04030201 } },
      midi::sysex7{ midi::manufacturer::midi9, { 5, 4, 3, 2, 1 } } },
    {
      "two messages",
      { { 0x3A164E09, 0x08070605 }, { 0x3A340403, 0x02010000 } },
      midi::sysex7{ midi::manufacturer::teac, { 9, 8, 7, 6, 5, 4, 3, 2, 1 } },
    },
    { "three messages",
      { { 0x36160021, 0x1D112233 }, { 0x36264455, 0x66771829 }, { 0x36353A4B, 0x5C6D7C00 } },
      midi::sysex7{ midi::manufacturer::ableton,
                    { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x18, 0x29, 0x3A, 0x4B, 0x5C, 0x6D, 0x7C } } },
    { "four messages",
      {
        { 0x31160020, 0x6B112233 },
        { 0x31264455, 0x66771829 },
        { 0x31263A4B, 0x5C6D7C0F },
        { 0x31311000, 0x00000000 },
      },
      midi::sysex7{
        midi::manufacturer::arturia,
        { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x18, 0x29, 0x3A, 0x4B, 0x5C, 0x6D, 0x7C, 0x0F, 0x10 } } }
};
