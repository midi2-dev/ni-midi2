# System Messages

Create system messages by using the

```cpp
system_message make_system_message(group_t, status_t, uint7_t data1 = 0, uint7_t data2 = 0);
system_message make_song_position_message(group_t, uint14_t position);
```

factory functions. Use

```cpp
bool is_system_message(const universal_packet&);
```

to filter for system messages, you may use the

```cpp
system_message_view(const universal_packet& ump)
```

for further inspection of the message:

```cpp
if (is_system_message(packet))
{
    auto m = system_message_view{ packet };

    group_t  g  = m.group();
    status_t s  = m.status();
    uint7_t  d1 = m.data_byte_1();
    uint7_t  d2 = m.data_byte_2();
}
```

Alternatively use

```cpp
std::optional<system_message_view> as_system_message_view(const universal_packet&);
```

to filter and view creation is a single call:

```cpp
auto c = make_song_position_message(0, 0x1234);
if (auto m = as_system_message_view(c))
{
    group_t g = m->group();
    status_t s = m->status();
    uint7_t d1 = m->data_byte_1();
    uint7_t d2 = m->data_byte_2();
    uint14_t pos = m->get_song_position();
}
```

More code examples can be found in [`system_message.examples.cpp`](system_message.examples.cpp).
