# Flex Data Messages

_WORK IN PROGRESS_

Code examples can be found in [`flex_data_message.examples.cpp`](flex_data_message.examples.cpp).

## Base Type

    struct flex_data_message : universal_packet
    {
        flex_data_message(
            group_t, packet_format, packet_address, uint4_t, status_t, status_t, uint32_t = 0, uint32_t = 0, uint32_t = 0);

        packet_format  format() const;
        packet_address address() const;
        status_t       status_bank() const;
        status_t       status() const;

        std::string        payload_as_string() const;
        static std::string payload_as_string(const universal_packet&);
    };

    bool is_flex_data_message(const universal_packet&);


### Factory Functions

    flex_data_message make_flex_data_message(group_t,
                                            packet_format,
                                            packet_address,
                                            uint4_t  channel,
                                            status_t status_bank,
                                            status_t status,
                                            uint32_t data1 = 0,
                                            uint32_t data2 = 0,
                                            uint32_t data3 = 0);
    flex_data_message make_flex_data_text_message(group_t,
                                                packet_format,
                                                packet_address,
                                                uint4_t                 channel,
                                                status_t                status_bank,
                                                status_t                status,
                                                const std::string_view& text);

    flex_data_message make_set_tempo_message(group_t, uint32_t ten_ns_per_quarter_note);
    flex_data_message make_set_time_signature_message(group_t,
                                                    uint8_t numerator,
                                                    uint8_t denominator,
                                                    uint8_t nr_32rd_notes);
    flex_data_message make_set_metronome_message(group_t,
                                                uint8_t num_clocks_per_primary_click,
                                                uint8_t bar_accent_part1,
                                                uint8_t bar_accent_part2,
                                                uint8_t bar_accent_part3,
                                                uint8_t num_subdivision_clicks1,
                                                uint8_t num_subdivision_clicks2);
    flex_data_message make_set_key_signature_message(
        group_t, packet_address, uint4_t channel, uint4_t sharps_or_flats, uint4_t tonic_note);
    flex_data_message make_set_chord_message(
        group_t, packet_address, uint4_t channel, uint32_t data1, uint32_t data2, uint32_t data3);


### Flex Data Message Data View:

    struct flex_data_message_view
    {
        explicit flex_data_message_view(const universal_packet&);

        group_t        group() const;
        packet_format  format() const;
        packet_address address() const;
        channel_t      channel() const;
        status_t       status_bank() const;
        status_t       status() const;
        uint32_t       data1() const;
        uint32_t       data2() const;
        uint32_t       data3() const;
        uint8_t        data_byte1() const;
        uint8_t        data_byte2() const;
        uint8_t        data_byte2() const;
        uint8_t        data_byte3() const;
        uint8_t        data_byte2() const;
        uint8_t        data_byte4() const;
        uint8_t        data_byte5() const;
        uint8_t        data_byte6() const;
        uint8_t        data_byte7() const;
        uint8_t        data_byte8() const;

        const std::string payload_as_string() const;
    };

    std::optional<flex_data_message_view> as_flex_data_message_view(const universal_packet&);
