# MIDI Capability Inquiry

Code examples can be found in [`capability_inquiry.examples.cpp`](capability_inquiry.examples.cpp).

The library implements MIDI-CI 1.2 as specified in M2-101-UM v1.2.

## Message Creation

MIDI-CI messages are represented by a

```cpp
namespace midi::ci {

struct message : universal_sysex::message
{
    message(subtype_t, muid_t source_muid, muid_t destination_muid, uint7_t device_id = 0x7F);

    uint7_t message_version() const;
    muid_t  source_muid() const;
    muid_t  destination_muid() const;

    void set_source_muid(muid_t);
    void set_destination_muid(muid_t);
};

}
```

`ci::message` is a `sysex7`. Use `send_sysex7()` to send it as a sequence of Data Messages:

```cpp
send_sysex7(m, group, [](const data_message& p) { /* send the packet */ });
```

Instead of using `ci::message` constructors one can create messages using factory functions.
The factory functions create messages with message version 2 (`ci::version`).
Factory functions with a `_v1` suffix create messages with message version 1.

The factory functions share these parameters:

* `source_muid` and `destination_muid` are 28 bit MUIDs. Use `ci::broadcast_muid` (`0x0FFFFFFF`) to address all devices.
* `device_id` addresses a MIDI channel (`0x00` to `0x0F`), the whole group (`0x7E`) or the whole function block (`0x7F`).
  The default is `0x7F`.

### Management Messages

```cpp
message make_discovery_inquiry(muid_t source_muid, const identity_t&, uint7_t categories,
                               uint28_t max_message_size, uint7_t output_path_id = 0);
message make_discovery_reply(muid_t source_muid, muid_t destination_muid, const identity_t&,
                             uint7_t categories, uint28_t max_message_size,
                             uint7_t output_path_id, uint7_t function_block = 0x7F);

message make_discovery_inquiry_v1(muid_t source_muid, const identity_t&, uint7_t categories,
                                  uint28_t max_message_size);
message make_discovery_reply_v1(muid_t source_muid, muid_t destination_muid, const identity_t&,
                                uint7_t categories, uint28_t max_message_size);

message make_endpoint_information_inquiry(muid_t source_muid, muid_t destination_muid, uint7_t status);
message make_endpoint_information_reply(muid_t source_muid, muid_t destination_muid, uint7_t status,
                                        const uint7_t* data, size_t data_size);

message make_ack_message(muid_t source_muid, muid_t destination_muid, uint7_t device_id,
                         subtype_t transaction, uint7_t status_code, uint7_t status_data,
                         uint7_t details[5], const std::string_view& message);

message make_nak_message(muid_t source_muid, muid_t destination_muid, uint7_t device_id,
                         subtype_t transaction, uint7_t status_code, uint7_t status_data,
                         uint7_t details[5], const std::string_view& message);
message make_nak_message(const capability_inquiry_view&, uint7_t status_code, uint7_t status_data,
                         uint7_t details[5], const std::string_view& message);

message make_nak_message_v1(muid_t source_muid, muid_t destination_muid, uint7_t device_id);
message make_nak_message_v1(const capability_inquiry_view&);

message make_invalidate_muid_message(muid_t source_muid, muid_t target_muid);
```

`make_discovery_inquiry()` sets the destination MUID to `ci::broadcast_muid`.

For the `categories` parameter of the _Discovery_ messages use any combination of

```cpp
namespace category {
    constexpr uint7_t profile_configuration = (1 << 2);
    constexpr uint7_t property_exchange     = (1 << 3);
    constexpr uint7_t process_inquiry       = (1 << 4);
}
```

The `make_nak_message()` overloads that take a `capability_inquiry_view` create the reply to the message in the view.
They swap its source and destination MUIDs, copy its device ID, and use its subtype as the `transaction`.

### Profile Configuration

