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

struct Turnstile : public Coupled {

    Turnstile(const std::string& id, const std::string& input_file)
        : Coupled(id)
    {
        // reset shared state
        FsmRule::lst    = 0;
        FsmRule::err_in = false;

        // states
        // locked (0): push self-loop, push-while-unlocked triggers acting (person passes)
        auto locked   = addComponent<FsmRule>("locked",   0, true,  0.001,
            std::vector<std::pair<int,int>>{{0,1},{1,1}},
            std::vector<std::pair<int,int>>{{1,1}});

        // unlocked (1): coin-while-locked triggers acting (gate opens), coin-while-unlocked is no-op
        auto unlocked = addComponent<FsmRule>("unlocked", 1, false, 0.001,
            std::vector<std::pair<int,int>>{{0,0},{1,0}},
            std::vector<std::pair<int,int>>{{0,0}});

        // input
        auto input = addComponent<lib::IEStream<int>>("input", input_file.c_str());

        // event broadcasts
        addCoupling(input->out, locked->event_in);
        addCoupling(input->out, unlocked->event_in);

        // transitions
        addCoupling(locked->out,   locked->transition_in);
        addCoupling(locked->out,   unlocked->transition_in);
        addCoupling(unlocked->out, locked->transition_in);
        addCoupling(unlocked->out, unlocked->transition_in);
    }
};

int main() {
    auto model = std::make_shared<Turnstile>(
        "turnstile",
        "../input_data/turnstile.txt"
    );
    auto rc = cadmium::RootCoordinator(model, 0.0);
    rc.setLogger<cadmium::CSVLogger>("../simulation_results/Turnstile_log.csv", ";");
    rc.start();
    rc.simulate(std::numeric_limits<double>::infinity());
    rc.stop();
    return 0;
}
