## Day 3

Written in Assembly for Linux x86_64.

For part2, I tried my hand at some optimization;

- SIMD intrinsics for faster reading of the buffer.
- Removed read/close syscalls - program only reads stdin

### How to build:

There are a couple binaries you can build, namely;
```sh
part1           part2
part2_simd      part2_optimized_simd
```

`binary=(binary_to_build); nasm -f elf64 ${binary}.asm -o ${binary}.o && ld ${binary}.o -o ${binary}`