```cpp
message make_profile_inquiry_message(muid_t source_muid, muid_t destination_muid, uint7_t device_id = 0x7F);

message make_profile_inquiry_reply(muid_t source_muid, muid_t destination_muid,
                                   const std::vector<profile_id>& enabled_profiles,
                                   const std::vector<profile_id>& disabled_profiles,
                                   uint7_t device_id = 0x7F);
message make_profile_inquiry_reply(muid_t source_muid, muid_t destination_muid,
                                   const profile_id* enabled_profiles, uint14_t num_enabled_profiles,
                                   const profile_id* disabled_profiles, uint14_t num_disabled_profiles,
                                   uint7_t device_id = 0x7F);

message make_profile_on_request(muid_t source_muid, muid_t destination_muid,
                                const profile_id&, const profile_destination&);
message make_profile_off_request(muid_t source_muid, muid_t destination_muid,
                                 const profile_id&, const profile_destination&);
message make_profile_enabled_notification(muid_t source_muid, muid_t destination_muid,
                                          const profile_id&, const profile_destination&);
message make_profile_disabled_notification(muid_t source_muid, muid_t destination_muid,
                                           const profile_id&, const profile_destination&);

message make_profile_added_notification(muid_t source_muid, muid_t destination_muid,
                                        const profile_id&, uint7_t device_id = 0x7F);
message make_profile_removed_notification(muid_t source_muid, muid_t destination_muid,
                                          const profile_id&, uint7_t device_id = 0x7F);

message make_profile_details_inquiry(muid_t source_muid, muid_t destination_muid,
                                     const profile_id&, uint7_t target, uint7_t device_id = 0x7F);
message make_profile_details_reply(muid_t source_muid, muid_t destination_muid,
                                   const profile_id&, uint7_t target,
                                   const uint7_t* data, size_t data_size, uint7_t device_id = 0x7F);

message make_profile_specific_data_message(muid_t source_muid, muid_t destination_muid,
                                           const profile_id&, const std::vector<uint7_t>& data,
                                           uint7_t device_id = 0x7F);
message make_profile_specific_data_message(muid_t source_muid, muid_t destination_muid,
                                           const profile_id&, const uint7_t* data, size_t data_size,
                                           uint7_t device_id = 0x7F);
```

#### Profile IDs

A _Profile_ is identified by a five byte `profile_id`:

```cpp
struct profile_id
{
    uint7_t byte1{ 0x7E }; // 0x7E: Standard Defined, or Manufacturer SysEx ID 1
    uint7_t byte2{ 0x00 }; // Profile Number MSB, or Manufacturer SysEx ID 2
    uint7_t byte3{ 0x00 }; // Profile Number LSB, or Manufacturer SysEx ID 3
    uint7_t byte4{ 0x00 }; // Profile Version, or Manufacturer Specific Info
    uint7_t byte5{ 0x00 }; // Profile Level, or Manufacturer Specific Info

    constexpr profile_id() = default;
    constexpr profile_id(uint7_t b1, uint7_t b2, uint7_t b3, uint7_t b4, uint7_t b5);
};

constexpr bool operator==(const profile_id&, const profile_id&);

constexpr bool same_profile(const profile_id&, const profile_id&);
```

`operator==` compares all five bytes.

Bytes 4 and 5 of a _Standard Defined Profile_ hold the _Profile Version_ and the
_Profile Level_, so two IDs that differ only in these bytes still identify the same
profile (M2-102). `same_profile()` compares Standard Defined Profile IDs by bytes 1 to 3
only. For _Manufacturer Specific Profiles_ the meaning of bytes 4 and 5 is defined by
the manufacturer, so `same_profile()` compares all five bytes.

Use `same_profile()` to find out whether an incoming message refers to a profile you
support, independent of its version and level:

```cpp
if (auto v = ci::as<ci::profile_id_view>(sx))
{
    if (ci::same_profile(v->profile(), my_profile))
    {
        // ...
    }
}
```

#### Profile Destinations

The _Set Profile On_, _Set Profile Off_, _Profile Enabled_ and _Profile Disabled_ messages take a `profile_destination`:

```cpp
struct profile_destination
{
    uint7_t  scope{ 0x7F };     // 0..15 = channel, 0x7E = group, 0x7F = function block
    uint14_t num_channels{ 0 }; // 0 if scope is group or function block, otherwise number of channels > 0

    static constexpr uint7_t group          = 0x7E;
    static constexpr uint7_t function_block = 0x7F;
};

constexpr profile_destination channel_profile(uint7_t first_channel, uint14_t num_channels = 1);
constexpr profile_destination group_profile;
constexpr profile_destination function_block_profile;
```

The factory functions write `scope` to the device ID field of the message.

### Property Exchange

