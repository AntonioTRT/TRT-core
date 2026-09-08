#pragma once

#include <Arduino.h>

class TrtApplicationSerial {
   public:
    void begin(unsigned long) {}

    template <typename TValue>
    size_t print(const TValue&) {
        return 0;
    }

    template <typename TValue>
    size_t println(const TValue&) {
        return 0;
    }

    size_t println() { return 0; }
};

extern TrtApplicationSerial trt_application_serial;