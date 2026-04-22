CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I ${CADMIUM}

bin_folder    := $(shell mkdir -p bin)
build_folder  := $(shell mkdir -p build)
results_folder:= $(shell mkdir -p simulation_results)

ATM.o: top_model/ATM.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) top_model/ATM.cpp -o build/ATM.o

telephone.o: top_model/telephone.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) top_model/telephone.cpp -o build/telephone.o

vendor_moore.o: top_model/vendor_moore.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) top_model/vendor_moore.cpp -o build/vendor_moore.o

parity_checker.o: top_model/parity_checker.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) top_model/parity_checker.cpp -o build/parity_checker.o

boiling.o: top_model/boiling.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) top_model/boiling.cpp -o build/boiling.o

vendor_mealy.o: top_model/vendor_mealy.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) top_model/vendor_mealy.cpp -o build/vendor_mealy.o

alarm_system.o: top_model/alarm_system.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) top_model/alarm_system.cpp -o build/alarm_system.o

traffic_light.o: top_model/traffic_light.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) top_model/traffic_light.cpp -o build/traffic_light.o

game_state.o: top_model/game_state.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) top_model/game_state.cpp -o build/game_state.o

turnstile.o: top_model/turnstile.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) top_model/turnstile.cpp -o build/turnstile.o

debounce.o: top_model/debounce.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) top_model/debounce.cpp -o build/debounce.o

sequence_detector.o: top_model/sequence_detector.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) top_model/sequence_detector.cpp -o build/sequence_detector.o

simulator: ATM.o telephone.o vendor_moore.o parity_checker.o boiling.o vendor_mealy.o alarm_system.o traffic_light.o game_state.o turnstile.o debounce.o sequence_detector.o
	$(CC) -g -o bin/ATM build/ATM.o
	$(CC) -g -o bin/telephone build/telephone.o
	$(CC) -g -o bin/vendor_moore build/vendor_moore.o
	$(CC) -g -o bin/parity_checker build/parity_checker.o
	$(CC) -g -o bin/boiling build/boiling.o
	$(CC) -g -o bin/vendor_mealy build/vendor_mealy.o
	$(CC) -g -o bin/alarm_system build/alarm_system.o
	$(CC) -g -o bin/traffic_light build/traffic_light.o
	$(CC) -g -o bin/game_state build/game_state.o
	$(CC) -g -o bin/turnstile build/turnstile.o
	$(CC) -g -o bin/debounce build/debounce.o
	$(CC) -g -o bin/sequence_detector build/sequence_detector.o

all: simulator

run_all: simulator
	bash scripts/run_all.sh

run: simulator
	cd bin && ./ATM
	cd bin && ./telephone
	cd bin && ./vendor_moore
	cd bin && ./parity_checker
	cd bin && ./boiling
	cd bin && ./vendor_mealy
	cd bin && ./alarm_system
	cd bin && ./traffic_light
	cd bin && ./game_state
	cd bin && ./turnstile
	cd bin && ./debounce
	cd bin && ./sequence_detector

clean:
	rm -f bin/* build/* simulation_results/*
