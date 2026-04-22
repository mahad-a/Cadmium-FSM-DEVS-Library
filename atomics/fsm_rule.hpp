#ifndef __FSM_RULE_HPP__
#define __FSM_RULE_HPP__

#include <iostream>
#include <vector>
#include <limits>
#include <utility>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct RuleState {
    int  state_no;
    bool init_state;    // true if this is the starting state
    int  tran;          // 1 = armed to evaluate the next event
    int  ev_id;
    bool ax;            // a transition rule matched
    bool action_fired;  // a mealy action rule matched
    double demo_time;
    double sigma;

    // transition rules: {last_state, event_code} → fires when lst==last_state and ev==event_code
    std::vector<std::pair<int,int>> rules;
    // mealy action rules: same format, also triggers output on acting port
    std::vector<std::pair<int,int>> action_rules;

    RuleState(int  state_no,
              bool init_state,
              double demo_time,
              std::vector<std::pair<int,int>> rules,
              std::vector<std::pair<int,int>> action_rules)
        : state_no(state_no),
          init_state(init_state),
          demo_time(demo_time),
          rules(std::move(rules)),
          action_rules(std::move(action_rules)),
          tran(0),
          ev_id(0),
          ax(false),
          action_fired(false),
          sigma(std::numeric_limits<double>::infinity())
    {}
};

std::ostream& operator<<(std::ostream& os, const RuleState& s) {
    os << "state_no:" << s.state_no
       << " init:"    << s.init_state
       << " tran:"    << s.tran
       << " ax:"      << s.ax;
    return os;
}

class FsmRule : public Atomic<RuleState> {
public:
    Port<int> event_in;
    Port<int> transition_in;
    Port<int> out;
    Port<int> errout;
    Port<int> acting;

    // shared across all instances — mirrors the original cd++ static variables
    inline static int  lst    = 0;     // last state that fired
    inline static bool err_in = false; // halts further transitions on error

    FsmRule(const std::string& id,
            int    state_no,
            bool   init_state,
            double demo_time,
            std::vector<std::pair<int,int>> rules,
            std::vector<std::pair<int,int>> action_rules = {})
        : Atomic<RuleState>(id,
              RuleState(state_no, init_state, demo_time,
                        std::move(rules), std::move(action_rules)))
    {
        event_in      = addInPort<int>("event_in");
        transition_in = addInPort<int>("transition_in");
        out           = addOutPort<int>("out");
        errout        = addOutPort<int>("errout");
        acting        = addOutPort<int>("acting");
    }

    void externalTransition(RuleState& state, double /*e*/) const override {
        // arm this state when transition_in arrives (unless global error)
        if (!transition_in->empty()) {
            state.tran = FsmRule::err_in ? 0 : 1;
        }

        if (!event_in->empty()) {
            state.ev_id = event_in->getBag().back();

            if (state.init_state) {
                // initial state fires on the first event without needing transition_in
                state.sigma = state.demo_time;
                return;
            }

            if (state.tran == 1) {
                bool rule_matched = false;
                for (const auto& [last_st, ev_code] : state.rules) {
                    if (FsmRule::lst == last_st && state.ev_id == ev_code) {
                        rule_matched = true;
                        break;
                    }
                }

                if (rule_matched) {
                    state.action_fired = false;
                    for (const auto& [last_st, ev_code] : state.action_rules) {
                        if (FsmRule::lst == last_st && state.ev_id == ev_code) {
                            state.action_fired = true;
                            break;
                        }
                    }
                    state.ax    = true;
                    state.tran  = 0;
                    state.sigma = state.demo_time;
                }
            }
        }
    }

    void output(const RuleState& state) const override {
        if (state.ax || state.init_state) {
            out->addMessage(state.ev_id);
        } else {
            // no matching rule: signal error and halt
            errout->addMessage(state.ev_id);
            FsmRule::err_in = true;
        }

        if (state.action_fired) {
            acting->addMessage(1);
        }

        // update shared last-state so other components know who just fired
        FsmRule::lst = state.state_no;
    }

    void internalTransition(RuleState& state) const override {
        state.init_state   = false;
        state.ax           = false;
        state.action_fired = false;
        state.sigma        = std::numeric_limits<double>::infinity();
    }

    [[nodiscard]] double timeAdvance(const RuleState& state) const override {
        return state.sigma;
    }
};

#endif // __FSM_RULE_HPP__
