#include <iostream>
#include <cstring>
#include <time.h>
#include <stdio.h>

char * timestamp(){
    time_t now = time(nullptr);
    char * time = asctime(gmtime(&now));
    time[strlen(time)-1] = '\0';    // Remove \n
    return time;
}
// statemachine
#define SM_PRINTF(f_, ...) printf("%s ", timestamp()), printf("machine: "), printf((f_), ##__VA_ARGS__), printf("\n")
// ultraheat_states
#define US_PRINTF(f_, ...) printf("%s ", timestamp()), printf("states:  "), printf((f_), ##__VA_ARGS__), printf("\n")
// ultraheat_message
#define UM_PRINTF(f_, ...) printf("%s ", timestamp()), printf("message: "), printf((f_), ##__VA_ARGS__), printf("\n")
// ultraheat_parser
#define UP_PRINTF(f_, ...) printf("%s ", timestamp()), printf("parser:  "), printf((f_), ##__VA_ARGS__), printf("\n")
// ultraheat_parser
#define IO_PRINTF(f_, ...) printf("%s ", timestamp()), printf("io:      "), printf((f_), ##__VA_ARGS__), printf("\n")
#define FIO_PRINTF(f_, ...) printf("%s ", timestamp()), printf("fileio:  "), printf((f_), ##__VA_ARGS__), printf("\n")

#include "statemachine.h"
#include "ultraheat_states.h"


class FileIO : public Ultraheat::IO {

    public:
        FileIO(const std::string& data)
            : Ultraheat::IO()
            , data(data)
            , idx(0)
            , rand_available(0)
        {
            std::srand(time(nullptr));
        }

        virtual void clear_buffers(void) {
            this->idx = 0;
        }

        int available() override {
            if (this->rand_available == 0) {
                generate_next();
            }

            FIO_PRINTF("available() == %d", this->rand_available);
            return this->rand_available;
        }

        bool read_array(uint8_t *data, size_t len) override {
            FIO_PRINTF("read_array() == %lu of %d", len, this->rand_available);
            if (len == 0) {
                // sort of timeout, don't allow reading 0 chars
                return false;
            }

            if (len > this->rand_available) {
                //timeout
                return false;
            }
            memcpy(data, this->data.c_str() + this->idx, len);
            data[len] = '\0';
            FIO_PRINTF("data: <%s>", data);
            this->idx += len;
            this->rand_available -= len;

            return true;
        }

        void write_array(const uint8_t *data, size_t len) override {
            printf("Wrote[%ld]: ", len);
            for (unsigned int i = 0; i < len; i++) {
                printf("%02x,", data[i]);
            }
            printf("\n");
        }

    private:
        const std::string& data;
        unsigned int idx;
        unsigned int rand_available;

        void generate_next() {
            this->rand_available = std::rand() % (10+1); // [0..10]
            //std::cout << "raw rand_available: " << this->rand_available << std::endl;
            FIO_PRINTF("data.length: %lu, idx: %d", this->data.length(), this->idx);

            if (this->data.length() < (this->idx + this->rand_available)) {
                this->rand_available = this->data.length() - this->idx;
            }
            //std::cout << "rand: " << this->rand_available << std::endl;
        }

};


const std::string data_wakeup = "foobar\r\nblah/LUGH50\r\nprutt";

// raw(...)raw makes it possible to use () inside the string without escaping
// any text instead of "raw" will work as well, its the syntax
const std::string data_message = R"raw(6.8(0002.039*MWh)6.26(00040.62*m3)9.21(71422831)
6.26*01(00000.00*m3)6.8*01(0000.000*MWh)
F(0)9.20(71422831)6.35(60*m)
6.6(0008.9*kW)6.6*01(0000.0*kW)6.33(000.144*m3ph)9.4(084.2*C&040.4*C)
6.31(0001067*h)6.32(0000000*h)9.22(R)9.6(000&71422831&0&000&71422831&0)
9.7(20000)6.32*01(0000000*h)6.36(01-01&00:00)6.33*01(000.000*m3ph)
6.8.1()6.8.2()6.8.3()6.8.4()6.8.5()
6.8.1*01()6.8.2*01()6.8.3*01()
6.8.4*01()6.8.5*01()
9.4*01(000.0*C&000.0*C)
6.36.1(2022-11-26)6.36.1*01(2000-00-00)
6.36.2(2022-10-19)6.36.2*01(2000-00-00)
6.36.3(2022-11-20)6.36.3*01(2000-00-00)
6.36.4(2022-10-28)6.36.4*01(2000-00-00)
6.36.5()6.36*02(01&00:00)9.36(2022-12-02&22:41:34)9.24(1.5*m3ph)
9.17(0)9.18()9.19()9.25()
9.1(0&1&0&0017&CECV&CECV&1&5.24&5.24&F&081008&040404&08&0&00&:5&00&20)
9.2(&&)9.29()9.31(0000759*h)
9.0.1(00000000)9.0.2(00000000)9.34.1(000.00000*m3)9.34.2(000.00000*m3)
8.26.1(00000000*m3)8.26.2(00000000*m3)
8.26.1*01(00000000*m3)8.26.2*01(00000000*m3)
6.26.1()6.26.4()6.26.5()
6.26.1*01()6.26.4*01()6.26.5*01()0.0(71422831)
!
)raw";

class UltraheatCheck : public Ultraheat::MessageStateObserver {

    public:
        UltraheatCheck()
            : wakeup_io(data_wakeup)
            , message_io(data_message)
            , idle(1000)
            , wakeup(this->wakeup_io)
            , message(this->message_io, this)
            , messages_sent(0)
        {
            this->idle.set_next(&this->wakeup);
            this->wakeup.set_next(&this->message);
            this->message.set_next(&this->idle);
        }

        void notify_message_sent() override {
            this->messages_sent++;
        }

        int run() {
            // setup
            this->state = &wakeup;
            this->state->enter();

            // loop
            // TODO 2: FILE_IO objects must be reset after each complete iteration
            unsigned int iters = 0;
            while (this->messages_sent < 3) {
                if (this->state->tick()) {
                    this->state->exit();
                    this->state = state->get_next();
                    this->state->enter();
                }
                // empty line separator between ticks
                printf("\n");

                if (++iters > 800) {
                    printf("Sent %d messages in %d iterations\n", this->messages_sent, iters);
                    printf("ERROR: too many iterations\n");
                    return 1;
                }
            }

            printf("Sent %d messages in %d iterations\n", this->messages_sent, iters);
            printf("Check done!\n");
            return 0;
        }

    private:
        FileIO wakeup_io;
        FileIO message_io;

        Ultraheat::IdleState idle;
        Ultraheat::WakeupState wakeup;
        Ultraheat::MessageState message;

        StateMachine::State* state;
        unsigned int messages_sent;
};


int main (int argc, char *argv[]) {

    UltraheatCheck check;
    return check.run();
}
