# ContentLoader

This repo contains the source for the native library used by [Vega](https://github.com/VegaLib/Vega) to load certain kinds of content files at runtime.

The library itself centralizes and generalizes a variety of open source and public domain file loading solutions. Nearly all new code added to the library only works to tie together the various codes and make the general functionality available through the ABI.

The end product is a shared library that is embedded into the Vega library, and extracted and loaded at runtime for use.

## License

This codebase (unlike Vega) is licensed under the MIT license. Whereas Vega is a large body of independent work, and is thus licensed under the (slightly) more restrictive Ms-PL, this work is mostly built from derived works that are either MIT licensed or public domain. This maintains the spirit of those licenses.

The full text of the MIT license can be found in the LICENSE file in this repository.

## Components

This library is made up of a wide set of other code solutions, which are listed here:


