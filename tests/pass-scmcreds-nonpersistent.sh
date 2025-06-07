#!/bin/sh

. "$(dirname "$0")/test-lib.sh"

setup_test

start_listener_and_send "-lR once" "" "hi"

check_starts_with "hi
@ANC: SCM_CREDS"

finish_test