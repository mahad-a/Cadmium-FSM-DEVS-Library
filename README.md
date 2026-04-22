# Cadmium FSM Library

A library of Finite State Machines built in Cadmium v2. Includes two atomic models — `FsmToken` (token-passing, Moore) and `FsmRule` (rule-based, Moore and Mealy) — and twelve FSM simulations built from them.

## Requirements

- g++ with C++17 support
- Cadmium v2 headers
- The `CADMIUM` environment variable set to your Cadmium v2 include directory

```bash
export CADMIUM=/path/to/cadmium_v2/include
```

## Building and Running

The easiest way to build and run everything at once is with the provided script:

```bash
./build_sim.sh
```

This will clean any previous build, compile all simulators, run all twelve simulations, and write the results to `simulation_results/`.

## Makefile Targets

If you prefer to run steps individually:

| Command | What it does |
|---|---|
| `make simulator` | Compiles all twelve FSM simulators into `bin/` |
| `make tests` | Compiles the two unit tests into `bin/` |
| `make all` | Compiles simulators and tests |
| `make run_all` | Builds and runs all simulators, writes CSV logs to `simulation_results/` via `scripts/run_all.sh` |
| `make clean` | Removes all build artifacts, binaries, and simulation results |

## Project Structure

```
atomics/            — FsmToken and FsmRule atomic model headers
top_model/          — one .cpp per FSM simulation
input_data/         — input event files (.txt) for each simulation
simulation_results/ — CSV output logs generated after running
test/               — unit tests for the atomic models
scripts/            — run_all.sh helper script
```

## Scripts

All scripts live in `scripts/` and assume the simulators have already been built via `make simulator` or `build_sim.sh`.

**`run_all.sh`**
Runs every simulation in sequence — all twelve models one after another. This is the script that `build_sim.sh` and `make run_all` call internally. Use this when you want to regenerate all CSV logs in one go.

**Individual run scripts (`run_ATM.sh`, `run_traffic_light.sh`, etc.)**
Each script runs a single FSM simulation and writes its CSV log to `simulation_results/`. Useful when you only want to re-run one model without touching the rest. There is one script per model:

- `run_ATM.sh` → `ATM_log.csv`
- `run_telephone.sh` → `Telephone_log.csv`
- `run_vendor_moore.sh` → `Vendor_Moore_log.csv`
- `run_alarm_system.sh` → `AlarmSystem_log.csv`
- `run_traffic_light.sh` → `TrafficLight_log.csv`
- `run_game_state.sh` → `GameState_log.csv`
- `run_parity_checker.sh` → `Parity_Checker_log.csv`
- `run_boiling.sh` → `Boiling_log.csv`
- `run_vendor_mealy.sh` → `Vendor_Mealy_log.csv`
- `run_turnstile.sh` → `Turnstile_log.csv`
- `run_debounce.sh` → `Debounce_log.csv`
- `run_sequence_detector.sh` → `SequenceDetector_log.csv`

## Models Included

**Moore — FsmToken (token-passing)**
- ATM Transaction Controller
- Telephone Call Handler
- Vending Machine (Moore)
- Alarm System
- Traffic Light Controller
- Game State Controller

**Moore and Mealy — FsmRule (rule-based)**
- Boiling Water Controller
- Parity Checker
- Vending Machine (Mealy)
- Turnstile
- Debounce Filter
- Sequence Detector "101"
