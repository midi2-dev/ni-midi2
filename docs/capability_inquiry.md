# MIDI Capability Inquiry

Code examples can be found in [`capability_inquiry.examples.cpp`](capability_inquiry.examples.cpp).

## Profile IDs

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
