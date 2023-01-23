#ifndef ULTRAHEAT_OTA
#define ULTRAHEAT_OTA

#include "esphome.h"

class UltraheatReader : public Component, public UARTDevice {

    public:

        UltraheatReader(UARTComponent *parent)
            : UARTDevice(parent)
        {}

        void setup() override {
        }

        void loop() override {
        }

    private:
};


#endif // ULTRAHEAT_OTA
