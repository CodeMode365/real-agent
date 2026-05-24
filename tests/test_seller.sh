#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"
MODULE_NAME=seller
echo "=== Testing Seller module ==="

reset_data

# Add with full data is persisted in the pipe-delimited format.
out=$(run_session "$(in_lines admin 1234 \
    2 1 Ramesh 9800000000 Pokhara "" \
    3 5)")
assert_contains "$out" "Seller added successfully" "add succeeds with data"
assert_file_contains "sellers.txt" "|Ramesh|9800000000|Pokhara" "record written to sellers.txt"

# Blank inputs are stored as N/A.
out=$(run_session "$(in_lines admin 1234 \
    2 1 "" "" "" "" \
    3 5)")
assert_contains "$out" "Seller added successfully" "add succeeds with blanks"
assert_file_contains "sellers.txt" "|N/A|N/A|N/A" "blank fields stored as N/A"

# Whitespace-only input is treated as blank too.
reset_data
out=$(run_session "$(in_lines admin 1234 \
    2 1 "   " "   " "   " "" \
    3 5)")
assert_file_contains "sellers.txt" "|N/A|N/A|N/A" "whitespace-only input -> N/A"

# Surrounding whitespace is trimmed on real values.
reset_data
out=$(run_session "$(in_lines admin 1234 \
    2 1 "  Alice  " "  111  " "  Lalitpur  " "" \
    3 5)")
assert_file_contains "sellers.txt" "1|Alice|111|Lalitpur" "input is trimmed"

# View renders key-value layout (no table headers).
out=$(run_session "$(in_lines admin 1234 \
    2 2 3 3 5)")
assert_contains "$out" "ID:" "view shows ID key"
assert_contains "$out" "Name:" "view shows Name key"
assert_contains "$out" "Contact:" "view shows Contact key"
assert_contains "$out" "Address:" "view shows Address key"
assert_not_contains "$out" "ID      Name" "view does not use the old table header"

# Edit with blank input keeps current values; non-blank fields update.
reset_data
out=$(run_session "$(in_lines admin 1234 \
    2 1 Bob 123 Address1 "" \
    2 2 1 Bobby "" "" "" \
    3 3 5)")
assert_contains "$out" "Seller updated successfully" "update succeeds"
assert_file_contains "sellers.txt" "1|Bobby|123|Address1" "blank input keeps current values"

# Delete an unlinked seller works.
reset_data
out=$(run_session "$(in_lines admin 1234 \
    2 1 ToDelete 000 nowhere "" \
    2 1 1 "" \
    3 5)")
assert_contains "$out" "Seller deleted successfully" "delete unlinked seller"
assert_file_line_count "sellers.txt" 0 "file is empty after delete"

# Sellers linked to a property cannot be deleted.
reset_data
out=$(run_session "$(in_lines admin 1234 \
    2 1 Linked 777 addr "" \
    3 \
    1 1 House LocX 500000 1200 1 "" \
    4 \
    2 2 1 1 "" \
    3 5)")
assert_contains "$out" "cannot be deleted" "linked seller delete rejected"
assert_file_contains "sellers.txt" "1|Linked|777|addr" "linked seller still present"

# Pipe characters in input are sanitized so the file format stays intact.
reset_data
out=$(run_session "$(in_lines admin 1234 \
    2 1 "Pi|pe" "1|2|3" "addr|here" "" \
    3 5)")
# A correctly-parsed record must split into exactly 4 fields (3 pipes).
pipes=$(grep -E '^1\|' "$REPO_ROOT/sellers.txt" | head -n 1 | tr -cd '|' | wc -c | tr -d ' ')
if [ "$pipes" = "3" ]; then
    bump_pass "pipes in input are sanitized (record has 3 delimiters)"
else
    bump_fail "pipe sanitation broken (record has $pipes delimiters)"
fi

report_module
