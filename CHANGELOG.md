# v1.11.0

* add data_byte accessors to `flex_data_message_view`
* improve documentation for `flex data` and `stream` messages
* add examples for `flex data` and `stream` messages

# v1.10.1

* fix AKAIs manufacturer ID

# v1.10.0

* improve documentation and add example code (part 1, more to follow)
* add convenience functions to add `uint32` (5 bytes) to `sysex7`

# v1.9.0

* remove redundant _midi2_ from APIs and deprecate old versions
* add `is_registered_per_note_controller_pitch_message()`
* add convinience conversions between `controller_value` and `pitch_7_25`

# v1.8.5

* support 14 bit in `controller_value`, useful for MIDI 1 (N)RPNs

# v1.8.4

* fix Property Exchange chunk validation

# v1.8.3

* more VS 2022 warning fixes

# v1.8.2

* fix VS 2022 warnings in as_double() tests

# v1.8.1

* fix signed/unsigned mismatch warning with older MSVC compilers

# v1.8.0

* fix MIDI CI profile messages, some implementations were not yet compatible with MIDI CI 1.2

# v1.7.0

* add note attribute helpers
* add pitch bend sensitivity helpers

# v1.6.0

* add support for double precision numbers
* fix under/overflows in +operators

# v1.5.1

* fix channel parameters to `channel_t` type alias

# v1.5.0

* sysex: support for polymorphic memory resources
* minor unit test tweaks
* CI: verify non-unity builds and pmr sysex option

# v1.4.0

* fix: add missing `target_muid` to `make_invalidate_muid_message` and `invalidate_muid_view`

# v1.3.0

* add 'is_midi2_<xxx>_message' for per note and registered / assignable controller messages
* add CI badges to README
* `pre-commit` updates
* sysex collectors: `clear()` sysex in `reset()`

# v1.2.0

* add support for System Exclusive 8 messages
* introduce `sysex7_packet`, move payload members from `data_message` into `sysex7_packet`

## Migration guide

`make_sysex7_<XXX>_packet` factory functions now return `sysex7_packet` instead of `data_packet`.
This might require minor changes to variable types and function signatures.

# v1.1.1

* fix `device_identity` handling in MIDI CI and stream messages
* fix faulty assertions in `sysex7::add_uint14` / `sysex7::add_uint28`

# v1.1.0

* cherry-pick UMP specification `scaleUp` as `upsample_x_to_ybit`
* add `as_midi1_channel_voice_message` and `as_midi1_channel_voice_message`
* add `send_sysex7` and `send_xxx` stream messages
* introduce `sysex::data_type` and optional custom sysex data allocator
* improve MIDI-CI message creation to avoid redundant resizes/allocs of sysex data vector
* introduce sysex7_collector::set_max_sysex_data_size
* some more minor tweaks and improvements

# v1.0.0

* initial public release
