# Matrix Calculator

A desktop matrix calculator built with C++17 and Qt 6, featuring typed matrix input, matrix operations, manual row transformations, step history, and JSON-based persistence.

> Status: v0.1 alpha — under active development.

## Screenshots

### Matrix Input

![Matrix input page](docs/images/matrix-calculator-preview-input-page.png)

### Matrix Calculation

![Matrix calculation page](docs/images/matrix-calculator-preview-calculate-page.png)

## Features

- Create and edit matrices using `int`, `double`, or exact `Rational` values.
- Store, search, and select matrices through an in-memory repository.
- Perform same-type matrix addition, subtraction, and multiplication.
- Apply manual elementary row operations: row swapping, row scaling, and row replacement.
- Review the matrix state before and after each manual row-operation step.
- Display matrices through a custom Qt Model/View widget, including stacked fraction rendering.
- Save and load matrix repositories using versioned JSON serialization.

## Tech Stack

- C++17
- Qt 6.5 (`Core` and `Widgets`)
- CMake 3.19+
- Qt Model/View architecture
- JSON serialization and local file persistence
- Git and GitHub

## Project Structure

```text
archive/      Repository, JSON serialization, and file persistence
calculation/  Calculation-step data structures
core/         Generic Matrix<T>, matrix operations, and Rational arithmetic
pages/        Matrix input and calculation pages
widgets/      Reusable matrix Model/View widgets and delegates
```

## Build and Run

### Requirements

- A C++17-compatible compiler
- Qt 6.5 or later with the `Core` and `Widgets` modules
- CMake 3.19 or later

### Build

```bash
git clone https://github.com/yueningli9529-sudo/matrix-calculator.git
cd matrix-calculator
cmake -S . -B build
cmake --build build
```

If CMake cannot locate Qt, provide the Qt installation prefix:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/platform
cmake --build build
```

Run the generated `MatrixCalculator` executable from the build directory. On macOS, open the generated application bundle:

```bash
open build/MatrixCalculator.app
```

## Current Limitations

- Binary operations currently require both matrices to use the same scalar type.
- Row transformations are entered manually; automatic REF and RREF workflows are not yet available.
- Archive loading and saving are explicitly initiated by the user.
- Automated test coverage and cross-platform build verification are still being expanded.
- The user interface is functional but remains under active visual and interaction refinement.

## Roadmap

- Add automated REF and RREF calculations.
- Strengthen validation and user-facing error reporting.
- Expand unit tests for matrix arithmetic, rational arithmetic, row operations, and serialization.
- Add continuous integration for clean builds and automated tests.
- Improve step management and calculation-history workflows.
- Package and verify releases for supported desktop platforms.