```cpp
message make_property_exchange_capabilities_inquiry(muid_t source_muid, muid_t destination_muid,
                                                    uint7_t max_num_requests = 1, uint7_t device_id = 0x7F);
message make_property_exchange_capabilities_reply(muid_t source_muid, muid_t destination_muid,
                                                  uint7_t max_num_requests = 1, uint7_t device_id = 0x7F);

message make_get_property_data_inquiry(muid_t source_muid, muid_t destination_muid,
                                       std::string_view resource,
                                       uint7_t request_id = 0, uint7_t device_id = 0x7F);
message make_get_property_data_inquiry(muid_t source_muid, muid_t destination_muid,
                                       std::string_view resource, const property_exchange::header_options&,
                                       uint7_t request_id = 0, uint7_t device_id = 0x7F);

message make_get_property_data_reply(muid_t source_muid, muid_t destination_muid,
                                     const property_exchange::header&,
                                     uint14_t number_of_chunks, uint14_t number_of_this_chunk,
                                     const property_exchange::chunk&,
                                     uint7_t request_id = 0, uint7_t device_id = 0x7F);
message make_get_property_data_reply(muid_t source_muid, muid_t destination_muid, int status,
                                     uint14_t number_of_chunks, uint14_t number_of_this_chunk,
                                     const property_exchange::chunk&,
                                     uint7_t request_id = 0, uint7_t device_id = 0x7F);
message make_get_property_data_reply(muid_t source_muid, muid_t destination_muid,
                                     int status, std::string_view message,
                                     uint14_t number_of_chunks, uint14_t number_of_this_chunk,
                                     const property_exchange::chunk&,
                                     uint7_t request_id = 0, uint7_t device_id = 0x7F);
message make_get_property_data_reply(muid_t source_muid, muid_t destination_muid,
                                     uint14_t number_of_chunks, uint14_t number_of_this_chunk,
                                     const property_exchange::chunk&,
                                     uint7_t request_id = 0, uint7_t device_id = 0x7F);

message make_set_property_data_inquiry(muid_t source_muid, muid_t destination_muid,
                                       std::string_view resource,
                                       uint14_t number_of_chunks, uint14_t number_of_this_chunk,
                                       const property_exchange::chunk&,
                                       uint7_t request_id = 0, uint7_t device_id = 0x7F);
message make_set_property_data_inquiry(muid_t source_muid, muid_t destination_muid,
                                       std::string_view resource, const property_exchange::header_options&,
                                       uint14_t number_of_chunks, uint14_t number_of_this_chunk,
                                       const property_exchange::chunk&,
                                       uint7_t request_id = 0, uint7_t device_id = 0x7F);
message make_set_property_data_inquiry(muid_t source_muid, muid_t destination_muid,
                                       uint14_t number_of_chunks, uint14_t number_of_this_chunk,
                                       const property_exchange::chunk&,
                                       uint7_t request_id = 0, uint7_t device_id = 0x7F);

message make_set_property_data_reply(muid_t source_muid, muid_t destination_muid, int status,
                                     uint7_t request_id = 0, uint7_t device_id = 0x7F);
message make_set_property_data_reply(muid_t source_muid, muid_t destination_muid,
                                     int status, std::string_view message,
                                     uint7_t request_id = 0, uint7_t device_id = 0x7F);

message make_subscription_inquiry(muid_t source_muid, muid_t destination_muid,
                                  std::string_view resource, std::string_view command, std::string_view subscribeId,
                                  uint7_t request_id = 0, uint7_t device_id = 0x7F);
message make_subscription_inquiry(muid_t source_muid, muid_t destination_muid,
                                  std::string_view resource, std::string_view command, std::string_view subscribeId,
                                  uint14_t number_of_chunks, uint14_t number_of_this_chunk,
                                  const property_exchange::chunk&,
                                  uint7_t request_id = 0, uint7_t device_id = 0x7F);
message make_subscription_inquiry(muid_t source_muid, muid_t destination_muid,
                                  uint14_t number_of_chunks, uint14_t number_of_this_chunk,
                                  const property_exchange::chunk&,
                                  uint7_t request_id = 0, uint7_t device_id = 0x7F);

message make_subscription_reply(muid_t source_muid, muid_t destination_muid, int status,
                                uint7_t request_id = 0, uint7_t device_id = 0x7F);
message make_subscription_reply(muid_t source_muid, muid_t destination_muid,
                                int status, std::string_view message,
                                uint7_t request_id = 0, uint7_t device_id = 0x7F);

message make_notify_message(muid_t source_muid, muid_t destination_muid, int status,
                            uint7_t request_id = 0, uint7_t device_id = 0x7F);
message make_notify_message(muid_t source_muid, muid_t destination_muid,
                            int status, std::string_view message,
                            uint7_t request_id = 0, uint7_t device_id = 0x7F);
```

