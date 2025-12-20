# Math Eval (C Expression Evaluator)

A small C library for parsing and evaluating mathematical expressions with support for:

- Infix notation input (e.g. `sin(x) + 3*4`)
- Conversion to **prefix (Polish) notation**
- Evaluation of prefix expressions
- Built-in math functions (`sin`, `cos`, `tan`, `sqrt`)
- Unary negation
- Integer and double variables
- User-defined variables via a public API
- `_Generic`-based function overloading for variable creation and setting

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

## Project Structure

```
.
├── math_eval.h     # Public API + internal declarations
├── math_eval.c     # Implementation
└── README.md
```

---

## Public API Overview

### Context management
```c
eval_ctx eval_create_context(void);
```

Creates and initializes an evaluation context.

---

### Variable creation (overloaded with `_Generic`)

```c
eval_create_var(&ctx, "x", 10);     // int
eval_create_var(&ctx, "y", 3.14);   // double
```

Backed by:
```c
void eval_create_int_var(eval_ctx* ctx, const char* name, int val);
void eval_create_double_var(eval_ctx* ctx, const char* name, double val);
```

---

### Variable assignment (overloaded)
```c
eval_set_var(&ctx, "x", 42);
eval_set_var(&ctx, "y", 2.718);
```

---

### Parsing and evaluation

```c
void create_equation(eval_ctx* ctx, const char* expr);
double solve(eval_ctx* ctx);
```

---

## Example Usage

```c
#include "math_eval.h"
#include <stdio.h>

int main(void) {
    eval_ctx ctx = eval_create_context();

    eval_create_var(&ctx, "x", 10);
    eval_create_var(&ctx, "y", 2.5);

    create_equation(&ctx, "sin(x) + y^2");
    double result = solve(&ctx);

    printf("Result: %f\n", result);
    return 0;
}
```

---

## Expression Semantics

### Prefix representation

| Infix | Prefix |
|-----|-------|
| `3 + 4` | `+ 3 4` |
| `sin(x)` | `SIN x` |
| `sin(3 + 4)` | `SIN + 3 4` |
| `-x` | `NEG x` |
| `x^2 + 1` | `+ ^ x 2 1` |

---

## Build

```sh
gcc -std=c11 -Wall -Wextra math_eval.c -lm -o math_eval
```

---

## License

Provided as-is for educational and experimental use.
