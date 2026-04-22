#ifndef __FSM_TOKEN_HPP__
#define __FSM_TOKEN_HPP__

#include <iostream>
#include <vector>
#include <limits>
#include <utility>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct TokenState {
    int  state_code;
    int  state_value;       // moore output emitted on state_out
    bool phase;             // true = active (holds token)
    int  temp_next_state;
    int  error_code;
    bool is_event;          // true → output transition_out, false → output state_out
    bool has_error;
    std::vector<std::pair<int,int>> transitions; // {event_code, next_state_code}
    double sigma;

    explicit TokenState(bool           phase,
                        int            state_code,
                        int            state_value,
                        std::vector<std::pair<int,int>> transitions)
        : phase(phase),
          state_code(state_code),
          state_value(state_value),
          transitions(std::move(transitions)),
          temp_next_state(0),
          error_code(0),
          is_event(false),
          has_error(false),
          sigma(std::numeric_limits<double>::infinity())
    {}
};

std::ostream& operator<<(std::ostream& out, const TokenState& s) {
    out << "code:" << s.state_code
        << " phase:" << s.phase
        << " value:" << s.state_value
        << " is_event:" << s.is_event;
    return out;
}

class FsmToken : public Atomic<TokenState> {
public:
    Port<int> event_in;
    Port<int> transition_in;
    Port<int> state_out;
    Port<int> transition_out;
    Port<int> error_out;

    FsmToken(const std::string& id,
             bool phase,
             int  state_code,
             int  state_value,
             std::vector<std::pair<int,int>> transitions)
        : Atomic<TokenState>(id,
              TokenState(phase, state_code, state_value, std::move(transitions)))
    {
        event_in       = addInPort<int>("event_in");
        transition_in  = addInPort<int>("transition_in");
        state_out      = addOutPort<int>("state_out");
        transition_out = addOutPort<int>("transition_out");
        error_out      = addOutPort<int>("error_out");
    }

    void externalTransition(TokenState& state, double /*e*/) const override {
        state.has_error = false;

        if (state.phase) {
            // active: look up incoming event in transition table
            if (!event_in->empty()) {
                int event = event_in->getBag().back();
                bool found = false;
                for (const auto& t : state.transitions) {
                    if (event == t.first) {
                        state.temp_next_state = t.second;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    state.has_error = true;
                    state.error_code = -999;
                }
                state.is_event = true;
                state.sigma = 0.0;
            }
        } else {
            // passive: activate when transition_in matches our code
            if (!transition_in->empty()) {
                int code = transition_in->getBag().back();
                if (code == state.state_code) {
                    state.is_event = false;
                    state.phase    = true;
                    state.sigma    = 0.0;
                }
            }
        }
    }

    void output(const TokenState& state) const override {
        if (state.has_error) {
            error_out->addMessage(state.error_code);
        } else if (state.is_event) {
            transition_out->addMessage(state.temp_next_state);
        } else {
            state_out->addMessage(state.state_value);
        }
    }

    void internalTransition(TokenState& state) const override {
        if (!state.has_error) {
            // deactivate after emitting transition_out; stay active after state_out
            state.phase = !state.is_event;
        }
        state.sigma = std::numeric_limits<double>::infinity();
    }

    [[nodiscard]] double timeAdvance(const TokenState& state) const override {
        return state.sigma;
    }
};

#endif // __FSM_TOKEN_HPP__
