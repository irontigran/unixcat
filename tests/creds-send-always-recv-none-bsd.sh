#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)
trap 'rm -f $socket $results' EXIT

# Test: BSD-style credentials - send credentials once, but no receiving specified.
# On BSD systems that support sending, credentials may be sent without a receive option.

./ucat -l "$socket" > "$results" < /dev/tty &
pid=$!
check_listener_creation $pid "$socket" || exit $hard_fail

check_python || exit $skipped
send_twice_separately "test\n" | ./ucat -S always "$socket" || exit $hard_fail
wait "$pid" 2>/dev/null || exit $hard_fail

check_pattern "test
@ANC: SCM_CRED*
test
@ANC: SCM_CRED*" "$results"
exit $?
