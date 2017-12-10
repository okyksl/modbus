#ifndef ModbusSerial_h
#define ModbusSerial_h

#if !defined(ARDUINO)
    #error "This class is specifically designed for Arduino"
#else
    #include "Modbus.h"
#endif

class ModbusSerial : public Modbus {
private:
    // Port configurations
    Stream*   _port;
    uint32_t  _baud;
    uint16_t  _format;
    uint16_t  _pin;
    uint16_t  _tin;
    uint16_t  _tout;
    
public:
    ModbusSerial(uint8_t slave, const uint16_t* size);
    ModbusSerial(uint8_t slave, const uint16_t* size, uint32_t codes);
    ModbusSerial(uint8_t slave, const uint16_t* size, uint32_t codes, ResponseCallback callback);
    ~ModbusSerial();
    
    void config(HardwareSerial* port, uint32_t baud, uint16_t format, uint16_t pin = -1); // configurate port
};

#endif /* ModbusSerial_h */
