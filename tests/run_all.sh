#!/usr/bin/env bash
# Orchestrator: backs up live data, runs every module test, prints analytics.
#
# Usage:
#   bash tests/run_all.sh                # runs everything including fuzzer
#   SKIP_FUZZ=1 bash tests/run_all.sh    # skip the fuzzer
#   FUZZ_ITERATIONS=200 bash tests/run_all.sh

set -u

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
export BINARY="$REPO_ROOT/build/Debug/outDebug"

if [ ! -x "$BINARY" ]; then
    echo "Binary not found at $BINARY. Building..."
    (cd "$REPO_ROOT" && make "build/Debug/outDebug") || {
        echo "Build failed."
        exit 1
    }
fi

# Backup the user's real data so tests run on a clean slate.
DATA_FILES=(sellers.txt clients.txt properties.txt sellerID.txt clientID.txt propertyID.txt temp.txt)
BACKUP_DIR="$(mktemp -d "${TMPDIR:-/tmp}/real-agent-data.XXXXXX")"
for f in "${DATA_FILES[@]}"; do
    if [ -f "$REPO_ROOT/$f" ]; then
        cp "$REPO_ROOT/$f" "$BACKUP_DIR/$f"
    fi
done

restore() {
    for f in "${DATA_FILES[@]}"; do
        rm -f "$REPO_ROOT/$f"
        if [ -f "$BACKUP_DIR/$f" ]; then
            cp "$BACKUP_DIR/$f" "$REPO_ROOT/$f"
        fi
    done
    rm -rf "$BACKUP_DIR"
}
trap restore EXIT

# Per-module result sink shared via env var.
export RESULTS_DIR="$(mktemp -d "${TMPDIR:-/tmp}/real-agent-tests.XXXXXX")"
: > "$RESULTS_DIR/results.tsv"
: > "$RESULTS_DIR/timing.tsv"

run_one() {
    local script="$1" name="$2"
    local start end elapsed
    start=$(date +%s.%N)
    bash "$script" || true
    end=$(date +%s.%N)
    elapsed=$(awk -v a="$start" -v b="$end" 'BEGIN { printf "%.3f", b - a }')
    printf '%s\t%s\n' "$name" "$elapsed" >> "$RESULTS_DIR/timing.tsv"
}

OVERALL_START=$(date +%s.%N)

run_one "$SCRIPT_DIR/test_admin.sh"    admin
run_one "$SCRIPT_DIR/test_seller.sh"   seller
run_one "$SCRIPT_DIR/test_client.sh"   client
run_one "$SCRIPT_DIR/test_property.sh" property
run_one "$SCRIPT_DIR/test_menu.sh"     menu

if [ -z "${SKIP_FUZZ:-}" ]; then
    run_one "$SCRIPT_DIR/fuzz.sh" fuzz
fi

OVERALL_END=$(date +%s.%N)
OVERALL_ELAPSED=$(awk -v a="$OVERALL_START" -v b="$OVERALL_END" \
    'BEGIN { printf "%.3f", b - a }')

echo
echo "============================================================"
echo "                       TEST ANALYTICS"
echo "============================================================"
awk -F'\t' \
    -v timing_file="$RESULTS_DIR/timing.tsv" '
    BEGIN {
        while ((getline line < timing_file) > 0) {
            n = split(line, t, "\t")
            if (n >= 2) timings[t[1]] = t[2]
        }
        close(timing_file)
        printf "%-10s %8s %8s %8s %8s %10s\n",
               "Module", "Tests", "Passed", "Failed", "Crashed", "Time(s)"
        printf "------------------------------------------------------------\n"
    }
    {
        total = $2 + $3
        time_val = (($1 in timings) ? timings[$1] : "n/a")
        printf "%-10s %8d %8d %8d %8d %10s\n",
               $1, total, $2, $3, $4, time_val
        sum_tests   += total
        sum_passed  += $2
        sum_failed  += $3
        sum_crashed += $4
    }
    END {
        printf "------------------------------------------------------------\n"
        printf "%-10s %8d %8d %8d %8d %10s\n",
               "TOTAL", sum_tests, sum_passed, sum_failed, sum_crashed, "-"
        if (sum_tests > 0) {
            pct = 100 * sum_passed / sum_tests
            printf "\nPass rate: %.1f%% (%d / %d)\n", pct, sum_passed, sum_tests
        }
    }
' "$RESULTS_DIR/results.tsv"

printf "Wall time: %ss\n" "$OVERALL_ELAPSED"
echo "============================================================"

# Exit non-zero if any test failed or any session crashed.
fail_count=$(awk -F'\t' '{ f += $3; c += $4 } END { print f + c + 0 }' \
    "$RESULTS_DIR/results.tsv")

rm -rf "$RESULTS_DIR"

if [ "$fail_count" -gt 0 ]; then
    exit 1
fi
