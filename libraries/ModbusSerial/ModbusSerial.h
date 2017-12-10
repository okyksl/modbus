#ifndef ModbusSerial_h
#define ModbusSerial_h

#if !defined(ARDUINO)
    #error "This class is specifically designed for Arduino"
#else
    #include "Modbus.h"
#endif

class ModbusSerial : public Modbus {
public:
    ModbusSerial(uint8_t slave, const uint16_t* size);
    ModbusSerial(uint8_t slave, const uint16_t* size, uint32_t codes);
    ModbusSerial(uint8_t slave, const uint16_t* size, uint32_t codes, ResponseCallback callback);
    ~ModbusSerial();
};

#endif /* ModbusSerial_h */
