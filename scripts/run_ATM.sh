#!/bin/bash
set -e
cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")"
cd bin && ./ATM
echo "done -> simulation_results/ATM_log.csv"
