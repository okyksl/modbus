#include "Modbus.h"

Modbus::Modbus(uint8_t slave, const uint16_t* size) : _slave(slave) {
    _size[0] = (size[0] + 8 - (size[0] % 8)) / 8;
    _size[1] = _size[0] + (size[1] + 8 - (size[1] % 8)) / 8;
    _size[2] = _size[1] + size[2] * 2;
    _size[3] = _size[2] + size[3] * 2;
    
    _memory = (uint8_t*) calloc(_size[3], sizeof(uint8_t));
}

/* Getters & Setters */
uint8_t Modbus::getSlave() {
    return _slave;
}

void Modbus::setSlave(uint8_t slave) {
    _slave = slave;
}
