# Multi-stage build for SSX3 PS2 Decompilation Project

# Stage 1: Builder stage with all dependencies
FROM debian:bookworm

# Install system dependencies for 32-bit support and cross-compilation
RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
    # Build tools
    build-essential \
    gcc-multilib \
    g++-multilib \
    ninja-build \
    git \
    # MIPS cross-compilation
    binutils-mips-linux-gnu \
    # Wine for running legacy Windows tools
    wine \
    wine32 \
    wine32:i386 \
    # Python environment
    python3 \
    python3-venv \
    python3-pip \
    # Additional utilities
    curl \
    wget && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Create non-root user for security
RUN useradd -m -u 1000 -s /bin/bash builder

# Set working directory
WORKDIR /workspace

# Copy project files
COPY --chown=builder:builder . .

# Ensure workspace is writable by builder
RUN chown -R builder:builder /workspace

# Download and install the EE GCC compiler
RUN mkdir -p /opt/eegcc && \
    wget -qP /tmp "https://github.com/AngheloAlf/SN-Systems-ProDG_for_PS2_3.01/releases/latest/download/eegcc_2.95.3_sn_v1.36.tar.gz" && \
    tar -xzf /tmp/eegcc_2.95.3_sn_v1.36.tar.gz -C /opt/eegcc && \
    rm /tmp/eegcc_2.95.3_sn_v1.36.tar.gz

# Set up Python virtual environment in /root (won't be shadowed by volume mount)
RUN python3 -m venv /root/venv && \
    /root/venv/bin/pip install --upgrade pip setuptools wheel && \
    /root/venv/bin/pip install -r /workspace/requirements.txt && \
    chmod -R a+rx /root/venv

# Use the venv by default
ENV PATH="/root/venv/bin:$PATH" \
    VIRTUAL_ENV="/root/venv" \
    WINEARCH=win32 \
    WINEPREFIX=/home/builder/.wine

# Create build automation script
RUN python3 << 'EOFPYTHON'
import subprocess
import sys
from pathlib import Path

script = """#!/bin/bash
set -e

cd /workspace

# Check disc file
if [ ! -f "disc/SLUS_207.72" ]; then
    echo "ERROR: disc/SLUS_207.72 not found!"
    exit 1
fi

# Ensure compiler is available (volume mount may overwrite tools/)
if [ ! -d "tools/cc/eegcc-2.95.3-V1.36" ]; then
    echo "=== Setting up compiler ==="
    mkdir -p tools/cc/eegcc-2.95.3-V1.36
    cp -r /opt/eegcc/* tools/cc/eegcc-2.95.3-V1.36/
fi

# Configure if needed
if [ ! -f "build.ninja" ]; then
    echo "=== Running configure ==="
    /root/venv/bin/python3 configure.py
fi

# Build
echo "=== Building project ==="
/root/venv/bin/python3 -m pip show ninja 2>/dev/null && ninja || (/root/venv/bin/python3 -m pip install ninja && ninja)

echo "=== Build complete ==="
exec "$@"
"""

Path("/entrypoint.sh").write_text(script)
subprocess.run(["chmod", "+x", "/entrypoint.sh"], check=True)
EOFPYTHON

# Set working directory
WORKDIR /workspace

# Run the build on startup
ENTRYPOINT ["/entrypoint.sh"]
CMD ["/bin/bash"]
