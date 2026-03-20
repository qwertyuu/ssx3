# SSX 3 PS2 Decompilation

Decompilation of SSX 3 (NTSC-U, SLUS_207.72) from MIPS R5900 assembly back to C++.

## Build

```bash
docker-compose run --rm ssx3-builder
```

Compiler: eegcc-2.95.3-V1.36 (PS2 EE GCC), flags: `-O2 -march=5900 -mabi=eabi`, run via Wine in Docker.

## Decompilation Workflow

1. Functions are split by splat into `asm/nonmatchings/<module>/<function>.s`
2. Source files in `src/` reference them with `INCLUDE_ASM("<module>", <function_name>);`
3. Decompiled C++ goes in `#ifdef SKIP_ASM` blocks after each `INCLUDE_ASM`:

```cpp
INCLUDE_ASM("module", function_name);
#ifdef SKIP_ASM
int function_name(int param) {
    // decompiled code
}
#endif
```

4. Build with Docker, verify with objdiff against `disc/SLUS_207.72`

## Ghidra Pipeline

Headless decompilation output for all 11,842 functions lives in `ghidra_out/` (gitignored).

To regenerate:
```bash
# Uses blacktop/ghidra Docker image with ghidra-emotionengine-reloaded extension
# Language ID: r5900:LE:32:default
# Script: scripts/DecompileAll.java
```

## Project Structure

- `src/` — C++ source files (307 files, most are empty stubs awaiting splitting)
- `asm/` — splat-generated assembly (gitignored, regenerated from ROM)
- `include/` — headers, `common.h` defines `uint`/`ulong`, `macro.inc` has assembler macros
- `config/symbol_addrs.txt` — 756 known function names and addresses
- `config/ssx3_us.yaml` — splat configuration
- `ghidra_out/` — Ghidra decompiler output per function (gitignored)
- `disc/SLUS_207.72` — original ROM (not in repo)

## Current Progress

| File | Functions | Status |
|------|-----------|--------|
| bxrandom.cpp | 10/10 | 100% |
| hashvalue.cpp | 6/6 | 100% |
| crowdrender2d.cpp | 8/8 | 100% |
| md5.cpp | 4/4 | 100% |
| bxstring.cpp | 44/44 | 100% |
| tags.cpp | 40/40 | 100% |
| **Total** | **112/112** | **All compile, not yet match-verified** |

Overall: ~0.7% of total .text (23KB / 3.3MB).

## Types

From `include/common.h`:
- `uint` = `unsigned int`
- `ulong` = `unsigned long`

## Key Conventions

- PS2 Emotion Engine: 64-bit registers, 32-bit addresses, custom instructions (sq, lq, daddu)
- `$gp`-relative globals use `bGp`/`iGp`/`DAT_` prefixes in Ghidra output
- Empty string singleton for cBXString: `D_0048DCD4` / `PTR_DAT_004a3e90`
- Standard C library functions are linked by address (memcpy = FUN_0041605c, strlen = FUN_00416810, etc.)
