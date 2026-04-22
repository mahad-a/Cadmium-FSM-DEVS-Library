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

struct VendingMachineFSM1 : public Coupled {

    VendingMachineFSM1(const std::string& id, const std::string& input_file)
        : Coupled(id)
    {
        // reset shared state
        FsmRule::lst    = 0;
        FsmRule::err_in = false;

        // states; zero fires acting on refund and dispense events
        auto zero    = addComponent<FsmRule>("zero",    0, true,  0.001,
            std::vector<std::pair<int,int>>{{3,2},{4,3}},
            std::vector<std::pair<int,int>>{{3,2},{4,3}});

        auto five    = addComponent<FsmRule>("five",    1, false, 0.001,
            std::vector<std::pair<int,int>>{{0,0}});

        auto ten     = addComponent<FsmRule>("ten",     2, false, 0.001,
            std::vector<std::pair<int,int>>{{0,1},{1,0}});

        auto fifteen = addComponent<FsmRule>("fifteen", 3, false, 0.001,
            std::vector<std::pair<int,int>>{{1,1}});

        auto twenty  = addComponent<FsmRule>("twenty",  4, false, 0.001,
            std::vector<std::pair<int,int>>{{2,1},{3,0}});

        // input
        auto input = addComponent<lib::IEStream<int>>("input", input_file.c_str());

        // event broadcasts
        addCoupling(input->out, zero->event_in);
        addCoupling(input->out, five->event_in);
        addCoupling(input->out, ten->event_in);
        addCoupling(input->out, fifteen->event_in);
        addCoupling(input->out, twenty->event_in);

        // transitions
        addCoupling(zero->out,    five->transition_in);
        addCoupling(zero->out,    ten->transition_in);

        addCoupling(five->out,    ten->transition_in);
        addCoupling(five->out,    fifteen->transition_in);

        addCoupling(ten->out,     twenty->transition_in);

        addCoupling(fifteen->out, zero->transition_in);
        addCoupling(fifteen->out, twenty->transition_in);

        addCoupling(twenty->out,  zero->transition_in);
    }
};

int main() {
    auto model = std::make_shared<VendingMachineFSM1>(
        "vending_machine_fsm1",
        "../input_data/vender_fsm1.txt"
    );
    auto rc = cadmium::RootCoordinator(model, 0.0);
    rc.setLogger<cadmium::CSVLogger>("../simulation_results/Vendor_Mealy_log.csv", ";");
    rc.start();
    rc.simulate(std::numeric_limits<double>::infinity());
    rc.stop();
    return 0;
}
