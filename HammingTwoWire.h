#ifndef HAMMING_TWO_WIRE_H
#define HAMMING_TWO_WIRE_H
#if defined(ARDUINO_AVR_UNO)
    #include "Arduino.h"
#endif
#include "Wire.h"
class HammingTwoWire : public TwoWire {
    public:
        HammingTwoWire(TwoWire& input_stream);
        virtual size_t write(uint8_t byte);
        virtual size_t write(const uint8_t *buffer, size_t size);
        virtual int read();
        virtual int available();
        virtual void flush();
        virtual int peek();
    protected:
        TwoWire& input_stream;
        uint8_t encode4(uint8_t nibble);
        uint8_t decode8(uint8_t code);
        //BLA::Matrix<4,7> generator_M;
        //BLA::Matrix<3,7> parity_check_M;
        // uint8_t generator_M
};

#endif