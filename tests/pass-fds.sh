#!/bin/sh

socket=$(mktemp -u sock.XXX)
results=$(mktemp result.XXX)

# macs have /tmp as a symlink to /private/tmp, so make sure the file path
# matches after following the symlink since we're testing for the correct
# filename.
if test -d /private/tmp; then
    fdfile=$(mktemp /private/tmp/fd.XXX)
else
    fdfile=$(mktemp /tmp/fd.XXX)
fi

clean_and_exit() {
    rm -f "$socket" "$results" "$fdfile"
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

# Start listener and capture PID
./ucat -l "$socket" > "$results" &
listener_pid=$!
wait_for_socket "$socket" "$listener_pid" || clean_and_exit $hard_fail

# Send data and wait for completion
echo "hi" | ./ucat --fd "$fdfile" "$socket" || clean_and_exit $hard_fail

# Give listener time to process and exit
sleep 0.2
wait $listener_pid 2>/dev/null || true

expected="hi
@ANC: SCM_RIGHTS $fdfile"
stat=$success
r=$(cat "$results")
if [ "$r" != "$expected" ]; then
    echo "expected $expected, got $r"
    stat=$fail
fi
clean_and_exit $stat
