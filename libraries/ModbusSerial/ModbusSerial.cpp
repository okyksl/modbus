#include "ModbusSerial.h"

ModbusSerial::ModbusSerial(uint8_t slave, const uint16_t* size) : Modbus(slave, size) {}
ModbusSerial::ModbusSerial(uint8_t slave, const uint16_t* size, uint32_t codes, ResponseCallback callback) :
    Modbus(slave, size, codes, callback) {}
ModbusSerial::~ModbusSerial() {}
