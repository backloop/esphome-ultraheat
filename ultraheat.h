//-------------------------------------------------------------------------------------
// ESPHome Landis+Gyr Ultraheat T550 District Heating Meter custom sensor
// Copyright 2022 Pablo Cases
//
// ESPHome related code inspired by:
// * https://github.com/psvanstrom/esphome-p1reader
//
// Landis+Gyr response parsing inspired by:
// * https://github.com/vpathuis/ultraheat
//
// MIT License
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-------------------------------------------------------------------------------------
#ifndef ULTRAHEAT
#define ULTRAHEAT

#include "esphome.h"
#include "ultraheat_message.h"
#include "ultraheat_parser.h"

#define BUF_SIZE 80
#define MAX_WAIT_TIME 2000
#define WAIT_TIME 100
#define MAX_ITERATIONS (MAX_WAIT_TIME / WAIT_TIME)

#define DEBUG_PIN (14)

class Ultraheat : public PollingComponent, public UARTDevice {

    public:
        //Sensor *model= new Sensor();

        Sensor *power_max_kw = new Sensor();
        Sensor *power_max_previous_year_kw = new Sensor();

        Sensor *heat_usage_mwh = new Sensor();
        Sensor *heat_previous_year_mwh = new Sensor();

        Sensor *volume_usage_m3 = new Sensor();
        Sensor *volume_previous_year_m3 = new Sensor();

        Sensor *error_number = new Sensor();
        Sensor *fault_hours = new Sensor();
        Sensor *fault_hours_previous_year = new Sensor();

        Sensor *flowrate_max_m3ph = new Sensor();
        Sensor *flowrate_max_previous_year_m3ph = new Sensor();
        Sensor *measuring_range_m3ph = new Sensor();

//        TextSensor *meter_date_time = new TextSensor();
        Sensor *measurement_period_minutes = new Sensor();
//        TextSensor *yearly_set_day = new TextSensor();
//        TextSensor *monthly_set_day = new TextSensor();
        Sensor *operating_hours = new Sensor();
        Sensor *flow_hours = new Sensor();

        Sensor *flow_temperature_max_c = new Sensor();
        Sensor *return_temperature_max_c = new Sensor();
        Sensor *flow_temperature_max_previous_year_c = new Sensor();
        Sensor *return_temperature_max_previous_year_c = new Sensor();

//        TextSensor *device_number = new TextSensor();
//        TextSensor *ownership_number = new TextSensor();
//        TextSensor *settings_and_firmware = new TextSensor();

        Ultraheat(UARTComponent *parent) : PollingComponent(20000), UARTDevice(parent)  {}

        float get_setup_priority() const override {
            return esphome::setup_priority::LATE;
        }

        void setup() override {
            pinMode(DEBUG_PIN, OUTPUT);
            digitalWrite(DEBUG_PIN, LOW);
        }

        void update() override {
            readUltraheatMessage();
        }

