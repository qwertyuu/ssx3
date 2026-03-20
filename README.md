# SSX 3 (2003)

This is a work-in-progress decompilation of SSX 3 (2003) for the PlayStation 2. This project builds the NTSC-U version of the game (`SLUS_207.72`, sha1 `77114dfd1205eaccf1ccc18c5f9650097fa78bd8`). Other regions and versions may be considered once NTSC-U has been completed and matched.

The main goal of this project is to decompile the game code to matching source code. This project doesn't contain any game assets or original code from the game's executable. It requires your own copy of the game to build and run this project.

## Progress

| File | Functions | Status |
|------|-----------|--------|
| src/bx/bxstring.cpp | 44/44 | 100% |
| src/dirtysock/tags.cpp | 40/40 | 100% |
| src/bxrandom.cpp | 10/10 | 100% |
| src/visualfx/crowdrender2d.cpp | 8/8 | 100% |
| src/hashvalue.cpp | 6/6 | 100% |
| src/md5.cpp | 4/4 | 100% |

**112 / ~11,800 functions decompiled (~0.7% of .text segment)**

All decompiled functions compile successfully. Match verification is pending.

## Setup

You will need:
- Docker and Docker Compose
- A NTSC-U disc image (obtained legally)

### 1. Prepare the disc

Create a `disc/` directory in the project root and place the extracted game files there:

```
ssx3/
├── disc/
│   └── SLUS_207.72    ← required
├── Dockerfile
├── docker-compose.yml
└── ...
```

### 2. Build and run

```bash
docker-compose build
docker-compose run --rm ssx3-builder
```

The container automatically downloads the compiler, runs `configure.py`, and builds the project.

### Alternative: Native Linux

```bash
apt install -y binutils-mips-linux-gnu ninja-build wine
./scripts/setup.sh
python -m venv venv && . venv/bin/activate && pip install -r requirements.txt
python3 configure.py
ninja
```

## Decompilation Workflow

Functions are split by [splat](https://github.com/ethteck/splat) into individual assembly files in `asm/nonmatchings/`. Decompiled C++ is written in `src/` using this pattern:

```cpp
INCLUDE_ASM("module", function_name);
#ifdef SKIP_ASM
ReturnType function_name(args) {
    // decompiled C++ code
}
#endif
```

The `INCLUDE_ASM` directive includes the original assembly. The `#ifdef SKIP_ASM` block contains the decompiled version, which is compiled instead when `SKIP_ASM` is defined.

### Ghidra decompiler

A headless Ghidra pipeline is available to generate pseudocode for all functions using the [ghidra-emotionengine-reloaded](https://github.com/chaoticgd/ghidra-emotionengine-reloaded) processor extension. Output goes to `ghidra_out/` (gitignored). See [DOCKER_SETUP.md](DOCKER_SETUP.md) for Docker details.

### Verification

Use [objdiff](https://github.com/encounter/objdiff) to compare compiled output against the original binary for byte-level matching.

## Project Structure

```
src/              C++ source files
include/          Headers (common.h, macro.inc)
asm/              Splat-generated assembly (gitignored)
config/           Splat config, symbol addresses
scripts/          Build and tooling scripts
ghidra_out/       Ghidra decompiler output (gitignored)
disc/             Original game files (not in repo)
tools/            Compiler and extensions
```

## Technical Details

- **Target**: PS2 Emotion Engine (MIPS R5900) — 64-bit registers, 32-bit addresses
- **Compiler**: eegcc-2.95.3-V1.36 with flags `-O2 -march=5900 -mabi=eabi`
- **Total .text size**: ~3.3 MB across ~11,800 functions
