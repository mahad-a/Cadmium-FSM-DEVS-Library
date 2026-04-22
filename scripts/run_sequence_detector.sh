#!/bin/bash
set -e
cd "$(dirname "$(dirname "${BASH_SOURCE[0]}")")"
cd bin && ./sequence_detector
echo "done -> simulation_results/SequenceDetector_log.csv"
