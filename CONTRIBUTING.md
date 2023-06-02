# Code Style

This project is using `snake_case` for identifiers and a C++ `std` library style API design.

Non-public member variables are prefixed with `m_`, setters names are prefixed with `set` and getters do not have any prefix.

    group_t m_group;

    group_t group() const;
    void set_group(group_t);

There is a `clang-format` provided, use this before committing / creating PRs.

# pre-commit Hooks

This project provides a [pre-commit](https://pre-commit.com/index.html) configuration file that fixes common file issues and runs [clang-format](https://github.com/ssciwr/clang-format-wheel) on changed files.

Install `pre-commit` using Python `pip` or the packet manager of your choice:

    # using pip
    pip install pre-commit

    # homebrew on macOS
    brew install pre-commit

    # apt on Ubuntu
    apt install pre-commit

Additionally you have to install the clang-format wheel:

    pip install clang-format

Install the pre-commit hooks in the local checkout:

    cd path/to/ni-midi2
    pre-commit install

# Unit Tests

The unit test framework used is [GoogleTest](https://github.com/google/googletest).

When making changes ensure that all existing tests keep working.

Provide new tests for new functionality and for validation of bug fixes.

# Test Coverage

You can enable code coverage by configuring with `CODE_COVERAGE=ON` or `CMAKE_BUILD_TYPE=Coverage`. Supported code coverage toolchains are `lcov/gcovr` and `llvm`.

Run the `ni-midi2-tests` executable and generate coverage reports:

    # llvm toolchain
    cd <path/to/build-dir>
    export LLVM_PROFILE_FILE=ni-midi2.profraw
    ./ni-midi2-tests
    llvm-profdata merge -sparse ni-midi2.profraw -o ni-midi2.profdata
    llvm-cov show -format=html -instr-profile=ni-midi2.profdata -object ni-midi2-tests -output-dir=ni-midi2-coverage-html -ignore-filename-regex='tests'

    # lcov/gcovr toolchain
    #!/bin/sh
    cd <path/to/build-dir>
    ./ni-midi2-tests
    lcov --capture --directory . --output-file coverage.info
    genhtml coverage.info --output-directory ni-midi2-coverage-html

When you add unit tests for new functionality ensure that your tests cover all new source code.
