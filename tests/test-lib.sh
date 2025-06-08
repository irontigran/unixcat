#!/bin/sh

# Test library for ucat tests
# Provides common functionality to reduce code duplication

# Standard test setup
setup_test() {
    socket=$(mktemp -u sock.XXX)
    results=$(mktemp result.XXX)
    
    success=0
    fail=1
    hard_fail=99
}

# Cleanup function with optional additional files
clean_and_exit() {
    exit_code="$1"
    shift
    rm -f "$socket" "$results" "$@"
    exit "$exit_code"
}

# Wait for socket to be ready with listener process check
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

# Start listener with options and send message
# Usage: start_listener_and_send "listener_options" "sender_options" "message"
start_listener_and_send() {
    listener_opts="$1"
    sender_opts="$2" 
    message="$3"
    
    # Start listener
    ./ucat "$listener_opts" "$socket" > "$results" &
    listener_pid=$!
    wait_for_socket "$socket" "$listener_pid" || clean_and_exit "$hard_fail"
    
    # Send message
    echo "$message" | ./ucat "$sender_opts" "$socket" || clean_and_exit "$hard_fail"
    
    # Wait for completion
    sleep 0.2
    wait "$listener_pid" 2>/dev/null || true
}

# Start listener only (for custom sending)
# Usage: start_listener "listener_options"
start_listener() {
    listener_opts="$1"
    
    ./ucat "$listener_opts" "$socket" > "$results" &
    listener_pid=$!
    wait_for_socket "$socket" "$listener_pid" || clean_and_exit "$hard_fail"
}

# Send message to existing listener
# Usage: send_message "sender_options" "message"
send_message() {
    sender_opts="$1"
    message="$2"
    
    echo "$message" | ./ucat "$sender_opts" "$socket" || clean_and_exit "$hard_fail"
}

# Wait for listener to complete
wait_for_listener() {
    sleep 0.2
    wait "$listener_pid" 2>/dev/null || true
}

# Read and return results
get_results() {
    cat "$results"
}

# Standard test completion
finish_test() {
    clean_and_exit "$success"
}

# Check if results match expected string exactly
check_exact_match() {
    expected="$1"
    actual="$(get_results)"
    
    if [ "$actual" != "$expected" ]; then
        echo "expected '$expected', actual was '$actual'"
        clean_and_exit "$fail"
    fi
}

# Check if results start with expected string and contain pattern
check_starts_with_contains() {
    expected_start="$1"
    pattern="$2"
    actual="$(get_results)"
    
    case "$actual" in
        "$expected_start"*"$pattern"*) ;;
        *)
            echo "expected to start with '$expected_start' and contain '$pattern', actual was '$actual'"
            clean_and_exit "$fail"
            ;;
    esac
}

# Check if results start with expected string
check_starts_with() {
    expected_start="$1"
    actual="$(get_results)"
    
    case "$actual" in
        "$expected_start"*) ;;
        *)
            echo "expected to start with '$expected_start', actual was '$actual'"
            clean_and_exit "$fail"
            ;;
    esac
}
