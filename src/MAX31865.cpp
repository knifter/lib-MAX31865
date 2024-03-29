#include "MAX31865.h"

#ifdef DEBUG_MAX31865
    #include "tools-log.h"
#endif

#define REG_CONFIG					0x00
#define 	CONFIG_BIAS 			0x80
#define 	CONFIG_MODEAUTO 		0x40
#define 	CONFIG_1SHOT 			0x20
#define 	CONFIG_3WIRE 			0x10
#define     CONFIG_FAULTDET_MASK    0x0C
#define 	CONFIG_FAULTCLEAR 		0x02
#define 	CONFIG_FILT50HZ 		0x01
#define REG_RTD_MSB 				0x01
#define REG_RTD_LSB 				0x02
#define REG_HIGHFAULT_MSB 			0x03
#define REG_HIGHFAULT_LSB 			0x04
#define REG_LOWFAULT_MSB 			0x05
#define REG_LOWFAULT_LSB 			0x06
#define REG_FAULTSTAT				0x07
#define     FAULTSTAT_HIGHTHRESH    0x80    // setHighThreshold
#define     FAULTSTAT_LOWTHRESH     0x40    // setLowThreshold
#define     FAULTSTAT_REFINLOW 	    0x20
#define     FAULTSTAT_REFINHIGH     0x10        
#define     FAULTSTAT_RTDINLOW 	    0x08
#define     FAULTSTAT_OVUV 		    0x04

bool MAX31865::begin(float Rnominal, float Rreference, bool threewires, bool net50hz)
{
	if(!SPIDevice::begin())
	{
		// ERROR("SPIDevice.begin() failed.");
		return false;
	};
    
    _automode = false;
    _Rnom = Rnominal;
    _Rref = Rreference;

    uint8_t config = 0x00;
    if(threewires)
        config |= CONFIG_3WIRE;
    if(net50hz)
    {
        config |= CONFIG_FILT50HZ;
    };
    config |= CONFIG_FAULTCLEAR;

    // both done by above code:
    // setAutoConvert(false);
    // clearFaults();
	writereg8(REG_CONFIG, config);

	return true;
};

void MAX31865::setThreeWires(bool usethree) 
{
	uint8_t config = readreg8(REG_CONFIG);

    if(usethree)
    {
        config |= CONFIG_3WIRE;
    }else{
        config &= ~CONFIG_3WIRE;
    };
	writereg8(REG_CONFIG, config);
};

// void MAX31865::setEnableBias(bool bias) 
// {
// 	uint8_t config = readreg8(REG_CONFIG);
// 	if (bias) 
//         config |= CONFIG_BIAS; // enable bias
// 	else 
// 		config &= ~CONFIG_BIAS; // disable bias
// 	writereg8(REG_CONFIG, config);
// };

void MAX31865::setAutoConvert(bool ac) 
{
	uint8_t config = readreg8(REG_CONFIG);
	if (ac)
		config |= (CONFIG_MODEAUTO | CONFIG_BIAS); 
	else
		config &= ~(CONFIG_MODEAUTO | CONFIG_BIAS);
	writereg8(REG_CONFIG, config);
    _automode = ac;
};

void MAX31865::setLowThreshold(uint16_t th) 
{
	writereg8(REG_LOWFAULT_LSB, th & 0xFF);
	writereg8(REG_LOWFAULT_MSB, th >> 8);
};

void MAX31865::setHighThreshold(uint16_t th) 
{
	writereg8(REG_HIGHFAULT_LSB, th & 0xFF);
	writereg8(REG_HIGHFAULT_MSB, th >> 8);
};

void MAX31865::setNet50(bool use50) 
{
    uint8_t config = readreg8(REG_CONFIG);

    if(use50)
    {
        config |= CONFIG_FILT50HZ;
    }else{
        config &= ~CONFIG_FILT50HZ;
    };
	writereg8(REG_CONFIG, config);
};

void MAX31865::clearFaults() 
{
	uint8_t config = readreg8(REG_CONFIG);
	config &= ~(CONFIG_FAULTDET_MASK | CONFIG_1SHOT); // .. while writing 0 to D5, D3, D2
  	config |= CONFIG_FAULTCLEAR;
  	writereg8(REG_CONFIG, config);
};

