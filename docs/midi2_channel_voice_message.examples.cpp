#include <midi/midi2_channel_voice_message.h>

#include <midi/channel_voice_message.h>

#include <cassert>

void registered_per_note_controller_message_examples()
{
    using namespace midi;

    const group_t          group   = 9;
    const channel_t        channel = 11;
    const note_nr_t        note    = 102;
    const controller_t     ctrl{ 7 };
    const controller_value val{ uint32_t{ 0x3456789A } };

    midi2_channel_voice_message rpnc = make_registered_per_note_controller_message(group, channel, note, ctrl, val);

    assert(is_registered_per_note_controller_message(rpnc));

    if (is_midi2_channel_voice_message(rpnc))
    {
        midi2_channel_voice_message_view v{ rpnc };

        assert(v.group() == group);
        assert(v.channel() == channel);
        assert(v.byte3() == note);
        assert(v.byte4() == ctrl);
        assert(v.data() == val.value);
    }

    assert(get_note_nr(rpnc) == note);
    assert(get_per_note_controller_index(rpnc) == ctrl);
    assert(get_controller_value(rpnc) == val);
}

void assignable_per_note_controller_message_examples()
{
    using namespace midi;

    const group_t          group   = 15;
    const channel_t        channel = 15;
    const note_nr_t        note{ 37 };
    const controller_t     ctrl{ 99 };
    const controller_value val{ 0.45 };

    midi2_channel_voice_message apnc = make_assignable_per_note_controller_message(group, channel, note, ctrl, val);

    assert(is_assignable_per_note_controller_message(apnc));

    if (auto v = as_midi2_channel_voice_message_view(apnc))
    {
        assert(v->group() == group);
        assert(v->channel() == channel);
        assert(v->byte3() == note);
        assert(v->byte4() == ctrl);
        assert(v->data() == val.value);
    }

    assert(get_note_nr(apnc) == note);
    assert(get_per_note_controller_index(apnc) == ctrl);
    assert(get_controller_value(apnc) == val);
}

void registered_controller_message_examples()
{
    using namespace midi;

    const group_t          group   = 0x3;
    const channel_t        channel = 0xE;
    const uint7_t          bank{ 0 };
    const uint7_t          index{ 99 };
    const controller_value val{ uint32_t{ 0x34AABBCC } };

    midi2_channel_voice_message rc = make_registered_controller_message(group, channel, bank, index, val);

    assert(is_registered_controller_message(rc));

    if (auto v = as_midi2_channel_voice_message_view(rc))
    {
        assert(v->group() == group);
        assert(v->channel() == channel);
        assert(v->byte3() == bank);
        assert(v->byte4() == index);
        assert(v->data() == val.value);
    }

    // TODO: accessor for bank+index?
    assert(get_controller_value(rc) == val);
}

void assignable_controller_message_examples()
{
    using namespace midi;

    const group_t          group   = 6;
    const channel_t        channel = 6;
    const uint7_t          bank{ 13 };
    const uint7_t          index{ 42 };
    const controller_value val{ 0.45 };

    midi2_channel_voice_message ac = make_assignable_controller_message(group, channel, bank, index, val);

    assert(is_assignable_controller_message(ac));

    if (is_midi2_channel_voice_message(ac))
    {
        midi2_channel_voice_message_view v{ ac };

        assert(v.group() == group);
        assert(v.channel() == channel);
        assert(v.byte3() == bank);
        assert(v.byte4() == index);
        assert(v.data() == val.value);
    }

    // TODO: accessor for bank+index?
    assert(get_controller_value(ac) == val);
}

void per_note_pitch_bend_message_examples()
{
    using namespace midi;

    const group_t    group   = 12;
    const channel_t  channel = 7;
    const note_nr_t  note    = 69;
    const pitch_bend pb{ -0.1234 };

    midi2_channel_voice_message pnpb = make_per_note_pitch_bend_message(group, channel, note, pb);

    assert(is_per_note_pitch_bend_message(pnpb));

    if (is_midi2_channel_voice_message(pnpb))
    {
        midi2_channel_voice_message_view v{ pnpb };

        assert(v.group() == group);
        assert(v.channel() == channel);
        assert(v.byte3() == note);
        assert(v.data() == pb.value);
    }

    assert(get_note_nr(pnpb) == note);
    assert(get_per_note_pitch_bend_value(pnpb) == pb);
}

void relative_registered_controller_message_examples()
{
    using namespace midi;

    const group_t              group   = 0x3;
    const channel_t            channel = 0xE;
    const uint7_t              bank{ 0 };
    const uint7_t              index{ 99 };
    const controller_increment inc{ -1234 };

    midi2_channel_voice_message rrc = make_relative_registered_controller_message(group, channel, bank, index, inc);

    // TODO??
    // assert(is_relative_registered_controller_message(rrc));

    if (is_midi2_channel_voice_message(rrc))
    {
        midi2_channel_voice_message_view v{ rrc };

        assert(v.group() == group);
        assert(v.channel() == channel);
        assert(v.byte3() == bank);
        assert(v.byte4() == index);
        // TODO: FIXME
        assert(v.data() == inc.value);
    }

    // TODO: get_controller_increment?
    // assert(get_controller_increment(rrc) == inc);
}

void relative_assignable_controller_message_examples()
{
    using namespace midi;

    const group_t              group   = 6;
    const channel_t            channel = 6;
    const uint7_t              bank{ 13 };
    const uint7_t              index{ 42 };
    const controller_increment inc{ 1024 };

    midi2_channel_voice_message rac = make_relative_assignable_controller_message(group, channel, bank, index, inc);

    // TODO?
    // assert(is_relative_registered_controller_message(rac));

    if (auto v = as_midi2_channel_voice_message_view(rac))
    {
        assert(v->group() == group);
        assert(v->channel() == channel);
        assert(v->byte3() == bank);
        assert(v->byte4() == index);
        // TODO: FIXME
        assert(v->data() == inc.value);
    }

    // TODO: get_controller_increment?
    // assert(get_controller_increment(rac) == inc);
}

