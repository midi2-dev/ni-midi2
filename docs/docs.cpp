extern void run_channel_voice_message_examples();
extern void run_data_message_examples();
extern void run_extended_data_message_examples();
extern void run_flex_data_message_examples();
extern void run_midi1_byte_stream_examples();
extern void run_midi1_channel_voice_message_examples();
extern void run_midi2_channel_voice_message_examples();
extern void run_stream_message_examples();
extern void run_sysex_collector_examples();
extern void run_sysex_examples();
extern void run_system_message_examples();
extern void run_types_examples();

int main()
{
    run_types_examples();

    run_system_message_examples();

    run_midi1_channel_voice_message_examples();
    run_midi2_channel_voice_message_examples();
    run_channel_voice_message_examples();
    run_midi1_byte_stream_examples();

    run_data_message_examples();
    run_sysex_examples();
    run_extended_data_message_examples();
    run_sysex_collector_examples();

    run_stream_message_examples();
    run_flex_data_message_examples();

    return 0;
}
