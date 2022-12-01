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

#include "esphome.h"

#define BUF_SIZE 60

class UltraheatMessage {
    public:
        //double model;
        double power_max_kw;
        double power_max_previous_year_kw;

        double heat_usage_mwh;
        double heat_previous_year_mwh;

        double volume_usage_m3;
        double volume_previous_year_m3;

        double error_number;
        double fault_hours;
        double fault_hours_previous_year;

        double flowrate_max_m3ph;
        double flowrate_max_previous_year_m3ph;
        double measuring_range_m3ph;

        double meter_date_time;
        double measurement_period_minutes;
        double yearly_set_day;
        double monthly_set_day;
        double operating_hours;
        double flow_hours;

        double flow_temperature_max_c;
        double return_temperature_max_c;
        double flow_temperature_max_previous_year_c;
        double return_temperature_max_previous_year_c;

        double device_number;
        double ownership_number;
        double settings_and_firmware;

        //bool crcOk = false;
};

class UltraheatReader : public PollingComponent, public UARTDevice {
    //const char* DELIMITERS = "()*:";
    uint8_t buffer[BUF_SIZE];

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

        Sensor *meter_date_time = new Sensor();
        Sensor *measurement_period_minutes = new Sensor();
        Sensor *yearly_set_day = new Sensor();
        Sensor *monthly_set_day = new Sensor();
        Sensor *operating_hours = new Sensor();
        Sensor *flow_hours = new Sensor();

        Sensor *flow_temperature_max_c = new Sensor();
        Sensor *return_temperature_max_c = new Sensor();
        Sensor *flow_temperature_max_previous_year_c = new Sensor();
        Sensor *return_temperature_max_previous_year_c = new Sensor();

        Sensor *device_number = new Sensor();
        Sensor *ownership_number = new Sensor();
        Sensor *settings_and_firmware = new Sensor();

        UltraheatReader(UARTComponent *parent) : PollingComponent(20000), UARTDevice(parent)  {}

        float get_setup_priority() const override {
            return esphome::setup_priority::LATE;
        }

        void setup() override {}

        void update() override {
            readUltraheatMessage();
        }

    private:

        void parseReponseBlock(UltraheatMessage* message, char* obisCode, char* value) {
            if (strncmp(obisCode, "F", 1) == 0) {
                message->error_number = atof(value);

            } else if (strncmp(obisCode, "6.6", 3) == 0) {
                message->power_max_kw = atof(value);

            } else if (strncmp(obisCode, "6.6*01", 6) == 0) {
                message->power_max_previous_year_kw = atof(value);

            } else if (strncmp(obisCode, "6.8", 3) == 0) {
                message->heat_usage_mwh = atof(value);

            } else if (strncmp(obisCode, "6.8*01", 6) == 0) {
                message->heat_previous_year_mwh = atof(value);

            } else if (strncmp(obisCode, "6.26", 4) == 0) {
                message->volume_usage_m3 = atof(value);

            } else if (strncmp(obisCode, "6.26*01", 7) == 0) {
                message->volume_previous_year_m3 = atof(value);

            } else if (strncmp(obisCode, "6.31", 4) == 0) {
                message->operating_hours = atof(value);

            } else if (strncmp(obisCode, "6.32", 4) == 0) {
                message->fault_hours = atof(value);

            } else if (strncmp(obisCode, "6.32*01", 6) == 0) {
                message->fault_hours_previous_year = atof(value);

            } else if (strncmp(obisCode, "6.33", 3) == 0) {
                message->flowrate_max_m3ph = atof(value);

            } else if (strncmp(obisCode, "6.33*01", 3) == 0) {
                message->flowrate_max_previous_year_m3ph = atof(value);

            } else if (strncmp(obisCode, "6.35", 4) == 0) {
                message->measurement_period_minutes = atof(value);

            } else if (strncmp(obisCode, "6.36", 4) == 0) {
                message->yearly_set_day = atof(value);

            } else if (strncmp(obisCode, "6.36*02", 7) == 0) {
                //message->monthly_set_day = atof(value);

            //6.36.1
            //6.36.1*01
            //6.36.2
            //6.36.2*01
            //6.36.3
            //6.36.3*01
            //6.36.4
            //6.36.4*01

            } else if (strncmp(obisCode, "9.1", 3) == 0) {
                //message->settings_and_firmware = atof(value);

            //9.2

            } else if (strncmp(obisCode, "9.4", 3) == 0) {
                // must split value
                //message->flow_temperature_max_c = atof(value);
                //message->return_temperature_max_c = atof(value);

            } else if (strncmp(obisCode, "9.4*01", 6) == 0) {
                // must split value
                //message->flow_temperature_max_previous_year_c = atof(value);
                //message->return_temperature_max_previous_year_c = atof(value);

            //9.6
            //9.7
            } else if (strncmp(obisCode, "9.20", 4) == 0) {
                message->device_number = atof(value);

            } else if (strncmp(obisCode, "9.21", 4) == 0) {
                message->ownership_number = atof(value);

            //9.22

            } else if (strncmp(obisCode, "9.24", 4) == 0) {
                message->measuring_range_m3ph = atof(value);

            } else if (strncmp(obisCode, "9.31", 4) == 0) {
                message->flow_hours = atof(value);

            } else if (strncmp(obisCode, "9.36", 4) == 0) {
                //message->meter_date_time = atof(value);

            }
        }

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

