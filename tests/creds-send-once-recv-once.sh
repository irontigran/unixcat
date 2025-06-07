#!/bin/sh

. "$(dirname "$0")/test-lib.sh"

setup_test

# Test: Send credentials once, receive once
# This should work on all systems that support credential sending
start_listener_and_send "-lR once" "-S once" "test message"

check_starts_with_contains "test message" "SCM_CRED"

finish_test