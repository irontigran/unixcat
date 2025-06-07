#!/bin/sh

. "$(dirname "$0")/test-lib.sh"

setup_test

# First test: receive once
start_listener_and_send "-lR once" "" "hi"

check_starts_with "hi
@ANC: SCM_CREDS"

# Clean up for second test
rm -f "$socket" "$results"

# Second test: receive always
start_listener_and_send "-lR always" "" "hi"

check_starts_with "hi
@ANC: SCM_CREDS2"

finish_test