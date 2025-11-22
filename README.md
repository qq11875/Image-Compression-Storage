# Advanced Lossy Image Compression Algorithms in C

![C](https://img.shields.io/badge/Language-C-blue)
![License](https://img.shields.io/badge/License-MIT-green)
[![Build](https://img.shields.io/badge/Build-passing-brightgreen)]( )

A high-performance implementation of three original lossy compression algorithms for 5-bit grayscale `.ebc` images (32 intensity levels). Built from scratch in pure C99 with heavy emphasis on **bit-level manipulation**, **modular design**, **memory efficiency**, and **visual quality vs compression trade-offs**.

Live demo images and compiled binaries available in the repository.

## Algorithms Implemented

| Algorithm      | Technique                              | Bits per 3×3 block | Compression Ratio* | Visual Quality | Magic |
|----------------|----------------------------------------|--------------------|---------------------|----------------|-------|
| `ebcBlock`     | Block mean averaging (3×3 → 1 value)   | 5 bits             | ~9:1                | Moderate       | `EC`  |
| `ebcR32`       | Vector Quantization with 32 random codebooks | 5 bits             | ~9:1                | High           | `E5`  |
| `ebcR128`      | Vector Quantization with 128 random codebooks| 7 bits             | ~6.4:1              | Very High      | `E7`  |

\* Approximate ratio on typical 512×512 ebc images

### Features
- Full compress + decompress pipeline for all three algorithms
- Custom bit-packed binary I/O (no external libraries)
- Reproducible randomness via user-provided seed (`rand()` seeded)
- Clean, heavily modularised codebase with extensive code reuse
- Zero memory leaks, full error handling, robust edge/corner block support
- Makefile with `all`, `clean`, and individual targets

## Why This Project Stands Out

- Deep understanding of **bit-level data packing** and binary file formats
- Real-world **algorithm design** trade-offs (speed vs quality vs ratio)
- Implementation of **vector quantization** concepts commonly used in legacy game consoles, texture compression (e.g. BCn/DXT), and embedded displays
- Strong **C systems programming** skills: manual memory management, pointer arithmetic, modular design, clean build system
- Excellent code quality and readability (originally graded at 100% code-style rubric)

## Usage

### Compression
```bash
# Block mean algorithm
./ebcBlock input.ebc output.ebcc

# Vector quantization (32 or 128 codebooks) - seed for reproducibility
./ebcR32  input.ebc output.ebcc 12345
./ebcR128 input.ebc output.ebcc 12345
```

### Decompression
```bash
./ebcUnblock compressed.ebcc output.ebc
./ebcU32     compressed.ebcc output.ebc
./ebcU128    compressed.ebcc output.ebc
```

### Convert result to viewable image (PGM)
```bash
./ebf2pgm output.ebc output.pgm
# Then open with any image viewer (GIMP, feh, etc.)
```

## Example Results

| Original (uncompressed) | ebcBlock | ebcR32 (seed 8732) | ebcR128 (seed 8732) |
|--------------------------|----------|--------------------|---------------------|
| Sharp details            | Blocky   | Minor noise        | Near-original quality |

(See `/examples/` folder for full-size before/after images)

## Building

```bash
make          # builds all 6 executables + helper tools
make clean    # removes objects and binaries
```

## File Format (ebc family)

All produced files follow the common header:

```bash
2-byte magic number
height (uint32) width (uint32)
[paradigm blocks - only for ebcR32/R128]
bit-packed compressed image data
```
Extensions used: `.ebcc` (generic compressed), `.ebc` (raw uncompressed)

## Possible Extensions (Future Ideas)
- K-means instead of random codebook selection
- Adaptive block sizes
- Multi-threaded compression
- WebAssembly port for in-browser demo
