#include <midi/capability_inquiry.h>
#include <midi/manufacturer.h>

#include <cassert>
#include <string>
#include <vector>

void discovery_examples()
{
    using namespace midi;

    constexpr muid_t  initiator_muid = 0x1234567;
    constexpr muid_t  responder_muid = 0x7654321;
    constexpr group_t group          = 0;

    const ci::identity_t initiator_identity{ manufacturer::native_instruments, 0x1730, 49, 0x00010005 };
    const ci::identity_t responder_identity{ manufacturer::native_instruments, 0x1740, 61, 0x00020001 };

    // the initiator sends a Discovery Inquiry to all devices
    const ci::message inquiry =
      ci::make_discovery_inquiry(initiator_muid, initiator_identity, ci::category::profile_configuration, 512);

    std::vector<data_message> packets;
    send_sysex7(inquiry, group, [&](const data_message& p) { packets.push_back(p); });

    // the responder replies to the initiator
    if (auto v = ci::as<ci::discovery_inquiry_view>(inquiry))
    {
        assert(v->destination_muid() == ci::broadcast_muid);

        const ci::message reply =
          ci::make_discovery_reply(responder_muid,
                                   v->source_muid(),
                                   responder_identity,
                                   ci::category::profile_configuration | ci::category::property_exchange,
                                   1024,
                                   v->output_path_id());

        if (auto r = ci::as<ci::discovery_reply_view>(reply))
        {
            assert(r->destination_muid() == initiator_muid);
            assert(r->identity() == responder_identity);
            assert(r->categories() & ci::category::property_exchange);
            assert(r->function_block() == 0x7F);
        }
    }
}

void nak_examples()
{
    using namespace midi;

    const sysex7 inquiry = ci::make_profile_inquiry_message(0x1234567, 0x7654321);

    // reply with a NAK if a message is not supported, see M2-101 for the status codes
    if (auto v = ci::as<capability_inquiry_view>(inquiry))
    {
        uint7_t details[5] = { 0, 0, 0, 0, 0 };

        const sysex7 nak = ci::make_nak_message(*v, 0x01, 0x00, details, "Profile Configuration not supported");

        if (auto n = ci::as<ci::nak_view>(nak))
        {
            assert(n->source_muid() == 0x7654321);
            assert(n->destination_muid() == 0x1234567);
            assert(n->transaction() == ci::subtype::profile_inquiry);
            assert(n->message() == "Profile Configuration not supported");
        }
    }
}

void profile_id_examples()
{
    using namespace midi;

    // a Standard Defined Profile: Profile Number 0x21, Version 1, Level 1
    constexpr ci::profile_id my_profile{ 0x7E, 0x00, 0x21, 0x01, 0x01 };

    // the same profile with a different Profile Version and Profile Level
    constexpr ci::profile_id other_version{ 0x7E, 0x00, 0x21, 0x02, 0x03 };

    static_assert(!(my_profile == other_version));
    static_assert(ci::same_profile(my_profile, other_version));

    // a Manufacturer Specific Profile is compared by all five bytes
    constexpr ci::profile_id manufacturer_profile{ 0x00, 0x21, 0x09, 0x01, 0x01 };
    static_assert(!ci::same_profile(manufacturer_profile, ci::profile_id{ 0x00, 0x21, 0x09, 0x02, 0x01 }));

    // check whether an incoming Profile Details Inquiry refers to my profile
    const sysex7 sx = ci::make_profile_details_inquiry(0x1234567, 0x7654321, other_version, 0x00);

    if (auto v = ci::as<ci::profile_id_view>(sx))
    {
        assert(ci::same_profile(v->profile(), my_profile));
        // reply with the details of my profile
        // ...
    }
}

