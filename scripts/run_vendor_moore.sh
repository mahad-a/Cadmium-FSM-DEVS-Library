#!/bin/bash
set -e
cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")"
cd bin && ./vendor_moore
echo "done -> simulation_results/Vendor_Moore_log.csv"
