#include <fstream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include "cadmium/simulation/root_coordinator.hpp"
#include "cadmium/simulation/logger/csv.hpp"
#include "cadmium/modeling/devs/coupled.hpp"
#include "cadmium/lib/iestream.hpp"
#include "../atomics/fsm_token.hpp"

using namespace cadmium;

struct ATM : public Coupled {

    ATM(const std::string& id, const std::string& input_file)
        : Coupled(id)
    {
        // states: {code, value}, transitions: {event → next_code}
        auto idle                  = addComponent<FsmToken>("idle",                  true,  0, 0,
            std::vector<std::pair<int,int>>{{0,1}});
        auto checkingpin           = addComponent<FsmToken>("checkingpin",           false, 1, 1,
            std::vector<std::pair<int,int>>{{1,0},{2,2}});
        auto selectingtransactions = addComponent<FsmToken>("selectingtransactions", false, 2, 2,
            std::vector<std::pair<int,int>>{{3,3},{9,7}});
        auto withdrawing           = addComponent<FsmToken>("withdrawing",           false, 3, 3,
            std::vector<std::pair<int,int>>{{4,4}});
        auto inputingamount        = addComponent<FsmToken>("inputingamount",        false, 4, 4,
            std::vector<std::pair<int,int>>{{5,5}});
        auto confirming            = addComponent<FsmToken>("confirming",            false, 5, 5,
            std::vector<std::pair<int,int>>{{7,6},{6,4}});
        auto withdrawaldone        = addComponent<FsmToken>("withdrawaldone",        false, 6, 6,
            std::vector<std::pair<int,int>>{{8,0}});
        auto depositing            = addComponent<FsmToken>("depositing",            false, 7, 7,
            std::vector<std::pair<int,int>>{{4,8}});
        auto insertingmoney        = addComponent<FsmToken>("insertingmoney",        false, 8, 8,
            std::vector<std::pair<int,int>>{{10,9}});
        auto depositdone           = addComponent<FsmToken>("depositdone",           false, 9, 9,
            std::vector<std::pair<int,int>>{{8,0}});

        // input
        auto input = addComponent<lib::IEStream<int>>("input", input_file.c_str());

        // event broadcasts
        addCoupling(input->out, idle->event_in);
        addCoupling(input->out, checkingpin->event_in);
        addCoupling(input->out, selectingtransactions->event_in);
        addCoupling(input->out, withdrawing->event_in);
        addCoupling(input->out, inputingamount->event_in);
        addCoupling(input->out, confirming->event_in);
        addCoupling(input->out, withdrawaldone->event_in);
        addCoupling(input->out, depositing->event_in);
        addCoupling(input->out, insertingmoney->event_in);
        addCoupling(input->out, depositdone->event_in);

        // transitions
        addCoupling(idle->transition_out,                  idle->transition_in);
        addCoupling(idle->transition_out,                  checkingpin->transition_in);
        addCoupling(checkingpin->transition_out,           idle->transition_in);
        addCoupling(checkingpin->transition_out,           selectingtransactions->transition_in);
        addCoupling(selectingtransactions->transition_out, withdrawing->transition_in);
        addCoupling(selectingtransactions->transition_out, depositing->transition_in);
        addCoupling(withdrawing->transition_out,           inputingamount->transition_in);
        addCoupling(inputingamount->transition_out,        confirming->transition_in);
        addCoupling(confirming->transition_out,            inputingamount->transition_in);
        addCoupling(confirming->transition_out,            withdrawaldone->transition_in);
        addCoupling(withdrawaldone->transition_out,        idle->transition_in);
        addCoupling(depositing->transition_out,            insertingmoney->transition_in);
        addCoupling(insertingmoney->transition_out,        depositdone->transition_in);
        addCoupling(depositdone->transition_out,           idle->transition_in);
    }
};

int main() {
    auto model = std::make_shared<ATM>("ATM", "../input_data/ATM.txt");
    auto rc = cadmium::RootCoordinator(model, 0.0);
    rc.setLogger<cadmium::CSVLogger>("../simulation_results/ATM_log.csv", ";");
    rc.start();
    rc.simulate(std::numeric_limits<double>::infinity());
    rc.stop();
    return 0;
}
