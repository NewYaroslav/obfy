# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]
- document lifetime of `*_ONCE` macros
- remove redundant `OBFY_TU_SALT` defaults
- add character-width check for obfuscated strings
- drop `constexpr` from `obf_string_impl` constructor

## [1.0.0] - 2025-09-07
- Initial release of the forked project.
- Added obfuscated function call macros `OBFY_CALL` and `OBFY_CALL_RET`.
- Provided compile-time string obfuscation via `OBFY_STR` and related helpers.
- Introduced numeric and control-flow obfuscation wrappers.
- Renamed multiple macros for clarity and consistency.
- Added obfuscated byte block macros `OBFY_BYTES` and `OBFY_BYTES_ONCE`.

