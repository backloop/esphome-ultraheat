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
                : next(nullptr)
            {}

            // executed when leaving a state
            virtual void enter() { SM_PRINTF("State::enter()"); };

            // periodically called
            // return: false - while exit condition is not met
            //         true - when exit condition met
            virtual bool tick() = 0;

            // executed when entering a state
            virtual void exit() { SM_PRINTF("State::exit()"); };

            void set_next(State* state) {
                this->next = state;
            }

            State* get_next() {
                return this->next;
            }

        protected:

        private:
            State* next;
    };

};

#endif // STATEMACHINE
