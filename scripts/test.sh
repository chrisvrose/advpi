#!/usr/bin/env bash
set -euo pipefail

REMOTE="${REMOTE:-geebo}"
BUILD_DIR="${BUILD_DIR:-build}"
REMOTE_DIR="${REMOTE_DIR:-~/advpi-build}"

TESTS=(
    endian_test
    mmio_test
    timer_test
)

# Build only the test targets
cmake --build "$BUILD_DIR" --target "${TESTS[@]}" -j"$(nproc)"

# Sync test binaries to the remote
rsync -az --info=progress2 "${TESTS[@]/#/"$BUILD_DIR/"}" "$REMOTE:$REMOTE_DIR/"

# Run the tests on the remote
for test in "${TESTS[@]}"; do
    echo "running $test"
    ssh "$REMOTE" "$REMOTE_DIR/$test"
done
