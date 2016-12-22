#ifndef MODELICA_ARDUINO_C
#define MODELICA_ARDUINO_C

#define DYMOLA_STATIC

#include <windows.h>
#include <stdio.h>
#include <conio.h>

#include <math.h> // for fmod()


#include "ModelicaUtilities.h"

extern "C" {

#include "ModelicaArduino.h"

}


#include <stdint.h>
//typedef bool BOOL;
typedef uint8_t BYTE;
typedef int8_t INT8;
typedef uint8_t UNINT8;
typedef int16_t INT16;
typedef uint16_t UINT16;
typedef int32_t INT32;
typedef uint32_t UNINT32;
typedef int64_t INT64;
typedef uint64_t UINT64;

// from Print.h
#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2


// from pins_arduino.h
#define LED_BUILTIN 13

#define PIN_A0   (0)
#define PIN_A1   (1)
#define PIN_A2   (2)
#define PIN_A3   (3)
#define PIN_A4   (4)
#define PIN_A5   (5)
#define PIN_A6   (6)
#define PIN_A7   (7)

static const uint8_t A0 = PIN_A0;
static const uint8_t A1 = PIN_A1;
static const uint8_t A2 = PIN_A2;
static const uint8_t A3 = PIN_A3;
static const uint8_t A4 = PIN_A4;
static const uint8_t A5 = PIN_A5;
static const uint8_t A6 = PIN_A6;
static const uint8_t A7 = PIN_A7;

// from Arduino.h
#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

/*
typedef struct {

	double time;
	//int builtInLedOn;
	double analog[8];

	// LED_BUILTIN, D0..D13
	//int digital[15];
	int portMode[15]; // 0 = input, 1 = digital, 2 = PWM
	double pulseWidth[15];

} State_t;
*/

class Arduino {

public:
	double time;
	double analog[8];
	int portMode[15]; // 0 = input, 1 = digital, 2 = PWM
	double pulseWidth[15];

#include "Blink.ino"

	void pinMode(int pin, int mode) {
		ModelicaFormatMessage("pinMode(%d, %d)\n", pin, mode);
		portMode[pin] = mode;
	}

	void digitalWrite(int pin, int val) {
		pulseWidth[pin] = (val == HIGH) ? 100 : 0;
		ModelicaFormatMessage("digitalWrite(%d, %d) -> %f\n", pin, val, pulseWidth[pin]);
	}

	void delay(int ms) {
		double end_time = time + ms * 1e-3;
		while (time < end_time) {
			// idle
		}
	}

};


class SoftSerial {

public:

    void begin(unsigned long baud) { }
    
	size_t println(int num, int base = DEC) {
		ModelicaFormatMessage("%d\n", num);
		return 0;
	}

};

SoftSerial Serial;

//static Arduino state_s;

//static long count;

/*
void s_pinMode(int pin, int mode) {
	ModelicaFormatMessage("pinMode(%d, %d)\n", pin, mode);
	state_s.portMode[pin] = mode;
}
*/

//#define pinMode(pin, mode) s_pinMode(pin, mode)

/*
void digitalWrite(int pin, int val) {
	
	state_s.pulseWidth[pin] = (val == HIGH) ? 100 : 0;

	//ModelicaFormatMessage("digitalWrite(%d, %d) -> %f\n", pin, val, state_s.pulseWidth[pin]);

}

void delay(int ms) {

	double end_time = state_s.time + ms * 1e-3;
	
	//ModelicaFormatMessage("delay(%d)\n", ms);

	while (state_s.time < end_time) {
		// idle
		// ModelicaFormatMessage("idle: %f < %f\n", state_s.time , end_time);
		//count++;
	}

	//ModelicaFormatMessage("count: %d\n", count);

}


//uint8_t analog_reference = DEFAULT;

void analogReference(uint8_t mode)
{
	// can't actually set the register here because the default setting
	// will connect AVCC and the AREF pin, which would cause a short if
	// there's something connected to AREF.
	//analog_reference = mode;
}


int analogRead(uint8_t pin) {

	// TODO: clip [0, 1023]
	return (state_s.analog[pin] / 5) * 1024;

	// TODO: error?
	//return 0;
}


void analogWrite(uint8_t pin, int val) {

	state_s.pulseWidth[pin] = (100.0 * val) / 255;

}
*/

//#include "Blink.ino"


DWORD WINAPI MyThreadFunction(LPVOID lpParam) {

	//State_t *state = (State_t *)lpParam;

	Arduino *instance = reinterpret_cast<Arduino *>(lpParam);

	//ModelicaFormatMessage("The parameter: %u.\n", *(DWORD*)lpParam);

	for(;;) {
		//state->builtInLedOn = fmod(state->time, 1) > 0.5 ? 5 : 0;
		instance->loop();
	}

	return 0;
}


void * ModelicaArduino_open() {

	DWORD dwThreadId, dwThrdParam = 1;
	HANDLE hThread;
	//Arduino *state;
	//count = 0;

	//ModelicaMessage("ModelicaArduino_open()\n");

	//state = &state_s; //malloc(sizeof(State_t));

	Arduino *instance = new Arduino();

	instance->setup();
 
	hThread = CreateThread(
	NULL, 				// default security attributes
	0, 					// use default stack size
	MyThreadFunction, 	// thread function
	instance, 		    // argument to thread function
	0, 					// use default creation flags
	&dwThreadId); 		// returns the thread identifier
	 
	ModelicaFormatMessage("The thread ID: %d.\n", dwThreadId);
	 
	// Check the return value for success. If something wrong...
	if (hThread == NULL)
		ModelicaFormatMessage("CreateThread() failed, error: %d.\n", GetLastError());
	//else, gives some prompt...
	else
		ModelicaFormatMessage("It seems the CreateThread() is OK lol!\n");

	if (CloseHandle(hThread) != 0)
		ModelicaFormatMessage("Handle to thread closed successfully.\n");

	//return (void *)NULL;

	return instance;
}


void ModelicaArduino_close(void *externalObject) {

	//ModelicaMessage("ModelicaArduino_close()\n");

	// TODO: clean up

}

void ModelicaArduino_update(void *instance, double time, double *analog, int *portMode, double *pulseWidth) {

	//*builtInLedOn = fmod(time, 1) > 0.5 ? 5 : 0;
	//State_t *state = (State_t *)instance;
	//state->time = time;
	//*builtInLedOn = state->builtInLedOn;

	Arduino *instance_ = reinterpret_cast<Arduino *>(instance);

	instance_->time = time;

	for (int i = 0; i < 6; i++) {
		instance_->analog[i] = analog[i];
	}

	for (int i = 0; i < 14; i++) {
		portMode[i] = instance_->portMode[i];
		pulseWidth[i] = instance_->pulseWidth[i];
	}

}

#endif // MODELICA_ARDUINO_C
