#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"
MODULE_NAME=admin
echo "=== Testing Admin module ==="

reset_data

# Correct credentials log in cleanly.
out=$(run_session "$(in_lines admin 1234 5)")
assert_contains "$out" "Login successful" "valid credentials succeed"
assert_contains "$out" "Program ended" "logout exits program"

# Wrong creds are rejected and the user can retry.
out=$(run_session "$(in_lines wrong wrong admin 1234 5)")
assert_contains "$out" "Invalid login" "wrong creds rejected"
assert_contains "$out" "Login successful" "retry with correct creds works"

# Blank username/password is rejected.
out=$(run_session "$(in_lines "" "" admin 1234 5)")
assert_contains "$out" "Invalid login" "blank credentials rejected"

# Inputs are trimmed before being compared.
out=$(run_session "$(in_lines "  admin  " "  1234  " 5)")
assert_contains "$out" "Login successful" "credentials are trimmed before comparison"

# EOF on stdin should not loop forever; the program must exit gracefully.
status=0
printf 'admin\n1234\n' | timeout "$SESSION_TIMEOUT" "$BINARY" >/dev/null 2>&1 || status=$?
if [ "$status" -lt 124 ]; then
    bump_pass "EOF after login exits without hanging"
else
    bump_fail "EOF after login (exit $status)"
fi

report_module