void profile_configuration_examples()
{
    using namespace midi;

    constexpr muid_t initiator_muid = 0x1234567;
    constexpr muid_t responder_muid = 0x7654321;

    constexpr ci::profile_id profile_a{ 0x7E, 0x00, 0x21, 0x01, 0x01 };
    constexpr ci::profile_id profile_b{ 0x7E, 0x00, 0x22, 0x01, 0x01 };

    // the responder reports one enabled and one disabled profile on channel 3
    const sysex7 reply = ci::make_profile_inquiry_reply(
      initiator_muid, responder_muid, { profile_a }, { profile_b }, ci::channel_profile(3).scope);

    if (auto v = ci::as<ci::profile_inquiry_reply_view>(reply))
    {
        assert(v->device_id() == 3);
        assert(v->enabled_profiles() == std::vector<ci::profile_id>{ profile_a });
        assert(v->disabled_profiles() == std::vector<ci::profile_id>{ profile_b });
    }

    // the initiator enables profile_b on channels 3 to 6
    const sysex7 on = ci::make_profile_on_request(initiator_muid, responder_muid, profile_b, ci::channel_profile(3, 4));

    if (auto v = ci::as<ci::profile_destination_view>(on))
    {
        assert(v->subtype() == ci::subtype::set_profile_on);
        assert(v->profile() == profile_b);
        assert(v->params().scope == 3);
        assert(v->num_channels() == 4);

        // the responder reports the enabled profile to all devices
        const sysex7 enabled =
          ci::make_profile_enabled_notification(responder_muid, ci::broadcast_muid, v->profile(), v->params());
        assert(ci::as<ci::profile_destination_view>(enabled));
    }

    // send Profile Specific Data for a profile on the whole group
    const sysex7 data = ci::make_profile_specific_data_message(
      initiator_muid, responder_muid, profile_a, std::vector<uint7_t>{ 0x10, 0x20, 0x30 }, ci::group_profile.scope);

    if (auto v = ci::as<ci::profile_specific_data_view>(data))
    {
        assert(v->device_id() == ci::profile_destination::group);
        assert(v->data_size() == 3);
        assert(v->data_begin()[0] == 0x10);
    }
}

void property_exchange_examples()
{
    using namespace midi;
    using namespace midi::ci::property_exchange;

    constexpr muid_t  initiator_muid = 0x1234567;
    constexpr muid_t  responder_muid = 0x7654321;
    constexpr uint7_t request_id     = 1;

    // the initiator requests the DeviceInfo resource
    const sysex7 inquiry = ci::make_get_property_data_inquiry(initiator_muid, responder_muid, DeviceInfo, request_id);

    if (auto v = ci::as<ci::get_property_data_view>(inquiry))
    {
        const auto header = std::string_view{ reinterpret_cast<const char*>(v->header_begin()), v->header_size() };
        assert(header == R"({"resource":"DeviceInfo"})");

        // the responder replies in a single chunk
        const std::string body = R"({"manufacturer":"Native Instruments"})";

        const sysex7 reply =
          ci::make_get_property_data_reply(responder_muid, v->source_muid(), 200, 1, 1, chunk{ body }, v->request_id());

        if (auto r = ci::as<ci::get_property_data_view>(reply))
        {
            assert(r->request_id() == request_id);
            assert(r->number_of_chunks() == 1);
            assert(r->number_of_this_chunk() == 1);
            assert(r->chunk_size() == body.size());
        }
    }

    // header options are added to the header JSON, numbers are written without quotes
    assert(make_rjson(tags::resource, ChannelList, header_options{ { tags::offset, "0" }, { tags::limit, "10" } }) ==
           R"({"resource":"ChannelList","offset":0,"limit":10})");
}

void process_inquiry_examples()
{
    using namespace midi;

    constexpr muid_t initiator_muid = 0x1234567;
    constexpr muid_t responder_muid = 0x7654321;

    // the initiator requests a MIDI Message Report for channel 0, see M2-101 for the bitmap values
    const sysex7 inquiry =
      ci::make_midi_message_report_inquiry(initiator_muid, responder_muid, 0x7F, 0x00, 0x00, 0x01, 0x00);

    if (auto v = ci::as<ci::midi_message_report_inquiry_view>(inquiry))
    {
        assert(v->note_data_message_types() == 0x01);

        // the responder announces the report, sends the MIDI messages, then ends the report
        const sysex7 reply = ci::make_midi_message_report_reply(responder_muid, 0x00, 0x00, 0x01, v->device_id());
        // ...
        const sysex7 end = ci::make_midi_message_report_end(responder_muid, v->device_id());

        assert(ci::as<ci::midi_message_report_reply_view>(reply));
        assert(capability_inquiry_view{ end }.subtype() == ci::subtype::midi_message_report_end);
    }
}

void run_capability_inquiry_examples()
{
    discovery_examples();
    nak_examples();
    profile_id_examples();
    profile_configuration_examples();
    property_exchange_examples();
    process_inquiry_examples();
}
