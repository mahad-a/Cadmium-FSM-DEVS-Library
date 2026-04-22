#!/bin/bash
set -e
cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")"
cd bin && ./parity_checker
echo "done -> simulation_results/Parity_Checker_log.csv"
