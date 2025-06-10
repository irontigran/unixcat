#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

# Linux only allows privileged processes to change credentials
if [ "$(id -u)" != 0 ]; then
    echo "skipping, requires root"
    exit $skipped
fi

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)
trap 'rm -f $socket $results' EXIT

# Test: Linux allows you to change credentials when sending them. The output
# should reflect the credentials provided on the command line.

bg_hold_stdin_open "./ucat -lR always $socket > $results"
pid=$!
check_listener_creation $pid "$socket" || exit $hard_fail

check_python || exit $skipped
send_twice_separately "test\n" | ./ucat --pid 1 --uid 0 --gid 0 -S always "$socket" || exit $hard_fail
wait "$pid" 2>/dev/null || exit $hard_fail

check_pattern "test
@ANC: SCM_CREDENTIALS pid=1,uid=0,gid=0
test
@ANC: SCM_CREDENTIALS pid=1,uid=0,gid=0" "$results"
exit $?

