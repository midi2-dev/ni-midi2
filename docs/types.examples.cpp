#include <midi/types.h>

#include <midi/channel_voice_message.h>
#include <midi/manufacturer.h>
#include <midi/universal_sysex.h>

void velocity_examples()
{
    using namespace midi;

    auto from7bit   = velocity{ uint7_t{ 44 } };
    auto from14bit  = velocity{ uint14_t{ 0x0312 } };
    auto fromFloat  = velocity{ 0.7f };
    auto fromDouble = velocity{ 0.98 };

    auto as7bit   = fromDouble.as_uint7();
    auto as14bit  = fromFloat.value;
    auto asFloat  = from14bit.as_float();
    auto asDouble = from7bit.as_double();

    auto m    = make_midi1_note_on_message(0, 0, note_nr_t{ 81 }, velocity{ 0.1 });
    auto velA = get_note_velocity(m);
}

void pitch_bend_examples()
{
    using namespace midi;

    auto from14bit  = pitch_bend{ uint14_t{ 0x1234 } };
    auto from32bit  = pitch_bend{ uint32_t{ 0xA0000000 } };
    auto fromFloat  = pitch_bend{ -0.385f };
    auto fromDouble = pitch_bend{ 0.98 };

    auto as14bit  = fromFloat.as_uint14();
    auto as32bit  = fromDouble.value;
    auto asFloat  = from32bit.as_float();
    auto asDouble = from14bit.as_double();

    auto increment = from14bit * pitch_bend_sensitivity{ 4. };
}

void pitch_increment_examples()
{
    using namespace midi;

    auto fromInt32  = pitch_increment{ -44444 }; // small fraction of a cent
    auto fromFloat  = pitch_increment{ -25.7f }; // 25700 cent
    auto fromDouble = pitch_increment{ 0.1 };    // 10 cent

    pitch_increment increment = fromFloat + fromDouble;
    increment += fromInt32;

    auto pitch = pitch_7_25{ note_nr_t{ 68 } } + increment;
}

void pitch_7_9_examples()
{
    using namespace midi;

    auto fromNoteNr = pitch_7_9{ note_nr_t{ 74 } };
    auto from16bit  = pitch_7_9{ uint16_t{ 0x88A0 } };
    auto fromFloat  = pitch_7_9{ 69.4f };
    auto fromDouble = pitch_7_9{ 127.0 };

    auto asNoteNr = fromFloat.note_nr();
    auto as16bit  = from16bit.value;
    auto asFloat  = fromDouble.as_float();
    auto asDouble = fromNoteNr.as_double();

    auto m = make_midi2_note_on_message(0, 0, note_nr_t{ 0 }, velocity{ 1. }, fromDouble);
}

void pitch_7_25_examples()
{
    using namespace midi;

    auto fromNoteNr = pitch_7_25{ note_nr_t{ 12 } };
    auto from32bit  = pitch_7_25{ uint32_t{ 0xA10BFFFF } };
    auto fromFloat  = pitch_7_25{ 69.4f };
    auto fromDouble = pitch_7_25{ 127.0 };
    auto from7_9    = pitch_7_25{ pitch_7_9{ 33.7 } };

    auto asNoteNr = fromFloat.note_nr();
    auto as32bit  = fromDouble.value;
    auto asFloat  = from32bit.as_float();
    auto asDouble = fromNoteNr.as_double();

    pitch_7_25 pitch;
    pitch = pitch_7_9{ 33.4 };
    pitch += pitch_increment{ -177993 };

    pitch = pitch + 0.45f;
    pitch = pitch + 4.999;
}

void pitch_bend_sensitivity_examples()
{
    using namespace midi;

    auto fromNoteNr = pitch_bend_sensitivity{ note_nr_t{ 2 } };
    auto fromFloat  = pitch_bend_sensitivity{ 3.5 };
    auto fromDouble = pitch_bend_sensitivity{ 0.98 };

    auto asNoteNr = fromFloat.note_nr();
    auto asFloat  = fromDouble.as_float();
    auto asDouble = fromNoteNr.as_double();

    pitch_increment i = pitch_bend{ -0.5 } * fromNoteNr;
}

void controller_increment_examples()
{
    using namespace midi;

    auto inc = controller_increment{ 0x02000000 };

    auto v = controller_value{ uint7_t{ 99 } } + inc;
}

