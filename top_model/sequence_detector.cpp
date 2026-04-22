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

struct SequenceDetector : public Coupled {

    SequenceDetector(const std::string& id, const std::string& input_file)
        : Coupled(id)
    {
        // reset shared state
        FsmRule::lst    = 0;
        FsmRule::err_in = false;

        // states tracking progress toward "101"; overlapping detection supported
        auto s0 = addComponent<FsmRule>("s0", 0, true,  0.001,
            std::vector<std::pair<int,int>>{{0,0},{2,0}});

        auto s1 = addComponent<FsmRule>("s1", 1, false, 0.001,
            std::vector<std::pair<int,int>>{{0,1},{1,1},{3,1}});

        auto s2 = addComponent<FsmRule>("s2", 2, false, 0.001,
            std::vector<std::pair<int,int>>{{1,0},{3,0}});

        // s3: pattern "101" detected — acting fires on this transition
        auto s3 = addComponent<FsmRule>("s3", 3, false, 0.001,
            std::vector<std::pair<int,int>>{{2,1}},
            std::vector<std::pair<int,int>>{{2,1}});

        // input
        auto input = addComponent<lib::IEStream<int>>("input", input_file.c_str());

        // event broadcasts
        addCoupling(input->out, s0->event_in);
        addCoupling(input->out, s1->event_in);
        addCoupling(input->out, s2->event_in);
        addCoupling(input->out, s3->event_in);

        // transitions
        addCoupling(s0->out, s0->transition_in);
        addCoupling(s0->out, s1->transition_in);

        addCoupling(s1->out, s1->transition_in);
        addCoupling(s1->out, s2->transition_in);

        addCoupling(s2->out, s0->transition_in);
        addCoupling(s2->out, s3->transition_in);

        addCoupling(s3->out, s1->transition_in);
        addCoupling(s3->out, s2->transition_in);
    }
};

int main() {
    auto model = std::make_shared<SequenceDetector>(
        "sequence_detector",
        "../input_data/sequence_detector.txt"
    );
    auto rc = cadmium::RootCoordinator(model, 0.0);
    rc.setLogger<cadmium::CSVLogger>("../simulation_results/SequenceDetector_log.csv", ";");
    rc.start();
    rc.simulate(std::numeric_limits<double>::infinity());
    rc.stop();
    return 0;
}
