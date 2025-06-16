#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)
trap 'rm -f $socket $results' EXIT

# Test: Linux-style credentials - Send credentials once, but no receiving specified
# On Linux, sending has no effect without -R specified

sleep 0.5 | ./ucat -l "$socket" > "$results" &
pid=$!
check_listener_creation $pid "$socket" || exit $hard_fail

check_python || exit $skipped
send_twice_separately "test\n" | ./ucat -S once "$socket" || exit $hard_fail
wait "$pid" 2>/dev/null || exit $hard_fail

check_exact_match "test
test" "$results"
exit $?
