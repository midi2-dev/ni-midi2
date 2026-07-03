# Mixed Data Set

Code examples can be found in [`mixed_data_set.examples.cpp`](mixed_data_set.examples.cpp).

_Mixed Data Set_ messages allow transfer of arbitrary (8 bit) data, the data being
split into _chunks_, each chunk consisting of a _header_ packet followed by _payload_
packets (14 data bytes per packet). Up to 16 _Mixed Data Sets_ (distinguished by
their `mds_id`) can be transmitted simultaneously within a group.

A complete _Mixed Data Set_ is represented by

```cpp
struct mixed_data_set
{
    using data_type = std::vector<uint8_t>;

    manufacturer_t manufacturerID;
    uint16_t       deviceID;       // 0xFFFF means 'all call'
    uint16_t       subID1;
    uint16_t       subID2;
    data_type      data;

    mixed_data_set(manufacturer_t);
    mixed_data_set(manufacturer_t, uint16_t device_id, uint16_t sub_id_1, uint16_t sub_id_2);
    mixed_data_set(manufacturer_t, uint16_t device_id, uint16_t sub_id_1, uint16_t sub_id_2, data_type);

    void clear();
};
```

Like in _System Exclusive_ messages, `deviceID`, `subID1` and `subID2` are defined by
other MMA/AMEI specifications when `manufacturerID` is a Universal SysEx ID, otherwise
their use is defined by the manufacturer.

On the wire the manufacturer ID is encoded in 16 bits, conversions are available as

```cpp
constexpr uint16_t       mixed_data_set_manufacturer_id(manufacturer_t);
constexpr manufacturer_t manufacturer_from_mixed_data_set_id(uint16_t);
```

## Message Creation and Filtering

Individual packets are represented by `mixed_data_set_header_packet` and
`mixed_data_set_payload_packet` (in `extended_data_message.h`), with corresponding
factory functions, filters and views:

```cpp
mixed_data_set_header_packet  make_mixed_data_set_header_packet(uint4_t mds_id, group_t = 0);
mixed_data_set_payload_packet make_mixed_data_set_payload_packet(uint4_t mds_id, group_t = 0);

bool is_mixed_data_set_header_packet(const universal_packet&);
bool is_mixed_data_set_payload_packet(const universal_packet&);
bool is_mixed_data_set_packet(const universal_packet&);

std::optional<mixed_data_set_header_packet_view>  as_mixed_data_set_header_packet_view(const universal_packet&);
std::optional<mixed_data_set_payload_packet_view> as_mixed_data_set_payload_packet_view(const universal_packet&);
```

## Sending Mixed Data Sets

Usually one does not create _Mixed Data Set_ packets manually, but uses

```cpp
template<typename Sender>
void send_mixed_data_set(const mixed_data_set&, uint4_t mds_id, group_t, Sender&&);

std::vector<extended_data_message> as_mixed_data_set_packets(const mixed_data_set&, uint4_t mds_id, group_t = 0);
```

`send_mixed_data_set` splits the data into chunks and packets and forwards them to
`sender(p)`. `as_mixed_data_set_packets` returns the resulting packets in a vector
instead.

## Collecting Mixed Data Sets

Use `mixed_data_set_collector` to reassemble _Mixed Data Sets_ from incoming packets:

```cpp
class mixed_data_set_collector
{
public:
    using callback = std::function<void(const mixed_data_set&, uint4_t mds_id)>;

    explicit mixed_data_set_collector(callback);

    void set_callback(callback);
    void set_max_data_size(size_t); // limit maximum size of accepted mixed data set data

    void feed(const universal_packet&);
    void reset();
};
```

Feed incoming packets into `feed()`. Once the final chunk of a _Mixed Data Set_ is
complete the callback is invoked with the reassembled `mixed_data_set` and its
`mds_id`. The collector maintains an independent collection state per `mds_id`, so
interleaved _Mixed Data Sets_ are supported. Aborted messages (a header with
_Number of this Chunk_ set to zero), out-of-order chunks and invalid headers discard
the collection state of the affected `mds_id`.
