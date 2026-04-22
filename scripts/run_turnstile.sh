#!/bin/bash
set -e
cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")"
cd bin && ./turnstile
echo "done -> simulation_results/Turnstile_log.csv"
