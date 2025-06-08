#!/bin/sh

# shellcheck source=tests/test-lib.sh
. "$(dirname "$0")/test-lib.sh"

setup_test

# Test: Linux-style credentials - Send credentials once, but no receiving specified
# On Linux, sending has no effect without -R specified
start_listener_and_send "-l" "-S once" "test message"

check_exact_match "test message"

finish_test