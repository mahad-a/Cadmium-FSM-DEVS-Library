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

struct Telephone : public Coupled {

    Telephone(const std::string& id, const std::string& input_file)
        : Coupled(id)
    {
        // states: {code, value}, transitions: {event → next_code}
        auto idle          = addComponent<FsmToken>("idle",          true,  0, 0,
            std::vector<std::pair<int,int>>{{0,1},{5,3}});
        auto gettingnumber = addComponent<FsmToken>("gettingnumber", false, 1, 1,
            std::vector<std::pair<int,int>>{{1,2},{4,0},{6,5}});
        auto ringingaside  = addComponent<FsmToken>("ringingaside",  false, 2, 2,
            std::vector<std::pair<int,int>>{{4,0},{2,4}});
        auto ringingbside  = addComponent<FsmToken>("ringingbside",  false, 3, 3,
            std::vector<std::pair<int,int>>{{4,0},{2,4}});
        auto speech        = addComponent<FsmToken>("speech",        false, 4, 4,
            std::vector<std::pair<int,int>>{{4,0},{3,5}});
        auto waitonhook    = addComponent<FsmToken>("waitonhook",    false, 5, 5,
            std::vector<std::pair<int,int>>{{4,0}});

        // input
        auto input = addComponent<lib::IEStream<int>>("input", input_file.c_str());

        // event broadcasts
        addCoupling(input->out, idle->event_in);
        addCoupling(input->out, gettingnumber->event_in);
        addCoupling(input->out, ringingaside->event_in);
        addCoupling(input->out, ringingbside->event_in);
        addCoupling(input->out, speech->event_in);
        addCoupling(input->out, waitonhook->event_in);

        // transitions
        addCoupling(idle->transition_out,          gettingnumber->transition_in);
        addCoupling(idle->transition_out,          ringingbside->transition_in);
        addCoupling(gettingnumber->transition_out, idle->transition_in);
        addCoupling(gettingnumber->transition_out, ringingaside->transition_in);
        addCoupling(gettingnumber->transition_out, waitonhook->transition_in);
        addCoupling(ringingaside->transition_out,  idle->transition_in);
        addCoupling(ringingaside->transition_out,  speech->transition_in);
        addCoupling(ringingbside->transition_out,  idle->transition_in);
        addCoupling(ringingbside->transition_out,  speech->transition_in);
        addCoupling(speech->transition_out,        idle->transition_in);
        addCoupling(speech->transition_out,        waitonhook->transition_in);
        addCoupling(waitonhook->transition_out,    idle->transition_in);
    }
};

int main() {
    auto model = std::make_shared<Telephone>("telephone", "../input_data/telephone.txt");
    auto rc = cadmium::RootCoordinator(model, 0.0);
    rc.setLogger<cadmium::CSVLogger>("../simulation_results/Telephone_log.csv", ";");
    rc.start();
    rc.simulate(std::numeric_limits<double>::infinity());
    rc.stop();
    return 0;
}
