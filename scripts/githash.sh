#!/bin/sh

# This script captures the current git commit hash and writes it to a file
# that can be included in the sdist for later builds.

OUTPUT_FILE=".githash"

# Check if we're in a git repository
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    echo "Warning: Not in a git repository. Skipping git hash capture." >&2
    exit 0
fi

# Get the short git hash
GITHASH=$(git rev-parse --short=16 HEAD)

if [ -z "$GITHASH" ]; then
    echo "Warning: Could not determine git hash." >&2
    exit 1
fi

# Write to file
echo "$GITHASH" > "$OUTPUT_FILE"
echo "Git hash '$GITHASH' written to $OUTPUT_FILE"
