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

            virtual void enter() { SM_PRINTF("State::enter()"); };
            virtual bool tick() = 0;
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
