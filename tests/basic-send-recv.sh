#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

setup_test

start_listener_and_send "-l" "" "hi"

check_exact_match "hi"

finish_test