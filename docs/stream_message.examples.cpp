#include <midi/stream_message.h>

#include <midi/manufacturer.h>

/* Most of the code in these examples is taken from the FreeRTOS firmware for ProtoZOA, take a look here:
      https://github.com/midi2-dev/AmeNote_Protozoa/blob/master/UUT_FreeRTOS/FreeRTOS_Tasks/UMPProcessing.cpp#L158
*/
static void reply_with_packet(const midi::universal_packet& p)
{
    using namespace midi;

    if (is_stream_message(p))
    {
        // you may want to inspect the other replies, too

        // Remark: in real world implementations you have to aggregate the name and serial strings in case of multi-part
        // replies!
        if (auto reply = as_endpoint_name_view(p))
        {
            auto name = reply->payload();
        }
        else if (auto reply = as_function_block_name_view(p))
        {
            auto name = reply->payload();
        }
        else if (auto reply = as_product_instance_id_view(p))
        {
            auto serial = reply->payload();
        }
    }
}

void endpoint_discovery_example()
{
    using namespace midi;

    auto discovery = make_endpoint_discovery_message(discovery_filter::endpoint_all);

    if (auto m = as_endpoint_discovery_view(discovery))
    {
        const auto identity   = device_identity{ manufacturer::native_instruments, 0x1730, 49, 0x00010005 };
        const auto ep_name    = std::string{ "Kontrol S49" };
        const auto serial     = std::string{ "12345678" };
        const auto protocol   = protocol::midi1;
        const auto extensions = extensions_t{ 0 };

        if (m->requests_info())
        {
            constexpr auto endpoint_info = make_endpoint_info_message(3, true, protocol::midi1 + protocol::midi2, 0);
            reply_with_packet(endpoint_info);
        }

        if (m->requests_device_identity())
        {
            const auto device_identity = make_device_identity_message(identity);
            reply_with_packet(device_identity);
        }

        if (m->requests_name())
        {
            send_endpoint_name(ep_name, reply_with_packet);
        }

        if (m->requests_product_instance_id())
        {
            send_product_instance_id(serial, reply_with_packet);
        }

        if (m->requests_stream_configuration())
        {
            reply_with_packet(make_stream_configuration_notification(protocol, extensions));
        }
    }
}

void function_block_discovery_example()
{
    using namespace midi;

    auto discovery = make_function_block_discovery_message(0xFF, discovery_filter::function_block_all);

    if (auto m = as_function_block_discovery_view(discovery))
    {
        constexpr uint8_t main         = 0;
        constexpr uint8_t din_port_out = 1;
        constexpr uint8_t din_port_in  = 2;

        if (m->requests_info())
        {
            if (m->requests_function_block(main))
            {
                constexpr auto reply =
                  midi::make_function_block_info_message(0, midi::function_block_options::bidirectional, 0);
                reply_with_packet(reply);
            }

            if (m->requests_function_block(din_port_out))
            {
                constexpr auto reply = midi::make_function_block_info_message(
                  din_port_out,
                  midi::function_block_options{ true,
                                                midi::function_block_options::direction_input,
                                                midi::function_block_options::midi1_31250,
                                                midi::function_block_options::ui_hint_as_direction,
                                                0x00,
                                                0 },
                  din_port_out);
                reply_with_packet(reply);
            }

            if (m->requests_function_block(din_port_in))
            {
                constexpr auto reply = midi::make_function_block_info_message(
                  din_port_in,
                  midi::function_block_options{ true,
                                                midi::function_block_options::direction_output,
                                                midi::function_block_options::midi1_31250,
                                                midi::function_block_options::ui_hint_as_direction,
                                                0x00,
                                                0 },
                  din_port_in);
                reply_with_packet(reply);
            }
        }

        if (m->requests_name())
        {
            if (m->requests_function_block(main))
            {
                constexpr auto reply = midi::make_function_block_name_message(midi::packet_format::complete, 0, "Main");
                reply_with_packet(reply);
            }

            if (m->requests_function_block(din_port_out))
            {
                constexpr auto reply =
                  midi::make_function_block_name_message(midi::packet_format::complete, din_port_out, "EXT OUT");
                reply_with_packet(reply);
            }

            if (m->requests_function_block(din_port_in))
            {
                constexpr auto reply =
                  midi::make_function_block_name_message(midi::packet_format::complete, din_port_in, "EXT IN");
                reply_with_packet(reply);
            }
        }
    }
}

void stream_configuration_example()
{
    using namespace midi;

    auto r = make_stream_configuration_request(protocol::midi2);

    if (auto m = as_stream_configuration_view(r))
    {
        auto new_protocol = m->protocol();
        reply_with_packet(midi::make_stream_configuration_notification(m->protocol(), m->extensions()));
    }
}

void run_stream_message_examples()
{
    endpoint_discovery_example();
    function_block_discovery_example();
    stream_configuration_example();
}
