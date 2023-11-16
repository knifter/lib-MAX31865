#include "template.h"

// #include "tools-log.h"

bool DEVTEMP::begin(uint8_t address)
{
	if(!TwoWireDevice::begin(address))
	{
		// ERROR("TwoWireDevice.begin() failed.");
		return false;
	};
	// if(!SPIDevice::begin())
	// {
	// 	// ERROR("SPIDevice.begin() failed.");
	// 	return false;
	// };

	// reset();

	// uint16_t s = get_status();
	// if(s == 0xFFFF)
	// {
	// 	// ERROR("SHT3X reset failed, STATUS=FFFF");
	// 	return false;
	// };
	
	return true;
};
