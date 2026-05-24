#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"
MODULE_NAME=menu
echo "=== Testing Menu navigation ==="

reset_data

# Invalid choices show 'Invalid' and re-prompt.
# Each invalid choice also consumes one extra line via waitForEnter, so we feed
# trailing 5s to make sure the logout path actually runs.
out=$(run_session "$(in_lines admin 1234 \
    99 "" abc "" "" "" 5)")
assert_contains "$out" "Invalid choice" "garbage main-menu input is rejected"
assert_contains "$out" "Program ended" "menu still exits cleanly after invalid input"

# Navigate into each submenu and back without doing anything.
out=$(run_session "$(in_lines admin 1234 \
    1 4 \
    2 3 \
    3 3 \
    5)")
assert_contains "$out" "Property Menu" "property submenu visited"
assert_contains "$out" "Seller Menu" "seller submenu visited"
assert_contains "$out" "Client Menu" "client submenu visited"
assert_contains "$out" "Program ended" "logout exits from main menu"

# Submenu invalid choice is also rejected without crashing.
out=$(run_session "$(in_lines admin 1234 \
    2 42 "" 3 5)")
assert_contains "$out" "Invalid choice" "seller submenu rejects garbage"

# Headers contain ASCII borders so the menu renders on Windows consoles.
out=$(run_session "$(in_lines admin 1234 5)")
assert_contains "$out" "+======" "header uses ASCII borders"
assert_contains "$out" "Main Menu" "main menu header rendered"

report_module
