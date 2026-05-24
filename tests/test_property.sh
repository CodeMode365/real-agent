#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"
MODULE_NAME=property
echo "=== Testing Property module ==="

reset_data

# Seed a seller; property add requires a valid seller id.
out=$(run_session "$(in_lines admin 1234 \
    2 1 SellerA 9800 addrA "" \
    3 5)")
assert_contains "$out" "Seller added successfully" "seed seller for property tests"

# Add a property with valid data tied to seller 1.
out=$(run_session "$(in_lines admin 1234 \
    1 1 House Kathmandu 5000000 1200 1 "" \
    4 5)")
assert_contains "$out" "Property added successfully" "add property with valid seller"
assert_file_contains "properties.txt" "|House|Kathmandu|5000000.00|1200|1|Available" \
    "property record persisted, status defaults to Available"

# Add with invalid seller id -> rejected, file unchanged.
lines_before=$(wc -l < "$REPO_ROOT/properties.txt" | tr -d ' ')
out=$(run_session "$(in_lines admin 1234 \
    1 1 Land Bhaktapur 100000 500 9999 "" \
    4 5)")
assert_contains "$out" "Seller ID not found" "unknown seller id rejected"
lines_after=$(wc -l < "$REPO_ROOT/properties.txt" | tr -d ' ')
if [ "$lines_after" = "$lines_before" ]; then
    bump_pass "properties.txt unchanged when seller id invalid"
else
    bump_fail "properties.txt grew despite invalid seller id ($lines_before -> $lines_after)"
fi

# Blank text fields become N/A; blank numeric fields become 0.
out=$(run_session "$(in_lines admin 1234 \
    1 1 "" "" "" "" 1 "" \
    4 5)")
assert_file_contains "properties.txt" "|N/A|N/A|0.00|0|1|Available" \
    "blank text -> N/A and blank numeric -> 0 on add"

# View renders key-value blocks separated by lines.
out=$(run_session "$(in_lines admin 1234 \
    1 2 3 4 5)")
assert_contains "$out" "Type:" "view shows Type key"
assert_contains "$out" "Location:" "view shows Location key"
assert_contains "$out" "Price:" "view shows Price key"
assert_contains "$out" "Size (sqft):" "view shows Size key"
assert_contains "$out" "Status:" "view shows Status key"
assert_not_contains "$out" "ID      Type" "view does not use the old table header"

# Edit keeping blanks should preserve current values; sold-status update goes through.
reset_data
out=$(run_session "$(in_lines admin 1234 \
    2 1 S1 1 addr "" \
    3 \
    1 1 House Pokhara 1000000 800 1 "" \
    2 2 1 "" "" "" "" "" Sold "" \
    3 4 5)")
assert_contains "$out" "Property updated successfully" "update property succeeds"
assert_file_contains "properties.txt" "1|House|Pokhara|1000000.00|800|1|Sold" \
    "blank edit keeps previous values, status changes to Sold"

# Search by client requirements: matches only available properties for that client.
reset_data
# 1 seller, 1 client with requirements, 2 properties (one matching, one not).
run_session "$(in_lines admin 1234 \
    2 1 S1 1 addr "" \
    3 5)" >/dev/null
run_session "$(in_lines admin 1234 \
    3 1 Alice addr 1 a@x Pokhara 500 2000 100000 2000000 0 "" \
    3 5)" >/dev/null
run_session "$(in_lines admin 1234 \
    1 1 House Pokhara 1500000 1000 1 "" \
    1 House Kathmandu 500000 600 1 "" \
    4 5)" >/dev/null

out=$(run_session "$(in_lines admin 1234 4 1 "")")
assert_contains "$out" "Pokhara" "matching Pokhara property is returned"
assert_not_contains "$out" "Kathmandu" "non-matching Kathmandu property is excluded"

# Delete a property.
out=$(run_session "$(in_lines admin 1234 \
    1 2 1 1 "" \
    3 4 5)")
assert_contains "$out" "Property deleted successfully" "delete property succeeds"

report_module