The factory functions write the header data as JSON:

| Parameters | Header data |
|---|---|
| `resource` | `{"resource":"<resource>"}` |
| `resource`, `header_options` | `{"resource":"<resource>","<key>":<value>,...}` |
| `resource`, `command`, `subscribeId` | `{"resource":"<resource>","command":"<command>","subscribeId":"<subscribeId>"}` |
| `status` | `{"status":<status>}` |
| `status`, `message` | `{"status":<status>,"message":"<message>"}` |
| none | empty |

The overloads without a `resource`, `status` or `header` parameter write an empty header. Use them for the chunks after the first chunk.
Pass `1` as `number_of_chunks` and `number_of_this_chunk` for a message with a single chunk.

The `property_exchange` namespace has these helpers:

```cpp
namespace property_exchange {

    // constructible from std::string, std::string_view, std::vector<uint7_t>,
    // const uint7_t* and size, or const char* and size
    struct header;
    struct chunk;

    using header_options = std::vector<std::pair<std::string_view, std::string_view>>;

    std::string make_rjson(std::string_view key, std::string_view value);
    std::string make_rjson(std::string_view key, int value);
    std::string make_rjson(std::string_view key, std::string_view value, const header_options&);
    std::string make_rjson(std::string_view key, int value, const header_options&);

    struct tags
    {
        static constexpr std::string_view resource{ "resource" };
        static constexpr std::string_view command{ "command" };
        static constexpr std::string_view status{ "status" };
        ...
    };

    inline constexpr std::string_view ResourceList{ "ResourceList" };
    inline constexpr std::string_view DeviceInfo{ "DeviceInfo" };
    inline constexpr std::string_view ChannelList{ "ChannelList" };
    ...
}
```

`header` and `chunk` do not copy the data. The data must stay valid until the factory function returns.

`make_rjson()` writes a `header_options` value as a JSON number if the value is an integer.
It writes all other values as JSON strings.

### Process Inquiry

```cpp
message make_process_inquiry_capabilities_inquiry(muid_t source_muid, muid_t destination_muid,
                                                  uint7_t device_id = 0x7F);
message make_process_inquiry_capabilities_reply(muid_t source_muid, muid_t destination_muid,
                                                uint7_t features, uint7_t device_id = 0x7F);

message make_midi_message_report_inquiry(muid_t source_muid, muid_t destination_muid,
                                         uint7_t data_control, uint7_t system_messages,
                                         uint7_t channel_controller_messages, uint7_t note_data_messages,
                                         uint7_t device_id = 0x7F);
message make_midi_message_report_reply(muid_t source_muid, uint7_t system_messages,
                                       uint7_t channel_controller_messages, uint7_t note_data_messages,
                                       uint7_t device_id = 0x7F);
message make_midi_message_report_end(muid_t source_muid, uint7_t device_id = 0x7F);
```

`make_midi_message_report_reply()` and `make_midi_message_report_end()` set the destination MUID to `ci::broadcast_muid`.

## Message Filtering

Filtering of MIDI-CI messages can be done using

```cpp
bool is_capability_inquiry_message(const sysex7&);
```

Use `capability_inquiry_view::subtype()` to find the message type. The `ci::subtype` namespace defines a constant for each subtype.

## Data Views

All MIDI-CI data views derive from

```cpp
struct capability_inquiry_view : universal_sysex::message_view
{
    explicit capability_inquiry_view(const sysex7&);

    uint7_t message_version() const;
    muid_t  source_muid() const;
    muid_t  destination_muid() const;

    static bool validate(const sysex7&);
};
```

`universal_sysex::message_view` adds `device_id()` and `subtype()`.

A data view stores a reference to the `sysex7`. The `sysex7` must stay valid while you use the view.

The constructors do not validate the message. Use `ci::as()` to check and create a view in a single statement:

```cpp
namespace ci {
    template<typename view>
    std::optional<view> as(const sysex7&);
}
```

`ci::as()` returns an empty optional if `view::validate()` returns `false`.
The `validate()` function of a specific view checks the subtype and the minimum size of the message.

```cpp
if (auto v = ci::as<ci::discovery_inquiry_view>(sx))
{
    auto identity = v->identity();
}
```