    protected:
//        int read_bytes_until(char c, uint8_t *buffer, const uint16_t buf_size);
//        int Ultraheat::read_bytes_until(char c, uint8_t *buffer, const uint16_t buf_size) {
        int read_bytes_until(char c, uint8_t *buffer, const uint16_t buf_size) {

            uint8_t iterations = 0;

            while (iterations < MAX_ITERATIONS) {

                uint8_t len = available();
                if (len == 0) {
                    //ESP_LOGD("rbu", "delay %d", iterations);
                    digitalWrite(DEBUG_PIN, HIGH);
                    delay(WAIT_TIME);
                    digitalWrite(DEBUG_PIN, LOW);
                    iterations++;
                } else {
                    // reset counter as data was found
                    iterations = 0;

                    //ESP_LOGD("rbu", "serbuf: %p, write: %p", serbuf, write);
                    //ESP_LOGD("rbu", "available: %d", len);

                    // avoid serbuf overrun
                    uint8_t readlen = (write+len)-serbuf < BUF_SIZE ? len : (serbuf+BUF_SIZE)-write;
                    //ESP_LOGD("rbu", "want len: %d", len);

                    if (readlen == 0) {
                        ESP_LOGE("rbu", "no space left in serbuf, needs extra: %ld", (write+len)-(serbuf+BUF_SIZE));
                        return 0;
                    }

                    if (!read_array(write, readlen)) {
                        // timeout
                        iterations++;
                        if (iterations >= MAX_ITERATIONS) {
                            ESP_LOGE("rbu", "timeout");
                            return 0;
                        }
                    }

                    // set new write pointer
                    write += readlen;

                    // terminate string
                    *write = 0;
                    //ESP_LOGD("rbu", "read %ld", write-serbuf);
                    //ESP_LOGD("rbu", "%s", serbuf);

                    char *pos = strchr((char *)serbuf, c);
                    if (pos != NULL) {
                        int len = (pos+1)-(char *)serbuf;
                        //ESP_LOGD("rbu", "output: %d", len);

                        if (len > (buf_size-1)) { // account for termination char
                            ESP_LOGE("rbu", "destination buffer is too small");
                            return 0;
                        }

                        // copy to output buffer
                        memcpy(buffer, serbuf, len);
                        *(buffer+len) = 0;

                        // cache remainder
                        len = strlen(pos+1);
                        memmove(serbuf, pos+1, len);
                        write = serbuf + len;
                        memset(write, 0, serbuf + BUF_SIZE - write);

                        // return count of bytes read
                        return strlen((char *)buffer);
                    }
                }
            }

            ESP_LOGI("rwr", "timeout");
            return 0;
        }


    private:
        uint8_t serbuf[BUF_SIZE];
        uint8_t *write;

        void publishSensors(UltraheatMessage* message) {
            power_max_kw->publish_state(message->power_max_kw);
            power_max_previous_year_kw->publish_state(message->power_max_previous_year_kw);

            heat_usage_mwh->publish_state(message->heat_usage_mwh);
            heat_previous_year_mwh->publish_state(message->heat_previous_year_mwh);

            volume_usage_m3->publish_state(message->volume_usage_m3);
            volume_previous_year_m3->publish_state(message->volume_previous_year_m3);

            error_number->publish_state(message->error_number);
            fault_hours->publish_state(message->fault_hours);
            fault_hours_previous_year->publish_state(message->fault_hours_previous_year);

            flowrate_max_m3ph->publish_state(message->flowrate_max_m3ph);
            flowrate_max_previous_year_m3ph->publish_state(message->flowrate_max_previous_year_m3ph);
            measuring_range_m3ph->publish_state(message->measuring_range_m3ph);

//            meter_date_time->publish_state(message->meter_date_time);
            measurement_period_minutes->publish_state(message->measurement_period_minutes);
//            yearly_set_day->publish_state(message->yearly_set_day);
//            monthly_set_day->publish_state(message->monthly_set_day);
            operating_hours->publish_state(message->operating_hours);
            flow_hours->publish_state(message->flow_hours);

            flow_temperature_max_c->publish_state(message->flow_temperature_max_c);
            return_temperature_max_c->publish_state(message->return_temperature_max_c);
            flow_temperature_max_previous_year_c->publish_state(message->flow_temperature_max_previous_year_c);
            return_temperature_max_previous_year_c->publish_state(message->return_temperature_max_previous_year_c);

//            device_number->publish_state(message->device_number);
//            ownership_number->publish_state(message->ownership_number);
//            settings_and_firmware->publish_state(message->settings_and_firmware);
        }


