#!/bin/sh

. "$(dirname "$0")/test-lib.sh"

setup_test

# Test: BSD-style credentials - Send credentials once, but no receiving specified
# On BSD systems that support sending, credentials may be sent without -R
start_listener_and_send "-l" "-S once" "test message"

check_starts_with_contains "test message" "SCM_CRED"

finish_test