            meter_date_time->publish_state(message->meter_date_time);
            measurement_period_minutes->publish_state(message->measurement_period_minutes);
            yearly_set_day->publish_state(message->yearly_set_day);
            monthly_set_day->publish_state(message->monthly_set_day);
            operating_hours->publish_state(message->operating_hours);
            flow_hours->publish_state(message->flow_hours);

            flow_temperature_max_c->publish_state(message->flow_temperature_max_c);
            return_temperature_max_c->publish_state(message->return_temperature_max_c);
            flow_temperature_max_previous_year_c->publish_state(message->flow_temperature_max_previous_year_c);
            return_temperature_max_previous_year_c->publish_state(message->return_temperature_max_previous_year_c);

            device_number->publish_state(message->device_number);
            ownership_number->publish_state(message->ownership_number);
            settings_and_firmware->publish_state(message->settings_and_firmware);
        }

        void readUltraheatMessage() {

            // wakeup at 300bps
            ESP_LOGD("wakeup", "start");
            ESP8266UartComponent *_parent = (ESP8266UartComponent *)parent_;
            //_parent->dump_config();
            //TODO why are we booting at 2400 on debug uart?, yaml and previously we booted at 300...

            // clear RX buffer, may contain a mix of 300 and 2400 bps data, just ignore
            int len;
            do {
                len = available();
                // avoid buffer overrun
                if (len >= BUF_SIZE) {
                    len = BUF_SIZE;
                }
                ESP_LOGD("wakeup", "trash: %d", len);
                read_array(buffer, len);
            } while (len);

            ESP_LOGD("wakeup", "change baud rate -> 300");
            // https://esphome.io/api/classesphome_1_1uart_1_1_u_a_r_t_component.html
            //set_baud_rate(300); // compile error
            //UARTComponent::set_baud_rate(300); //cannot call member function without object
            //parent_->set_baud_rate(300);

            _parent->set_baud_rate(300);
            _parent->setup();
            //_parent->dump_config();

            //home/backloop/.platformio/packages/framework-arduinoespressif8266/cores/esp8266/uart.h:125:6: note: declared here
            // void uart_set_baudrate(uart_t* uart, int baud_rate);
            //uart_set_baudrate(uart, 300);

            ESP_LOGD("wakeup", "send wakeup");
            // "<bunch of zeroes>/?!\r\n"
            const uint8_t wakeup[] ="\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2F\x3F\x21\x0D\x0A";
            write_array(&wakeup[0], sizeof(wakeup));

            // readback device name
            const uint8_t prod_str[] = "/LUGCUH50\r\n";

            // initialize buffer
            memset (buffer, 0, BUF_SIZE);

            uint8_t iterations = 0;
            int old_cnt = 0;
            uint8_t *write = buffer;
            while (strcmp((char *)buffer, (char *)prod_str) != 0) {

                // wait for slow 300bps chars to arrive
                delay(100);

                uint8_t len = available();

                if (len > 0) {
                    //ESP_LOGD("wakeup", "buffer: %p, write: %p", buffer, write);
                    //ESP_LOGD("wakeup", "available: %d", len);
                    bool success = read_array(write, len);
                    // TOD: check success == false for timeout...
                    // TODO: buffer overrun, check len...

                    // terminate string
                    *(write + len) = 0;

                    //ESP_LOGD("wakeup", "read len: %d", len);
                    //ESP_LOGD("wakeup", "new: <%s>", buffer);

                    // find last ocurrence of start character
                    char *start = strrchr((char *)buffer, prod_str[0]);

                    // find last ocurrence of end character
                    char *end = strrchr((char *)buffer, prod_str[sizeof(prod_str)-1]);

                    //ESP_LOGD("wakeup", "buffer: %p, write: %p, start: %p, end: %p", buffer, write, start, end);
                    //ESP_LOGD("wakeup", "buffer: %p, write: %p -> len: %lu", buffer, write, write-buffer);
                    //for (uint8_t *c = buffer; c < (write+len); c++) {
                    //    ESP_LOGD("wakep", "0x%x", *c);
                    //}

                    if (start == NULL) {
                        start = (char *)buffer;
                    }

                    if (end == NULL) {
                        end = (char *)(write + len);
                    }
                    //ESP_LOGD("wakeup", "buffer: %p, write: %p, start: %p, end: %p, len: %lu", buffer, write, start, end, end - start);

                    // cut away surrounding noise
                    memmove(buffer, start, end - start);

                    // set new write pointer
                    write = (uint8_t *)(buffer + (size_t)(end - start));
                    //write += len;

                    // terminate string
                    *write = 0;
                    ESP_LOGD("wakeup", "stripped: <%s>", buffer);
                }

                int cnt = 0;
                for (cnt = 0; cnt < sizeof(prod_str); cnt++) {
                    if (buffer[cnt] != prod_str[cnt]) break;
                }
                if (cnt == old_cnt) {
                    iterations++;
                    if (iterations >= 10) {
                        ESP_LOGD("wakeup", "timeout...");
                        return;
                    }
                } else {
                    // assume matching chars don't decrease over time
                    old_cnt = cnt;
                    iterations = 0;
                }
            }

            // trim trailing characters
            // use strtok() side effect that it replaces first occurrence of delimiter with '\0'
            strtok((char *)buffer, "\r\n");

            ESP_LOGD("wakeup", "product: <%s>", buffer);
            return;

//            ESP_LOGD("data", "change baud rate -> 2400");
//            // read message at 2400bps
//            //Serial.set_baud_rate(2400);
//            //parent_->set_baud_rate(2400); //compiles, but with no effect as UartComponent->set_baud_rate() only stores this data in RAM..
//            _parent->set_baud_rate(2400);
//            _parent->setup();
//            //_parent->dump_config();
//
//            //delay(5000);
//            if (0) { //available()) {
//
//                UltraheatMessage message = UltraheatMessage();
//                unsigned int wait_time = 0;
//                bool complete = false;
//
//                while (wait_time < 1000) {
//
//                    int len = Serial.readBytesUntil('\n', buffer, BUF_SIZE);
//
//                    if (len > 0) {
//                        ESP_LOGD("data", "%s", buffer);
//
//                        if (buffer[0] == '!') {
//                            complete = true;
//                            break;
//                        }
//
//                        // row might contain multiple data items with dirrerent format
//                        // common parts are <obisCode>(<value>)
//                        // <value>, may contain multiple data points separated by '&'
//                        // if multiple data points exist, then each might contain a
//                        // corresponding <unit>
//                        if (strchr(buffer, '(') != NULL) {
//                            char *saveptr;
//                            char* obisCode = strtok_r(buffer, "(", &saveptr);
//                            char* value = strtok_r(NULL, ")", &saveptr);
//                            ESP_LOGD("data", "%s, %s", obisCode, value);
//                            //do {
//                            //    parseReponseBlock(&message, obisCode, value);
//                            //    obisCode = strtok_r(NULL, "(", &saveptr);
//                            //    value = strtok_r(NULL, ")", &saveptr);
//                            //} while (obisCode != NULL);
//                        }
//                    }
//                    // clean buffer
//                    memset(buffer, 0, BUF_SIZE - 1);
//
//                    if (!available()) {
//                        // wait for more data
//                        int time = 40;
//                        delay(40);
//                        wait_time += time;
//                    }
//                }
//
//                if (complete) {
//                    publishSensors(&message);
//                }
//            }
        }
};
