# ExprTk Source Code Research Report

This document is a detailed technical analysis of the ExprTk (Mathematical Expression
Toolkit) C++ library, based on a thorough reading of every source file in the `src/`
directory (~47,000 lines of code across 23 header files).

---

## Table of Contents

1. [Executive Summary](#1-executive-summary)
2. [Architecture Overview](#2-architecture-overview)
3. [File Map and Dependencies](#3-file-map-and-dependencies)
4. [Core Infrastructure (`common.hpp`, `types.hpp`)](#4-core-infrastructure)
5. [Lexer and Tokenization (`lexer.hpp`)](#5-lexer-and-tokenization)
6. [AST Node System](#6-ast-node-system)
7. [Operators and Numeric Processing (`operators.hpp`)](#7-operators-and-numeric-processing)
8. [Function Interfaces (`functions.hpp`)](#8-function-interfaces)
9. [Symbol Table (`symbol_table.hpp`)](#9-symbol-table)
10. [Expression Wrapper (`expression.hpp`)](#10-expression-wrapper)
11. [Parser and Compiler (`parser.hpp`)](#11-parser-and-compiler)
12. [Expression Generator and Optimizations](#12-expression-generator-and-optimizations)
13. [Extras and Utilities (`extras.hpp`)](#13-extras-and-utilities)
14. [Runtime Libraries](#14-runtime-libraries)
15. [Memory Management](#15-memory-management)
16. [Configuration and Preprocessor Switches](#16-configuration-and-preprocessor-switches)
17. [Type System and Template Design](#17-type-system-and-template-design)
18. [Notable Design Patterns](#18-notable-design-patterns)
19. [Quantitative Summary](#19-quantitative-summary)

---

## 1. Executive Summary

ExprTk is a header-only, template-based C++ library for parsing, compiling, and
evaluating mathematical expressions at runtime. It supports scalar arithmetic, vector
operations, string manipulation, control flow (if/else, loops, switch), user-defined
functions, and a rich set of built-in math functions.

The library is designed around four primary user-facing types:

- **`symbol_table<T>`** — binds variables, vectors, strings, constants, and functions
- **`expression<T>`** — holds a compiled expression (an AST) and evaluates it
- **`parser<T>`** — compiles an expression string into an AST using symbol tables
- **`function_compositor<T>`** — defines new functions from expression strings

The runtime flow is:

1. Register symbols (variables, functions, etc.) in a `symbol_table<T>`.
2. Attach symbol tables to an `expression<T>` instance.
3. Use `parser<T>::compile()` to tokenize, parse, and compile the expression string.
4. Call `expression.value()` to evaluate the compiled AST.

All code lives in the `exprtk` namespace. The scalar type `T` is a template parameter
throughout (typically `double` or `float`, but any numeric type is supported via
tag-dispatched traits).

---

## 2. Architecture Overview

The system is organized as a classic compiler pipeline:

```
Source String
     │
     ▼
┌──────────┐
│  Lexer   │  lexer.hpp — tokenization, comment stripping, escape handling
└────┬─────┘
     │ Token stream
     ▼
┌──────────┐
│  Parser  │  parser.hpp — recursive descent + precedence climbing
└────┬─────┘
     │ AST (expression_node<T> tree)
     ▼
┌──────────────┐
│  Expression  │  expression.hpp — owns the AST, evaluates via value()
└──────────────┘
     │
     ▼
   Result (T)
```

**Lexer → Parser → AST → Evaluation** is the primary data flow. The parser resolves
symbols against registered symbol tables and builds an AST of `expression_node<T>`
subtypes. Evaluation is a simple recursive `value()` call down the tree.

### Layer Breakdown

| Layer | Files | Responsibility |
|-------|-------|----------------|
| Infrastructure | `common.hpp`, `types.hpp`, `info.hpp`, `timer.hpp` | Macros, traits, numeric utilities, type definitions |
| Tokenization | `lexer.hpp` | Source → token stream |
| AST Nodes | `nodes_base.hpp`, `nodes_variable.hpp`, `nodes_control.hpp`, `nodes_string.hpp`, `nodes_call.hpp`, `nodes_assign.hpp`, `nodes_compound.hpp` | AST node types and evaluation logic |
| Node Allocation | `node_allocator.hpp` | Factory for AST node construction |
| Operators | `operators.hpp` | Operator structs with `process()` methods |
| Functions | `functions.hpp` | User-extendable function interfaces |
| Symbol Binding | `symbol_table.hpp` | Name → value/function registration |
| Expression | `expression.hpp` | Compiled expression ownership and evaluation |
| Compiler | `parser.hpp` | Parsing, type checking, optimization, AST construction |
| Extras | `extras.hpp` | Function compositor, calculus, variable collection |
| RTL | `rtl_io.hpp`, `rtl_io_file.hpp`, `rtl_vecops.hpp` | Standard library packages |

---

## 3. File Map and Dependencies

### Include Order (from `src/exprtk.hpp`)

The umbrella header includes files in a carefully ordered sequence that respects
forward declarations and dependencies:

```
common.hpp          ← macros, traits, numeric utilities
lexer.hpp           ← tokenization (depends on common.hpp)
types.hpp           ← vector_view, type_store, results_context
nodes_base.hpp      ← base AST node, operator_type enum, destruction
nodes_control.hpp   ← control flow nodes (if, while, for, switch)
nodes_variable.hpp  ← variable, vector, swap nodes
nodes_string.hpp    ← string operation nodes
nodes_assign.hpp    ← assignment, vector arithmetic, special functions
nodes_call.hpp      ← function call dispatch nodes
operators.hpp       ← operator policy structs
nodes_compound.hpp  ← compound/synthesized nodes (vov, cov, etc.)
node_allocator.hpp  ← node factory
functions.hpp       ← function base classes
symbol_table.hpp    ← symbol registration and lookup
expression.hpp      ← compiled expression wrapper
parser.hpp          ← core compiler
extras.hpp          ← compositor, calculus, helpers
timer.hpp           ← cross-platform timer
rtl_io.hpp          ← print/println
rtl_io_file.hpp     ← file I/O
rtl_vecops.hpp      ← vector operations
info.hpp            ← library metadata, macro cleanup
```

### File Sizes

| File | Lines | Purpose |
|------|-------|---------|
| `parser.hpp` | 19,739 | Core compiler — by far the largest |
| `nodes_assign.hpp` | 2,775 | Assignment and vector arithmetic nodes |
| `nodes_compound.hpp` | 2,433 | Synthesized compound expression nodes |
| `lexer.hpp` | 2,374 | Tokenization and lexer helpers |
| `common.hpp` | 2,237 | Core infrastructure |
| `nodes_variable.hpp` | 1,872 | Variable and vector nodes |
| `rtl_vecops.hpp` | 1,853 | Vector operation runtime library |
| `extras.hpp` | 1,764 | Function compositor and utilities |
| `symbol_table.hpp` | 1,755 | Symbol table |
| `nodes_base.hpp` | 1,653 | Base AST types |
| `nodes_control.hpp` | 1,655 | Control flow nodes |
| `nodes_string.hpp` | 1,593 | String operation nodes |
| `operators.hpp` | 1,235 | Operator policy structs |
| `nodes_call.hpp` | 1,030 | Function call nodes |
| `expression.hpp` | 711 | Expression wrapper |
| `rtl_io_file.hpp` | 509 | File I/O runtime |
| `types.hpp` | 507 | Core type definitions |
| `node_allocator.hpp` | 461 | Node factory |
| `functions.hpp` | 322 | Function interfaces |
| `rtl_io.hpp` | 167 | Console I/O runtime |
| `timer.hpp` | 133 | Timer |
| `info.hpp` | 54 | Library info |
| `exprtk.hpp` | 27 | Umbrella header |

---

## 4. Core Infrastructure

### `common.hpp` (2,237 lines)

This is the foundational header providing macros, character classification, numeric
utilities, type traits, and runtime check interfaces.

#### Macros

| Macro | Purpose |
|-------|---------|
| `exprtk_debug(params)` | Debug printf (gated by `exprtk_enable_debugging`) |
| `exprtk_error_location` | Stringified file:line for error messages |
| `exprtk_override` | `override` on C++11+, empty otherwise |
| `exprtk_final` | `final` on C++11+, empty otherwise |
| `exprtk_delete` | `= delete` on C++11+, empty otherwise |
| `exprtk_fallthrough` | Compiler-specific fallthrough annotation |
| `pod_set_zero_value(T)` | `memset`-based zeroing for float/double/long double |

#### Character Classification

Functions in `details` namespace: `is_whitespace`, `is_operator_char`, `is_letter`,
`is_digit`, `is_letter_or_digit`, `is_left_bracket`, `is_right_bracket`, `is_sign`,
`is_invalid`, `is_valid_string_char`, `is_hex_digit`. These are used by the lexer
for tokenization.

#### String Utilities

- `imatch` — case-insensitive comparison (unless `exprtk_disable_caseinsensitivity`)
- `cleanup_escapes` — processes `\n`, `\t`, `\0Xnn`, etc.
- `wc_match` / `wc_imatch` — wildcard pattern matching (`*` and `?`)
- `sequence_match` — pattern matching with difference reporting

#### Reserved Word Tables

Static arrays define language keywords:

- `reserved_words[]` — `abs`, `acos`, `and`, `avg`, `break`, `case`, `ceil`, `clamp`,
  `continue`, `cos`, `default`, `deg2grad`, `deg2rad`, `else`, `equal`, `erfc`,
  `exp`, `false`, `floor`, `for`, `frac`, `grad2deg`, `hypot`, `if`, `ilike`,
  `in`, `inrange`, `like`, `log`, `log10`, `log2`, `logn`, `max`, `min`, `mod`,
  `mul`, `nand`, `ncdf`, `nor`, `not`, `not_equal`, `null`, `or`, `pow`, `rad2deg`,
  `repeat`, `return`, `root`, `round`, `roundn`, `sgn`, `shl`, `shr`, `sin`,
  `sinc`, `sqrt`, `sum`, `swap`, `switch`, `tan`, `true`, `trunc`, `until`,
  `var`, `while`, `xnor`, `xor`, etc.
- `reserved_symbols[]` — `break`, `case`, `continue`, `default`, `false`, `for`, `if`,
  `else`, `ilike`, `in`, `like`, `and`, `nand`, `nor`, `not`, `null`, `or`, `repeat`,
  `return`, `shl`, `shr`, `swap`, `switch`, `true`, `until`, `var`, `while`, `xnor`, `xor`

#### Numeric Constants and Traits

- Constants: `e`, `pi`, `pi_2`, `pi_4`, `pi_180`, `_1_pi`, `_2_pi`, `_180_pi`,
  `log2`, `sqrt2`
- `number_type<T>` — tags: `real_type_tag`, `int_type_tag`, `unknown_type_tag`
- `epsilon_type<T>` — per-type epsilon (1e-12 for double, 1e-7 for float)
- `numeric_info<T>` — `length`, `size`, `bound_length`, `min_exp`, `max_exp`
- `fast_exp<T,N>` — compile-time power specializations for N = 0..10

#### Tag-Dispatched Numeric Operations

The `numeric::details` namespace provides `*_impl()` functions for every math operation,
dispatched on `real_type_tag` vs `int_type_tag`. This handles `abs`, `acos`, `ceil`,
`clamp`, `equal`, `expm1`, `erf`, `floor`, `frac`, `log1p`, `modulus`, `ncdf`,
`nequal`, `pow`, `root`, `round`, `roundn`, `sgn`, `sinc`, `trunc`, `xor_opr`,
`xnor_opr`, etc.

#### String-to-Number Conversion

`string_to_type_converter_impl_ref` is a sophisticated parser that handles:
- Standard decimal: `123.456`
- Scientific notation: `1.23e10`, `1.23E-5`
- Negative numbers: `-42.5`
- Hex prefix: `0xDEAD`
- Special values: `NaN`, `Inf`, `+Inf`, `-Inf`

It uses a `pow10[]` lookup table for fast decimal-to-binary conversion.

#### Runtime Check Interfaces

Four abstract interfaces allow users to hook into runtime behavior:

| Interface | Purpose |
|-----------|---------|
| `loop_runtime_check` | Limits loop iterations; detects timeouts |
| `vector_access_runtime_check` | Bounds-checks vector element access |
| `assert_check` | Custom assertion handler |
| `compilation_check` | Controls compilation continuation on errors |

### `types.hpp` (507 lines)

Defines the core value types used throughout the library:

- **`vector_view<T>`** — a non-owning, rebindable view over contiguous `T` data. Supports
  reference tracking (`data_ref_`, `size_ref_`) so that when the underlying data
  moves, all views are updated. Methods: `rebase()`, `set_ref()`, `remove_ref()`,
  `set_size()`.

- **`type_store<T>`** — a tagged union for generic function parameters. Stores a
  `void*`/`T*` pointer, size, and a `store_type` enum (`e_scalar`, `e_vector`,
  `e_string`). Provides typed views: `scalar_view`, `vector_view`, `string_view`.

- **`results_context<T>`** — container for return statement results. Holds a vector of
  `type_store` entries. Accessed by `return_node` and `return_envelope_node`.

---

## 5. Lexer and Tokenization

### `lexer.hpp` (2,374 lines)

The lexer converts a source string into a flat list of tokens. It is entirely
self-contained within `exprtk::lexer`.

#### Token Types

The `token::token_type` enum defines ~40 token types:

| Category | Tokens |
|----------|--------|
| Values | `e_number`, `e_symbol`, `e_string` |
| Arithmetic | `e_add` (+), `e_sub` (-), `e_mul` (*), `e_div` (/), `e_mod` (%), `e_pow` (^) |
| Comparison | `e_lt` (<), `e_gt` (>), `e_eq` (=), `e_lte` (<=), `e_gte` (>=), `e_ne` (!=) |
| Assignment | `e_assign` (:=), `e_addass` (+=), `e_subass` (-=), `e_mulass` (*=), `e_divass` (/=), `e_modass` (%=) |
| Brackets | `e_lbracket`, `e_rbracket`, `e_lsqrbracket`, `e_rsqrbracket`, `e_lcrlbracket`, `e_rcrlbracket` |
| Shift | `e_shr` (>>), `e_shl` (<<) |
| Other | `e_comma`, `e_colon`, `e_ternary` (?), `e_swap` (<=>) |
| Errors | `e_error`, `e_err_symbol`, `e_err_number`, `e_err_string`, `e_err_sfunc` |
| Control | `e_eof`, `e_none` |

#### Tokenization Algorithm

The main loop in `generator::process()`:

1. Initialize iterators over the source string.
2. Call `scan_token()` repeatedly until end-of-input or error.
3. `scan_token()` dispatches based on the current character:
   - **Whitespace** → skip
   - **Comment** (`#`, `//`, `/* */`) → skip (unless `exprtk_disable_comments`)
   - **Operator char** → `scan_operator()` (handles 1, 2, and 3-character operators)
   - **Letter** → `scan_symbol()` (identifiers, keywords; allows dots in names like `foo.bar`)
   - **Digit or `.`** → `scan_number()`
   - **`$`** → `scan_special_function()` (pattern: `$fNN`, e.g. `$f12`)
   - **`'`** → `scan_string()` (with escape sequences)
   - **`~`** → single-char symbol

#### Number Parsing

`scan_number()` handles:
- Integers: `12345`
- Decimals: `123.456`, `.1234`
- Scientific notation: `1.23e10`, `1.23E-5`, `.5e+3`

It tracks state flags (`dot_found`, `e_found`, `post_e_sign_found`) and rejects
malformed numbers (double dots, missing exponent digits, etc.).

#### String Parsing

`scan_string()` handles single-quoted strings with escape sequences:
- Standard escapes: `\n`, `\t`, `\r`, `\0`, `\a`, `\b`, `\f`, `\v`
- Hex escapes: `\0Xnn` (e.g., `\0X41` for 'A')
- Invalid characters in strings generate `e_err_string` tokens.

#### Post-Tokenization Processing

The lexer provides a pipeline of post-processing helpers, run by `helper_assembly`:

| Phase | Helper Class | Purpose |
|-------|-------------|---------|
| Modifiers | `symbol_replacer` | Replaces symbols (e.g., `true`→`1`) |
| Joiners | `operator_joiner` | Merges spaced operators (`: =`→`:=`, `+ =`→`+=`) |
| Inserters | `commutative_inserter` | Inserts implicit `*` (e.g., `2x`→`2*x`, `)(` →`)*(`) |
| Scanners | `bracket_checker` | Validates bracket matching |
| Scanners | `numeric_checker<T>` | Validates number literals parse as `T` |
| Scanners | `sequence_validator` | Validates 2-token sequences |
| Scanners | `sequence_validator_3tokens` | Validates 3-token sequences |

#### `parser_helper`

Wraps the lexer `generator` for the parser's consumption, providing:
- `token_is(type)` / `token_is(value)` — match-and-advance
- `peek_token_is()` — lookahead without consuming
- `store_token()` / `restore_token()` — backtracking support
- `token_is_arithmetic_opr()`, `token_is_ineq_opr()`, `token_is_loop()` — category checks

---

## 6. AST Node System

The AST is implemented as a tree of `expression_node<T>` subclasses. Each node
overrides `value()` to compute its result. Evaluation is a recursive virtual dispatch
down the tree — there is no separate visitor or interpreter.

### Base Node (`nodes_base.hpp`, 1,653 lines)

#### `expression_node<T>`

The root of the node hierarchy. Key interface:

```cpp
virtual T value() const;                     // Evaluate (returns NaN by default)
virtual expression_node<T>* branch(size_t);  // Child access (returns nullptr)
virtual node_type type() const;              // Node kind (returns e_none)
virtual bool valid() const;                  // Validity check (returns true)
```

Inherits from `node_collector_interface` (for tree traversal) and `node_depth_base`
(for depth computation).

#### `node_type` Enum

Approximately 120 node kinds including: `e_constant`, `e_unary`, `e_binary`,
`e_trinary`, `e_quaternary`, `e_variable`, `e_vector`, `e_vecelem`, `e_stringvar`,
`e_function`, `e_vafunction`, `e_genfunction`, `e_strfunction`, `e_conditional`,
`e_while`, `e_repeat`, `e_for`, `e_switch`, `e_return`, `e_assert`, `e_assign`,
`e_vecvalass`, `e_vecvecass`, `e_vecopvalass`, `e_vecvecarith`, `e_vov`, `e_cov`,
`e_voc`, etc.

#### `operator_type` Enum

Approximately 150 operator kinds organized in groups:
- Arithmetic: `e_add`, `e_sub`, `e_mul`, `e_div`, `e_mod`, `e_pow`
- Comparison: `e_lt`, `e_lte`, `e_eq`, `e_ne`, `e_gte`, `e_gt`
- Logical: `e_and`, `e_or`, `e_xor`, `e_nand`, `e_nor`, `e_xnor`
- Unary: `e_abs`, `e_acos`, `e_asin`, `e_atan`, `e_ceil`, `e_cos`, `e_exp`,
  `e_floor`, `e_log`, `e_neg`, `e_pos`, `e_round`, `e_sgn`, `e_sin`, `e_sqrt`,
  `e_tan`, `e_trunc`, etc.
- Assignment: `e_assign`, `e_addass`, `e_subass`, `e_mulass`, `e_divass`, `e_modass`
- Special functions: `e_sf00`–`e_sf99` (3-arg), `e_sf4ext00`–`e_sf4ext61` (4-arg)

#### Supporting Infrastructure

- **`vec_data_store<T>`** — reference-counted vector data storage with `control_block`
  (ref count, size, data pointer, type flag). Used for temporary vector results.
- **`vector_holder<T>`** — abstraction over vector sources with polymorphic
  implementations: `array_vector_impl`, `sequence_vector_impl`, `vector_view_impl`,
  `resizable_vector_impl`.
- **Loop unrolling** — `loop_unroll::details` provides `batch_size` (16, or 4 with
  `exprtk_disable_superscalar_unroll`), `remainder`, and `upper_bound` for
  vectorized loops.
- **Node predicates** — `is_literal_node()`, `is_variable_node()`, `is_binary_node()`,
  `is_string_node()`, `branch_deletable()`, etc.
- **`node_collection_destructor`** — BFS-traverses the AST, collects all deletable
  nodes, and deletes them in reverse order (leaves first).

### Variable Nodes (`nodes_variable.hpp`, 1,872 lines)

| Node Class | `value()` Returns | Purpose |
|------------|-------------------|---------|
| `variable_node` | `*value_` (direct dereference) | Scalar variable reference |
| `vector_node` | `vds().data()[0]` | Whole-vector reference |
| `vector_size_node` | `T(vector_holder_->size())` | Vector size query |
| `vector_elem_node` | `vector_base_[index_->value()]` | Dynamic vector element access |
| `vector_celem_node` | `vector_base_[index_]` | Compile-time-known vector element |
| `vector_elem_rtc_node` | Same + bounds check | Runtime-checked element access |
| `rebasevector_elem_node` | Uses `vector_holder_->data()` | Rebindable vector element |

**Vector initialization nodes** are specialized by pattern:
- `vector_init_zero_value_node` — fills with zero via `set_zero_value`
- `vector_init_single_constvalue_node` — fills with a compile-time constant
- `vector_init_single_value_node` — fills with a runtime value
- `vector_init_iota_*_node` — fills with arithmetic sequence (base + i*increment),
  with four variants for const/non-const combinations of base and increment

**Swap nodes**: `swap_node` (scalar), `swap_generic_node` (generic via `ivariable`),
`swap_vecvec_node` (element-wise vector swap).

**Key interfaces**:
- `ivariable<T>` — `ref()` accessor for assignable values
- `vector_interface<T>` — `size()`, `base_size()`, `vec()`, `vds()` for vector access
- `range_pack<T>` — range bounds (constant or expression) with `operator()` for resolution

### Control Flow Nodes (`nodes_control.hpp`, 1,655 lines)

| Node | `value()` Behavior |
|------|--------------------|
| `null_node` | Returns NaN |
| `literal_node` | Returns stored constant |
| `unary_node` | `numeric::process(op, child.value())` |
| `binary_node` | `numeric::process(op, left.value(), right.value())` |
| `binary_ext_node<Op>` | `Op::process(left.value(), right.value())` — policy-based |
| `trinary_node` | Dispatches on op: `inrange`, `clamp`, `iclamp` |
| `conditional_node` | `is_true(cond) ? consequent.value() : alternative.value()` |
| `cons_conditional_node` | `is_true(cond) ? consequent.value() : NaN` |
| `while_loop_node` | `while (is_true(cond)) result = body.value()` |
| `repeat_until_loop_node` | `do { result = body.value() } while (is_false(cond))` |
| `for_loop_node` | `init; while (is_true(cond)) { result = body.value(); incr.value() }` |
| `switch_node` | First matching case's consequent |
| `multi_switch_node` | All matching cases; returns last |
| `break_node` | Throws `break_exception<T>` with return value |
| `continue_node` | Throws `continue_exception` |

Each loop type has three additional variants:
- `*_rtc_node` — with `loop_runtime_checker` for iteration limits
- `*_bc_node` — with break/continue exception handling
- `*_bc_rtc_node` — both runtime checking and break/continue

### String Nodes (`nodes_string.hpp`, 1,593 lines)

All string nodes are conditionally compiled (`#ifndef exprtk_disable_string_capabilities`).
String nodes return NaN from `value()` — their string results are accessed through
the `string_base_node` and `range_interface` mixins.

| Node | Purpose |
|------|---------|
| `stringvar_node` | References a string variable |
| `string_range_node` | `str[r0:r1]` substring |
| `const_string_range_node` | Constant string substring |
| `generic_string_range_node` | Computed substring |
| `string_concat_node` | `str0 + str1` |
| `swap_string_node` | Swaps two string variables |
| `swap_genstrings_node` | In-place character swap with loop unrolling |
| `assignment_string_node` | `str := value` (assign or append) |
| `assignment_string_range_node` | `str[r0:r1] := value` |
| `conditional_string_node` | `if cond then str0 else str1` |
| `str_vararg_node` | Variable-argument string expression |

String ranges are represented by `range_pack<T>` which holds constant or expression
bounds and resolves them at evaluation time.

### Function Call Nodes (`nodes_call.hpp`, 1,030 lines)

| Node | Purpose |
|------|---------|
| `function_N_node<T,F,N>` | Fixed-arity (0–20) function call |
| `vararg_function_node` | Variable-arity scalar function |
| `generic_function_node` | Generic function (mixed scalar/vector/string args) |
| `string_function_node` | Generic function returning string |
| `multimode_genfunction_node` | Overloaded generic function |
| `multimode_strfunction_node` | Overloaded string-returning function |
| `return_node` | Return statement (throws `return_exception`) |
| `return_envelope_node` | Catches `return_exception` at expression boundary |

**`function_N_node` dispatch**: Uses `evaluate_branches<T,N>` to unroll branch
evaluation into a `T v[N]` array, then `invoke<T,N>` to call
`function(v[0], v[1], ..., v[N-1])`. Both are template-specialized for N = 1..20.

**`generic_function_node`** categorizes each branch as scalar, vector, or string at
init time, constructs `type_store` entries for each, and calls the user function
with a `parameter_list_t`.

### Assignment Nodes (`nodes_assign.hpp`, 2,775 lines)

Handles all forms of assignment and vector arithmetic.

**Scalar assignments:**
- `assignment_node` — `var = expr`
- `assignment_op_node<Op>` — `var Op= expr` (e.g., `var += expr`)
- `assignment_vec_elem_node` — `vec[i] = expr`
- `assignment_rebasevec_elem_node` — rebindable vector element assignment

**Vector assignments (with loop unrolling):**
- `assignment_vec_node` — `vec = scalar` (fill)
- `assignment_vecvec_node` — `vec0 = vec1` (element-wise copy)
- `assignment_vec_op_node<Op>` — `vec Op= scalar`
- `assignment_vecvec_op_node<Op>` — `vec0 Op= vec1`

**Vector arithmetic (produces temporary vectors):**
- `vec_binop_vecvec_node<Op>` — `vec0 Op vec1`
- `vec_binop_vecval_node<Op>` — `vec Op scalar`
- `vec_binop_valvec_node<Op>` — `scalar Op vec`
- `unary_vector_node<Op>` — `Op(vec)` (element-wise unary)
- `conditional_vector_node` — `if cond then vec0 else vec1`

**Special functions:**
- `sf3_node` / `sf4_node` — 3/4-argument special functions via `SpecialFunction::process()`
- `sf3_var_node` / `sf4_var_node` — optimized versions holding direct variable references

**Short-circuit evaluation:**
- `scand_node` — short-circuit AND (`is_true(left) && is_true(right)`)
- `scor_node` — short-circuit OR

**Assert:**
- `assert_node` — evaluates condition, calls `assert_check::handle_assert()` on failure

### Compound Nodes (`nodes_compound.hpp`, 2,433 lines)

These are synthesized optimization nodes that reduce indirection for common patterns:

| Pattern | Node | `value()` |
|---------|------|-----------|
| var Op var | `vov_node<Op>` | `Op::process(v0_, v1_)` |
| const Op var | `cov_node<Op>` | `Op::process(c_, v_)` |
| var Op const | `voc_node<Op>` | `Op::process(v_, c_)` |
| var Op branch | `vob_node<Op>` | `Op::process(v_, branch.value())` |
| branch Op var | `bov_node<Op>` | `Op::process(branch.value(), v_)` |
| const Op branch | `cob_node<Op>` | `Op::process(c_, branch.value())` |
| branch Op const | `boc_node<Op>` | `Op::process(branch.value(), c_)` |
| unary(var) | `unary_variable_node<Op>` | `Op::process(v_)` |
| unary(var) Op unary(var) | `uvouv_node` | `f_(u0_(v0_), u1_(v1_))` |

**Multi-operand synthesis:**
- `T0oT1<T0,T1,Op>` — 2-operand with generic types
- `T0oT1oT2<T0,T1,T2,Op0,Op1,Mode>` — 3-operand with two operations and two
  evaluation modes (left-to-right vs right-to-left grouping)
- `T0oT1oT2oT3<T0,T1,T2,T3,Op0,Op1,Op2,Mode>` — 4-operand with five evaluation modes

**Special function synthesis:**
- `T0oT1oT2_sf3<SF3Op>` — 3-arg special function with direct variable references
- `T0oT1oT2_sf3ext<SF3Op>` — extended 3-arg
- `T0oT1oT2oT3_sf4<SF4Op>` — 4-arg special function
- `T0oT1oT2oT3_sf4ext<SF4Op>` — extended 4-arg

**Integer power nodes:**
- `ipow_node<PowOp>` — `PowOp::result(v_)` for compile-time integer power
- `bipow_node<PowOp>` — `PowOp::result(branch.value())`
- `ipowinv_node` / `bipowinv_node` — inverse power variants

**String compound nodes:**
- `sos_node` — string Op string
- `str_xrox_node` / `str_xoxr_node` / `str_xroxr_node` — string range comparisons
- `str_sogens_node` — generic string comparison with dynamic ranges
- `sosos_node` — 3-string operations (e.g., `inrange`)

---

## 7. Operators and Numeric Processing

### `operators.hpp` (1,235 lines)

Operators are implemented as small policy structs with static methods. Each operator
struct provides:

- `process(T, T)` — compute the operation
- `assign(T&, T)` — in-place operation (for compound assignment)
- `type()` — corresponding `node_type`
- `operation()` — corresponding `operator_type`

#### Unary Operators

Generated via the `exprtk_define_unary_op(OpName)` macro, which creates a struct with
`process(v)` calling `numeric::OpName(v)`. Covers ~35 unary functions: `abs`, `acos`,
`acosh`, `asin`, `asinh`, `atan`, `atanh`, `ceil`, `cos`, `cosh`, `cot`, `csc`,
`d2g`, `d2r`, `erf`, `erfc`, `exp`, `expm1`, `floor`, `frac`, `g2d`, `log`,
`log10`, `log1p`, `log2`, `ncdf`, `neg`, `notl`, `pos`, `r2d`, `round`, `sec`,
`sgn`, `sin`, `sinc`, `sinh`, `sqrt`, `tan`, `tanh`, `trunc`.

#### Binary Operators

| Struct | Operation | Supports `assign` |
|--------|-----------|--------------------|
| `add_op<T>` | `t1 + t2` | `t1 += t2` |
| `sub_op<T>` | `t1 - t2` | `t1 -= t2` |
| `mul_op<T>` | `t1 * t2` | `t1 *= t2` |
| `div_op<T>` | `t1 / t2` | `t1 /= t2` |
| `mod_op<T>` | `modulus(t1, t2)` | Yes |
| `pow_op<T>` | `pow(t1, t2)` | Yes |

#### Comparison and Logical Operators

Comparison ops (`lt_op`, `lte_op`, `gt_op`, `gte_op`, `eq_op`, `ne_op`) return 1.0
for true, 0.0 for false. They also have string overloads.

Logical ops (`and_op`, `or_op`, `nand_op`, `nor_op`, `xor_op`, `xnor_op`) use
`is_true()` to evaluate truthiness.

String-specific ops: `in_op` (substring search), `like_op` (wildcard match),
`ilike_op` (case-insensitive wildcard match).

#### Variable-Argument Operators

| Struct | Purpose | Unrolled for |
|--------|---------|--------------|
| `vararg_add_op` | Sum | 0–5 args |
| `vararg_mul_op` | Product | 0–5 args |
| `vararg_avg_op` | Average | Uses `vararg_add_op / size` |
| `vararg_min_op` | Minimum | 1–5 args |
| `vararg_max_op` | Maximum | 1–5 args |
| `vararg_mand_op` | Multi-AND | 1–5 args |
| `vararg_mor_op` | Multi-OR | 1–5 args |
| `vararg_multi_op` | Multi-expression (returns last) | 0–8 args |

#### Vector Operators

`vec_add_op`, `vec_mul_op`, `vec_avg_op`, `vec_min_op`, `vec_max_op` — operate on
`vector_interface<T>` with loop unrolling (batch size 4 or 16).

---

## 8. Function Interfaces

### `functions.hpp` (322 lines)

Three base classes for user-defined functions:

#### `ifunction<T>` (fixed arity, 0–20 parameters)

Inherits `function_traits`. The user overrides `operator()(const T&, ...)` for the
desired arity. Default implementations return NaN. The `param_count` member is set at
construction.

#### `ivararg_function<T>` (variable arity)

Override `operator()(const std::vector<T>&)`. Receives all arguments as a vector.

#### `igeneric_function<T>` (mixed types)

Override `operator()(parameter_list_t)` for scalar return, or
`operator()(std::string&, parameter_list_t)` for string return.

Return type enum: `e_rtrn_scalar`, `e_rtrn_string`, `e_rtrn_overload`.

The `parameter_sequence` string describes expected parameter types (e.g., `"TVS"` for
scalar, vector, string). Overloaded functions use `|`-separated sequences.

#### `function_traits`

Non-template base providing metadata:
- `allow_zero_parameters_` — whether 0-arg calls are valid
- `has_side_effects_` — if false, enables constant folding
- `min_num_args_` / `max_num_args_` — arity constraints

Helper functions: `enable_zero_parameters()`, `disable_has_side_effects()`,
`set_min_num_args()`, `set_max_num_args()`.

#### `stringvar_base<T>`

Wrapper for bound string variables with `rebase()` for rebinding.

---

## 9. Symbol Table

### `symbol_table.hpp` (1,755 lines)

The `symbol_table<T>` class provides a registry for all named entities an expression
can reference.

#### Storage Architecture

Internally, `symbol_table` uses a `control_block` with reference counting. The
`control_block` owns an `st_data` struct containing seven `type_store<>` instances:

| Store | Key Type | Value Type | Contents |
|-------|----------|------------|----------|
| `variable_store` | `std::string` | `variable_node<T>*` | Scalar variables and constants |
| `function_store` | `std::string` | `ifunction<T>*` | Fixed-arity functions |
| `vararg_function_store` | `std::string` | `ivararg_function<T>*` | Variable-arity functions |
| `generic_function_store` | `std::string` | `igeneric_function<T>*` | Generic functions (scalar return) |
| `string_function_store` | `std::string` | `igeneric_function<T>*` | Generic functions (string return) |
| `overload_function_store` | `std::string` | `igeneric_function<T>*` | Overloaded generic functions |
| `vector_store` | `std::string` | `vector_holder<T>*` | Vectors |
| `stringvar_store` | `std::string` | `stringvar_node<T>*` | String variables |

Each `type_store` is a `std::map<std::string, std::pair<bool, type_ptr>>` where the
bool indicates whether the entry is constant.

#### Symbol Resolution

Lookup is case-insensitive by default (via `details::ilesscompare`). Names are validated
by `valid_symbol()` which requires: non-empty, starts with letter, contains only
alphanumeric/underscore/dot characters, and is not a reserved word.

#### Registration API

Variables:
- `add_variable(name, T& ref, is_const)` — binds a reference
- `add_constant(name, value)` — creates owned storage in `local_symbol_list_`
- `create_variable(name, value)` — creates owned variable

Vectors (multiple overloads):
- `add_vector(name, T* data, size)` — raw pointer
- `add_vector(name, std::vector<T>&)` — std::vector reference
- `add_vector(name, vector_view<T>&)` — view reference
- `add_vector(name, std::deque<T>&)` — deque reference

Functions:
- `add_function(name, ifunction<T>&)` — fixed-arity
- `add_function(name, ivararg_function<T>&)` — variable-arity
- `add_function(name, igeneric_function<T>&)` — generic (auto-routed by return type)
- `add_function(name, ff00_functor)` … `add_function(name, ff15_functor)` — free
  function pointers (wrapped in `freefuncNN`)

Constants:
- `add_constants()` — registers `pi`, `epsilon`, `infinity`
- `add_pi()`, `add_epsilon()`, `add_infinity()` — individual constants

Packages:
- `add_package(Package&)` — calls `Package::register_package(*this)`

#### Mutability

`symtab_mutability_type`: `e_mutable` (default) or `e_immutable`. Immutable tables
reject variable additions.

---

## 10. Expression Wrapper

### `expression.hpp` (711 lines)

The `expression<T>` class is the user-facing handle for a compiled expression.

#### Ownership Model

Uses a `control_block` with reference counting. The control block owns:
- `expr` — root `expression_node<T>*` (the AST)
- `local_data_list` — heap-allocated objects (expressions, vector holders, raw data,
  strings) registered during compilation
- `results` — optional `results_context<T>` for return statements
- `return_invoked` — flag set when a `return` was executed

Copy/assign shares the control block (increments ref count). Destruction decrements;
when the count reaches zero, the entire AST and local data are deleted.

#### Evaluation

```cpp
T expression<T>::value()  →  control_block_->expr->value()
```

The root node's `value()` recursively evaluates the entire tree. This is the only
evaluation mechanism — there is no bytecode interpreter.

#### Symbol Table Attachment

An expression holds a vector of `symbol_table<T>` references (up to ~64). The parser
searches all attached symbol tables when resolving symbols.

#### Public API

- `value()` / `operator()()` — evaluate
- `operator T()` — implicit conversion to result
- `operator bool()` — truthiness of result
- `register_symbol_table(symtab)` — attach symbol table
- `get_symbol_table(index)` — retrieve attached table
- `results()` — access return results
- `return_invoked()` — check if return was executed
- `release()` — explicitly destroy the expression

#### Error Types (`parser_error` namespace)

- `error_mode` enum: `e_unknown`, `e_syntax`, `e_token`, `e_numeric`, `e_symtab`,
  `e_lexer`, `e_synthesis`, `e_helper`, `e_parser`
- `type` struct: `token`, `mode`, `diagnostic`, `src_location`, `error_line`,
  `line_no`, `column_no`
- `make_error()`, `to_str()`, `update_error()`, `dump_error()` — error construction
  and reporting

---

## 11. Parser and Compiler

### `parser.hpp` (19,739 lines)

The parser is the largest and most complex component. It inherits from
`lexer::parser_helper` and performs recursive descent parsing with operator precedence
climbing.

#### Compilation Flow

`parser<T>::compile(expression_string, expression)` proceeds through:

1. **Reset** — clear all internal state (errors, scope, synthesis state)
2. **Validate** — check settings are valid
3. **Lex** — tokenize the expression string via `init()`
4. **Post-lex processing** — run the helper assembly pipeline:
   - `commutative_inserter` — insert implicit `*`
   - `operator_joiner` — merge multi-char operators
   - `symbol_replacer` — replace `true`/`false`/etc.
   - `bracket_checker` — validate bracket matching
   - `numeric_checker` — validate number literals
   - `sequence_validator` — validate token sequences
5. **Halt check** — optional `compilation_check` callback
6. **Parse** — `parse_corpus()` builds the AST
7. **Wrap** — if return statements exist, wrap in `return_envelope_node`
8. **Register** — attach the AST and local data to the expression

#### Parsing Algorithm

The core is a precedence-climbing parser with 15 precedence levels:

| Level | Operators |
|-------|-----------|
| 0 | Assignment: `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `<=>` |
| 1–2 | Logical OR: `or`, `\|`, `nor`, `xor`, `xnor` |
| 3–4 | Logical AND: `and`, `&`, `nand`, `in`, `like`, `ilike` |
| 5–6 | Comparison: `<`, `<=`, `==`, `!=`, `>=`, `>` |
| 7–8 | Additive: `+`, `-` |
| 10–11 | Multiplicative: `/`, `*`, `%` |
| 12 | Power: `^` (right-associative) |
| 13 | Unary `+` |

`parse_expression(precedence)` loops: check if the current operator's left precedence
is above the threshold, consume it, parse the right operand with the operator's right
precedence, and synthesize a binary node. This naturally handles associativity and
precedence.

`parse_branch(precedence)` handles atoms and prefix operators:
- Unary `-` and `+`
- Parenthesized sub-expressions
- Literal numbers
- Symbols (variables, functions, keywords)
- Ternary `?`
- String literals
- Control structures (`if`, `while`, `for`, `switch`, `return`, etc.)

#### Symbol Resolution

`parse_symtab_symbol()` searches for symbols in this order:

1. Symbol table variable → `variable_node` or `literal_node` (for constants)
2. Local scope (`scope_element_manager`) → local variable, vector, or string
3. String variable → `parse_string()`
4. Vector → `parse_vector()`
5. Unknown symbol resolver (if enabled) → user callback
6. Functions (fixed-arity, vararg, generic, string, overloaded)

#### Scope Management

`scope_element_manager` tracks local variables defined with `var`, `var[]`, and string
definitions. `scope_handler` is an RAII guard that increments scope depth on
construction and deactivates scope elements on destruction.

#### Statement Parsing

The parser handles a rich set of statements:

| Statement | Parsing Method |
|-----------|----------------|
| `if`/`else` | `parse_conditional_statement()` — two forms: `if(cond,then,else)` and `if (cond) { ... } else { ... }` |
| `?:` ternary | `parse_ternary_conditional_statement()` |
| `while` | `parse_while_loop()` |
| `repeat`/`until` | `parse_repeat_until_loop()` |
| `for` | `parse_for_loop()` — init, condition, increment, body |
| `switch`/`case` | `parse_switch_statement()` |
| `return` | `parse_return_statement()` |
| `var x := ...` | `parse_define_var_statement()` |
| `var v[n] := {...}` | `parse_define_vector_statement()` |
| `var s := '...'` | `parse_define_string_statement()` |
| `swap(a,b)` | `parse_swap_statement()` |
| `assert(cond,msg,id)` | `parse_assert_statement()` |
| `break` / `continue` | `parse_break_statement()` / `parse_continue_statement()` |

#### Key Inner Classes

**`settings_store`** — configurable settings with fluent API:
- Enable/disable: base functions, control structures, logic ops, arithmetic ops,
  assignment ops, inequality ops
- Feature flags: replacer, joiner, numeric check, bracket check, sequence check,
  commutative check, strength reduction, variable definition, zero return
- Limits: `max_stack_depth` (default 400), `max_node_depth` (default 10000)

**`expression_generator`** — the AST node factory. Holds operator dispatch maps
(`unary_op_map_`, `binary_op_map_`, `sf3_map_`, `sf4_map_`) and a `synthesize_map_`
for strength reduction. Key methods: `operator()` for unary/binary/literal nodes,
`conditional()`, `while_loop()`, `for_loop()`, `switch_statement()`, `return_node()`,
`vector_element()`, `assert_call()`.

**`type_checker`** — validates generic function call signatures. Parses prototype
strings like `"T:TVS|T:TT"` (return type : param types, `|`-separated overloads)
and matches against actual parameter types at the call site.

**`state_t`** — holds the current operator state during precedence climbing (left
and right precedence, operation type, token).

**`stack_limit_handler`** — RAII guard that increments/decrements a stack depth
counter, preventing unbounded recursion.

---

## 12. Expression Generator and Optimizations

The `expression_generator` (inside `parser.hpp`) is responsible for building optimized
AST nodes. It applies several optimization strategies:

### Constant Folding

When all children of an operation are constants (`is_constant_foldable()`), the
generator evaluates the operation at compile time and replaces the subtree with a
`literal_node`. This applies to:
- Binary operations
- Unary operations
- Switch/multi-switch branches
- Vararg functions (sum, mul, avg, min, max, etc.)
- Special functions (sf3, sf4)
- Conditional expressions where the condition is constant

### Strength Reduction (Synthesis)

When `strength_reduction_enabled()` is true, the generator replaces generic binary
nodes with specialized compound nodes:

- `var + var` → `vov_node<add_op>` (avoids virtual dispatch for children)
- `var * const` → `voc_node<mul_op>` (avoids evaluating a literal node)
- `const + branch` → `cob_node<add_op>`
- `var + var + var` → synthesized ternary nodes
- `var * var + var * var` → synthesized quaternary nodes

The `synthesize_map_` maps `(left_node_type, right_node_type)` string keys to
synthesis functions. Over 60 synthesis patterns are registered.

### Special Function Recognition

The parser recognizes patterns of 3 and 4 variable operations that match known
special functions (`sf00`–`sf99` for 3-arg, `sf4ext00`–`sf4ext61` for 4-arg).
These are compiled into `sf3_node` / `sf4_node` with direct variable references,
avoiding tree traversal entirely.

Examples of recognized patterns:
- `(x + y) / z` → `sf00`
- `(x + y) * z` → `sf01`
- `(x - y) / z` → `sf02`
- `(x * y) + z` → `sf06`
- `ax + b` → `sf48`

### Unary Negation Simplification

`simplify_unary_negation_branch()` handles chains of negation:
- `--x` → `x` (double negation removal)
- `-const` → `literal_node(-const)`

### Integer Power Optimization

Expressions like `x^5` are compiled into `ipow_node` which uses the template
metaprogramming `fast_exp<T,N>` for compile-time unrolled multiplication chains
instead of calling `std::pow()`.

### Multi-Expression Simplification

`simplify()` in `parse_corpus()`:
- Drops constant nodes and side-effect-free nodes
- Stops at return/break/continue statements
- Collapses single-expression sequences
- Wraps remaining sequences in `vararg_multi_op`

---

## 13. Extras and Utilities

### `extras.hpp` (1,764 lines)

#### Variable and Function Collection

`collect_variables()` and `collect_functions()` parse an expression and extract the
names of all referenced variables or functions. Uses `collector_helper` with a
`resolve_as_vector` unknown symbol resolver that temporarily creates vector symbols
for unknowns.

#### Numerical Calculus

| Function | Algorithm |
|----------|-----------|
| `integrate(expr, var, a, b, n)` | Simpson's rule with `n` intervals |
| `derivative(expr, var, h)` | 4-point central difference |
| `second_derivative(expr, var, h)` | 5-point central difference |
| `third_derivative(expr, var, h)` | 5-point central difference |

Each accepts either a `T& var` reference or a variable name string.

#### One-Shot Evaluation

`compute()` functions compile and evaluate an expression in one call:
- `compute(expr_str, result)` — no variables
- `compute(expr_str, x, result)` — single variable `x`
- `compute(expr_str, x, y, result)` — two variables
- `compute(expr_str, x, y, z, result)` — three variables

#### Polynomial Evaluation

`polynomial<T,N>` evaluates polynomials via Horner's method for degrees 1–12:
`p(x) = c_N * x^N + c_{N-1} * x^{N-1} + ... + c_1 * x + c_0`

Registered as an `ifunction<T>` with arity N+1 (x plus N+1 coefficients).

#### Function Compositor

`function_compositor<T>` allows defining new functions from expression strings:

```cpp
compositor.add(function()
    .name("f")
    .var("x").var("y")
    .expression("x^2 + y^2"));
```

Internally:
1. The expression is wrapped: `var v0{}; var v1{}; ~{ user_expression };`
2. Compiled using the compositor's internal parser and symbol table
3. Wrapped in a `func_Nparam` (0–6 args) that copies arguments into local variables
   before evaluation

Supports recursion via `pre()`/`post()` stack management in `base_func`. When a
composited function calls itself, arguments and locals are saved/restored on an
internal stack.

---

## 14. Runtime Libraries

### Console I/O (`rtl_io.hpp`, 167 lines)

Guarded by `#ifndef exprtk_disable_rtl_io`.

| Function | Behavior |
|----------|----------|
| `print(...)` | Prints scalars, vectors, strings (space-separated) |
| `println(...)` | Same, with trailing newline |

Both extend `igeneric_function<T>` and accept any mix of parameter types. Scalar
format is configurable (default `"%10.5f"`). Registered via `rtl::io::package<T>`.

### File I/O (`rtl_io_file.hpp`, 509 lines)

Guarded by `#ifndef exprtk_disable_rtl_io_file`.

| Function | Signatures | Description |
|----------|-----------|-------------|
| `open` | `S`, `SS` | Open file (read/write/readwrite) |
| `close` | `T` | Close file handle |
| `write` | `TS`, `TST`, `TV`, `TVT` | Write string/vector to file |
| `read` | `TS`, `TST`, `TV`, `TVT` | Read into string/vector buffer |
| `getline` | `T` | Read line (returns string) |
| `eof` | `T` | Check end-of-file |

File handles are `file_descriptor` pointers cast to `T`. The underlying I/O uses
`std::ifstream`, `std::ofstream`, or `std::fstream` in binary mode.

### Vector Operations (`rtl_vecops.hpp`, 1,853 lines)

Guarded by `#ifndef exprtk_disable_rtl_vecops`.

A comprehensive BLAS-like vector operation library:

| Category | Functions |
|----------|-----------|
| Predicates | `all_true`, `all_false`, `any_true`, `any_false`, `count` |
| Rearrangement | `copy`, `reverse`, `rol` (rotate left), `ror` (rotate right), `shift_left`, `shift_right`, `sort`, `nthelement` |
| Initialization | `assign`, `iota` |
| Aggregation | `sumk` (Kahan summation) |
| Linear algebra | `axpy`, `axpby`, `axpyz`, `axpbyz`, `axpbsy`, `axpbsyz`, `axpbz` |
| Differencing | `diff` |
| Dot product | `dot`, `dotk` (Kahan) |
| Thresholding | `threshold_below`, `threshold_above` |
| Element-wise | `min_elemwise`, `max_elemwise`, `select` (ternary) |

Most functions support optional range parameters `[r0, r1)` for operating on
sub-vectors. All extend `igeneric_function<T>` with overloaded parameter signatures.

---

## 15. Memory Management

### Ownership Model

The library uses a tree-based ownership model with reference counting at the
expression level:

1. **Expression owns the AST** — `expression<T>::control_block` owns the root
   `expression_node<T>*`. When the last copy of an expression is destroyed, the
   entire node tree is deleted.

2. **Symbol table owns its nodes** — `symbol_table<T>::control_block` owns
   `variable_node`, `vector_holder`, `stringvar_node` instances. These are not
   deleted by the expression — `branch_deletable()` returns false for variable and
   string nodes.

3. **Local data registration** — during compilation, the parser registers locally
   created objects (temporary variables, vectors, strings) with the expression via
   `register_local_var()`. These are destroyed with the expression.

### Node Deletion

`node_collection_destructor<Node>` performs safe deletion:
1. BFS traversal starting from the root
2. Calls `collect_nodes()` on each node to gather its children
3. Filters to only `branch_deletable()` nodes
4. Deletes in reverse order (leaves first, root last)

This avoids the stack overflow risk of recursive deletion in deep trees.

### Reference Counting

Both `symbol_table` and `expression` use `control_block` structs with manual reference
counting (no `shared_ptr`). Copy/assign increments the count; destruction decrements.
When count reaches zero, the data is deallocated.

### Vector View References

`vector_view<T>` maintains reference lists (`data_ref_`, `size_ref_`) so that when
the underlying data is reallocated, all views can be updated via `rebase()`. This is
critical for `std::vector` and `std::deque` bindings where the data pointer can change.

---

## 16. Configuration and Preprocessor Switches

ExprTk provides extensive compile-time configuration via preprocessor defines:

| Define | Effect |
|--------|--------|
| `exprtk_disable_string_capabilities` | Removes all string support |
| `exprtk_disable_comments` | Disables `#`, `//`, `/* */` comments |
| `exprtk_disable_break_continue` | Removes break/continue from loops |
| `exprtk_disable_return_statement` | Removes return statement support |
| `exprtk_disable_enhanced_features` | Disables enhanced features |
| `exprtk_disable_superscalar_unroll` | Reduces loop unroll batch from 16 to 4 |
| `exprtk_disable_rtl_io` | Disables console I/O runtime |
| `exprtk_disable_rtl_io_file` | Disables file I/O runtime |
| `exprtk_disable_rtl_vecops` | Disables vector operations runtime |
| `exprtk_disable_caseinsensitivity` | Makes symbol lookup case-sensitive |
| `exprtk_enable_debugging` | Enables debug output |

Runtime parser settings provide additional configuration:
- Enable/disable specific base functions (e.g., disable `sin` but keep `cos`)
- Enable/disable control structures (e.g., disable `for` loops)
- Enable/disable operator categories
- Set stack depth and node depth limits
- Enable/disable local variable definitions
- Enable/disable strength reduction

---

## 17. Type System and Template Design

### Template Parameter `T`

The entire library is templated on scalar type `T`. While typically `double` or
`float`, any type that satisfies the numeric interface can be used.

Type dispatch uses tag-based specialization:
- `number_type<T>::type` → `real_type_tag` or `int_type_tag`
- `epsilon_type<T>::value()` → per-type epsilon
- `numeric_info<T>` → bounds, sizes

Each math operation has dual implementations:
- `*_impl(v, real_type_tag)` — uses standard math library (`std::sin`, etc.)
- `*_impl(v, int_type_tag)` — integer-appropriate implementation

### `functor_t<T>`

Centralizes function pointer types:
- `qfunc_t` — quaternary: `T (*)(T, T, T, T)`
- `tfunc_t` — ternary: `T (*)(T, T, T)`
- `bfunc_t` — binary: `T (*)(T, T)`
- `ufunc_t` — unary: `T (*)(T)`

### Compile-Time Metaprogramming

- `fast_exp<T,N>` — template recursion for integer powers
- `evaluate_branches<T,N>` — unrolls branch evaluation
- `invoke<T,N>` — unrolls function invocation
- `T0oT1oT2<..., ProcessMode>` — mode-dispatched evaluation
- `param_to_str<IsConst>` — type-to-character mapping for synthesis

---

## 18. Notable Design Patterns

### Policy-Based Design

Operators and special functions are implemented as policy structs passed as template
parameters. This enables the compiler to inline operations and eliminate virtual
dispatch for synthesized nodes. For example, `vec_binop_vecvec_node<add_op<T>>`
statically binds the addition operation.

### Two-Phase Construction

Many nodes use two-phase construction: the constructor sets up pointers and metadata,
then `init()` performs dynamic setup (like categorizing generic function parameters).
This allows the parser to construct nodes incrementally.

### Branch Pair Pattern

Child node ownership is tracked via `branch_t = std::pair<expression_node<T>*, bool>`.
The boolean indicates whether the node should be deleted with its parent. Variable
and string nodes (which reference symbol table data) are never deleted by the tree.

### RAII Scope Management

`scope_handler` uses RAII to manage scope depth: construction enters a new scope,
destruction deactivates local symbols and exits. This ensures scoping is correct even
when parsing fails midway.

### Exception-Based Control Flow

Break and continue are implemented via C++ exceptions (`break_exception`,
`continue_exception`, `return_exception`). Loop nodes catch these to implement the
control flow. This is a pragmatic choice that avoids threading control flow flags
through the evaluation tree.

### Synthesis Map Pattern

The expression generator maintains a map from `(left_type, right_type)` string keys
to synthesis functions. When building a binary node, it looks up whether a specialized
synthesis exists for the operand types. This keeps the optimization logic decoupled
from the parsing logic.

---

## 19. Quantitative Summary

| Metric | Count |
|--------|-------|
| Total source lines | ~46,859 |
| Source files | 23 |
| Largest file | `parser.hpp` (19,739 lines, 42% of codebase) |
| AST node types | ~120 (`node_type` enum) |
| Operator types | ~150 (`operator_type` enum) |
| Unary math functions | ~35 |
| Binary operators | 6 arithmetic + 6 comparison + 6 logical + 3 string |
| Special functions | 100 three-arg + 62 four-arg |
| User function interfaces | 3 (`ifunction`, `ivararg_function`, `igeneric_function`) |
| Max function arity | 20 parameters |
| Precedence levels | 15 |
| Synthesis patterns | 60+ |
| RTL vector operations | 30+ functions |
| Preprocessor switches | 11+ |
| Namespaces | `exprtk`, `exprtk::details`, `exprtk::lexer`, `exprtk::parser_error`, `exprtk::rtl::*` |
