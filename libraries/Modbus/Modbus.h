#ifndef Modbus_h
#define Modbus_h

#if defined(ARDUINO)
    #include "Arduino.h"
#else
    #include <stdlib.h>
#endif

class Modbus {
private:
    uint8_t* _memory; // data
    
protected:
    uint8_t _slave; // slave id
    uint16_t _size[4]; // cumulative memory sizes
    
    uint8_t read(uint16_t address); // read data
    void write(uint16_t address, uint8_t value); // write data
    
public:
    Modbus(uint8_t slave, const uint16_t* size);
    
    // Getters & Setters
    uint8_t getSlave();
    void setSlave(uint8_t slave);

    // Register Read/Write
    bool getCoil(uint16_t offset);
    bool getStatus(uint16_t offset);
    uint16_t getHolding(uint16_t offset);
    uint16_t getInput(uint16_t offset);
    
    void setCoil(uint16_t offset, bool value);
    void setStatus(uint16_t offset, bool value);
    void setHolding(uint16_t offset, uint16_t value);
    void setInput(uint16_t offset, uint16_t value);
};

#endif /* Modbus_h */
