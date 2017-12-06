# Modbus Library

This is a simple, lightweight library that allows you to communicate via Modbus protocol. Designed with the concerns of low memory availability and simplicity, this library is suitable for devices such as Arduino. Supports *Modbus Serial* & *Modbus Ethernet* communication on **slave** mode. (Hopefully, there will be support for **master** mode)

## Features

*  Adjustable memory sizes for register types
*  User defined function code support
*  Enable/disable function codes
*  Extensible to different communication protocols (Abstracts communication layer)
*  Default functions supported:
    *  0x01 - Read Coils
    *  0x02 - Read Discrete Input
    *  0x03 - Read Holding Registers
    *  0x04 - Read Input Registers
    *  0x05 - Write Single Coil
    *  0x06 - Write Single Register
    *  0x0F - Write Multiple Coils
    *  0x10 - Write Multiple Registers

## Getting Started

### Installing (Arduino)

Copy the base Modbus library and desired Modbus communication libraries as a folder to the **/libraries** folder of the Arduino IDE.

### Libraries
*  Modbus Core
    *  Register manipulation, user defined function support, enable/disable functions
*  Modbus Serial
    *  Serial communication configuration
*  Modbus Ethernet
    *  Ethernet communication configuration

## Acknowledgments

