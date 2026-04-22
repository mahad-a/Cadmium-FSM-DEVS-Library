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

struct GameStateController : public Coupled {

    GameStateController(const std::string& id, const std::string& input_file)
        : Coupled(id)
    {
        // states: {code, value}, transitions: {event → next_code}
        auto menu    = addComponent<FsmToken>("menu",    true,  0, 0,
            std::vector<std::pair<int,int>>{{0,1}});
        auto playing = addComponent<FsmToken>("playing", false, 1, 1,
            std::vector<std::pair<int,int>>{{1,2},{2,3}});
        auto paused  = addComponent<FsmToken>("paused",  false, 2, 2,
            std::vector<std::pair<int,int>>{{3,1},{4,0}});
        auto ended   = addComponent<FsmToken>("ended",   false, 3, 3,
            std::vector<std::pair<int,int>>{{5,0}});

        // input
        auto input = addComponent<lib::IEStream<int>>("input", input_file.c_str());

        // event broadcasts
        addCoupling(input->out, menu->event_in);
        addCoupling(input->out, playing->event_in);
        addCoupling(input->out, paused->event_in);
        addCoupling(input->out, ended->event_in);

        // transitions
        addCoupling(menu->transition_out,    playing->transition_in);

        addCoupling(playing->transition_out, paused->transition_in);
        addCoupling(playing->transition_out, ended->transition_in);

        addCoupling(paused->transition_out,  playing->transition_in);
        addCoupling(paused->transition_out,  menu->transition_in);

        addCoupling(ended->transition_out,   menu->transition_in);
    }
};

int main() {
    auto model = std::make_shared<GameStateController>(
        "game_state",
        "../input_data/game_state.txt"
    );
    auto rc = cadmium::RootCoordinator(model, 0.0);
    rc.setLogger<cadmium::CSVLogger>("../simulation_results/GameState_log.csv", ";");
    rc.start();
    rc.simulate(std::numeric_limits<double>::infinity());
    rc.stop();
    return 0;
}
