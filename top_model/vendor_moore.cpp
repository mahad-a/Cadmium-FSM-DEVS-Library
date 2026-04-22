#include <fstream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include "cadmium/simulation/root_coordinator.hpp"
#include "cadmium/modeling/devs/coupled.hpp"
#include "cadmium/lib/iestream.hpp"
#include "../atomics/fsm_token.hpp"
#include "cadmium/simulation/logger/csv.hpp"

using namespace cadmium;

struct VendingMachine : public Coupled {

    VendingMachine(const std::string& id, const std::string& input_file)
        : Coupled(id)
    {
        // states: {code, value}, transitions: {event → next_code}
        auto zero    = addComponent<FsmToken>("zero",    true,  0,  0,
            std::vector<std::pair<int,int>>{{0,1},{1,2}});
        auto five    = addComponent<FsmToken>("five",    false, 1,  5,
            std::vector<std::pair<int,int>>{{0,2},{1,3}});
        auto ten     = addComponent<FsmToken>("ten",     false, 2, 10,
            std::vector<std::pair<int,int>>{{0,3},{1,4}});
        auto fifteen = addComponent<FsmToken>("fifteen", false, 3, 15,
            std::vector<std::pair<int,int>>{{0,4},{1,4},{2,0}});
        auto twenty  = addComponent<FsmToken>("twenty",  false, 4, 20,
            std::vector<std::pair<int,int>>{{3,0}});

        // input
        auto input = addComponent<lib::IEStream<int>>("input", input_file.c_str());

        // event broadcasts
        addCoupling(input->out, zero->event_in);
        addCoupling(input->out, five->event_in);
        addCoupling(input->out, ten->event_in);
        addCoupling(input->out, fifteen->event_in);
        addCoupling(input->out, twenty->event_in);

        // transitions
        addCoupling(zero->transition_out,    five->transition_in);
        addCoupling(zero->transition_out,    ten->transition_in);
        addCoupling(five->transition_out,    ten->transition_in);
        addCoupling(five->transition_out,    fifteen->transition_in);
        addCoupling(ten->transition_out,     fifteen->transition_in);
        addCoupling(ten->transition_out,     twenty->transition_in);
        addCoupling(fifteen->transition_out, twenty->transition_in);
        addCoupling(fifteen->transition_out, zero->transition_in);
        addCoupling(twenty->transition_out,  zero->transition_in);
    }
};

int main() {
    auto model = std::make_shared<VendingMachine>("vending_machine", "../input_data/vendor.txt");
    auto rc = cadmium::RootCoordinator(model, 0.0);
    rc.setLogger<cadmium::CSVLogger>("../simulation_results/Vendor_Moore_log.csv", ";");
    rc.start();
    rc.simulate(std::numeric_limits<double>::infinity());
    rc.stop();
    return 0;
}
