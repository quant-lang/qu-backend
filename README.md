# Qu-backend

Native code generation backend for the [Quant](https://github.com/quant-lang/quant) compiler.

The backend takes QuIR, selects instructions, generates machine code and writes object files. No external assembler is needed.

> Note: the backend is still in development together with Quant.

## Pipeline

```text
QuIR module -> instruction selection -> machine code -> ELF / PE object
```

## Features

- QuIR definitions in `include/qu-ir/`.
- x86-64 and AArch64 instruction selectors.
- ELF64 relocatable objects.
- PE32+ executables for Windows, including an import table.
- Custom OS ABIs implemented through `Target` classes.
- Supported targets: `x86_64-linux`, `aarch64-linux`, `x86_64-windows`, `aarch64-zeropoint`.

## Layout

```text
include/
  backend/    Backend API, targets and object writers
  qu-ir/      QuIR definitions
src/
  backend/    Backend implementation
  qu-ir/      QuIR utilities
```

## Build

Requirements:
- CMake 3.20+
- C++20 compiler

```bash
git clone https://github.com/quant-lang/qu-backend.git
cd qu-backend

cmake -B build
cmake --build build
```

Select backends at configure time:

```bash
cmake -B build -DQUANT_BACKENDS="x86_64-linux;aarch64-linux"
```

Set the default target if needed:

```bash
cmake -B build \
  -DQUANT_BACKENDS="aarch64-zeropoint" \
  -DQUANT_DEFAULT_TARGET="aarch64-zeropoint"
```

Install the headers, static library and CMake package:

```bash
cmake --install build --prefix /usr/local
```

## Usage

The backend can be included in the compiler through a sibling checkout or CMake `FetchContent`:

```cmake
include(FetchContent)

FetchContent_Declare(quant-backend
    GIT_REPOSITORY https://github.com/quant-lang/qu-backend.git
    GIT_TAG        main
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(quant-backend)

target_link_libraries(your-target PUBLIC quant-backend) // Or PRIVATE
```

Include the API and QuIR definitions:

```cpp
#include "backend/native_backend.h"
#include "qu-ir/qu-ir.h"

quant::codegen::NativeBackend backend;
std::vector<std::uint8_t> object = backend.generate(program, /*emit_start=*/true, target);
```

## License

[GPL-3.0](https://www.gnu.org/licenses/gpl-3.0.html)
