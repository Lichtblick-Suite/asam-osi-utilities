# ASAM OSI Utilities

![Build](https://github.com/Lichtblick-Suite/asam-osi-utilities/actions/workflows/ubuntu.yml/badge.svg)

A utility library that provides essential tools, helper functions, and implementation best practices for working with the [ASAM Open Simulation Interface (OSI)](https://opensimulationinterface.github.io/osi-antora-generator/asamosi/latest/specification/index.html) standard. This library streamlines OSI data handling and makes integration into applications and simulations easier.

> **_NOTE:_**   The main branch actively tracks and implements the latest OSI specification developments, including promising proposed changes. This project does not feature a stable release targeting a specific OSI version yet!

## Features

- Cross-platform C++ library and Python APIs
  - > **_NOTE:_**  Python API is planned but not yet implemented.
  - > **_NOTE:_**  Windows should be supported but not yet tested, contributions welcome!
- Easily integrate dealing with OSI data and trace files into your own projects
- More OSI utility features planned, feature requests and pull requests welcome!

## Build Instructions

### Install required dependencies

- Compiler (e.g. GCC, Clang, MSVC)
- [CMake](https://cmake.org/download/)
- [zstd](https://github.com/facebook/zstd)
- [lz4](https://github.com/lz4/lz4)
- [Protobuf](https://github.com/protocolbuffers/protobuf)

**Note:** On Debian/Ubuntu, you can install the required dependencies using the following commands:

```bash
sudo apt install build-essential cmake libzstd-dev liblz4-dev protobuf-compiler doxygen
```

### Build

Clone the repository incl. submodules

```bash
git clone --recurse-submodules https://github.com/Lichtblick-Suite/asam-osi-utilities.git
```

Create build directory and configure using CMake

```bash
mkdir build && cd build
cmake ..
```

Build using CMake

```bash
cmake --build . -j4
```

## Examples

Check out the examples in the [examples](examples) folder for usage examples.
Further information can be found in the [examples/README.md](examples/README.md).

## Contributing

We welcome contributions!
This library aims to grow alongside ASAM OSI to provide the community with helpful tools and utilities.
If you have ideas for new features or improvements, you are encouraged to open issues and submit pull requests.
