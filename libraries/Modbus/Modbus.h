#ifndef Modbus_h
#define Modbus_h

#if defined(ARDUINO)
    #include "Arduino.h"
#else
    #include <stdlib.h>
#endif

#define DEFAULT_CODES  0x0000C03F // corresponds to all library supported functions

// Function codes
enum FunctionCode {
    READ_COIL_STATUS   = 0x01, // Read Coils
    READ_INPUT_STATUS  = 0x02, // Read Discrete Inputs
    READ_HOLDING_REGS  = 0x03, // Read Holding Registers
    READ_INPUT_REGS    = 0x04, // Read Input Registers
    WRITE_COIL         = 0x05, // Write Single Coil
    WRITE_REG          = 0x06, // Write Single Register
    WRITE_COILS        = 0x0F, // Write Multiple Coils
    WRITE_REGS         = 0x10, // Write Multiple Registers
};

// Request types
enum RequestType {
    SLAVE_MISMATCH     = 0x00, // Slave ID mismatch
    SLAVE_MATCH        = 0x01, // Slave ID match
    BROADCAST          = 0x02  // Broadcast request
};

// Reply types
enum ReplyType {
    NONE               = 0x00, // Don't respond
    SUCCESS            = 0x01, // Success response
    ECHO               = 0x02, // Echo the request back
    EXCEPTION          = 0x03, // Exception response
};

// Exception codes
enum ExceptionCode {
    ILLEGAL_FUNCTION   = 0x01, // Illegal function code
    ILLEGAL_ADDRESS    = 0x02, // Illegal data address
    ILLEGAL_VALUE      = 0x03, // Illegal data value
    SLAVE_FAILURE      = 0x04, // Failed to process the request
};

typedef ReplyType (*ResponseCallback)(RequestType request, uint8_t** const buffer, uint8_t& length);

class Modbus {
private:
    uint8_t* _memory; // data
    
protected:
    uint8_t _slave; // slave id
    uint16_t _size[4]; // cumulative memory sizes
    uint32_t _codes; // function code on/off bitmask
    ResponseCallback _callback; // callback for custom function codes
    
    uint8_t* _buffer; // buffer for request/response data
    uint8_t _length; // length of the buffer
    
    void allocate(uint8_t length); // reallocates buffer with given length
    
    uint8_t read(uint16_t address); // read data
    uint8_t read(uint16_t address, uint8_t offset); // read data with offset

    void write(uint16_t address, uint8_t value); // write data
    void write(uint16_t address, uint8_t offset, uint8_t value); // write data with offset

    // Communication Layer virtual functions
    virtual bool receive() { return false; }; // receive data
    virtual RequestType process() { return SLAVE_MISMATCH; }; // process data and determine request type
    virtual bool send(ReplyType reply) { return false; }; // send data
    
    // Reply functions
    ReplyType response(RequestType request); // reply to the request
    ReplyType exception(uint8_t fcode, ExceptionCode excode); // exception reply to the request
    
public:
    Modbus(uint8_t slave, const uint16_t* size);
    Modbus(uint8_t slave, const uint16_t* size, uint32_t codes);
    Modbus(uint8_t slave, const uint16_t* size, uint32_t codes, ResponseCallback callback);

    // Getters & Setters
    uint8_t getSlave();
    void setSlave(uint8_t slave);

    ResponseCallback getCallback();
    void setCallback(ResponseCallback callback);
    
    // Enable & Disable function codes
    uint32_t getCodes();
    void setCodes(uint32_t codes);
    bool isCodeEnabled(uint8_t code);
    void setCodeEnabled(uint8_t code);
    void setCodeDisabled(uint8_t code);
    
    // Register Read/Write
    bool getCoil(uint16_t offset);
    bool getStatus(uint16_t offset);
    uint16_t getHolding(uint16_t offset);
    uint16_t getInput(uint16_t offset);
    
    void setCoil(uint16_t offset, bool value);
    void setStatus(uint16_t offset, bool value);
    void setHolding(uint16_t offset, uint16_t value);
    void setInput(uint16_t offset, uint16_t value);
    
    void loop(); // lifecycle loop
};

#endif /* Modbus_h */
