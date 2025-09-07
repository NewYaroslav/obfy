# Obfuscation Macros

This document summarizes the public macros provided by **obfy** and shows short usage examples.

## String obfuscation

- `OBFY_STR`, `OBFY_WSTR`
- `OBFY_STR_ONCE`, `OBFY_WSTR_ONCE`

```cpp
const char* msg = OBFY_STR("secret");
const wchar_t* wmsg = OBFY_WSTR(L"wide");
auto tmp = OBFY_STR_ONCE("one-shot");
auto wtmp = OBFY_WSTR_ONCE(L"w-one-shot");
```

`*_ONCE` macros return a temporary that zeroes its storage on destruction. Pointers from `c_str()` remain valid only within the full expression; copy the string if a longer lifetime is required.

## Byte block obfuscation

- `OBFY_BYTES`
- `OBFY_BYTES_ONCE`

Requires including `obfy_bytes.hpp`.

```cpp
const unsigned char* key = OBFY_BYTES("\x01\x02\x03\x04");
auto tmp_block = OBFY_BYTES_ONCE("\xAA\xBB");
```

`OBFY_BYTES_ONCE` behaves similarly, returning a temporary block that clears its contents when destroyed.

## Function call obfuscation

Requires including `obfy_call.hpp`. Define `OBFY_ENABLE_FSM_CALL` to enable the state-machine wrapper.

- `OBFY_CALL`
- `OBFY_CALL_RET`

```cpp
int r = OBFY_CALL_RET(int, add, 1, 2);
OBFY_CALL(do_work);
```

## Numeric obfuscation

- `OBFY_V`, `OBFY_N`
- `OBFY_RATIO_D`, `OBFY_RATIO_F`
- `OBFY_BIT_CAST`

```cpp
OBFY_V(x) = 42;
int n = OBFY_N(10);
double pi = OBFY_RATIO_D(314, 100);
float f = OBFY_RATIO_F(22, 7);
auto bits = OBFY_BIT_CAST<unsigned>(0.5f);
```

## Control-flow obfuscation

- `OBFY_BEGIN_CODE` / `OBFY_END_CODE`
- `OBFY_IF` / `OBFY_ELSE` / `OBFY_ENDIF`
- `OBFY_FOR` / `OBFY_ENDFOR`, `OBFY_WHILE` / `OBFY_ENDWHILE`
- `OBFY_REPEAT` / `OBFY_AS_LONG_AS`
- `OBFY_CASE` / `OBFY_WHEN` / `OBFY_DO` / `OBFY_DONE` / `OBFY_OR` / `OBFY_DEFAULT` / `OBFY_ENDCASE`
- `OBFY_RETURN`, `OBFY_BREAK`, `OBFY_CONTINUE`

```cpp
OBFY_BEGIN_CODE
    OBFY_IF(check())
        OBFY_RETURN(42);
    OBFY_ELSE
        OBFY_FOR(int i = 0; i < 3; ++i)
            OBFY_CONTINUE;
        OBFY_ENDFOR
    OBFY_ENDIF

    OBFY_CASE(n)
        OBFY_WHEN(1) OBFY_OR(2) OBFY_DO { OBFY_BREAK; } OBFY_DONE
        OBFY_DEFAULT OBFY_DO { OBFY_BREAK; } OBFY_DONE
    OBFY_ENDCASE
OBFY_END_CODE
```
