#!/bin/bash
set -e
cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")"
cd bin && ./vendor_mealy
echo "done -> simulation_results/Vendor_Mealy_log.csv"
