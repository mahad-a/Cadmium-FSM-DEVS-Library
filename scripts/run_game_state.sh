#!/bin/bash
set -e
cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")"
cd bin && ./game_state
echo "done -> simulation_results/GameState_log.csv"
