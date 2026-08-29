#!/usr/bin/env bash
set -euo pipefail

REMOTE="${REMOTE:-geebo}"
BIOS="${BIOS:-bios/arm_init.bin}"
BUILD_DIR="${BUILD_DIR:-build}"
REMOTE_DIR="${REMOTE_DIR:-~/advpi-build}"

cmake --build "$BUILD_DIR" -j"$(nproc)"
pushd bios
make -j4
popd

# Sync the whole build directory and BIOS to the remote
rsync -az --info=progress2 "$BUILD_DIR/" "$REMOTE:$REMOTE_DIR/"
rsync -a "$BIOS" "$REMOTE:$REMOTE_DIR/"

BIOS_FILENAME="$REMOTE_DIR/$(basename "$BIOS")"
ssh "$REMOTE" "BIOS_LOCATION=$BIOS_FILENAME $REMOTE_DIR/advpi"
