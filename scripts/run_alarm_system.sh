#!/bin/bash
set -e
cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")"
cd bin && ./alarm_system
echo "done -> simulation_results/AlarmSystem_log.csv"
