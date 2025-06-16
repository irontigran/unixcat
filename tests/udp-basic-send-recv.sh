#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

# Test: sending/receiving using datagram sockets.

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)
trap 'rm -f $socket $results' EXIT

./ucat -l --udp "$socket" > "$results" &
pid=$!
check_listener_creation $pid "$socket" || exit $hard_fail

check_python || exit $skipped
send_slowly "test1" | ./ucat --udp "$socket" || exit $hard_fail

check_exact_match "test1" "$results"
exit $?

