#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_DIR/bin"

echo "=== moore fsms ==="
./ATM            && echo "  ATM"
./telephone      && echo "  telephone"
./vendor_moore   && echo "  vendor_moore"
./alarm_system   && echo "  alarm_system"
./traffic_light  && echo "  traffic_light"
./game_state     && echo "  game_state"
./parity_checker && echo "  parity_checker"
./boiling        && echo "  boiling"

echo ""
echo "=== mealy fsms ==="
./vendor_mealy      && echo "  vendor_mealy"
./turnstile         && echo "  turnstile"
./debounce          && echo "  debounce"
./sequence_detector && echo "  sequence_detector"

echo ""
echo "done -> $PROJECT_DIR/simulation_results/"
