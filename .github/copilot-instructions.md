# Copilot Instructions for ni-midi2

## Project Overview
- **ni-midi2** is a C++17 library for Universal MIDI Packets (UMP) 1.1 and MIDI-CI 1.2, providing base types and helpers for all major MIDI 2.0 message types.
- Major components: UMP packet/message structs, data views for inspection, factory functions for message creation, helpers for MIDI 1 byte stream, and Sysex collectors.
- Key directories:
  - `src/`: Core implementation
  - `inc/midi/`: Public headers
  - `docs/`: Markdown docs and code examples
  - `tests/`: Unit tests for all message types and helpers

## Key Patterns & Conventions
- **Message Creation:** Use free factory functions (e.g., `make_midi2_note_on_message`)—do not use constructors directly for packets/messages.
- **Message Inspection:** Use data view structs (e.g., `midi2_channel_voice_message_view`) or `as_*_view` helpers for safe casting/validation.
- **Type Safety:** Strongly typed wrappers for MIDI fields (e.g., `velocity`, `pitch_7_25`, `controller_value`). Use provided math operators and constructors for conversions.
- **Sysex Handling:** Use `sysex7_collector`/`sysex8_collector` for collecting and handling Sysex messages.
- **MIDI 1 Byte Stream:** Use `midi1_byte_stream_parser` and helpers for conversion to/from UMP.
- **Testing:** Unit tests are in `tests/`, mirroring message/component structure. Use these as reference for usage patterns.

## Build & Test Workflow
- Use CMake (C++17 required). Example:
  ```sh
  cmake -B build-vscode -DNIMIDI2_TESTS=ON -DNIMIDI2_EXAMPLES=ON
  cmake --build build-vscode
  ./build-vscode/ni-midi2-tests
  ```
- Options:
  - `NIMIDI2_TESTS`: Build tests (default ON if standalone)
  - `NIMIDI2_EXAMPLES`: Build code examples (default ON)
  - `NIMIDI2_TREAT_WARNINGS_AS_ERRORS`: Enable for CI/contributions
  - `NIMIDI2_UNITY_BUILDS`: Unity builds ON by default
- GTest 1.11+ required for tests (auto-detected by CMake)

## Integration & Extensibility
- No external runtime dependencies except C++17 STL and (optionally) GTest for tests.
- Designed for inclusion via CMake `add_subdirectory` or as a standalone build.
- See `docs/` for detailed usage and extension examples.

## Examples & References
- See `docs/*.examples.cpp` for idiomatic usage.
- See `tests/` for edge cases and validation patterns.
- For new message types, follow the pattern: factory function → data view struct → test in `tests/`.

## Contribution Notes
- Enable warnings as errors for PRs: `-DNIMIDI2_TREAT_WARNINGS_AS_ERRORS=ON`
- Follow existing type safety and factory/data view patterns.
- Document new features in `docs/` and add example/test coverage.

---
For more, see `README.md` and `docs/`.
