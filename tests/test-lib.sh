#!/bin/sh

# Test library for ucat tests
# Provides common functionality to reduce code duplication

export success=0
export fail=1
export skipped=77
export hard_fail=99

# Usage: hold_stdin_open <command>
# Run a command in the background while holding stdin open. Useful because we
# want to run ucat listeners in the background and allow them to poll stdin
# without being affected by foreground processes. If we don't do this, stdin is
# closed, but then receives a POLLIN signal, causing ucat to try and read from
# it and then exit.
bg_hold_stdin_open() {
    cmd="$1"
    # only need to hold open stdin if it's a tty
    if tty -s; then
        eval "$cmd < $(tty) &"
    else
        eval "$cmd &"
    fi
}

# Usage: check_listener_creation <pid> <socket>
# Synchronization to make sure that a process (specified by pid) creates a unix
# domain socket (specified by socket path) before moving on.
check_listener_creation() {
    pid="$1"
    socket="$2"
    timeout=5
    count=0
    while [ $count -lt $timeout ]; do
        # Check if listener process still exists to avoid too-long waits if the
        # process died.
        if ! kill -0 "$pid" 2>/dev/null; then
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

# Usage: send_twice_separately <msg>
# Prints the msg to stdout twice, with a slight delay in between, in order to
# trigger two separate sendmsg calls, and therefore two recvmsg calls. This is
# important for testing functionality where we expect the second recvmsg call
# to be different than the first.
send_twice_separately() {
    msg="$1"
    python3 -c \
        "import time; print('$msg', end='', flush=True); time.sleep(0.1); print('$msg', end='', flush=True)"
}

# send_twice separately requires python, but we can't check inside that
# function because it's used in a pipeline. So check before calling
# send_twice_separately using this.
check_python() {
    if ! command -v python3 >/dev/null 2>&1; then
        echo "skipping: no python3 available"
        return $skipped
    fi
}

# Usage: check_exact_match "match this exactly" filename
# Check if results match expected string exactly
check_exact_match() {
    expected="$1"
    actual=$(cat "$2")
    
    if [ "$actual" != "$expected" ]; then
        echo "expected '$expected', actual was '$actual'"
        return $fail
    fi
    return $success
}

# Usage: check_pattern "first line\n*\nlast line\n" filename
# Check if the file contents match a shell string pattern.
check_pattern() {
    pattern="$1"
    str=$(cat "$2")

    # shellcheck disable=SC2254
    case "$str" in
        $pattern) return $success ;;
        *)
            echo "expected to match '$pattern', actual was '$str'"
            return $fail
            ;;
    esac
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
