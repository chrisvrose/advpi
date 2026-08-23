#!/bin/bash
# Copy real shared libraries from ARM host for cross-compilation linking
# Usage: ./copy_libs.sh [remote_host] [output_dir]
# Example: ./copy_libs.sh geebo ./stubs

set -e

REMOTE_HOST="${1:-geebo}"
OUTDIR="${2:-./stublib/stubs}"

mkdir -p "$OUTDIR"

# Libraries needed for SDL3 cross-compilation
LIBS="libjack
libpipewire-0.3
libpulse
libsndio
libfribidi
libthai
libdrm
libgbm
libwayland-egl
libwayland-cursor
libwayland-client
libEGL
libdecor-0
libxkbcommon
libunwind
libunwind-generic"

echo "Copying libraries from $REMOTE_HOST to $OUTDIR..."

for lib in $LIBS; do
    # Find the real library file (resolve symlinks)
    real_lib=$(ssh "$REMOTE_HOST" "find /usr/lib -name '${lib}.so*' -type f 2>/dev/null | head -1")
    if [ -n "$real_lib" ]; then
        # Copy the real .so file
        scp -p "$REMOTE_HOST:$real_lib" "$OUTDIR/"
        # Create the .so symlink the linker expects
        ln -sf "$(basename "$real_lib")" "$OUTDIR/${lib}.so"
        echo "✓ $lib"
    else
        echo "✗ $lib not found"
    fi
done

echo ""
echo "Done. Libraries in $OUTDIR:"
ls -la "$OUTDIR"
