// WriteLight library include file
#ifndef WriteLight_h
#define WriteLight_h

#include "Arduino.h"

//#ifdef BRAIN
//  #define SERIAL Serial1
//#else
//  #define SERIAL Serial2
//#endif

// light components
enum LIGHT_UNIT {
   LIGHT_UNIT_START=0,
   LIGHT_UNIT_1=0,
   LIGHT_UNIT_2=1,
   LIGHT_UNIT_3=2,
   LIGHT_UNIT_4=3,
   LIGHT_UNIT_TOP=4, // for cycling (TOP > last valid light)
	 LIGHT_UNIT_ALL=4,  
	 LIGHT_UNIT_END=5 // for testing valid values
};

const byte LIGHT_MODE_SERIAL_READ(B0001);
const byte LIGHT_MODE_SERIAL_WRITE(B0010);
const byte LIGHT_MODE_LOCAL_WRITE(B0100);
const byte LIGHT_MODE_ANIMATE(B1000);
const byte LIGHT_MODE_SERIAL_DEBUG(B10000);

const int LIGHT_UNIT_PINS[7][3]={  // b, g, r 
								{45,3,4}, // blue quad
								{5,6,7},  // red quadrant
								{8,9,10}, // yellow quadrant
								{11,12,13}, // green quadrant
								{44,44,44}, // dummy values for NONE
								{44,44,44}, // dummy values for ALL
								{44,44,44}};// dummy values for END (to be safe) 
								
// forward declaration of RGB output
//void WriteLight(LIGHT_UNIT u, int r, int g, int b);

class WriteLight {
  public:
    WriteLight(LIGHT_UNIT u);
    void write(byte r, byte g, byte b);
	void announce(void);
	static void setup(byte light_mode);
	static bool isAnimated(void);
	static void loop(void);

private:
	void write_serial(byte r, byte g, byte b);
	void write_debug(byte r, byte g, byte b);
	void write_analog(byte r, byte g, byte b);
	void _write_analog(byte r, byte g, byte b, 
	int pin_r, int pin_g, int pin_b);
	static void read_serial(void);
	 
	LIGHT_UNIT _light_unit;
	static byte _light_mode;

};




#endif

