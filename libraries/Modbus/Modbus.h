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
    
public:
    Modbus(uint8_t slave, const uint16_t* size);
    
    // Getters & Setters
    uint8_t getSlave();
    void setSlave(uint8_t slave);
};

#endif /* Modbus_h */
