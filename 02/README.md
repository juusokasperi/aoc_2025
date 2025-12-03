## Day 3

Written in Assembly for Linux x86_64.

For part2, I tried my hand at some optimization;

- SIMD intrinsics for faster reading of the buffer. `part2_optimized_simd` attempts to read 32 bytes at once, `part2_optimized_simd_64` 64 bytes.
- Removed read/close syscalls for the `_optimized_` binaries - programs only read stdin (`./part2_optimized_simd_64 < input`)
- Plus, added -static flag to linker since the program works standalone

With this I achieve very fast results, with the input given by AoC, timing gets my whole program to run ~1.4ms.

### How to build:

There are a couple binaries you can build, namely;
```sh
part1           part2
part2_simd      part2_optimized_simd
part2_optimized_simd_64
```

`binary=(binary_to_build); nasm -f elf64 ${binary}.asm -o ${binary}.o && ld ${binary}.o -o ${binary} -static`
