#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"
MODULE_NAME=client
echo "=== Testing Client module ==="

reset_data

# Add a seller first so seller-id requirements can be tested too.
out=$(run_session "$(in_lines admin 1234 \
    2 1 Linker 555 addr "" \
    3 5)")
assert_contains "$out" "Seller added successfully" "seed seller for client tests"

# Add client with full data + requirements bound to seller 1.
out=$(run_session "$(in_lines admin 1234 \
    3 1 Alice "addr 1" 9811111111 alice@example.com \
    Kathmandu 500 2000 100000 500000 1 "" \
    3 5)")
assert_contains "$out" "Client added successfully" "add client with data succeeds"
assert_file_contains "clients.txt" "|Alice|addr 1|9811111111|alice@example.com|Kathmandu|500|2000|100000.00|500000.00|1" \
    "client record persisted with all fields"

# Add client with all-blank fields => N/A for strings, 0 for numerics.
out=$(run_session "$(in_lines admin 1234 \
    3 1 "" "" "" "" "" "" "" "" "" "" "" \
    3 5)")
assert_contains "$out" "Client added successfully" "add client with blanks succeeds"
assert_file_contains "clients.txt" "|N/A|N/A|N/A|N/A|N/A|0|0|0.00|0.00|0" \
    "blank fields stored as N/A / 0"

# Edit a client keeping unchanged fields blank.
reset_data
run_session "$(in_lines admin 1234 \
    2 1 SellerOne 111 here "" \
    3 5)" >/dev/null
out=$(run_session "$(in_lines admin 1234 \
    3 1 Bob "Bob Addr" 555 bob@example.com Pokhara 100 1000 5000 50000 0 "" \
    3 \
    3 2 2 1 Bobby "" "" "" "" "" "" "" "" "" "" \
    4 3 5)")
assert_contains "$out" "Client updated successfully" "update client succeeds"
assert_file_contains "clients.txt" "1|Bobby|Bob Addr|555|bob@example.com|Pokhara|100|1000|5000.00|50000.00|0" \
    "blank input on edit keeps current values"

# Update requirements only, leaving unchanged ones blank.
out=$(run_session "$(in_lines admin 1234 \
    3 2 3 1 "" "" 1500 "" "" 0 "" \
    4 3 5)")
assert_contains "$out" "Client requirements updated successfully" "requirements update succeeds"
assert_file_contains "clients.txt" "1|Bobby|Bob Addr|555|bob@example.com|Pokhara|100|1500|5000.00|50000.00|0" \
    "only changed requirement fields are updated"

# Delete a client.
out=$(run_session "$(in_lines admin 1234 \
    3 2 1 1 "" \
    4 3 5)")
assert_contains "$out" "Client deleted successfully" "delete client succeeds"
assert_file_line_count "clients.txt" 0 "clients.txt empty after delete"

# View renders key-value layout (with the new fields).
out=$(run_session "$(in_lines admin 1234 \
    3 1 Vista "vista addr" 1 v@x "" "" "" "" "" "" "" \
    3 \
    3 2 4 3 5)")
assert_contains "$out" "Pref Location:" "view shows Pref Location"
assert_contains "$out" "Area Range:" "view shows Area Range"
assert_contains "$out" "Price Range:" "view shows Price Range"

report_module
