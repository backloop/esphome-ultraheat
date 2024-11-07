#ifndef STATEMACHINE
#define STATEMACHINE

#include <iostream>
#include <cstring>
#include <stdio.h>

#ifndef SM_PRINTF
#define SM_PRINTF(...)
#endif

namespace StateMachine {

    class State {

        public:

            State ()
                : next_state(nullptr)
            {}

            // executed when entering a state
            virtual void enter() { SM_PRINTF("State::enter()"); };

            // periodically called
            // return: false - if exit condition is not met
            //         true  - if exit condition is met
            virtual bool tick() = 0;

            // executed when leaving a state
            virtual void exit() { SM_PRINTF("State::exit()"); };

            void set_next_state(State* next_state) {
                this->next_state = next_state;
            }

            State* get_next_state() {
                return this->next_state;
            }

        protected:

        private:
            State* next_state;
    };

};

#endif // STATEMACHINE
