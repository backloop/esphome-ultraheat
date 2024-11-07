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

#define BUF_SIZE 80
#define DEBUG_PIN (14)

class SerialIO : public Ultraheat::IO {

    public:
        SerialIO(UARTComponent* uart, unsigned int baud_rate)
            : Ultraheat::IO()
            , uart(uart)
            , baud_rate(baud_rate)
        {}

        void setup() override {
            // wakeup at 300bps
            ESP_LOGD("serial", "start");
            ESP8266UartComponent *uart_8266 = (ESP8266UartComponent *)this->uart;
            //uart8266->dump_config();
            //TODO why are we booting at 2400 when using debug uart? previously we booted at 300...

            // clear RX buffer, may contain a mix of 300 and 2400 bps data,
            // just ignore contents
            int len;
            uint8_t buffer[BUF_SIZE];
            do {
                len = this->available();
                // avoid buffer overrun
                if (len >= BUF_SIZE) {
                    len = BUF_SIZE;
                }
                ESP_LOGD("serial", "clearing RX: %d", len);
                this->read_array(buffer, len);
            } while (len);

            ESP_LOGD("serial", "set baud rate -> %d", this->baud_rate);
            uart_8266->set_baud_rate(this->baud_rate);
            uart_8266->setup();
            //uart_8266->dump_config();
        }

        int available() override {
            return this->uart->available();
        }

        bool read_array(uint8_t *data, size_t len) override {
            return this->uart->read_array(data, len);
        }

        //void write_array(const std::vector<uint8_t> &data) override {
        void write_array(const uint8_t *data, size_t len) override {
            this->uart->write_array(data, len);
        }

    private:
        UARTComponent* uart;
        unsigned int baud_rate;
};

//class UltraheatReader : public PollingComponent, public UARTDevice {
class UltraheatReader : public Component, public UARTDevice, public Ultraheat::MessageStateObserver {

    public:
        //Sensor *model= new Sensor();

//        Sensor *power_max_kw = new Sensor();
//        Sensor *power_max_previous_year_kw = new Sensor();
//
        Sensor *heat_usage_mwh = new Sensor();
//        Sensor *heat_previous_year_mwh = new Sensor();
//
//        Sensor *volume_usage_m3 = new Sensor();
//        Sensor *volume_previous_year_m3 = new Sensor();
//
//        Sensor *error_number = new Sensor();
//        Sensor *fault_hours = new Sensor();
//        Sensor *fault_hours_previous_year = new Sensor();
//
//        Sensor *flowrate_max_m3ph = new Sensor();
//        Sensor *flowrate_max_previous_year_m3ph = new Sensor();
//        Sensor *measuring_range_m3ph = new Sensor();
//
////        TextSensor *meter_date_time = new TextSensor();
//        Sensor *measurement_period_minutes = new Sensor();
////        TextSensor *yearly_set_day = new TextSensor();
////        TextSensor *monthly_set_day = new TextSensor();
//        Sensor *operating_hours = new Sensor();
//        Sensor *flow_hours = new Sensor();
//
//        Sensor *flow_temperature_max_c = new Sensor();
//        Sensor *return_temperature_max_c = new Sensor();
//        Sensor *flow_temperature_max_previous_year_c = new Sensor();
//        Sensor *return_temperature_max_previous_year_c = new Sensor();
//
////        TextSensor *device_number = new TextSensor();
////        TextSensor *ownership_number = new TextSensor();
////        TextSensor *settings_and_firmware = new TextSensor();

//        UltraheatReader(UARTComponent *parent)
//            : PollingComponent(20000)
//            , UARTDevice(parent)
//        {}

        UltraheatReader(UARTComponent *parent)
            : UARTDevice(parent)
            , wakeup_io(parent, 300)
            , message_io(parent, 2400)
            , idle(10 /*seconds between readouts*/)
            , wakeup(wakeup_io)
            , message(message_io, *this)
        {
            pinMode(DEBUG_PIN, OUTPUT);
            digitalWrite(DEBUG_PIN, LOW);

            this->idle.set_next(&this->wakeup);
            this->wakeup.set_next(&this->message);
            this->message.set_next(&this->idle);
            this->state = &this->wakeup;
        }

        float get_setup_priority() const override {
            return esphome::setup_priority::LATE;
        }

        void setup() override {
            this->state->enter();
        }

        // Component
        void loop() override {
            if (this->state->tick()) {
                this->state->exit();
                this->state = state->get_next();
                this->state->enter();
            }
        }

        void message_received(Ultraheat::UltraheatMessage& message) override {
            this->publishSensors(message);
        }

        // PollingComponent
        //void update() override {
        //}

    private:
        SerialIO wakeup_io;
        SerialIO message_io;
        Ultraheat::IdleState idle;
        Ultraheat::WakeupState wakeup;
        Ultraheat::MessageState message;
        StateMachine::State* state;

        void publishSensors(Ultraheat::UltraheatMessage& message) {
//            this->power_max_kw->publish_state(message->power_max_kw);
//            this->power_max_previous_year_kw->publish_state(message->power_max_previous_year_kw);
//
            this->heat_usage_mwh->publish_state(message.heat_usage_mwh);
//            this->heat_previous_year_mwh->publish_state(message->heat_previous_year_mwh);
//
//            this->volume_usage_m3->publish_state(message->volume_usage_m3);
//            this->volume_previous_year_m3->publish_state(message->volume_previous_year_m3);
//
//            this->error_number->publish_state(message->error_number);
//            this->fault_hours->publish_state(message->fault_hours);
//            this->fault_hours_previous_year->publish_state(message->fault_hours_previous_year);
//
//            this->flowrate_max_m3ph->publish_state(message->flowrate_max_m3ph);
//            this->flowrate_max_previous_year_m3ph->publish_state(message->flowrate_max_previous_year_m3ph);
//            this->measuring_range_m3ph->publish_state(message->measuring_range_m3ph);
//
////            this->meter_date_time->publish_state(message->meter_date_time);
//            this->measurement_period_minutes->publish_state(message->measurement_period_minutes);
////            this->yearly_set_day->publish_state(message->yearly_set_day);
////            this->monthly_set_day->publish_state(message->monthly_set_day);
//            this->operating_hours->publish_state(message->operating_hours);
//            this->flow_hours->publish_state(message->flow_hours);
//
//            this->flow_temperature_max_c->publish_state(message->flow_temperature_max_c);
//            this->return_temperature_max_c->publish_state(message->return_temperature_max_c);
//            this->flow_temperature_max_previous_year_c->publish_state(message->flow_temperature_max_previous_year_c);
//            this->return_temperature_max_previous_year_c->publish_state(message->return_temperature_max_previous_year_c);
//
////            this->device_number->publish_state(message->device_number);
////            this->ownership_number->publish_state(message->ownership_number);
////            this->settings_and_firmware->publish_state(message->settings_and_firmware);
        }
};


#endif //ULTRAHEAT
