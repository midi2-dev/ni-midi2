# Stream Messages

_WORK IN PROGRESS_

Code examples can be found in [`stream_message.examples.cpp`](stream_message.examples.cpp).

## Base Type

    struct stream_message : universal_packet
    {
        stream_message(status_t, packet_format = packet_format::complete);

        packet_format format();
        void          set_format(packet_format);
    };

    bool is_stream_message(const universal_packet&);


### Factory Functions

    stream_message make_endpoint_discovery_message(uint8_t filter,
                                                            uint8_t ump_version_major = 1,
                                                            uint8_t ump_version_minor = 1);
    stream_message make_endpoint_info_message(uint8_t num_function_blocks,
                                                        bool    static_function_blocks,
                                                        uint8_t protocols,
                                                        uint8_t extensions,
                                                        uint8_t ump_version_major = 1,
                                                        uint8_t ump_version_minor = 1);
    stream_message make_device_identity_message(const device_identity&);
    stream_message make_endpoint_name_message(packet_format, const std::string_view&);
    stream_message make_product_instance_id_message(packet_format, const std::string_view&);
    stream_message make_stream_configuration_request(protocol_t, extensions_t = 0);
    stream_message make_stream_configuration_notification(protocol_t, extensions_t = 0);

    stream_message make_function_block_discovery_message(uint8_t function_block, uint8_t filter);
    stream_message make_function_block_info_message(uint7_t function_block,
                                                            uint2_t direction,
                                                            group_t first_group,
                                                            uint4_t num_groups_spanned = 1);
    stream_message make_function_block_info_message(uint7_t function_block,
                                                            const function_block_options&,
                                                            group_t first_group,
                                                            uint4_t num_groups_spanned = 1);
    stream_message make_function_block_name_message(packet_format,
                                                            uint7_t                 function_block,
                                                            const std::string_view& n);


    struct function_block_options
    {
        // active
        bool active = true;

        // directions
        static constexpr uint2_t direction_input  = 0b01; //!< Input, Function Block receives MIDI Messages only
        static constexpr uint2_t direction_output = 0b10; //!< Output, Function Block transmits MIDI Messages only
        static constexpr uint2_t bidirectional =
        0b11; //!<  Bidirectional Connections. Every Input Group member has a matching Output Group.

        uint2_t direction = bidirectional;

        // MIDI 1
        static constexpr uint2_t not_midi1          = 0b00; //!< Not MIDI 1.0
        static constexpr uint2_t midi1_unrestricted = 0b01; //!< MIDI 1.0 - don't restrict Bandwidth
        static constexpr uint2_t midi1_31250        = 0b10; //!< Restrict Bandwidth to 31.25Kbps

        uint2_t midi1 = not_midi1;

        // ui hints
        static constexpr uint2_t ui_hint_as_direction = 0b00;
        static constexpr uint2_t ui_hint_receiver     = 0b01;
        static constexpr uint2_t ui_hint_sender       = 0b10;

        uint2_t ui_hint = ui_hint_as_direction;

        uint8_t ci_message_version     = 0x00;
        uint8_t max_num_sysex8_streams = 0;
    };

## Multi Part Message helpers

    template<typename Sender>
    void send_endpoint_name(std::string_view, Sender&&);

    template<typename Sender>
    void send_product_instance_id(std::string_view, Sender&&);

    template<typename Sender>
    void send_function_block_name(uint7_t function_block, std::string_view, Sender&&);

## Data Views

### Endpoint Discovery View

    namespace discovery_filter {
        constexpr uint8_t endpoint_info        = 0b00001;
        constexpr uint8_t device_identity      = 0b00010;
        constexpr uint8_t endpoint_name        = 0b00100;
        constexpr uint8_t product_instance_id  = 0b01000;
        constexpr uint8_t stream_configuration = 0b10000;
        constexpr uint8_t endpoint_all         = 0b11111;

        constexpr uint8_t function_block_info = 0b01;
        constexpr uint8_t function_block_name = 0b10;
        constexpr uint8_t function_block_all  = 0b11;
    } // namespace discovery_filter

    struct endpoint_discovery_view
    {
        endpoint_discovery_view(const universal_packet&);

        uint8_t  ump_version_major() const;
        uint8_t  ump_version_minor() const;
        uint16_t ump_version() const;

        uint8_t filter() const;

        bool requests_info() const;
        bool requests_device_identity() const;
        bool requests_name() const;
        bool requests_product_instance_id() const;
        bool requests_stream_configuration() const;
    };


    std::optional<endpoint_discovery_view> as_endpoint_discovery_view(const universal_packet&);

### Endpoint Info View

    struct endpoint_info_view
    {
        endpoint_info_view(const universal_packet&);

        uint8_t  ump_version_major() const;
        uint8_t  ump_version_minor() const;
        uint16_t ump_version() const;

        uint8_t num_function_blocks() const;
        bool    static_function_blocks() const;
        uint8_t protocols() const;
        uint8_t extensions() const;
    };

    std::optional<endpoint_info_view> as_endpoint_info_view(const universal_packet&);

### Device Identity View

    struct device_identity_view
    {
        device_identity_view(const universal_packet&);

        device_identity identity() const;
    };

    std::optional<device_identity_view> as_device_identity_view(const universal_packet&);

### Endpoint Name View

    struct endpoint_name_view
    {
        endpoint_name_view(const universal_packet&);

        packet_format format() const;
        std::string payload() const;
    };

    std::optional<endpoint_name_view> as_endpoint_name_view(const universal_packet&);

### Product Instance ID View

    struct product_instance_id_view
    {
        product_instance_id_view(const universal_packet&);

        packet_format format() const;
        std::string payload() const;
    };

    std::optional<product_instance_id_view> as_product_instance_id_view(const universal_packet&);

### Stream Configuration View

    struct stream_configuration_view
    {
        stream_configuration_view(const universal_packet&);

        protocol_t   protocol() const;
        extensions_t extensions() const;
    };

    std::optional<stream_configuration_view> as_stream_configuration_view(const universal_packet&);

### Function Block Discovery View

    struct function_block_discovery_view
    {
        function_block_discovery_view(const universal_packet&);

        uint8_t function_block() const;
        uint8_t filter() const;

        bool requests_function_block(uint8_t block) const;
        bool requests_info() const;
        bool requests_name() const;
    };

    std::optional<function_block_discovery_view> as_function_block_discovery_view(const universal_packet&);

### Function Block Info View

    struct function_block_info_view
    {
        function_block_info_view(const universal_packet&);

        bool    active() const;
        uint8_t function_block() const;

        uint8_t direction() const;
        uint8_t midi1() const;
        uint8_t ui_hint() const;

        uint8_t first_group() const;
        uint8_t num_groups_spanned() const;

        uint7_t ci_message_version() const;
        uint8_t max_num_sysex8_streams() const;
    };

    std::optional<function_block_info_view> as_function_block_info_view(const universal_packet&);


### Function Block Name View

    struct function_block_name_view
    {
        function_block_name_view(const universal_packet&);

        packet_format format() const;
        uint8_t       function_block() const;
        std::string   payload() const;
    };

    std::optional<function_block_name_view> as_function_block_name_view(const universal_packet&);
