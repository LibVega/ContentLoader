# ContentLoader

This repo contains the source for the native library used by [Vega](https://github.com/VegaLib/Vega) to load certain kinds of content files at runtime.

The library itself centralizes and generalizes a variety of open source and public domain file loading solutions. Nearly all new code added to the library only works to tie together the various codes and make the general functionality available through the ABI.

The end product is a shared library that is embedded into the Vega library, and extracted and loaded at runtime for use.

## Build

Pre-compiled versions of the library are hosted in the `Dependencies/Native` folder in the Vega project. This repo serves only as the source and build files for the library. After changes are made, the library should be rebuilt and the binaries updated within the Vega repo.

To build:

1. Run the `generate.bat` (Windows) or `generate.sh` (not Windows) script.
2. Find the build files generated in the `build` directory.
3. Open in IDE or build from command line. The generated binaries will be in `build/bin`.

[Premake](https://premake.github.io/) is used as the build generator. Their binaries are rehosted under the original license.

## License

This codebase (unlike Vega) is licensed under the MIT license. Whereas Vega is a large body of independent work, and is thus licensed under the (slightly) more restrictive Ms-PL, this work is mostly built from derived works that are either MIT licensed or public domain. This maintains the spirit of those licenses.

The full text of the MIT license can be found in the LICENSE file in this repository.

## Components

This library is made up of a wide set of other code solutions, which are listed here:


