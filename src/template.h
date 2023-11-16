#ifndef __DEVTEMP_H
#define __DEVTEMP_H

#include <Arduino.h>
#include <TwoWireDevice.h>
// #include <SPIDevice.h>

// CONFIG
#define DEVTEMP_ADDRESS_DEFAULT      (0x44)
#define DEVTEMP_ADDRESS_ALT	       (0x45)
// #define DEVTEMP_SPI_SPEED			  4E6

class DEVTEMP : public TwoWireDevice
// class DEVTEMP : public SPIDevice
{
	public:
		//constructors
		DEVTEMP(const uint8_t addr = DEVTEMP_ADDRESS_DEFAULT) : TwoWireDevice(addr) {};
   		DEVTEMP(TwoWire& wire, const uint8_t addr = DEVTEMP_ADDRESS_DEFAULT)  : TwoWireDevice(wire, addr) {};
        // DEVTEMP(SPIClass& spi, const uint8_t pin_cs)
        //     : SPIDevice(spi, pin_cs, DEVTEMP_SPI_SPEED, SPI_MSBFIRST, SPI_MODE1) {};
        ~DEVTEMP() {};

		bool begin(uint8_t address = 0);
		// void reset();
	
    protected:

	private:
        // Device instance is non-copyable
		DEVTEMP(const DEVTEMP&);
		DEVTEMP& operator=(const DEVTEMP&);
};

#endif // __DEVTEMP_H
