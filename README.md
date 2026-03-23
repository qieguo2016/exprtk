# exprtk

A C++ Mathematical Expression Toolkit Library based on https://github.com/ArashPartow/exprtk.

## Major Changes

1. Split the original single header into modular files for easier iteration, with a merge script to generate a single hpp output
2. Added AST node arena allocator to improve memory efficiency

## Development Commands

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
