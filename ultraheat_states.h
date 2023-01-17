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

namespace Ultraheat {
#define DATA_SIZE (128)
#define LINE_SIZE (DATA_SIZE - 1) // make space for string termination

    class IO {

        public:
            // inherit base class' constructors
            IO()
                : data()
                , write(data)
            {}

            virtual void setup() {};
            virtual int available() = 0;
            virtual bool read_array(uint8_t *data, size_t len) = 0;
            //virtual void write_array(const std::vector<uint8_t> &data) {};
            virtual void write_array(const uint8_t *data, size_t len) = 0;
            virtual void clear_buffers(void) {}

            // 0 - line is complete
            // 1 - nothing to read
            // 2 - buffer full
            // 3 - read timeout
            // 4 - return data buffer is too small
            // 5 - line is not complete
            int read_line_nonblock(uint8_t* data, size_t max_size) {

                int available = this->available();

                if (!available) {
                    return 1;
                }

                int space_remaining = (this->data + LINE_SIZE) - this->write;
                if (space_remaining <= 0) {
                    return 2;
                }
                std::cout << "space_remaining:" << space_remaining << std::endl;

                int possible = available < space_remaining ? available : space_remaining;

                if (!this->read_array(this->write, possible)) {
                    return 3;
                }
                this->write += possible;
                *this->write = '\0';
                std::cout << "data: <" << this->data << ">" << std::endl;

                // find newline
                char c = '\n';
                char *pos = strchr((char *)this->data, c);

                if (pos == NULL) {
                    return 5;
                }

                // include newline
                pos++;

                int len = pos  - (char *)this->data;

                if (len > (max_size-1)) { // account for string termination
                    return 4;
                }

                // copy to output buffer
                memcpy(data, this->data, len);
                *(data+len) = 0;

                // move remainder to beginning
                len = (this->write - this->data) - len;
                memmove(this->data, pos, len);
                this->write = this->data + len;
                memset(this->write, 0, this->data + DATA_SIZE - this->write);

                // remainder
                std::cout << "remainder: <" << this->data << ">" << std::endl;

                return 0;
            }

        private:
            uint8_t data[DATA_SIZE];
            uint8_t *write;
    };


    class IdleState : public StateMachine::State {

        public:
            // inherit base class' constructors
            //using StateMachine::State::State;

            IdleState(unsigned int idle_ms)
                : StateMachine::State()
                , idle_ms(idle_ms)
            {}

            void enter() override {
                std::cout << "IdleState::enter()" << std::endl;
                // init idle counter
            }
            bool tick() override {
                std::cout << "IdleState::tick()" << std::endl;
                // count idle time
                return true;
            }

        private:
            unsigned int idle_ms;
    };


    class WakeupState : public StateMachine::State {

        public:
            WakeupState(IO& io)
                : StateMachine::State()
                , io(io)
            {}

            void enter() override {
                std::cout << "WakeupState::enter()" << std::endl;

                this->io.setup();

                // "<bunch of zeroes>/?!\r\n"
                const uint8_t wakeup[] ="\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2F\x3F\x21\x0D\x0A";
                this->io.write_array(&wakeup[0], sizeof(wakeup));

                this->io.clear_buffers();
            }

            bool tick() override {
                std::cout << "WakeupState::tick()" << std::endl;

                int ret;
                uint8_t buffer[DATA_SIZE];
                ret = this->io.read_line_nonblock(buffer, DATA_SIZE);
                if (ret) {
                    std::cout << "io.read_line_nonblock: " << ret << std::endl;
                    return false;
                }

                std::cout  << "line: <" << buffer << ">" << std::endl;

                // find last ocurrence of start character
                char *start = strrchr((char *)buffer, '/');

                if (start == NULL) {
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

                std::cout  << "line: " << buffer << std::endl;

                return true;
            }

        private:
            IO& io;

    };


    class MessageState : public StateMachine::State {

        public:
            MessageState(IO& io)
                : StateMachine::State()
                , io(io)
            {}

            void enter() override {
                std::cout << "MessageState::enter()" << std::endl;
                // setup uart
                this->io.setup();
                // clear buffers
                this->io.clear_buffers();
            }

            bool tick() override {
                std::cout << "MessageState::tick()" << std::endl;

                int ret;
                uint8_t buffer[DATA_SIZE];
                ret = this->io.read_line_nonblock(buffer, DATA_SIZE);
                if (ret) {
                    std::cout << "io.read_line_nonblock: " << ret << std::endl;
                    return false;
                }

                std::cout  << "line: " << buffer << std::endl;

                // trim trailing characters, assume these exist only at the end
                // use strtok() side effect that it replaces first occurrence of delimiter with '\0'
                strtok((char *)buffer, "\r\n");

                std::cout  << "line: " << buffer << std::endl;

                if (buffer[0] == '!') {
                    std::cout << "message done!" << std::endl;
                    // TODO: send
                    return true;
                }

                parser.parseResponseRow(this->message, (char*)buffer);
                this->message.print();
                return false;
            }

        private:
            IO& io;
            UltraheatParser parser;
            UltraheatMessage message;
    };

};

#endif // ULTRAHEAT_STATES
