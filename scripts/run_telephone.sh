#!/bin/bash
set -e
cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")"
cd bin && ./telephone
echo "done -> simulation_results/Telephone_log.csv"
