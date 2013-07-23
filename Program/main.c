//Delay 3 is max
// Example 1a: Turn on every other segment on 7-seg display
#include <hidef.h>      /* common defines and macros */
#include <mc9s12dg256.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"
#include "string.h"
#include "stdio.h"
#include "main_asm.h" /* interface to the assembly module */

#define RTI_FLAG 0b00010001
#define CW_SLOW 0b00000001
#define CW_FAST 0b00000010
#define CCW_SLOW 0b00000100
#define CCW_FAST 0b00001000
#define STOP_GAS 0b00001000
#define SWEEP_SERVO 0b00010000
#define SWEEP_NEUTRAL 0b01000000
#define RANDOM_SERVO 0b00100000
#define NORTH 0b00000001
#define NORTHEAST 0b00000011
#define EAST 0b00000010
#define SOUTHEAST 0b00000110
#define SOUTH 0b00000100
#define SOUTHWEST 0b00001100
#define WEST 0b00001000
#define NORTHWEST 0b00001001
#define LEFT 3500
#define RIGHT 4750
#define STRAIGHT 4275
#define STOP 0
#define TWOFEETFAST 4375
#define TWOFEETSLOW 8650
#define CIRCLEDELAY 21770
unsigned char FAST_DELAY;
unsigned char currentPTH;
unsigned char engineSpeed;
unsigned int direction;
 int index;

int delay;
int i,n; 
char buf[20];
void moveIndex(void);
unsigned char motor[8]= { NORTH, NORTHEAST, EAST,SOUTHEAST, SOUTH,SOUTHWEST, WEST, NORTHWEST};
const int leftMax = LEFT;
const int neutral = STRAIGHT;
const int rightMax = RIGHT;

char sweepLeft = 0;
char seed = 1;
unsigned char modeChanged = 1;
void beep(int mil);
void raceMode(void);
void drivingTest(void);
void speedUp(void);
void main(void) {
  /* put your own code here */
  PLL_init();        // set system clock frequency to 24 MHz
  lcd_init();
  servo76_init();
  set_servo76(STRAIGHT);
  sound_init(); 
  DDRB  = 0xff;       // Port B is output
  DDRJ  = 0xff;       // Port J is output
  DDRP  = 0xff;       // Port P is output
  DDRH = 0x00;
  currentPTH = 0;
  TSCR1 = 0b10000000;
  delay = 0;
  i = 0;
  CRGINT |= CRGINT_RTIE_MASK;
  RTICTL = RTI_FLAG;
  EnableInterrupts;
  for(;;){
    FAST_DELAY = 35;
    if(PTH == 0x00){
		  drivingTest();
    } else{
		  raceMode();
    }
    engineSpeed = STOP;
    ms_delay(10000);
  } // end main loop	//
}// end main	//
void raceMode(void){
	clear_lcd();
	type_lcd("Race Mode");
	speedUp();
	ms_delay(10000); 
}
void speedUp(void){
	direction = STRAIGHT;
	engineSpeed = CCW_FAST;   
	for(index = 0; index < 25; index++) {
		FAST_DELAY--;
		ms_delay(100); 
	}
}
void drivingTest(void){
	// Turn on Buzzer, Delay 3 sec, Turn off Buzzer	//
	clear_lcd();
	type_lcd("Stop - Beep");    
	beep(3000); 

	// Move forward quickly 2 feet	//
	clear_lcd();
	type_lcd("Moving Forward");
	direction = STRAIGHT;
	engineSpeed = CCW_FAST;
	ms_delay(TWOFEETFAST); 
	engineSpeed = STOP;

	// Turn buzzer on, Wait 2 seconds, Turn buzzer off	//
	clear_lcd();
	type_lcd("Stop - Beep");
	beep(2000);

	// Move backward quickly 2 feet
	clear_lcd();
	type_lcd("Moving Backward");
	direction = STRAIGHT;
	engineSpeed = CW_FAST;
	ms_delay(TWOFEETFAST); 

	// Turn buzzer on, wait 2 seconds, Turn Buzzer off	//
	clear_lcd();
	type_lcd("Stop - Beep");
	beep(2000);

	// Move Forward quickly one foot	//
	clear_lcd();
	type_lcd("1 Foot Fast");
	direction = STRAIGHT;
	engineSpeed = CCW_FAST;
	ms_delay(TWOFEETFAST/2); 

	// Move Forward slowly one more foot	//
	clear_lcd();
	type_lcd("1 Foot Slow");
	engineSpeed = CCW_SLOW;
	ms_delay(TWOFEETSLOW/2);

	// wait 2 seconds 	//
	clear_lcd();
	type_lcd("Wait 2 Sec.");
	engineSpeed = STOP;
	direction = STRAIGHT;
	ms_delay(2000);

	//Figure 8	//
	clear_lcd();
	type_lcd("FIGURE 8");
	direction = LEFT;
	engineSpeed = CCW_FAST;
	ms_delay(CIRCLEDELAY);
	direction = RIGHT;
	ms_delay(CIRCLEDELAY);

	// wait 2 seconds	//
	clear_lcd();
	type_lcd("WAIT - 2 SEC");
	engineSpeed = STOP;
	ms_delay(2000);

	// Perform a 3 point turn	//
	clear_lcd();
	type_lcd("3-POINT TURN");
	engineSpeed = CCW_FAST;
	direction = LEFT;
	ms_delay(CIRCLEDELAY * 0.255);
	engineSpeed = CW_FAST;
	direction = RIGHT;
	ms_delay(CIRCLEDELAY * 0.255);
	direction = STRAIGHT;
	engineSpeed = CCW_FAST;
	ms_delay(TWOFEETFAST); 

	// Wait 5 seconds	//
	clear_lcd();
	type_lcd("Wait 5 sec.");
	engineSpeed = STOP;
	ms_delay(5000);

	// Drive in a crazy fashion for 5 seconds	//
	clear_lcd();
	type_lcd("We cray cray!");
	engineSpeed = CCW_FAST;
	direction = LEFT;
	ms_delay(1000);
	direction = RIGHT;
	ms_delay(1000);
	direction = LEFT;
	ms_delay(1000);
	direction = RIGHT;
	ms_delay(1000);
	direction = LEFT;
	ms_delay(500);
	direction = RIGHT;
	ms_delay(500);

	// Drive Forward as fast as possible	//
	clear_lcd();
	type_lcd("Need For Speed");
	speedUp();
	ms_delay(6000);
}
interrupt 7 void RTI_Handler(void){
  if(!delay){
    moveIndex();
    PORTB = motor[i];  
  }else{
    --delay;
  }
  CRGFLG |= CRGFLG_RTIF_MASK;
}
void moveIndex(void){ 
   modeChanged = 0;
   switch(engineSpeed){
    case CW_SLOW:
      i = (i < 7) ? i + 1: 0;
      delay = 70;
      break;
    case CW_FAST:
      i = (i < 7) ? i + 1: 0;
      delay = FAST_DELAY;
      break;
    case CCW_SLOW:
      i = (i > 0) ? i - 1: 7;
      delay = 70;
      break;
    case CCW_FAST:
      i = (i > 0) ? i - 1: 7;
      delay = FAST_DELAY;
      break;
   }
   set_servo76(direction);   
}
void interrupt 13 beep_handler() {
  tone(15500);  // pitch is is the frequency in Hz   400 is a good value
}

void beep(int mil) 
{
    CRGINT = 0x00;
    DisableInterrupts;
    sound_on();
    ms_delay(mil);
    sound_off();
    EnableInterrupts;
    CRGINT = 0x80;      
}