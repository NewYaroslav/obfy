# Obfuscation Macros

This document summarizes the public macros provided by **obfy** and shows short usage examples.

## String obfuscation

- `OBFY_STR`, `OBFY_WSTR`
- `OBFY_STR_ONCE`, `OBFY_WSTR_ONCE`

```cpp
const char* msg = OBFY_STR("secret");
auto tmp = OBFY_STR_ONCE("one-shot");
```

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
    OBFY_ENDIF
OBFY_END_CODE
```
