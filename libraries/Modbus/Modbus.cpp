#include "Modbus.h"

Modbus::Modbus(uint8_t slave, const uint16_t* size) : Modbus(slave, size, DEFAULT_CODES) {}
Modbus::Modbus(uint8_t slave, const uint16_t* size, uint32_t codes) : Modbus(slave, size, codes, NULL) {};
Modbus::Modbus(uint8_t slave, const uint16_t* size, uint32_t codes, ResponseCallback callback) :
    _slave(slave), _codes(codes), _callback(callback) {
    _size[0] = (size[0] + 8 - (size[0] % 8)) / 8;
    _size[1] = _size[0] + (size[1] + 8 - (size[1] % 8)) / 8;
    _size[2] = _size[1] + size[2] * 2;
    _size[3] = _size[2] + size[3] * 2;
    
    _memory = (uint8_t*) calloc(_size[3], sizeof(uint8_t));
};

void Modbus::allocate(uint8_t length) {
    free(_buffer);
    
    _length = length;
    _buffer = (uint8_t *) malloc(_length * sizeof(uint8_t));
}

/* Read & Write Data */
uint8_t Modbus::read(uint16_t address) {
    return _memory[address];
}

uint8_t Modbus::read(uint16_t address, uint8_t offset) {
    uint8_t high = _memory[address];
    uint8_t low = _memory[address + 1];
    return (high << offset) | (low >> (8 - offset));
}

void Modbus::write(uint16_t address, uint8_t value) {
    _memory[address] = value;
}

void Modbus::write(uint16_t address, uint8_t offset, uint8_t value) {
    uint8_t remaining = 8 - offset;
    _memory[address] = ((_memory[address] >> remaining) << remaining) | (value >> offset);
    _memory[address + 1] = (value << remaining) | ((_memory[address + 1] << offset) >> offset);
}


/* Getters & Setters */
uint8_t Modbus::getSlave() {
    return _slave;
}

void Modbus::setSlave(uint8_t slave) {
    _slave = slave;
}

ResponseCallback Modbus::getCallback() {
    return _callback;
}

void Modbus::setCallback(ResponseCallback callback) {
    _callback = callback;
}

/* Enable & Disable function codes */
uint32_t Modbus::getCodes() {
    return _codes;
}

void Modbus::setCodes(uint32_t codes) {
    _codes = codes;
}

bool Modbus::isCodeEnabled(uint8_t code) {
    return _codes & (((uint32_t)1) << (code - 1));
}

void Modbus::setCodeEnabled(uint8_t code) {
    _codes |= (((uint32_t)1) << (code - 1));
}

void Modbus::setCodeDisabled(uint8_t code) {
    code = code & ~(((uint32_t)1) << (code - 1));
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
