# exprtk

A C++ Mathematical Expression Toolkit Library based on https://github.com/ArashPartow/exprtk.

## Major Changes

1. Split the original single header into modular files for easier iteration, with a merge script to generate a single hpp output
2. Added AST node arena allocator to improve memory efficiency

