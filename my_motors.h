#ifndef MY_MOTORS_H
#define MY_MOTORS_H

#include "my_mcu.h"
//#include <peripheral/outcompare.h>	// Output Compare lib:
//#include <peripheral/incap.h>		// Input capture lib:

#define DC_ZEROPCT			0
#define DC_50PCT			128		//50% duty cycle = 0.5 * PR = 0.5 * 256 = 128
#define DC_60PCT			154		//60% duty cycle = 0.6 * PR = 0.6 * 256 = 154
#define DC_65PCT			166		// etc...
#define DC_70PCT			179
#define DC_75PCT			192		//for the most part, we will always use a duty cycle between 60% (slow) and 85% (full throttle) (and 0% to stop)
#define DC_80PCT			205
#define PR_BASE       256
#define FORWARD         1
#define REVERSE         0

void switchMotors(short forward);
void setMotorsReverse(void);
void setMotorsForward(void);
void configOCModule2(void);
void configOCModule3(void);
void writeOC2(unsigned int newOC2RValue, unsigned int newOC2RSValue);
void writeOC3(unsigned int newOC3RValue, unsigned int newOC3RSValue);





#endif
