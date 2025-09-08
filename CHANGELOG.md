# Changelog

All notable changes to this project will be documented in this file.

## [1.0.1] - 2025-09-08
- Added vcpkg port.
- Tweaked CMake configuration.
- Updated README.

## [1.0.0] - 2025-09-07
- Initial release of the forked project.
- Added obfuscated function call macros `OBFY_CALL` and `OBFY_CALL_RET`.
- Provided compile-time string obfuscation via `OBFY_STR` and related helpers.
- Introduced numeric and control-flow obfuscation wrappers.
- Renamed multiple macros for clarity and consistency.
- Added obfuscated byte block macros `OBFY_BYTES` and `OBFY_BYTES_ONCE`.
- Added: `OBFY_STR/OBFY_WSTR`, `OBFY_BYTES/OBFY_BYTES_ONCE`
- Improved: TU-salt, runtime tweak
- Fixed: `refholder::operator==` constraints

