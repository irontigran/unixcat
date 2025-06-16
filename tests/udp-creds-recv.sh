#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)
trap 'rm -f $socket $results' EXIT

# Test: credential passing over datagram sockets

./ucat -l --udp -R always "$socket" > "$results" &
pid=$!
check_listener_creation $pid "$socket" || exit $hard_fail

check_python || exit $skipped
send_slowly "test1" | ./ucat --udp "$socket" || exit $hard_fail

# Check that credentials were received and displayed
expected="test1@ANC: SCM_CRED*"
check_pattern "$expected" "$results"
exit $?