void midi2_note_message_examples()
{
    using namespace midi;

    const group_t   group   = 3;
    const channel_t channel = 8;
    const note_nr_t note_nr{ 23 };
    const velocity  vel{ uint7_t{ 100 } };
    const pitch_7_9 pitch{ 81.4 };
    const uint8_t   attribute{ 123 };
    const uint16_t  attribute_data{ 4567 };

    midi2_channel_voice_message note_on = make_midi2_note_on_message(group, channel, note_nr, vel);

    if (is_midi2_channel_voice_message(note_on))
    {
        midi2_channel_voice_message_view v{ note_on };

        assert(v.group() == group);
        assert(v.channel() == channel);
        assert(v.byte3() == note_nr);
        assert(v.data() == (uint32_t(vel.value) << 16));
    }

    assert(is_note_on_message(note_on));
    assert(get_note_nr(note_on) == note_nr);
    assert(get_note_velocity(note_on) == vel);
    assert(get_note_pitch(note_on) == pitch_7_9{ note_nr });

    midi2_channel_voice_message note_on_with_pitch = make_midi2_note_on_message(group, channel, note_nr, vel, pitch);

    if (auto v = as_midi2_channel_voice_message_view(note_on_with_pitch))
    {
        assert(v->group() == group);
        assert(v->channel() == channel);
        assert(v->byte3() == note_nr);
        assert(v->byte4() == 0x03);
        assert(v->data() == ((uint32_t(vel.value) << 16) | pitch.value));
    }

    assert(is_note_on_message(note_on_with_pitch));
    assert(is_note_on_with_attribute(note_on_with_pitch, 3));
    assert(is_note_on_with_pitch_7_9(note_on_with_pitch));
    assert(get_note_nr(note_on_with_pitch) == note_nr);
    assert(get_note_velocity(note_on_with_pitch) == vel);
    assert(get_note_pitch(note_on_with_pitch) == pitch);
    assert(get_midi2_note_attribute(note_on_with_pitch) == 3);

    midi2_channel_voice_message note_on_with_attribute =
      make_midi2_note_on_message(group, channel, note_nr, vel, attribute, attribute_data);

    if (is_midi2_channel_voice_message(note_on_with_attribute))
    {
        midi2_channel_voice_message_view v{ note_on_with_attribute };

        assert(v.group() == group);
        assert(v.channel() == channel);
        assert(v.byte3() == note_nr);
        assert(v.byte4() == attribute);
        assert(v.data() == ((uint32_t(vel.value) << 16) | attribute_data));
    }

    assert(is_note_on_message(note_on_with_attribute));
    assert(is_note_on_with_attribute(note_on_with_attribute, attribute));
    assert(get_note_nr(note_on_with_attribute) == note_nr);
    assert(get_note_velocity(note_on_with_attribute) == vel);
    assert(get_midi2_note_attribute(note_on_with_attribute) == attribute);
    assert(get_midi2_note_attribute_data(note_on_with_attribute) == attribute_data);

    midi2_channel_voice_message note_off = make_midi2_note_off_message(group, channel, note_nr, vel);

    if (auto v = as_midi2_channel_voice_message_view(note_off))
    {
        assert(v->group() == group);
        assert(v->channel() == channel);
        assert(v->byte3() == note_nr);
        assert(v->data() == (uint32_t(vel.value) << 16));
    }

    assert(is_note_off_message(note_off));
    assert(get_note_nr(note_off) == note_nr);
    assert(get_note_velocity(note_off) == vel);
}

void midi2_poly_pressure_message_examples()
{
    using namespace midi;

    const group_t          group   = 9;
    const channel_t        channel = 12;
    const note_nr_t        note_nr{ 71 };
    const controller_value value{ 0.94 };

    midi2_channel_voice_message poly_pressure = make_midi2_poly_pressure_message(group, channel, note_nr, value);

    if (is_midi2_channel_voice_message(poly_pressure))
    {
        midi2_channel_voice_message_view v{ poly_pressure };

        assert(v.group() == group);
        assert(v.channel() == channel);
        assert(v.byte3() == note_nr);
        assert(v.data() == value.value);
    }

    assert(is_poly_pressure_message(poly_pressure));
    assert(get_note_nr(poly_pressure) == note_nr);
    assert(get_poly_pressure_value(poly_pressure) == value);
}

void midi2_control_change_message_examples()
{
    using namespace midi;

    const group_t          group   = 2;
    const channel_t        channel = 4;
    const controller_t     ctrl_nr{ 24 };
    const controller_value value{ uint32_t{ 0x12345678 } };

    midi2_channel_voice_message control_change = make_midi2_control_change_message(group, channel, ctrl_nr, value);

    if (auto v = as_midi2_channel_voice_message_view(control_change))
    {
        assert(v->group() == group);
        assert(v->channel() == channel);
        assert(v->byte3() == ctrl_nr);
        assert(v->data() == value.value);
    }

    assert(is_control_change_message(control_change));
    assert(get_controller_nr(control_change) == ctrl_nr);
    assert(get_controller_value(control_change) == value);
}

