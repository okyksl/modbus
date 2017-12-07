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

ReplyType Modbus::response(RequestType request) {
    uint8_t fcode = _buffer[0];
    if (!isCodeEnabled(fcode)) {
        return exception(fcode, ILLEGAL_FUNCTION);
    }
    
    uint16_t address = ((uint16_t) _buffer[1]) << 8 | _buffer[2];
    uint16_t length = ((uint16_t) _buffer[3]) << 8 | _buffer[4];
    
    ReplyType reply;
    switch (fcode) {
        case READ_COIL_STATUS: {
            if (address + length > _size[0] * 8) {
                return exception(fcode, ILLEGAL_ADDRESS);
            }
            
            uint8_t len = (length + 7) / 8;
            allocate(2 + len);
            _buffer[0] = fcode;
            _buffer[1] = len;
            
            for (uint8_t i = 0; i < len - 1; i++) {
                _buffer[2 + i] = read((address / 8) + i, address % 8);
            }
            uint8_t remaining = length % 8 == 0 ? 0 : (8 - length % 8);
            _buffer[2 + len - 1] = (read((address / 8) + len - 1, address % 8) >> remaining) << remaining;
            
            reply = SUCCESS;
            break;
        }
        case READ_INPUT_STATUS: {
            if (address + length > (_size[1] - _size[0]) * 8) {
                return exception(fcode, ILLEGAL_ADDRESS);
            }
            address += _size[0] * 8;
            
            uint8_t len = (length + 7) / 8;
            allocate(2 + len);
            _buffer[0] = fcode;
            _buffer[1] = len;
            
            for (uint8_t i = 0; i < len - 1; i++) {
                _buffer[2 + i] = read((address / 8) + i, address % 8);
            }
            uint8_t remaining = length % 8 == 0 ? 0 : (8 - length % 8);
            _buffer[2 + len - 1] = (read((address / 8) + len - 1, address % 8) >> remaining) << remaining;
            
            reply = SUCCESS;
            break;
        }
        case READ_HOLDING_REGS: {
            if (address + length > (_size[3] - _size[2])) {
                return exception(fcode, ILLEGAL_ADDRESS);
            }
            address += _size[2];
            
            uint8_t len = length * 2;
            allocate(2 + len);
            _buffer[0] = fcode;
            _buffer[1] = len;
            
            for (uint8_t i = 0; i < len; i++) {
                _buffer[2 + i] = read(address + i);
            }
            
            reply = SUCCESS;
            break;
        }
        case READ_INPUT_REGS: {
            if (address + length > (_size[2] - _size[1])) {
                return exception(fcode, ILLEGAL_ADDRESS);
            }
            address += _size[1];
            
            uint8_t len = length * 2;
            allocate(2 + len);
            _buffer[0] = fcode;
            _buffer[1] = len;
            
            for (uint8_t i = 0; i < len; i++) {
                _buffer[2 + i] = read(address + i);
            }
            
            reply = SUCCESS;
            break;
        }
        case WRITE_COIL: {
            if (address > _size[0] * 8) {
                return exception(fcode, ILLEGAL_ADDRESS);
            }
            
            if (length != 0xFF00 && length != 0x0000) {
                return exception(fcode, ILLEGAL_VALUE);
            }
            
            setCoil(address, length);
            
            reply = ECHO;
            break;
        }
        case WRITE_REG: {
            if (address > (_size[3] - _size[2])) {
                return exception(fcode, ILLEGAL_ADDRESS);
            }
            
            setHolding(address, length);
            
            reply = ECHO;
            break;
        }
        case WRITE_COILS: {
            if (address + length > _size[0] * 8) {
                return exception(fcode, ILLEGAL_ADDRESS);
            }
            
            uint8_t len = _buffer[5];
            for (uint8_t i = 0; i < len - 1; i++) {
                write(address / 8, address % 8, _buffer[6 + i]);
            }
            uint8_t remaining = length % 8 == 0 ? 0 : (8 - length % 8);
            write(address / 8 + len - 1, address % 8, ((_buffer[6 + len - 1] >> remaining) << remaining) |
                  ((read(address / 8 + len - 1, address % 8) << (8 - remaining)) >> (8 - remaining)));
            _length = 8;
            
            reply = ECHO;
            break;
        }
        case WRITE_REGS: {
            if (address + length > (_size[3] - _size[2])) {
                return exception(fcode, ILLEGAL_ADDRESS);
            }
            
            uint8_t len = _buffer[5];
            for (uint8_t i = 0; i < len / 2; i++) {
                setHolding(address + i, ((uint16_t)_buffer[6 + i * 2] << 8 | _buffer[6 + i * 2 + 1]));
            }
            _length = 8;
            
            reply = ECHO;
            break;
        }
        default: {
            if (_callback) {
                return _callback(request, &_buffer, _length);
            }
            return exception(fcode, ILLEGAL_FUNCTION);
        }
    }
    
    if (request == BROADCAST) {
        return NONE;
    }
    return reply;
}

ReplyType Modbus::exception(uint8_t fcode, ExceptionCode excode) {
    allocate(2);
    _buffer[1] = fcode | (1 << 7);
    _buffer[2] = excode;
    return EXCEPTION;
}
