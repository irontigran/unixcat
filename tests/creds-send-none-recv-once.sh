#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)
trap 'rm -f $socket $results' EXIT

# Test: No sending option, just receive once. The receiver should get
# credentials only on the first message.

bg_hold_stdin_open "./ucat -lR once $socket > $results"
pid=$!
check_listener_creation $pid "$socket" || exit $hard_fail

check_python || exit $skipped
send_twice_separately "test\n" | ./ucat "$socket" || exit $hard_fail
wait "$pid" 2>/dev/null || exit $hard_fail

check_pattern "test
@ANC: SCM_CRED*
test" "$results"
exit $?
