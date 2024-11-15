# Utility Messages

## Message Creation and Filtering

Utility Messages are represented by a

    struct utility_message : universal_packet
    {
        utility_message(status_t, uint16_t payload = 0u);

        // utility messages are groupless since UMP 1.1
        constexpr group_t group() const      = delete;
        constexpr void    set_group(group_t) = delete;
    };

As Utility messages are groupless the `group` accessors are deleted.

Instead of using `utility_message` constructors one can create messages using factory functions:

    constexpr utility_message make_utility_message(status_t, uint16_t data);

Filtering of Utility Messages can be done checking `universal_packet::type()` against
`packet_type::utility` or use

    bool universal_packet::is_utility_message();

Currently the only use case for Utility messages are [Jitter Reduction Timestamps](../inc/midi/jitter_reduction_timestamps.h).

## Message View

Once validated against `packet_type::utility` one can create a `utility_message_view`
for a `universal_packet`:

    struct utility_message_view
    {
        utility_message_view(const universal_packet&);

        constexpr status_t status() const;
        constexpr uint16_t payload() const;
    };
