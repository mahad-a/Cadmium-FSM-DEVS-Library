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

struct ParityChecker : public Coupled {

    ParityChecker(const std::string& id, const std::string& input_file)
        : Coupled(id)
    {
        // reset shared state
        FsmRule::lst    = 0;
        FsmRule::err_in = false;

        // states: {last_state, event_code} rules
        auto even = addComponent<FsmRule>("even", 0, true,  0.001,
            std::vector<std::pair<int,int>>{{0,0},{1,1}});
        auto odd  = addComponent<FsmRule>("odd",  1, false, 0.001,
            std::vector<std::pair<int,int>>{{0,1},{1,0}});

        // input
        auto input = addComponent<lib::IEStream<int>>("input", input_file.c_str());

        // event broadcasts
        addCoupling(input->out, even->event_in);
        addCoupling(input->out, odd->event_in);

        // transitions — both states arm each other after every transition
        addCoupling(even->out, even->transition_in);
        addCoupling(even->out, odd->transition_in);
        addCoupling(odd->out,  even->transition_in);
        addCoupling(odd->out,  odd->transition_in);
    }
};

int main() {
    auto model = std::make_shared<ParityChecker>(
        "mealy_parity_checker",
        "../input_data/mealy.txt"
    );
    auto rc = cadmium::RootCoordinator(model, 0.0);
    rc.setLogger<cadmium::CSVLogger>("../simulation_results/Parity_Checker_log.csv", ";");
    rc.start();
    rc.simulate(std::numeric_limits<double>::infinity());
    rc.stop();
    return 0;
}
