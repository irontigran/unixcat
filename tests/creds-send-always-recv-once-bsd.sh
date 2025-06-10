#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)
trap 'rm -f $socket $results' EXIT

# Test: specifying -S always with -R once on BSD should result in _different_
# credential messages. On the first message, the -R will overrule -S, and all
# of the following messages will be of the -S type.

bg_hold_stdin_open "./ucat -lR once $socket > $results"
pid=$!
check_listener_creation $pid "$socket" || exit $hard_fail

check_python || exit $skipped
send_twice_separately "test\n" | ./ucat -S always "$socket" || exit $hard_fail
wait "$pid" 2>/dev/null || exit $hard_fail

check_pattern "test
@ANC: SCM_CREDS *uid*euid*gid*egid*
test
@ANC: SCM_CREDS *pid*uid*euid*gid*" "$results"
exit $?
