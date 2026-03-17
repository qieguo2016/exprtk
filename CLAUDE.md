# CLAUDE.md

## Development commands

- Build all examples and test binaries: `make`
- Build one target from `example/` or `test/`: `make <target-name>`
  - Example: `make exprtk_test`
  - Example: `make exprtk_simple_example_01`
- Run the main test binary after building it: `./bin/exprtk_test`
- Run the benchmark binary: `make exprtk_benchmark && ./bin/exprtk_benchmark`
- Strip built binaries: `make strip_bin`
- Run valgrind over all existing built binaries: `make valgrind`
- Build benchmark with PGO flow: `make pgo`
- Regenerate the merged single-header distribution after changing files in `src/`: `make build`
- Clean generated artifacts: `make clean`

## Repository structure

├── Makefile       # Build Command
├── README.md      # For human, NOT FOR AI AGENT
├── example        # Small example programs that exercise features.
├── exprtk.hpp     # Auto generated single-header hpp with `make build`. NEVER EDIT.
├── scripts
│   └── merge_hpp.py    # Scripts of generates the single-header `exprtk.hpp`
├── src
│   ├── common.hpp
│   ├── expression.hpp      # Compiled-expression wrapper and evaluation entrypoints.
│   ├── exprtk.hpp          # Umbrella header entry
│   ├── extras.hpp
│   ├── functions.hpp       # Callable-function interfaces and function traits.
│   ├── info.hpp
│   ├── lexer.hpp           # Tokenization and parser helper utilities.
│   ├── node_allocator.hpp  # AST node allocation support.
│   ├── nodes_*.hpp         # AST node implementations grouped by concern.
│   ├── operators.hpp       # Pperator metadata and dispatch helpers.
│   ├── parser.hpp          # Core compiler from tokens to AST
│   ├── rtl_io.hpp
│   ├── rtl_io_file.hpp
│   ├── rtl_vecops.hpp
│   ├── symbol_table.hpp    # Binding layer for variables, vectors, strings, constants, functions.
│   ├── timer.hpp
│   └── types.hpp
└── test
    ├── exprtk_benchmark.cpp            # Benchmark
    ├── exprtk_functional_ext_test.txt  # Test cases
    ├── exprtk_functional_test.txt      # Test cases
    └── exprtk_test.cpp                 # Standalone C++ test script


## Architecture overview

ExprTk is a header-only C++ Mathematical expression engine. 

The main runtime flow is:

1. Register variables, vectors, strings, and callable functions in `exprtk::symbol_table<T>`.
2. Attach one or more symbol tables to an `exprtk::expression<T>` instance.
3. Use `exprtk::parser<T>` to tokenize and compile an expression string into an AST owned by the expression object.
4. Evaluate the compiled expression through `expression.value()` / `operator()`, which walks the compiled node tree.

## Working conventions for this repo

- Make changes in `src/` first. If public distribution output must stay in sync, regenerate top-level `exprtk.hpp` afterwards with `make build`.
- When changing parser or evaluation behavior, validate with `make exprtk_test && ./bin/exprtk_test`.
- When changing public API or examples-facing behavior, also build at least one relevant target from `example/` to confirm include-path and integration behavior still work.
