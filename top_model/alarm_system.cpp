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

struct AlarmSystem : public Coupled {

    AlarmSystem(const std::string& id, const std::string& input_file)
        : Coupled(id)
    {
        // states: {code, value}, transitions: {event → next_code}
        auto disarmed  = addComponent<FsmToken>("disarmed",  true,  0, 0,
            std::vector<std::pair<int,int>>{{0,1}});
        auto armed     = addComponent<FsmToken>("armed",     false, 1, 1,
            std::vector<std::pair<int,int>>{{1,0},{2,2}});
        auto triggered = addComponent<FsmToken>("triggered", false, 2, 2,
            std::vector<std::pair<int,int>>{{1,0},{3,0}});

        // input
        auto input = addComponent<lib::IEStream<int>>("input", input_file.c_str());

        // event broadcasts
        addCoupling(input->out, disarmed->event_in);
        addCoupling(input->out, armed->event_in);
        addCoupling(input->out, triggered->event_in);

        // transitions
        addCoupling(disarmed->transition_out,  armed->transition_in);

        addCoupling(armed->transition_out,     disarmed->transition_in);
        addCoupling(armed->transition_out,     triggered->transition_in);

        addCoupling(triggered->transition_out, disarmed->transition_in);
    }
};

int main() {
    auto model = std::make_shared<AlarmSystem>(
        "alarm_system",
        "../input_data/alarm_system.txt"
    );
    auto rc = cadmium::RootCoordinator(model, 0.0);
    rc.setLogger<cadmium::CSVLogger>("../simulation_results/AlarmSystem_log.csv", ";");
    rc.start();
    rc.simulate(std::numeric_limits<double>::infinity());
    rc.stop();
    return 0;
}
