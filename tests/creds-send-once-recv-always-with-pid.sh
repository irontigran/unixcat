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
# should reflect the credentials provided on the command line. But when sending
# them only once, later credential messages should revert to the real ones.

./ucat -lR always "$socket" > "$results" < $(tty) &
pid=$!
check_listener_creation $pid "$socket" || exit $hard_fail

check_python || exit $skipped
send_twice_separately "test\n" | ./ucat --pid 1 --uid 0 --gid 0 -S once "$socket" || exit $hard_fail
wait "$pid" 2>/dev/null || exit $hard_fail

# The pattern is a little hacky, but the point is that the second credential
# message should be different from the first.
check_pattern "test
@ANC: SCM_CREDENTIALS pid=1,uid=0,gid=0
test
@ANC: SCM_CREDENTIALS pid=[1-9][0-9]*,uid=[0-9]*,gid=[0-9]" "$results"
exit $?


