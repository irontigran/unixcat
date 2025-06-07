#!/bin/sh

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)

clean_and_exit() {
    rm -f "$socket" "$results"
    exit "$1"
}

success=0
fail=1
hard_fail=99

# Wait for socket to be ready
wait_for_socket() {
    socket="$1"
    listener_pid="$2"
    timeout=5
    count=0
    while [ $count -lt $timeout ]; do
        # Check if listener process still exists
        if ! kill -0 "$listener_pid" 2>/dev/null; then
            return 1
        fi
        if [ -S "$socket" ]; then
            return 0
        fi
        sleep 0.1
        count=$((count + 1))
    done
    return 1
}

# Test: No credentials explicitly sent, but receive once requested
# The receiver will still get credentials from the connecting process
# Start listener and capture PID
./ucat -lR once "$socket" > "$results" &
listener_pid=$!
wait_for_socket "$socket" "$listener_pid" || clean_and_exit $hard_fail

# Send data and wait for completion
echo "test message" | ./ucat "$socket" || clean_and_exit $hard_fail

# Give listener time to process and exit
sleep 0.2
wait $listener_pid 2>/dev/null || true

# Should see the message plus credential information
expected_start="test message"
r=$(cat "$results")
case "$r" in
    "$expected_start"*"SCM_CRED"*) ;;
    *)
        echo "expected to start with '$expected_start' and contain credential info, actual was '$r'"
        clean_and_exit $fail
        ;;
esac

clean_and_exit $success