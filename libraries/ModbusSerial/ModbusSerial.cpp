#include "ModbusSerial.h"

ModbusSerial::ModbusSerial(uint8_t slave, const uint16_t* size) : Modbus(slave, size) {}
ModbusSerial::ModbusSerial(uint8_t slave, const uint16_t* size, uint32_t codes, ResponseCallback callback) :
    Modbus(slave, size, codes, callback) {}
ModbusSerial::~ModbusSerial() {}

void ModbusSerial::config(HardwareSerial* port, uint32_t baud, uint16_t format, uint16_t pin = -1) {
    _port = port;
    _pin = pin;
    (*port).begin(baud, format);
    
    delay(2000);
    
    if (_pin >= 0) {
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, LOW);
    }
    
    if (baud > 19200) {
        _tin = 750;
        _tout = 1750;
    } else {
        _tin = 15000000 / baud;
        _tout = 35000000 / baud;
    }
}
