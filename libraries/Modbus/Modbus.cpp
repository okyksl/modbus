#include "Modbus.h"

Modbus::Modbus(uint8_t slave, const uint16_t* size) : _slave(slave) {
    _size[0] = (size[0] + 8 - (size[0] % 8)) / 8;
    _size[1] = _size[0] + (size[1] + 8 - (size[1] % 8)) / 8;
    _size[2] = _size[1] + size[2] * 2;
    _size[3] = _size[2] + size[3] * 2;
    
    _memory = (uint8_t*) calloc(_size[3], sizeof(uint8_t));
}

/* Read & Write Data */
uint8_t Modbus::read(uint16_t address) {
    return _memory[address];
}

void Modbus::write(uint16_t address, uint8_t value) {
    _memory[address] = value;
}

/* Getters & Setters */
uint8_t Modbus::getSlave() {
    return _slave;
}

void Modbus::setSlave(uint8_t slave) {
    _slave = slave;
}

/* Register Read/Write */
bool Modbus::getCoil(uint16_t offset) {
    uint8_t result = read(offset / 8);
    return result & (1 << (7 - offset % 8));
}

bool Modbus::getStatus(uint16_t offset) {
    uint8_t result = read(_size[0] + offset / 8);
    return result & (1 << (7 - offset % 8));
}

uint16_t Modbus::getHolding(uint16_t offset) {
    uint16_t high = read(_size[2] + offset * 2);
    uint16_t low = read(_size[2] + offset * 2 + 1);
    return (high << 8) | low;
}

uint16_t Modbus::getInput(uint16_t offset) {
    uint16_t high = read(_size[1] + offset * 2);
    uint16_t low = read(_size[1] + offset * 2 + 1);
    return (high << 8) | low;
}

void Modbus::setCoil(uint16_t offset, bool value) {
    uint8_t result = read(offset / 8);
    result &= ~(1 << (7 - offset % 8));
    result |= (1 << (7 - offset % 8));
    write(offset / 8, result);
}

void Modbus::setStatus(uint16_t offset, bool value) {
    uint8_t result = read(_size[0] + offset / 8);
    result &= (1 << (7 - offset % 8));
    result |= (1 << (7 - offset % 8));
    write(_size[0] + offset / 8, result);
}

void Modbus::setHolding(uint16_t offset, uint16_t value) {
    uint8_t high = value >> 8;
    uint8_t low = (value << 8) >> 8;
    write(_size[2] + offset * 2, high);
    write(_size[2] + offset * 2 + 1, low);
}

void Modbus::setInput(uint16_t offset, uint16_t value) {
    uint8_t high = value >> 8;
    uint8_t low = (value << 8) >> 8;
    write(_size[1] + offset * 2, high);
    write(_size[1] + offset * 2 + 1, low);
}

