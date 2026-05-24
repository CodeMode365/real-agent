#!/usr/bin/env bash
# Shared helpers for module tests.
#
# Conventions:
#   - Each test file sources this file and sets MODULE_NAME.
#   - Each test file calls report_module at the end.
#   - When RESULTS_DIR is exported, per-module counts are appended to
#     "$RESULTS_DIR/results.tsv" as: name<TAB>passed<TAB>failed<TAB>crashed
#   - When a session is killed by the timeout or exits non-zero, the
#     crash counter is bumped so the orchestrator can include it in analytics.

set -u

REPO_ROOT="${REPO_ROOT:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)}"
BINARY="${BINARY:-$REPO_ROOT/build/Debug/outDebug}"
SESSION_TIMEOUT="${SESSION_TIMEOUT:-10}"

DATA_FILES=(sellers.txt clients.txt properties.txt sellerID.txt clientID.txt propertyID.txt temp.txt)

MODULE_NAME="${MODULE_NAME:-unknown}"
MODULE_PASSED=0
MODULE_FAILED=0
MODULE_CRASHED=0
MODULE_FAILURES=()

reset_data() {
    cd "$REPO_ROOT"
    for f in "${DATA_FILES[@]}"; do
        rm -f "$f"
    done
}

in_lines() {
    # Each argument becomes one input line. Empty args produce blank lines.
    printf '%s\n' "$@"
}

run_session() {
    # $1 = input string (newline-separated). Returns binary's combined output.
    # If the binary crashes (signal exit > 128) or times out we record it.
    cd "$REPO_ROOT"
    local out status
    out=$(printf '%s' "$1" | timeout "$SESSION_TIMEOUT" "$BINARY" 2>&1) || status=$?
    status=${status:-0}
    if [ "$status" -ge 124 ]; then
        MODULE_CRASHED=$((MODULE_CRASHED + 1))
        MODULE_FAILURES+=("session crashed/hung with exit $status")
    fi
    printf '%s' "$out"
}

run_session_raw() {
    # Like run_session but reads stdin verbatim. Useful for fuzz scripts.
    cd "$REPO_ROOT"
    local status
    timeout "$SESSION_TIMEOUT" "$BINARY" >/dev/null 2>&1 || status=$?
    status=${status:-0}
    return "$status"
}

_record_pass() {
    MODULE_PASSED=$((MODULE_PASSED + 1))
    printf '  [PASS] %s\n' "$1"
}

_record_fail() {
    MODULE_FAILED=$((MODULE_FAILED + 1))
    MODULE_FAILURES+=("$1")
    printf '  [FAIL] %s\n' "$1"
}

assert_contains() {
    # $1 = haystack, $2 = needle, $3 = description
    if printf '%s' "$1" | grep -qF -- "$2"; then
        _record_pass "$3"
    else
        _record_fail "$3 (missing: $2)"
    fi
}

assert_not_contains() {
    if printf '%s' "$1" | grep -qF -- "$2"; then
        _record_fail "$3 (unexpected: $2)"
    else
        _record_pass "$3"
    fi
}

assert_file_contains() {
    # $1 = file (relative to REPO_ROOT), $2 = needle, $3 = description
    cd "$REPO_ROOT"
    if [ -f "$1" ] && grep -qF -- "$2" "$1"; then
        _record_pass "$3"
    else
        _record_fail "$3 (file $1 missing: $2)"
    fi
}

assert_file_line_count() {
    # $1 = file, $2 = expected line count, $3 = description
    cd "$REPO_ROOT"
    local actual=0
    if [ -f "$1" ]; then
        actual=$(wc -l < "$1" | tr -d ' ')
    fi
    if [ "$actual" -eq "$2" ]; then
        _record_pass "$3"
    else
        _record_fail "$3 (file $1 had $actual lines, expected $2)"
    fi
}

bump_pass() { _record_pass "$1"; }
bump_fail() { _record_fail "$1"; }

report_module() {
    printf '\n%s: %d passed, %d failed, %d crashed\n' \
        "$MODULE_NAME" "$MODULE_PASSED" "$MODULE_FAILED" "$MODULE_CRASHED"
    if [ "${#MODULE_FAILURES[@]}" -gt 0 ]; then
        printf '  Failures:\n'
        for f in "${MODULE_FAILURES[@]}"; do
            printf '    - %s\n' "$f"
        done
    fi
    if [ -n "${RESULTS_DIR:-}" ]; then
        printf '%s\t%d\t%d\t%d\n' \
            "$MODULE_NAME" "$MODULE_PASSED" "$MODULE_FAILED" "$MODULE_CRASHED" \
            >> "$RESULTS_DIR/results.tsv"
    fi
}