uint8_t MAX31865::readFaults() //max31865_fault_cycle_t fault_cycle) 
{
    // if (fault_cycle) 
    // {
    //     uint8_t cfg_reg = readreg8(REG_CONFIG);
    //     cfg_reg &= 0x11; // mask out wire and filter bits
    //     switch(fault_cycle)
    //     {
    //         case MAX31865_FAULT_AUTO:
    //             writereg8(REG_CONFIG, (cfg_reg | 0b10000100));
    //             delay(1);
    //             break;
    //         case MAX31865_FAULT_MANUAL_RUN:
    //             writereg8(REG_CONFIG, (cfg_reg | 0b10001000));
    //             return 0;
    //         case MAX31865_FAULT_MANUAL_FINISH:
    //             writereg8(REG_CONFIG, (cfg_reg | 0b10001100));
    //             return 0;
    //         case MAX31865_FAULT_NONE:
    //         default:
    //             break;
    //     };
    // };
    return readreg8(REG_FAULTSTAT);
};

#ifdef DEBUG_MAX31865
void MAX31865::printFaults()
{
    uint8_t f = readFaults();
    String msg;
    if(f & 0x80) // D7
        msg += "D7(RTDIN_Open) ";
    if(f & 0x40) // D6
        msg += "D6(RTDIN_Short) ";
    if(f & 0x20) // D5
        msg += "D5 ";
    if(f & 0x10) // D4
        msg += "D4 ";
    if(f & 0x08) // D3
        msg += "D3 ";
    if(f & 0x04) // D2
        msg += "D2(Over/Under Voltage!) ";
    ERROR("Faults: %s", msg.c_str());
};
#endif

void MAX31865::startOneshot()
{

#ifdef DEBUG_MAX31865
    DBG("Start one-shot");
    clearFaults();
#endif
    
    // Turn on bias and wait a while
    // setEnableBias(true);
    uint8_t cfg = readreg8(REG_CONFIG);
    cfg |= CONFIG_BIAS;
    writereg8(REG_CONFIG, cfg);
    // delay(10);

    // Start oneshot
    cfg |= CONFIG_1SHOT;
    writereg8(REG_CONFIG, cfg);
    delay(65);

    // And bias off
    // setEnableBias(false); // Disable bias current again to reduce selfheating.
    cfg &= ~CONFIG_BIAS;
    writereg8(REG_CONFIG, cfg);

    return;
};

uint16_t MAX31865::getRaw()
{
    uint16_t val = readreg16(REG_RTD_MSB);
    if(val & 0x0001)
    {
#ifdef DEBUG_MAX31865
        printFaults();
#else
        clearFaults();
#endif
        return 0xFFFF;
    };
    return val >> 1;
};

float MAX31865::getTemperature() 
{
    if(!_automode)
        startOneshot();
    
    uint16_t raw = getRaw();    
    float rtd = raw * _Rref / 32768.0;
#ifdef DEBUG_MAX31865
    DBG("raw = %u, R = %.1f Ohms", raw, rtd);
#endif

    if(raw == 0xFFFF) // error
        return NAN;

    // Datasheet, page 10: R(T) = R0(1 + aT + bT^2 + c(T - 100)T^3)
    // a = 3.90830 x 10-3
    // b = -5.77500 x 10-7
    // c = -4.18301 x 10-12 for -200C < T < 0C, or c = 0 for 0C < T < 850C
    
#define RTD_A   3.9083e-3
#define RTD_B   -5.775e-7
    // float Z1 = -RTD_A;
#define RTD_Z1      (-RTD_A)
    // float Z2 = RTD_A * RTD_A - (4 * RTD_B);
#define RTD_Z2      (RTD_A * RTD_A - (4 * RTD_B))
    float Z3 = (4 * RTD_B) / _Rnom;
    // float Z4 = 2 * RTD_B;
#define RTD_Z4      (2*RTD_B);

    float temp = RTD_Z2 + (Z3 * rtd);
    temp = (sqrt(temp) + RTD_Z1) / RTD_Z4;

    if (temp >= 0)
        return temp;

    rtd /= _Rnom;
    rtd *= 100; // normalize to 100 ohm
    float rpoly = rtd; 

    // ^1
    temp = -242.02;
    temp += 2.2228 * rpoly;

    // ^2
    rpoly *= rtd;
    temp += 2.5859e-3 * rpoly;

    //^3
    rpoly *= rtd;
    temp -= 4.8260e-6 * rpoly;

    //^4
    rpoly *= rtd;
    temp -= 2.8183e-8 * rpoly;

    //^5
    rpoly *= rtd;
    temp += 1.5243e-10 * rpoly;

    return temp;
};
