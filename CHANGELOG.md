# v1.1

* cherry-pick UMP specification `scaleUp` as `upsample_x_to_ybit``
* add `as_midi1_channel_voice_message` and `as_midi1_channel_voice_message``
* add `send_sysex7` and `send_xxx` stream messages
* introduce `sysex::data_type` and optional custom sysex data allocator
* improve MIDI-CI message creation to avoid redundant resizes/allocs of sysex data vector
* introduce sysex7_collector::set_max_sysex_data_size
* some more minor tweaks and improvements

# v1.0

* initial public release