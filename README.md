# Math Eval (C Expression Evaluator)

A small C library for parsing and evaluating mathematical expressions with support for:

- Infix notation input (e.g. `sin(x) + 3*4`)
- Conversion to **prefix (Polish) notation**
- Evaluation of prefix expressions
- Built-in math functions (`sin`, `cos`, `tan`, `sqrt`)
- Unary negation
- Integer and double variables
- User-defined variables via a public API

This project is designed to be used **as a library** inside other C programs, not just as a standalone calculator.

---

## Features

### Supported operators
| Operator | Meaning | Associativity |
|--------|--------|--------------|
| `+` | addition | left |
| `-` | subtraction | left |
| `*` | multiplication | left |
| `/` | division | left |
| `^` | power | right |
| unary `-` | negation | right |

### Supported functions
- `sin(x)`
- `cos(x)`
- `tan(x)`
- `sqrt(x)`

(All trig functions use **radians**, via `<math.h>`.)

### Variable support
- Integer variables
- Double variables
- Variables are resolved during evaluation using an evaluation context

---

### Error Handling
- All errors print error messages to stdout
- All errors set the `eval_context` member `error` to the respective error
- **NOTE:** if an error is encountered the program will most likely crash (it will still print the error message)

#### Error Types
| Error Type | Description |
|--------|-----------------|
| `UnknownOperator` | The parser came across an operator token that it does not recognize|
| `UnknownToken` | The parser came across a token that it could not resolve into a number, variable, function, or operator |
| `TooManyVariables` | The number of variables added to the program was greater than the maximum number of variables allowed (defined in `MAX_DICT_SIZE`) |

## Project Structure

All functions are contained in one `.c` file and one `.h` file so to be easily added to relevant projects

---

## Public API Overview

### Context management
```c
eval_ctx eval_create_context(void);
```

Creates and initializes an evaluation context.

---

### Variable creation

```c
eval_create_var(&ctx, "x", 10);     // int
eval_create_var(&ctx, "y", 3.14f);   // double
```
---

### Variable assignment
```c
eval_set_var(&ctx, "x", 42);
eval_set_var(&ctx, "y", 2.718);
```

---

### Parsing and evaluation

```c
void eval_create_equation(&ctx, "sqrt(x) * y");
double eval_solve(&ctx);
```

---

## Example Usage

```c
#include "math_eval.h"
#include <stdio.h>

int main(void) {
    eval_ctx ctx = eval_create_context();

    eval_create_var(&ctx, "x", 10);
    eval_create_var(&ctx, "y", 2.5f);

    eval_create_equation(&ctx, "sqrt(x) * y");
    double result = eval_solve(&ctx);

    printf("Result: %f\n", result);
    return 0;
}
```

---

## Build

**NOTE:** there is no entrypoint in this file, so you will encounter a linker error unless one is added
```sh
gcc math_eval.c -o math_eval
```

---

## License

MIT License