        bool read_wakeup_response(uint8_t *buffer, uint8_t len) {

            // device name
            const char str[] = "/LUGCUH50\r\n";

//            if (!read_bytes_until_match(prod_str, buffer, BUF_SIZE)) {
//                ESP_LOGD("wakeup", "timeout...");
//            }

            // read until last char in string. assume it is unique
            if (!read_bytes_until(str[strlen(str)-1], buffer, BUF_SIZE)) {
                ESP_LOGE("rwr", "timeout or buffer full...");
                return false;
            }

            // trim preceeding characters
            // TODO: use strstr((char *)buffer, str) to find substring, then remove header
            char *header = strrchr((char *)buffer, (char)str[0]);
            if (header == NULL) {
                ESP_LOGE("rwr", "could not find first char in response string...");
                return false;
            }

            if (header > (char *)buffer) {
                memmove((char *)buffer, header, strlen(header)+1); //include termination char
            }
            //ESP_LOGD("rwr", "buffer: %s", buffer);

            if (strcmp((char *)buffer, (char *)str)) {
                // no match
                ESP_LOGE("rwr", "head and tail match, but not the rest: <%s> != <%s>", buffer, str);
                return false;
            }

            // trim trailing characters, assume these exist only at the end
            // use strtok() side effect that it replaces first occurrence of delimiter with '\0'
            strtok((char *)buffer, "\r\n");

            ESP_LOGI("rwr", "product: <%s>", buffer);
            return true;
        }

        void readUltraheatMessage() {
            uint8_t buffer[BUF_SIZE];

            UltraheatParser parser;

            // wakeup at 300bps
            ESP_LOGD("wakeup", "start");
            ESP8266UartComponent *_parent = (ESP8266UartComponent *)parent_;
            //_parent->dump_config();
            //TODO why are we booting at 2400 when using debug uart? previously we booted at 300...

            // clear RX buffer, may contain a mix of 300 and 2400 bps data, just ignore
            int len;
            do {
                len = available();
                // avoid buffer overrun
                if (len >= BUF_SIZE) {
                    len = BUF_SIZE;
                }
                ESP_LOGD("wakeup", "clearing RX: %d", len);
                read_array(buffer, len);
            } while (len);

            //ESP_LOGD("wakeup", "change baud rate -> 300");
            //parent_->set_baud_rate(300);
            _parent->set_baud_rate(300);
            _parent->setup();
            //_parent->dump_config();

            ESP_LOGI("wakeup", "send wakeup");
            // "<bunch of zeroes>/?!\r\n"
            const uint8_t wakeup[] ="\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2F\x3F\x21\x0D\x0A";
            write_array(&wakeup[0], sizeof(wakeup));

            // device needs time to wakeup
            //delay(500);

            // initialize caching buffer
            memset (serbuf, 0, BUF_SIZE);
            write = serbuf;

            ESP_LOGI("wakeup", "read wakeup response");
            if (!read_wakeup_response(buffer, BUF_SIZE)) {
                ESP_LOGE("wakeup", "could not read wakeup response");
                return;
            }

            //ESP_LOGD("data", "change baud rate -> 2400");
            // read message at 2400bps
            //parent_->set_baud_rate(2400); //compiles, but with no effect as UartComponent->set_baud_rate() only stores this data in RAM..
            _parent->set_baud_rate(2400);
            _parent->setup();
            //_parent->dump_config();

            // device needs time to switch baud rate
            delay(500);

            int iterations = 0;
            while(!available() && iterations < MAX_ITERATIONS) {
                iterations++;
                delay(WAIT_TIME);
            }

            if (iterations >= MAX_ITERATIONS) {
                ESP_LOGE("message", "timeout");
                return;
            }

            UltraheatMessage message;
            unsigned int wait_time = 0;
            bool complete = false;

            // initialize caching buffer
            memset (serbuf, 0, BUF_SIZE);
            write = serbuf;

            iterations = 0;
            while (iterations < 24) {

                int len = read_bytes_until('\n', buffer, BUF_SIZE);

                if (len == 0) {
                    ESP_LOGE("message", "timeout or buffer full");
                    return;
                } else {
                    // trim trailing characters, assume these exist only at the end
                    // use strtok() side effect that it replaces first occurrence of delimiter with '\0'
                    strtok((char *)buffer, "\r\n");

                    //ESP_LOGD("message", "%s", buffer);

                    if (buffer[0] == '!') {
                        complete = true;
                        break;
                    }

                    parser.parseResponseRow(&message, (char*) buffer);
                }

                // clean buffer
                memset(buffer, 0, BUF_SIZE);

                if (!available()) {
                    delay(WAIT_TIME);
                }

                iterations++;
            }

            if (complete) {
                message.print();
                publishSensors(&message);
            }
        }
};

#endif //ULTRAHEAT
