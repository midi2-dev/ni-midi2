#include <midi/channel_voice_message.h>

#include <cassert>

void note_on_examples()
{
    using namespace midi;

    const note_nr_t note_nr{ 99 };
    const velocity  vel{ 0.9876 };
    const pitch_7_9 default_pitch{ note_nr };
    const pitch_7_9 custom_pitch{ 99.02 };

    auto m1 = make_midi1_note_on_message(0, 0, note_nr, vel);
    auto m2 = make_midi2_note_on_message(0, 0, note_nr, vel);
    auto m3 = make_midi2_note_on_message(0, 0, note_nr, vel, custom_pitch);
    auto m4 = make_midi2_note_on_message(0, 0, note_nr, vel, uint8_t{ 42 }, uint16_t{ 1234 });

    assert(is_note_on_message(m1));
    assert(is_note_on_message(m2));
    assert(is_note_on_message(m3));
    assert(is_note_on_message(m4));

    assert(get_note_nr(m1) == note_nr);
    assert(get_note_nr(m2) == note_nr);
    assert(get_note_nr(m3) == note_nr);
    assert(get_note_nr(m4) == note_nr);
    assert(get_note_velocity(m1) == velocity{ vel.as_uint7() });
    assert(get_note_velocity(m2) == vel);
    assert(get_note_velocity(m3) == vel);
    assert(get_note_velocity(m4) == vel);
    assert(get_note_pitch(m1) == default_pitch);
    assert(get_note_pitch(m2) == default_pitch);
    assert(get_note_pitch(m3) == custom_pitch);
    assert(get_note_pitch(m4) == default_pitch);
}

void note_off_examples()
{
    using namespace midi;

    const note_nr_t note_nr{ 71 };
    const velocity  vel{ 0.1 };

    auto m1 = make_midi1_note_off_message(0, 0, note_nr, vel);
    auto m2 = make_midi2_note_off_message(0, 0, note_nr, vel);
    auto m3 = make_midi1_note_on_message(0, 0, note_nr, velocity{ uint7_t{ 0 } });

    assert(is_note_off_message(m1));
    assert(is_note_off_message(m2));
    assert(is_note_off_message(m3));
    assert(get_note_nr(m1) == note_nr);
    assert(get_note_nr(m2) == note_nr);
    assert(get_note_nr(m3) == note_nr);
    assert(get_note_velocity(m1) == velocity{ vel.as_uint7() });
    assert(get_note_velocity(m2) == vel);
    assert(get_note_velocity(m3) == velocity{ uint7_t{ 64 } });
}

void poly_pressure_examples()
{
    using namespace midi;

    note_nr_t        note_nr{ 34 };
    controller_value pressure{ 0.75 };

    auto m1 = make_midi1_poly_pressure_message(0, 0, note_nr, pressure);
    auto m2 = make_midi2_poly_pressure_message(0, 0, note_nr, pressure);

    assert(is_poly_pressure_message(m1));
    assert(is_poly_pressure_message(m2));

    assert(get_note_nr(m1) == note_nr);
    assert(get_note_nr(m2) == note_nr);

    assert(get_poly_pressure_value(m2) == pressure);
    assert(get_poly_pressure_value(m1) == controller_value{ pressure.as_uint7() });
}

void control_change_examples()
{
    using namespace midi;

    controller_t     ctrl{ control_change::brightness };
    controller_value val{ uint32_t{ 0x41221892 } };

    auto m1 = make_midi1_control_change_message(0, 0, ctrl, val);
    auto m2 = make_midi2_control_change_message(0, 0, ctrl, val);

    assert(is_control_change_message(m1));
    assert(is_control_change_message(m2));

    assert(get_controller_nr(m1) == ctrl);
    assert(get_controller_nr(m2) == ctrl);

    assert(get_controller_value(m2) == val);
    assert(get_controller_value(m1) == controller_value{ val.as_uint7() });
}

void program_change_examples()
{
    using namespace midi;

    program_t program{ 7 };

    // TODO: demonstrate MIDI2 bank
    auto m1 = make_midi1_program_change_message(0, 0, program);
    auto m2 = make_midi2_program_change_message(0, 0, program);

    assert(is_program_change_message(m1));
    assert(is_program_change_message(m2));

    assert(get_program_value(m1) == program);
    assert(get_program_value(m2) == program);
}

void channel_pressure_examples()
{
    using namespace midi;

    controller_value pressure{ 0.75 };

    auto m1 = make_midi1_channel_pressure_message(0, 0, pressure);
    auto m2 = make_midi2_channel_pressure_message(0, 0, pressure);

    assert(is_channel_pressure_message(m1));
    assert(is_channel_pressure_message(m2));

    assert(get_channel_pressure_value(m2) == pressure);
    assert(get_channel_pressure_value(m1) == controller_value{ pressure.as_uint7() });
}

void channel_pitch_bend_examples()
{
    using namespace midi;

    pitch_bend pb{ -0.04 };

    auto m1 = make_midi1_pitch_bend_message(0, 0, pb);
    auto m2 = make_midi2_pitch_bend_message(0, 0, pb);

    assert(is_channel_pitch_bend_message(m1));
    assert(is_channel_pitch_bend_message(m2));

    assert(get_channel_pitch_bend_value(m2) == pb);
    assert(get_channel_pitch_bend_value(m1) == pitch_bend{ pb.as_uint14() });
}

void channel_voice_message_processor(const midi::universal_packet& p)
{
    using namespace midi;

    auto allocate_voice     = [](note_nr_t, velocity, pitch_7_9) {};
    auto release_voice      = [](note_nr_t, velocity) {};
    auto voice_pressure     = [](note_nr_t, controller_value) {};
    auto voice_controller   = [](note_nr_t, uint8_t, controller_value) {};
    auto voice_pitch_bend   = [](note_nr_t, pitch_bend) {};
    auto channel_controller = [](controller_t, controller_value) {};
    auto channel_pitch_bend = [](pitch_bend) {};

    if (is_note_on_message(p))
    {
        allocate_voice(get_note_nr(p), get_note_velocity(p), get_note_pitch(p));
    }
    else if (is_note_off_message(p))
    {
        release_voice(get_note_nr(p), get_note_velocity(p));
    }
    else if (is_poly_pressure_message(p))
    {
        voice_pressure(get_note_nr(p), get_poly_pressure_value(p));
    }
    else if (is_registered_per_note_controller_message(p))
    {
        voice_controller(get_note_nr(p), get_per_note_controller_index(p), get_controller_value(p));
    }
    else if (is_per_note_pitch_bend_message(p))
    {
        voice_pitch_bend(get_note_nr(p), get_per_note_pitch_bend_value(p));
    }
    else if (is_control_change_message(p))
    {
        channel_controller(get_controller_nr(p), get_controller_value(p));
    }
    else if (is_channel_pitch_bend_message(p))
    {
        channel_pitch_bend(get_channel_pitch_bend_value(p));
    }
}

void run_channel_voice_message_examples()
{
    note_on_examples();
    note_off_examples();
    poly_pressure_examples();
    control_change_examples();
    program_change_examples();
    channel_pressure_examples();
    channel_pitch_bend_examples();
    channel_voice_message_processor(midi::universal_packet{ 0x20901234 });
}
