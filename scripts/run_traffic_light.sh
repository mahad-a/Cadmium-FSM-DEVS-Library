#!/bin/bash
set -e
cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")"
cd bin && ./traffic_light
echo "done -> simulation_results/TrafficLight_log.csv"
