#!/bin/sh

. "$(dirname "$0")/test-lib.sh"

setup_test

# Test: No credentials explicitly sent, but receive once requested
# The receiver will still get credentials from the connecting process
start_listener_and_send "-lR once" "" "test message"

check_starts_with_contains "test message" "SCM_CRED"

finish_test