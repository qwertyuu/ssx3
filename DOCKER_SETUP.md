# SSX3 Docker Setup Guide

## Prerequisites

- Docker installed and running
- Docker Compose (optional, but recommended)
- A NTSC-U copy of the SSX 3 PS2 game disc (extracted)

## Setup Instructions

### 1. Extract Game Disc

You need to extract your game disc first:

- **CHD format:** Use `chdman` or emulator extraction tools
- **Redump/ISO format:** Use 7-Zip, WinRAR, or extraction tools

Extract the files so you have the `SLUS_207.72` executable.

### 2. Prepare Disc Directory

Create a `disc` directory in the project root and place the extracted game files there:

```
h:\GitHub\ssx3\
├── disc/
│   ├── SLUS_207.72        ← The main executable (required)
│   └── [other game files]
├── Dockerfile
├── docker-compose.yml
└── ...
```

### 3. Build the Docker Image

```bash
docker-compose build
```

This creates the image **without** including the disc (keeps image size small).

### 4. Run the Container

The first time you run it, the container will automatically run `configure.py` to set up the build:

```bash
docker-compose run --rm ssx3-builder
```

Inside the container:

```bash
# Check that configure ran by verifying the build output is generated
ls out/

# Build the project
. venv/bin/activate
ninja

# Or rebuild from scratch
ninja clean && ninja
```

## Build Commands (Inside Container)

```bash
# Activate Python virtual environment
. venv/bin/activate

# Run configure (usually automatic on first run)
python3 configure.py

# Build the project
ninja

# Clean build artifacts
ninja clean

# Rebuild from scratch
ninja clean && ninja
```

## Development Workflow

For iterative development:

```bash
# Start container
docker-compose run --rm ssx3-builder

# Inside container, run builds
. venv/bin/activate
ninja -j 4  # parallel build with 4 jobs

# Exit when done
exit
```

## Troubleshooting

### Missing disc file

**Error:** `FileNotFoundError: [Errno 2] No such file or directory: '/workspace/disc/SLUS_207.72'`

**Solution:** Ensure you've extracted your game disc and placed the `SLUS_207.72` file in the `disc/` directory.

### Wine Issues

If you encounter Wine permission errors:

```bash
WINEPREFIX=/home/builder/.wine wineboot -i
```

### Out of Memory

The MIPS compiler can be memory-intensive. Limit parallel build jobs:

```bash
ninja -j 2  # Instead of default
```

### Disk Space

Build artifacts are stored in `out/`. Ensure your system has at least 5GB free space.

## Notes

- The container runs as a non-root user (`builder:1000`) for security
- Wine 32-bit support is enabled for cross-compilation tools
- MIPS binutils are available for cross-targeting PS2 architecture
- Build outputs are in the `out/` directory
- The disc directory is mounted at runtime (not baked into the image)
