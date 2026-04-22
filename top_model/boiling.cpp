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

struct BoilingWater : public Coupled {

    BoilingWater(const std::string& id, const std::string& input_file)
        : Coupled(id)
    {
        // reset shared state
        FsmRule::lst    = 0;
        FsmRule::err_in = false;

        // states
        auto cold    = addComponent<FsmRule>("cold",    0, true,  0.001,
            std::vector<std::pair<int,int>>{{3,3}});

        auto heating = addComponent<FsmRule>("heating", 1, false, 0.001,
            std::vector<std::pair<int,int>>{{0,0},{1,0}});

        auto boiling = addComponent<FsmRule>("boiling", 2, false, 0.001,
            std::vector<std::pair<int,int>>{{1,2},{2,0}});

        auto cooling = addComponent<FsmRule>("cooling", 3, false, 0.001,
            std::vector<std::pair<int,int>>{{1,1},{2,1},{3,1}});

        // input
        auto input = addComponent<lib::IEStream<int>>("input", input_file.c_str());

        // event broadcasts
        addCoupling(input->out, cold->event_in);
        addCoupling(input->out, heating->event_in);
        addCoupling(input->out, boiling->event_in);
        addCoupling(input->out, cooling->event_in);

        // transitions
        addCoupling(cold->out,    heating->transition_in);

        addCoupling(heating->out, boiling->transition_in);
        addCoupling(heating->out, heating->transition_in);
        addCoupling(heating->out, cooling->transition_in);

        addCoupling(boiling->out, cooling->transition_in);
        addCoupling(boiling->out, boiling->transition_in);

        addCoupling(cooling->out, cooling->transition_in);
        addCoupling(cooling->out, heating->transition_in);
        addCoupling(cooling->out, cold->transition_in);
    }
};

int main() {
    auto model = std::make_shared<BoilingWater>(
        "boiling_water",
        "../input_data/boiling.txt"
    );
    auto rc = cadmium::RootCoordinator(model, 0.0);
    rc.setLogger<cadmium::CSVLogger>("../simulation_results/Boiling_log.csv", ";");
    rc.start();
    rc.simulate(std::numeric_limits<double>::infinity());
    rc.stop();
    return 0;
}
