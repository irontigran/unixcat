#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)

# macs have /tmp as a symlink to /private/tmp, so make sure the file path
# matches after following the symlink since we're testing for the correct
# filename.
if test -d /private/tmp; then
    fdfile=$(mktemp /private/tmp/fd.XXX)
else
    fdfile=$(mktemp /tmp/fd.XXX)
fi
trap 'rm -f $socket $results $fdfile' EXIT

./ucat -l "$socket" > "$results" &
pid=$!
check_listener_creation $pid "$socket" || exit $hard_fail

printf "test1" | ./ucat --fd "$fdfile" "$socket" || exit $hard_fail
wait "$pid" 2>/dev/null || exit $hard_fail

expected="test1@ANC: SCM_RIGHTS $fdfile"
check_exact_match "$expected" "$results"
exit $?
