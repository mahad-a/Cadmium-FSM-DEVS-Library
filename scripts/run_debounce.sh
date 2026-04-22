#!/bin/bash
set -e
cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")"
cd bin && ./debounce
echo "done -> simulation_results/Debounce_log.csv"