void controller_value_examples()
{
    using namespace midi;

    auto from7bit   = controller_value{ uint7_t{ 44 } };
    auto from32bit  = controller_value{ uint32_t{ 0x17224028 } };
    auto fromFloat  = controller_value{ 0.34f };
    auto fromDouble = controller_value{ 0.7754 };

    auto as7bit   = fromDouble.as_uint7();
    auto as32bit  = fromFloat.value;
    auto asFloat  = from32bit.as_float();
    auto asDouble = from7bit.as_double();

    controller_value v{ 0.923157 };
    v += controller_increment{ 0x200 };
    v = v + controller_increment{ -0x400 };

    auto m  = make_midi2_control_change_message(0, 0, controller_t{ 21 }, v);
    auto v2 = get_controller_value(m);
}

void device_identity_examples()
{
    using namespace midi;

    constexpr device_identity identity{ manufacturer::native_instruments, 0x2200, 49, 0x1234 };

    auto m = universal_sysex::make_identity_reply(identity);
}

void downsample_examples()
{
    using namespace midi;

    auto as7bit1 = downsample_16_to_7bit(0x1234);
    auto as7bit2 = downsample_32_to_7bit(0xFEDCBA98);
    auto as14bit = downsample_32_to_7bit(0xFEDCBA98);
}

void upsample_examples()
{
    using namespace midi;

    auto as16bit  = upsample_7_to_16bit(uint7_t{ 42 });
    auto as32bit1 = upsample_7_to_32bit(uint7_t{ 42 });
    auto as32bit2 = upsample_14_to_32bit(uint14_t{ 0x0567 });

    auto as14bit = upsample_x_to_ybit(0x34, 6, 14);
}

void types_readme_code()
{
    using namespace midi;

    const pitch_bend_sensitivity cur_pitch_bend_sensitivity{ note_nr_t{ 2 } };
    const pitch_7_25             a_pitch_7_25{ note_nr_t{ 68 } };
    const pitch_bend             a_pitch_bend_value{ -0.44f };

    auto pitch_with_pb = a_pitch_7_25 + a_pitch_bend_value * cur_pitch_bend_sensitivity;

    controller_value     a_controller_value{ uint32_t{ 0x1234567 } };
    controller_increment a_controller_increment{ 0x20000 };

    a_controller_value += a_controller_increment;

    auto from7bit  = controller_value{ uint7_t{ 44 } };
    auto from32bit = controller_value{ uint32_t{ 0x45883312 } };
    auto fromFloat = pitch_7_25{ 66.1f };

    {
        auto v   = velocity{ uint7_t{ 44 } };
        auto pb1 = pitch_bend{ uint32_t{ 0x62311258 } };
        auto pb2 = pitch_bend{ -0.128 };
        auto p1  = pitch_7_25{ 81.3f };
        auto p2  = pitch_7_25{ note_nr_t{ 68 } };
        auto c1  = controller_value{ uint7_t{ 126 } };
        auto c2  = controller_value{ 0.93 };
    }

    {
        auto pb1 = pitch_bend{ uint32_t{ 0x62311258 } }.as_double(); // [-1.0 .. 1.0]
        auto pb2 = pitch_bend{ -0.128 }.as_uint14();                 // 14 bit MIDI 1
        auto p1  = pitch_7_25{ 81.3f }.as_float();                   // [0.0 .. 128.0)
        auto c1  = controller_value{ uint7_t{ 126 } }.as_double();   // [0.0 .. 1.0]
    }
    {
        pitch_7_25 p1{ note_nr_t{ 68 } };
        p1 += pitch_increment{ -199 };
        pitch_increment pbi = pitch_bend{ -0.128 } * pitch_bend_sensitivity{ note_nr_t{ 2 } };
        pitch_7_25      p3  = p1 + pbi;
        pitch_7_25      p4  = p3 + 0.011f;

        controller_value v1 = controller_value{ 0.44 } + controller_increment{ -1234 };
        v1 += controller_increment{ 0x4000 };
    }
}

void run_types_examples()
{
    velocity_examples();
    pitch_bend_examples();
    pitch_increment_examples();
    pitch_7_9_examples();
    pitch_7_25_examples();
    pitch_bend_sensitivity_examples();
    controller_increment_examples();
    controller_value_examples();
    device_identity_examples();
    downsample_examples();
    upsample_examples();
    types_readme_code();
}
