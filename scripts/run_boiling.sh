#!/bin/bash
set -e
cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")"
cd bin && ./boiling
echo "done -> simulation_results/Boiling_log.csv"
