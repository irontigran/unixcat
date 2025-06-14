#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

# Test that --udp and --seq are mutually exclusive

socket=$(mktemp -u sock.XXX)
trap 'rm -f $socket' EXIT

# Test that using both --udp and --seq fails
if ./ucat --udp --seq "$socket" 2>/dev/null; then
    echo "Expected failure when using both --udp and --seq"
    exit $fail
fi

exit $success
