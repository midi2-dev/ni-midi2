# Common Types and Scaling Helpers

Code examples can be found in [`types.examples.cpp`](types.examples.cpp).

## Common Types

To improve readability, the library defines type aliases for several MIDI entities:

```cpp
using uint2_t  = std::uint8_t;
using uint4_t  = std::uint8_t;
using uint7_t  = std::uint8_t;
using uint8_t  = std::uint8_t;
using uint14_t = std::uint16_t;
using uint16_t = std::uint16_t;
using uint28_t = std::uint32_t;
using uint32_t = std::uint32_t;
using int32_t  = std::int32_t;
using size_t   = std::size_t;

using group_t        = uint4_t;
using status_t       = uint8_t;
using channel_t      = uint4_t;
using note_nr_t      = uint7_t;
using controller_t   = uint7_t;
using program_t      = uint7_t;
using muid_t         = uint28_t;
using manufacturer_t = uint32_t;
using protocol_t     = uint8_t;
using extensions_t   = uint8_t;
```

Intentionally no strong types are used, this may change in a future version, maybe configurable by a `cmake` option.

Additionally strong types are available for core MIDI concepts:

```cpp
struct velocity;
struct pitch_bend;
struct pitch_increment;
struct pitch_7_9;
struct pitch_7_25;
struct pitch_bend_sensitivity;
struct controller_increment;
struct controller_value;
struct device_identity;
```

Instances of these types can be initialized with native MIDI 1 bit depths (7 or 14) and with high resolution values (16 or 32 bits).

```cpp
auto v   = velocity{ uint7_t{ 44 } };
auto pb1 = pitch_bend{ uint32_t { 0x62311258 } };
auto pb2 = pitch_bend{ -0.128 };
auto p1  = pitch_7_25{ 81.3f };
auto p2  = pitch_7_25{ note_nr_t { 68 } };
auto c1  = controller_value{ uint7_t{ 126 } };
auto c2  = controller_value{ 0.93 };
```

One can query the underlying value for different bit depths / types:

```cpp
auto pb1 = pitch_bend{ uint32_t { 0x62311258 } }.as_double(); // [-1.0 .. 1.0]
auto pb2 = pitch_bend{ -0.128 }.as_uint14(); // 14 bit MIDI 1
auto p1  = pitch_7_25{ 81.3f }.as_float();   // [0.0 .. 128.0)
auto c1  = controller_value{ uint7_t{ 126 } }.as_double(); // [0.0 .. 1.0]
```

There are numerical operators to do math with related types:

```cpp
pitch_7_25 p1 += pitch_increment{ -199 };
pitch_increment pbi = pitch_bend{ -0.128 } * pitch_bend_sensitivity{ note_nr_t{ 2 } };
pitch_7_25 p3 = p1 + pbi;
pitch_7_25 p4 = p3 + 0.011f;

controller_value v1 = controller_value{ 0.44 } + controller_increment{ -1234 };
v1 += controller_increment{ 0x4000 };
```

## Scaling Helpers

The MIDI 2 specifications stricly defines how values should be down- and upscaled between different bit depths.

There are predefined helper functions for the typical use cases:

```cpp
auto as7bit1 = downsample_16_to_7bit(0x1234);
auto as7bit2 = downsample_32_to_7bit(0xFEDCBA98);
auto as14bit = downsample_32_to_7bit(0xFEDCBA98);

auto as16bit  = upsample_7_to_16bit(uint7_t{ 42 });
auto as32bit1 = upsample_7_to_32bit(uint7_t{ 42 });
auto as32bit2 = upsample_14_to_32bit(uint14_t{ 0x0567 });
```

Additionally there is a function that upsamples values between arbitrary bit depths:

```cpp
auto as14bit = upsample_x_to_ybit(0x34, 6, 14);
```

Different to upsampling downsampling can be implemented using standard bit shifting:

```cpp
auto as14bit = 20bitValue >> 6;
````
