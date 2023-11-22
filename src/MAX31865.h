#ifndef __DEVTEMP_H
#define __DEVTEMP_H

#include <Arduino.h>
#include <SPIDevice.h>

// CONFIG
#define MAX31865_SPI_SPEED			1E6

class MAX31865 : public SPIDevice
{
	public:
		//constructors
        MAX31865(SPIClass& spi, const uint8_t pin_cs)
            : SPIDevice(spi, pin_cs, MAX31865_SPI_SPEED, SPI_MSBFIRST, SPI_MODE1) {};
        ~MAX31865() {};

		bool begin(float Rnominal = 100, float Rref = 430, bool threewires = false, bool net50hz = true);		

		void setThreeWires(bool);
		void setEnableBias(bool);
		void setAutoConvert(bool);
		void setNet50(bool);
		void setLowThreshold(uint16_t th);
		void setHighThreshold(uint16_t th);

		// uint8_t readFaults();
		void clearFaults();

		void startOneshot();
		uint16_t getRaw();
		float getTemperature(); // respects automode / oneshot

	private:
		bool _automode = false; // oneshot mode
		float _Rnom, _Rref;

        // Device instance is non-copyable
		MAX31865(const MAX31865&);
		MAX31865& operator=(const MAX31865&);
};

#endif // __DEVTEMP_H
