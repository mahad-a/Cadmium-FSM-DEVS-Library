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

struct TrafficLight : public Coupled {

    TrafficLight(const std::string& id, const std::string& input_file)
        : Coupled(id)
    {
        // states: {code, value}, transitions: {event → next_code}
        auto red        = addComponent<FsmToken>("red",        true,  0, 0,
            std::vector<std::pair<int,int>>{{0,1}});
        auto green      = addComponent<FsmToken>("green",      false, 1, 1,
            std::vector<std::pair<int,int>>{{1,2},{2,3}});
        auto yellow     = addComponent<FsmToken>("yellow",     false, 2, 2,
            std::vector<std::pair<int,int>>{{3,0}});
        auto ped_yellow = addComponent<FsmToken>("ped_yellow", false, 3, 3,
            std::vector<std::pair<int,int>>{{3,0}});

        // input
        auto input = addComponent<lib::IEStream<int>>("input", input_file.c_str());

        // event broadcasts
        addCoupling(input->out, red->event_in);
        addCoupling(input->out, green->event_in);
        addCoupling(input->out, yellow->event_in);
        addCoupling(input->out, ped_yellow->event_in);

        // transitions
        addCoupling(red->transition_out,        green->transition_in);

        addCoupling(green->transition_out,      yellow->transition_in);
        addCoupling(green->transition_out,      ped_yellow->transition_in);

        addCoupling(yellow->transition_out,     red->transition_in);
        addCoupling(ped_yellow->transition_out, red->transition_in);
    }
};

int main() {
    auto model = std::make_shared<TrafficLight>(
        "traffic_light",
        "../input_data/traffic_light.txt"
    );
    auto rc = cadmium::RootCoordinator(model, 0.0);
    rc.setLogger<cadmium::CSVLogger>("../simulation_results/TrafficLight_log.csv", ";");
    rc.start();
    rc.simulate(std::numeric_limits<double>::infinity());
    rc.stop();
    return 0;
}