### Management Message Views

```cpp
struct discovery_inquiry_view
{
    identity_t identity() const;
    uint7_t    categories() const;
    uint28_t   maximum_message_size() const;
    uint7_t    output_path_id() const;
};

struct discovery_reply_view
{
    identity_t identity() const;
    uint7_t    categories() const;
    uint28_t   maximum_message_size() const;
    uint7_t    output_path_id() const;
    uint7_t    function_block() const;
};

struct endpoint_information_inquiry_view
{
    uint7_t status() const;
};

struct endpoint_information_reply_view
{
    uint7_t        status() const;
    uint14_t       information_data_length() const;
    const uint7_t* information_data() const;
};

struct ack_view // and nak_view
{
    uint7_t          transaction() const;
    uint7_t          status_code() const;
    uint7_t          status_data() const;
    const uint7_t*   details() const;
    uint14_t         message_length() const;
    const uint7_t*   message_data() const;
    std::string_view message() const;
};

struct invalidate_muid_view
{
    muid_t target_muid() const;
};
```

For a message with message version 1, `output_path_id()` returns `0` and `function_block()` returns `0x7F`.

### Profile Configuration Views

```cpp
struct profile_inquiry_view
{
};

struct profile_inquiry_reply_view
{
    bool     has_enabled_profiles() const;
    bool     has_disabled_profiles() const;
    uint14_t num_enabled_profiles() const;
    uint14_t num_disabled_profiles() const;

    std::vector<profile_id> enabled_profiles() const;
    std::vector<profile_id> disabled_profiles() const;
};

struct profile_id_view
{
    profile_id profile() const;
};

struct profile_destination_view : profile_id_view
{
    uint14_t            num_channels() const;
    profile_destination params() const;
};

struct profile_details_inquiry_view : profile_id_view
{
    uint7_t target() const;
};

struct profile_details_reply_view : profile_id_view
{
    uint7_t        target() const;
    uint14_t       target_data_length() const;
    const uint7_t* target_data() const;
};

struct profile_specific_data_view : profile_id_view
{
    const uint7_t* data_begin() const;
    const uint7_t* data_end() const;
    size_t         data_size() const;
};
```

Some views accept more than one subtype:

| View | Subtypes |
|---|---|
| `profile_id_view` | all Profile Configuration messages except _Profile Inquiry_ and _Profile Inquiry Reply_ |
| `profile_destination_view` | _Set Profile On_, _Set Profile Off_, _Profile Enabled_, _Profile Disabled_ |

`profile_destination_view::params()` returns the device ID as `scope`.

### Property Exchange Views

```cpp
struct property_exchange_capabilities_view
{
    uint7_t maximum_number_of_requests() const;
    uint7_t pe_version_major() const;
    uint7_t pe_version_minor() const;
};

struct property_exchange::property_data_message_view
{
    uint7_t request_id() const;

    const uint7_t* header_begin() const;
    const uint7_t* header_end() const;
    size_t         header_size() const;

    uint14_t number_of_chunks() const;
    uint14_t number_of_this_chunk() const;

    const uint7_t* chunk_begin() const;
    const uint7_t* chunk_end() const;
    size_t         chunk_size() const;
};

struct get_property_data_view : property_exchange::property_data_message_view {};
struct set_property_data_view : property_exchange::property_data_message_view {};
struct subscription_view      : property_exchange::property_data_message_view {};
struct notify_view            : property_exchange::property_data_message_view {};
```

`property_exchange_capabilities_view`, `get_property_data_view`, `set_property_data_view` and `subscription_view`
accept the inquiry and the reply.

The library does not parse the header JSON. Use your own JSON parser on the range from `header_begin()` to `header_end()`.

### Process Inquiry Views

```cpp
struct process_inquiry_capabilities_reply_view
{
    uint7_t supported_features() const;
};

struct midi_message_report_inquiry_view
{
    uint7_t message_data_control() const;
    uint7_t system_message_types() const;
    uint7_t channel_controller_message_types() const;
    uint7_t note_data_message_types() const;
};

struct midi_message_report_reply_view
{
    uint7_t system_message_types() const;
    uint7_t channel_controller_message_types() const;
    uint7_t note_data_message_types() const;
};
```

The library has no views for the _Process Inquiry Capabilities Inquiry_ and the _MIDI Message Report End_ messages.
Use `capability_inquiry_view` for them.
