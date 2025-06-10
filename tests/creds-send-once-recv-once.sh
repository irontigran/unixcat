#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"
socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)
trap 'rm -f $socket $results' EXIT

# Test: Send credentials once, receive once
# This should work on all systems that support credential sending

bg_hold_stdin_open "./ucat -lR once $socket > $results"
pid=$!
check_listener_creation $pid "$socket" || exit $hard_fail

check_python || exit $skipped
send_twice_separately "test\n" | ./ucat -S once "$socket" || exit $hard_fail
wait "$pid" 2>/dev/null || exit $hard_fail

check_pattern "test
@ANC: SCM_CRED*
test" "$results"
exit $?
