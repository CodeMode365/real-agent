#!/usr/bin/env bash
# Random-input fuzzer. Feeds N sessions of random lines into the binary and
# checks that it never crashes (signal exit > 128) or hangs.
#
# Env knobs:
#   FUZZ_ITERATIONS - number of sessions (default 50)
#   FUZZ_LINES      - lines per session (default 80)
#   SESSION_TIMEOUT - seconds before a session is killed (default 10)
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"
MODULE_NAME=fuzz
echo "=== Fuzzing random inputs ==="

FUZZ_ITERATIONS="${FUZZ_ITERATIONS:-50}"
FUZZ_LINES="${FUZZ_LINES:-80}"

random_line() {
    case $((RANDOM % 10)) in
        0) printf '\n' ;;                                   # blank
        1) printf '%d\n' "$((RANDOM % 10))" ;;              # small int
        2) printf '%d\n' "$((RANDOM - 16000))" ;;           # signed int (incl negatives)
        3) printf '%d.%d\n' "$((RANDOM % 100))" "$((RANDOM % 1000))" ;;  # float
        4) printf '5\n' ;;                                  # tries to logout
        5) printf '%s\n' "abc$((RANDOM))def" ;;             # mixed alpha+digit
        6) printf '   spaced   \n' ;;                       # leading/trailing spaces
        7) printf 'pipe|chars|here\n' ;;                    # pipe injection
        8) printf '%s\n' "$(head -c 200 /dev/urandom | tr -dc 'a-zA-Z0-9 ./_-')" ;;  # long-ish
        9) printf '\xc2\xa1unicode\xe2\x9c\x93\n' ;;        # non-ASCII
    esac
}

generate_session() {
    # Start with a valid login so we get past the gate; then random soup.
    printf 'admin\n1234\n'
    local i
    for ((i = 0; i < FUZZ_LINES; i++)); do
        random_line
    done
    # Try hard to terminate via logout choice from main menu.
    printf '5\n5\n5\n5\n'
}

reset_data

crashes=0
runs=0
for ((iter = 1; iter <= FUZZ_ITERATIONS; iter++)); do
    runs=$((runs + 1))
    status=0
    generate_session | timeout "$SESSION_TIMEOUT" "$BINARY" >/dev/null 2>&1 || status=$?
    if [ "$status" -ge 128 ] && [ "$status" -ne 130 ]; then
        # 130 = SIGINT; 124 = timeout (handled separately). Anything >=128 is a fatal signal.
        crashes=$((crashes + 1))
        printf '  [CRASH] iter %d exited %d\n' "$iter" "$status"
    elif [ "$status" -eq 124 ]; then
        crashes=$((crashes + 1))
        printf '  [HANG ] iter %d timed out\n' "$iter"
    fi
done

if [ "$crashes" -eq 0 ]; then
    bump_pass "$runs random sessions ran without crashing or hanging"
else
    MODULE_CRASHED=$((MODULE_CRASHED + crashes))
    bump_fail "$crashes / $runs random sessions crashed or hung"
fi

# After fuzzing, every line in each data file must still be parseable
# (i.e. have the expected number of pipe-separated fields).
check_format() {
    local file="$1" expected_pipes="$2" name="$3"
    cd "$REPO_ROOT"
    [ -f "$file" ] || { bump_pass "$name file absent after fuzz (ok)"; return; }
    local bad=0 total=0
    while IFS= read -r line; do
        [ -z "$line" ] && continue
        total=$((total + 1))
        local pipes
        pipes=$(printf '%s' "$line" | tr -cd '|' | wc -c | tr -d ' ')
        [ "$pipes" -eq "$expected_pipes" ] || bad=$((bad + 1))
    done < "$file"
    if [ "$bad" -eq 0 ]; then
        bump_pass "$name file intact after fuzz ($total records)"
    else
        bump_fail "$name file has $bad malformed records out of $total"
    fi
}

check_format sellers.txt    3  "sellers.txt"
check_format clients.txt    10 "clients.txt"
check_format properties.txt 6  "properties.txt"

report_module
