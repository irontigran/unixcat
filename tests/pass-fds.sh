#!/bin/sh

. "$(dirname "$0")/test-lib.sh"

setup_test

# macs have /tmp as a symlink to /private/tmp, so make sure the file path
# matches after following the symlink since we're testing for the correct
# filename.
if test -d /private/tmp; then
    fdfile=$(mktemp /private/tmp/fd.XXX)
else
    fdfile=$(mktemp /tmp/fd.XXX)
fi

start_listener "-l"

echo "hi" | ./ucat --fd "$fdfile" "$socket" || clean_and_exit $hard_fail "$fdfile"

wait_for_listener

expected="hi
@ANC: SCM_RIGHTS $fdfile"
check_exact_match "$expected"

clean_and_exit $success "$fdfile"