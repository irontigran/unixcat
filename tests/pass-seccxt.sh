#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)

trap 'rm -f $socket $results' EXIT

./ucat -l --security "$socket" > "$results" &
pid=$!
check_listener_creation $pid "$socket" || exit $hard_fail

printf "test1\n" | ./ucat "$socket" || exit $hard_fail
wait "$pid" 2>/dev/null || exit $hard_fail

if ! selinuxenabled > /dev/null 2>&1; then
    pattern="test1"
else
    pattern="test1
@ANC: SCM_SECURITY*"
fi

check_pattern "$pattern" "$results"
exit $?
