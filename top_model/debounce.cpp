#include <fstream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include "cadmium/simulation/root_coordinator.hpp"
#include "cadmium/modeling/devs/coupled.hpp"
#include "cadmium/lib/iestream.hpp"
#include "../atomics/fsm_rule.hpp"
#include "cadmium/simulation/logger/csv.hpp"

using namespace cadmium;

struct DebounceFSM : public Coupled {

    DebounceFSM(const std::string& id, const std::string& input_file)
        : Coupled(id)
    {
        // reset shared state
        FsmRule::lst    = 0;
        FsmRule::err_in = false;

        // states
        // idle fires on low self-loop, bounce (acting), and confirmed fall (acting)
        auto idle        = addComponent<FsmRule>("idle",        0, true,  0.001,
            std::vector<std::pair<int,int>>{{0,0},{1,0},{2,0}},
            std::vector<std::pair<int,int>>{{1,0},{2,0}});

        // debouncing fires on first high reading (no action yet)
        auto debouncing  = addComponent<FsmRule>("debouncing",  1, false, 0.001,
            std::vector<std::pair<int,int>>{{0,1}});

        // stable_high fires on confirmed rise (acting) and high self-loop
        auto stable_high = addComponent<FsmRule>("stable_high", 2, false, 0.001,
            std::vector<std::pair<int,int>>{{1,1},{2,1}},
            std::vector<std::pair<int,int>>{{1,1}});

        // input
        auto input = addComponent<lib::IEStream<int>>("input", input_file.c_str());

        // event broadcasts
        addCoupling(input->out, idle->event_in);
        addCoupling(input->out, debouncing->event_in);
        addCoupling(input->out, stable_high->event_in);

        // transitions
        addCoupling(idle->out,        idle->transition_in);
        addCoupling(idle->out,        debouncing->transition_in);

        addCoupling(debouncing->out,  idle->transition_in);
        addCoupling(debouncing->out,  stable_high->transition_in);

        addCoupling(stable_high->out, idle->transition_in);
        addCoupling(stable_high->out, stable_high->transition_in);
    }
};

int main() {
    auto model = std::make_shared<DebounceFSM>(
        "debounce",
        "../input_data/debounce.txt"
    );
    auto rc = cadmium::RootCoordinator(model, 0.0);
    rc.setLogger<cadmium::CSVLogger>("../simulation_results/Debounce_log.csv", ";");
    rc.start();
    rc.simulate(std::numeric_limits<double>::infinity());
    rc.stop();
    return 0;
}
