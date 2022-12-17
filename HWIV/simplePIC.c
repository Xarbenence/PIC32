#include <xc.h>          // Load the proper header for the processor
#define MAXCYCLES 1000000 // define MAXCYCLES as 1000000
#define DELTACYCLES 100000 // define DELTACYCLES as 100000
void delay(int cycles); // call delay with cycles

int input = MAXCYCLES; // intialize input to delay() as equal to MAXCYCLES

int main(void) {
  TRISF = 0xFFFC;        // Pins 0 and 1 of Port F are LED1 and LED2.  Clear
                         // bits 0 and 1 to zero, for output.  Others are inputs.
  LATFbits.LATF0 = 0;    // Turn LED1 on and LED2 off.  These pins sink current
  LATFbits.LATF1 = 0;    // on the NU32, so "high" (1) = "off" and "low" (0) = "on"
  // start by setting both LEDs to off


  while(1) {
    delay(input); 
    LATFINV = 0b11; // Inverts the state of the LEDs every time.
    input = input - DELTACYCLES; // decrease input to delay() by DELTACYCLES
    if(input <= 0){ // if our input to delay() is <= to 0, reset it to MAXCYCLES
      input = MAXCYCLES;
    }

  }
  return 0;
}


void delay(int cycles) {
  int j;
  
  for (j = 0; j < cycles; j++) { // number is cycles
    while(!PORTDbits.RD7) {
        ;   // Pin D7 is the USER switch, low (FALSE) if pressed.
    }
  }
}





/*
#include <xc.h>          // Load the proper header for the processor

void delay(void);

int main(void) {
  TRISF = 0xFFFC;        // Pins 0 and 1 of Port F are LED1 and LED2.  Clear
                         // bits 0 and 1 to zero, for output.  Others are inputs.
  LATFbits.LATF0 = 0;    // Turn LED1 on and LED2 off.  These pins sink current
  LATFbits.LATF1 = 1;    // on the NU32, so "high" (1) = "off" and "low" (0) = "on"

  while(1) {
    delay();
    LATFINV = 0x0003;    // toggle LED1 and LED2; same as LATFINV = 0x3;
  }
  return 0;
}

void delay(void) {
  int j;
  for (j = 0; j < 1000000; j++) { // number is 1 million
    while(!PORTDbits.RD7) {
        ;   // Pin D7 is the USER switch, low (FALSE) if pressed.
    }
  }
}
*/ 
