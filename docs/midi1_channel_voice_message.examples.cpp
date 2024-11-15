#include <midi/midi1_channel_voice_message.h>

#include <midi/channel_voice_message.h>

#include <cassert>

void midi1_note_message_examples()
{
    using namespace midi;

    const group_t   group   = 3;
    const channel_t channel = 8;
    const note_nr_t note_nr{ 23 };
    const velocity  vel{ uint7_t{ 100 } };

    midi1_channel_voice_message note_on  = make_midi1_note_on_message(group, channel, note_nr, vel);
    midi1_channel_voice_message note_off = make_midi1_note_off_message(group, channel, note_nr);

    if (is_midi1_channel_voice_message(note_on))
    {
        midi1_channel_voice_message_view v{ note_on };

        assert(v.group() == group);
        assert(v.channel() == channel);
        assert(v.data_byte_1() == note_nr);
        assert(v.data_byte_2() == vel.as_uint7());
    }

    if (auto v = as_midi1_channel_voice_message_view(note_off))
    {
        assert(v->group() == group);
        assert(v->channel() == channel);
        assert(v->data_byte_1() == note_nr);
        assert(v->data_byte_2() == 64);
    }

    assert(is_note_on_message(note_on));
    assert(is_note_off_message(note_off));
    assert(get_note_nr(note_on) == note_nr);
    assert(get_note_nr(note_off) == note_nr);
    assert(get_note_velocity(note_on) == vel);
    assert(get_note_velocity(note_off) == velocity{});
    assert(get_note_pitch(note_on) == pitch_7_9{ note_nr });
}

void midi1_poly_pressure_message_examples()
{
    using namespace midi;

    const group_t          group   = 9;
    const channel_t        channel = 12;
    const note_nr_t        note_nr{ 71 };
    const controller_value value{ uint7_t{ 83 } };

    midi1_channel_voice_message poly_pressure = make_midi1_poly_pressure_message(group, channel, note_nr, value);

    if (is_midi1_channel_voice_message(poly_pressure))
    {
        midi1_channel_voice_message_view v{ poly_pressure };

        assert(v.group() == group);
        assert(v.channel() == channel);
        assert(v.data_byte_1() == note_nr);
        assert(v.data_byte_2() == value.as_uint7());
    }

    assert(is_poly_pressure_message(poly_pressure));
    assert(get_note_nr(poly_pressure) == note_nr);
    assert(get_poly_pressure_value(poly_pressure) == value);
}

void midi1_control_change_message_examples()
{
    using namespace midi;

    const group_t          group   = 2;
    const channel_t        channel = 4;
    const controller_t     ctrl_nr{ 24 };
    const controller_value value{ uint7_t{ 87 } };

    midi1_channel_voice_message control_change = make_midi1_control_change_message(group, channel, ctrl_nr, value);

    if (auto v = as_midi1_channel_voice_message_view(control_change))
    {
        assert(v->group() == group);
        assert(v->channel() == channel);
        assert(v->data_byte_1() == ctrl_nr);
        assert(v->data_byte_2() == value.as_uint7());
    }

    assert(is_control_change_message(control_change));
    assert(get_controller_nr(control_change) == ctrl_nr);
    assert(get_controller_value(control_change) == value);
}

void midi1_program_change_message_examples()
{
    using namespace midi;

    const group_t      group   = 0;
    const channel_t    channel = 0;
    const controller_t program{ 99 };

    midi1_channel_voice_message program_change = make_midi1_program_change_message(group, channel, program);

    if (auto v = as_midi1_channel_voice_message_view(program_change))
    {
        assert(v->group() == group);
        assert(v->channel() == channel);
        assert(v->data_byte_1() == program);
    }

    assert(is_program_change_message(program_change));
    assert(get_program_value(program_change) == program);
}

void midi1_channel_pressure_message_examples()
{
    using namespace midi;

    const group_t          group   = 15;
    const channel_t        channel = 7;
    const controller_value pressure{ uint7_t{ 121 } };

    midi1_channel_voice_message channel_pressure = make_midi1_channel_pressure_message(group, channel, pressure);

    if (is_midi1_channel_voice_message(channel_pressure))
    {
        midi1_channel_voice_message_view v{ channel_pressure };

        assert(v.group() == group);
        assert(v.channel() == channel);
        assert(v.data_byte_1() == pressure.as_uint7());
    }

    assert(is_channel_pressure_message(channel_pressure));
    assert(get_channel_pressure_value(channel_pressure) == pressure);
}

void midi1_pitch_bend_message_examples()
{
    using namespace midi;

    const group_t    group   = 3;
    const channel_t  channel = 4;
    const pitch_bend value{ uint14_t{ 0x2412 } };

    midi1_channel_voice_message pb = make_midi1_pitch_bend_message(group, channel, value);

    if (is_midi1_channel_voice_message(pb))
    {
        midi1_channel_voice_message_view v{ pb };

        assert(v.group() == group);
        assert(v.channel() == channel);
        assert(v.data_byte_1() == (value.as_uint14() & 0x7F));
        assert(v.data_byte_2() == (value.as_uint14() >> 7));
    }

    assert(is_channel_pitch_bend_message(pb));
    assert(get_channel_pitch_bend_value(pb) == value);
}

void run_midi1_channel_voice_message_examples()
{
    midi1_note_message_examples();
    midi1_poly_pressure_message_examples();
    midi1_control_change_message_examples();
    midi1_program_change_message_examples();
    midi1_channel_pressure_message_examples();
    midi1_pitch_bend_message_examples();
}