void midi2_program_change_message_examples()
{
    using namespace midi;

    const group_t      group   = 0;
    const channel_t    channel = 0;
    const controller_t program{ 99 };
    const uint14_t     bank{ 0x144 };

    midi2_channel_voice_message program_change = make_midi2_program_change_message(group, channel, program);

    if (auto v = as_midi2_channel_voice_message_view(program_change))
    {
        assert(v->group() == group);
        assert(v->channel() == channel);
        assert(v->data() == 0x63000000);
    }

    assert(is_program_change_message(program_change));
    assert(get_program_value(program_change) == program);

    midi2_channel_voice_message program_change_with_bank =
      make_midi2_program_change_message(group, channel, program, bank);

    if (is_midi2_channel_voice_message(program_change_with_bank))
    {
        midi2_channel_voice_message_view v{ program_change_with_bank };

        assert(v.group() == group);
        assert(v.channel() == channel);
        assert(v.data() == 0x63000244);
    }

    assert(is_program_change_message(program_change_with_bank));
    assert(get_program_value(program_change_with_bank) == program);
    // TODO: retrieve optional bank value
}

void midi2_channel_pressure_message_examples()
{
    using namespace midi;

    const group_t          group   = 15;
    const channel_t        channel = 7;
    const controller_value pressure{ 0.86f };

    midi2_channel_voice_message channel_pressure = make_midi2_channel_pressure_message(group, channel, pressure);

    if (is_midi2_channel_voice_message(channel_pressure))
    {
        midi2_channel_voice_message_view v{ channel_pressure };

        assert(v.group() == group);
        assert(v.channel() == channel);
        assert(v.data() == pressure.value);
    }

    assert(is_channel_pressure_message(channel_pressure));
    assert(get_channel_pressure_value(channel_pressure) == pressure);
}

void midi2_pitch_bend_message_examples()
{
    using namespace midi;

    const group_t    group   = 3;
    const channel_t  channel = 4;
    const pitch_bend value{ -0.31 };

    midi2_channel_voice_message pb = make_midi2_pitch_bend_message(group, channel, value);

    if (is_midi2_channel_voice_message(pb))
    {
        midi2_channel_voice_message_view v{ pb };

        assert(v.group() == group);
        assert(v.channel() == channel);
        assert(v.data() == value.value);
    }

    assert(is_channel_pitch_bend_message(pb));
    assert(get_channel_pitch_bend_value(pb) == value);
}

void per_note_management_message_examples()
{
    using namespace midi;

    const group_t   group   = 13;
    const channel_t channel = 4;
    const note_nr_t note_nr = 111;

    midi2_channel_voice_message pnm_reset =
      make_per_note_management_message(group, channel, note_nr, note_management::reset);

    if (is_midi2_channel_voice_message(pnm_reset))
    {
        midi2_channel_voice_message_view v{ pnm_reset };

        assert(v.group() == group);
        assert(v.channel() == channel);
        assert(v.byte3() == note_nr);
        assert(v.byte4() == note_management::reset);
    }

    // TODO: is_per_note_managment_message, is_per_note_managment_reset, is_per_note_managment_detach
    assert(get_note_nr(pnm_reset) == note_nr);

    midi2_channel_voice_message pnm_detach =
      make_per_note_management_message(group, channel, note_nr, note_management::detach);

    if (auto v = as_midi2_channel_voice_message_view(pnm_detach))
    {
        assert(v->group() == group);
        assert(v->channel() == channel);
        assert(v->byte3() == note_nr);
        assert(v->byte4() == note_management::detach);
    }

    // TODO: is_per_note_managment_message, is_per_note_managment_reset, is_per_note_managment_detach
    assert(get_note_nr(pnm_detach) == note_nr);
}

void run_midi2_channel_voice_message_examples()
{
    registered_per_note_controller_message_examples();
    assignable_per_note_controller_message_examples();
    registered_controller_message_examples();
    assignable_controller_message_examples();
    relative_registered_controller_message_examples();
    relative_assignable_controller_message_examples();
    per_note_pitch_bend_message_examples();
    midi2_note_message_examples();
    midi2_poly_pressure_message_examples();
    midi2_control_change_message_examples();
    midi2_program_change_message_examples();
    midi2_channel_pressure_message_examples();
    midi2_pitch_bend_message_examples();
    per_note_management_message_examples();
}
