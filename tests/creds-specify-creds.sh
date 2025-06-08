#!/bin/sh

# Linux only allows privileged processes to change credentials
skipped=77
if [ "$(id -u)" != 0 ]; then
    echo "skipping, requires root"
    exit $skipped
fi

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

setup_test

start_listener "-lR always"

printf "message1\nmessage2\n" | ./ucat --pid 1 --uid 0 --gid 0 "$socket" || clean_and_exit $hard_fail

wait_for_listener

check_starts_with_contains "message1" "pid=1"
check_starts_with_contains "message1" "uid=0"
check_starts_with_contains "message1" "gid=0"

finish_test
