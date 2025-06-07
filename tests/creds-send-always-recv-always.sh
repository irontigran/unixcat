#!/bin/sh

. "$(dirname "$0")/test-lib.sh"

setup_test

# Test: Send credentials always, receive always
# Multiple messages should each have credential info
start_listener "-lR always"

# Send multiple messages through one connection
printf "message1\nmessage2\n" | ./ucat -S always "$socket" || clean_and_exit $hard_fail

wait_for_listener

# Should see both messages with credential information
# Credentials are received once per connection
check_starts_with_contains "message1" "message2"
check_starts_with_contains "message1" "SCM_CRED"

finish_test