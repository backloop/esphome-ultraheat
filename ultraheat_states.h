#ifndef ULTRAHEAT_STATES
#define ULTRAHEAT_STATES

#include <iostream>
#include <cstring>
#include <stdio.h>
#include <vector>

//default to no debug prints
#ifndef US_PRINTF
#define US_PRINTF(...)
#endif

#include "ultraheat_parser.h"
#include "ultraheat_message.h"
#include "ultraheat_io.h"

# if defined(unix)
#include <climits>
#include <sys/time.h>
#elif defined(_WIN32)
// TODO
#endif

namespace Ultraheat {

    class IdleState : public StateMachine::State {

        public:
            // inherit base class' constructors
            //using StateMachine::State::State;

            IdleState(unsigned int idle_s)
                : StateMachine::State()
                , idle_ms(idle_s * 1000)
            {}

            void enter() override {
                US_PRINTF("IdleState::enter()");
                start_ms = millis();
            }
            bool tick() override {
                // return true if enough time has passed
                unsigned long current_ms = millis();
                unsigned long delta_ms;
                if (current_ms < start_ms) {
                    // we have wrapped
                    delta_ms = (ULONG_MAX - start_ms + current_ms);
                } else {
                    delta_ms = current_ms - start_ms;
                }
                US_PRINTF("IdleState::tick(%lu)", delta_ms);
                return (delta_ms > idle_ms);
            }

        private:
            unsigned long idle_ms;
            unsigned long start_ms;

#if defined(unix)
            unsigned long millis() {
                struct timeval time_now{};
                gettimeofday(&time_now, nullptr); // ignore SUCCESS/FAIL return value for now

                time_t msecs_time = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);

                return msecs_time;
            }
#elif defined(_WIN32)
            // TODO
#endif
    };


    class WakeupState : public StateMachine::State {

        public:
            WakeupState(IO& io)
                : StateMachine::State()
                , io(io)
            {}

            void enter() override {
                US_PRINTF("WakeupState::enter()");

                this->io.setup();

                // "<bunch of zeroes>/?!\r\n"
                const uint8_t wakeup[] ="\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2F\x3F\x21\x0D\x0A";
                this->io.write_array(&wakeup[0], sizeof(wakeup));

                this->io.clear_buffers();
            }

            bool tick() override {
                US_PRINTF("WakeupState::tick()");

                int ret;
                uint8_t buffer[DATA_SIZE];
                ret = this->io.read_line_nonblock(buffer, DATA_SIZE);
                if (ret) {
                    US_PRINTF("io.read_line_nonblock: %d", ret);
                    return false;
                }

                US_PRINTF("line: <%s>", buffer);

                // find last ocurrence of start character
                char *start = strrchr((char *)buffer, '/');

                if (start == nullptr) {
                    // Start char could not be found this is not the line
                    // we're looking for, continue
                    return false;
                }

                if (start > (char*)buffer) {
                    // cut away preceeding noise
                    int len = strlen((char*)buffer) - (start - (char*)buffer);
                    memmove(buffer, start, len);
                    *(buffer + len) = '\0';
                }

                // trim trailing characters, assume these exist only at the end
                // use strtok() side effect that it replaces first occurrence of delimiter with '\0'
                strtok((char *)buffer, "\r\n");

                US_PRINTF("line: %s", buffer);

                return true;
            }

        private:
            IO& io;

    };

    class MessageStateObserver {
        public:
            virtual void message_received(Ultraheat::UltraheatMessage& message) = 0;
    };

    class MessageState : public StateMachine::State {

        public:
            MessageState(IO& io, MessageStateObserver& observer)
                : StateMachine::State()
                , io(io)
                , observer(observer)
            {}

            unsigned int messages_sent = 0;

            void enter() override {
                US_PRINTF("MessageState::enter()");
                // setup uart
                this->io.setup();
                // clear buffers
                this->io.clear_buffers();
            }

            bool tick() override {
                US_PRINTF("MessageState::tick()");

                int ret;
                uint8_t buffer[DATA_SIZE];
                ret = this->io.read_line_nonblock(buffer, DATA_SIZE);
                if (ret) {
                    US_PRINTF("io.read_line_nonblock: %d", ret);
                    return false;
                }

                US_PRINTF("line: %s", buffer);

                // trim trailing characters, assume these exist only at the end
                // use strtok() side effect that it replaces first occurrence of delimiter with '\0'
                strtok((char *)buffer, "\r\n");

                US_PRINTF("line: %s", buffer);

                if (buffer[0] == '!') {
                    US_PRINTF("message complete!");
                    this->observer.message_received(this->message);
                    return true;
                }

                parser.parseResponseRow(this->message, (char*)buffer);
                this->message.print();
                return false;
            }

        private:
            IO& io;
            MessageStateObserver& observer;
            UltraheatParser parser;
            UltraheatMessage message;
    };

};

#endif // ULTRAHEAT_STATES
