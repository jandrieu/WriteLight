// writeLight.cpp
#include "WriteLight.h"
#include "Arduino.h"

 
WriteLight::WriteLight(LIGHT_UNIT u) :_light_unit(u){}

void WriteLight::announce() {
  	switch(_light_unit) {
  		case LIGHT_UNIT_1: Serial.println("LightUnit 0");break;
  		case LIGHT_UNIT_2: Serial.println("LightUnit 1");break;
  		case LIGHT_UNIT_3: Serial.println("LightUnit 2");break;
  		case LIGHT_UNIT_4: Serial.println("LightUnit 3");break;
  		case LIGHT_UNIT_ALL: Serial.println("LightUnit ALL");break;
  	}
}

void WriteLight::write(byte r, byte g, byte b) {
	if(_light_mode & LIGHT_MODE_SERIAL_WRITE) {
			write_serial(r,g,b);
	}
	
	if(_light_mode & LIGHT_MODE_LOCAL_WRITE) {
			write_analog(r,g,b);
	}

	if(_light_mode & LIGHT_MODE_SERIAL_DEBUG) {
			write_debug(r,g,b);
	}

}

void WriteLight::write_serial(byte r, byte g, byte b){
  // first, trim any high values (we use 255 and 254 as markers)
	byte R(r),G(g),B(b);
	
	r = r > 253 ? 253 : r;
	g = g > 253 ? 253 : g;
	b = b > 253 ? 253 : b;

	Serial2.write(255);
	Serial2.write(254);
	Serial2.write(_light_unit);
	Serial2.write(b);
	Serial2.write(g);
	Serial2.write(r);
	Serial2.write(254);
	Serial2.write(255);
	Serial2.flush();
}
void WriteLight::write_debug(byte r, byte g, byte b){
	Serial.print(" Debug: ");
	Serial.print(_light_unit);
	Serial.print(" ");
	Serial.print(r);
	Serial.print(" ");
	Serial.print(g);
	Serial.print(" ");
	Serial.println(b);
}



void WriteLight::write_analog(byte r, byte g, byte b) {
	if(_light_unit == LIGHT_UNIT_ALL) {
		
		for(int u(LIGHT_UNIT_1); u< LIGHT_UNIT_TOP; u++) {
			_write_analog(r,g,b,
			LIGHT_UNIT_PINS[u][0],
			LIGHT_UNIT_PINS[u][1],
			LIGHT_UNIT_PINS[u][2]);		
		}
		return;
	} else {
			_write_analog(r,g,b,
			LIGHT_UNIT_PINS[_light_unit][0],
			LIGHT_UNIT_PINS[_light_unit][1],
			LIGHT_UNIT_PINS[_light_unit][2]);		
	}							
}
void WriteLight::_write_analog(byte r, byte g, byte b, 
	int pin_r, int pin_g, int pin_b) {

	analogWrite(pin_r, r);
	analogWrite(pin_g, g);
	analogWrite(pin_b, b);
	return;
}	
void WriteLight::setup(byte mode) {
	_light_mode = mode;

 if(_light_mode & LIGHT_MODE_LOCAL_WRITE) {
	  for(int u(LIGHT_UNIT_1); u< LIGHT_UNIT_TOP; u++) {
				pinMode(LIGHT_UNIT_PINS[u][0],OUTPUT);
				pinMode(LIGHT_UNIT_PINS[u][1],OUTPUT);
				pinMode(LIGHT_UNIT_PINS[u][2],OUTPUT);
		}
	}
	
	WriteLight::WriteLight(LIGHT_UNIT_ALL).write(255,255,255);
}

bool WriteLight::isAnimated(void) {
	return _light_mode & LIGHT_MODE_ANIMATE;
}

void WriteLight::read_serial(void){

  // char input[8];

  // while(Serial1.available() && 
    // Serial1.peek()!= 255) {
    // Serial1.read();
  // }	

  // if(Serial1.available() >= 8) {
    // Serial1.readBytes(input,8);
  
    // int i;
    // for(i=0;i<8;i++) {
      // Serial.print((byte) input[i]);
      // Serial.print("\t");
    // }
    // Serial.println("");  
  // }
  
  // return;



	// discard up to the first possible header ("FF")
	while(Serial1.available() && 
		(byte)Serial1.peek()!= 255) {
		Serial1.read();
	}		
	if(Serial1.available() >=8) {	
// 255 254  C 	R 	G 	B 	255 254
// 0	1	2	3	4	5	6	7
	// ok, we have 6 good bytes in the Serial1 buffer
		char command[8];
		Serial1.readBytes(command,8);
		 Serial.print("brain:");
 		 Serial.print((byte)command[0]);
 		 Serial.print(" ");
 		 Serial.print((byte)command[1]);
 		 Serial.print("::");
 		 Serial.print((byte)command[2]);
 		 Serial.print(" ");
 		 Serial.print((byte)command[3]);
 		 Serial.print(" ");
 		 Serial.print((byte)command[4]);
 		 Serial.print(" ");
 		 Serial.print((byte)command[5]);
 		 Serial.print("::");
 		 Serial.print((byte)command[6]);
 		 Serial.print(" ");
 		 Serial.print((byte)command[7]);

		if((byte)command[6] != 254 || (byte)command[7] !=255) {
			Serial.println("\t\t\tERROR DISCARDED");
			while(Serial1.available() && 
				(byte)Serial1.peek()!= 255) {
				Serial1.read();
			}
			Serial1.read();
			return;
		} else {
//			Serial.println("\t\t\tACCEPTED");
		}
		//leave if command is out of range;		
		if(command[2] < LIGHT_UNIT_START || command[2] >= LIGHT_UNIT_END) {
			return;
		} 
		
		// the lights can handle 0-255, so they *can't* be out of range
		
 		WriteLight light(static_cast<LIGHT_UNIT>(command[2]));
 		light.write(command[3],command[4],command[5]);
	}
  	
}

void WriteLight::loop(void) {
	if(_light_mode & LIGHT_MODE_SERIAL_READ) {
		//Serial1.println("reading Serial1");
		read_serial();
	}

//	Serial1.println("TEST");
}

byte WriteLight::_light_mode(NULL);
