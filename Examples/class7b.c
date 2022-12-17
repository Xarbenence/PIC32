#include "NU32.h"          // constants, funcs for startup and UART

#define MAX_MESSAGE_LENGTH 200

int main(void) {
  
  
  NU32_Startup(); // cache on, interrupts on, LED/button init, UART init

  //d7

  TRISD = 0b11111100; // not a good way
  TRISDSET = 0b10000000; // D7 an input
  TRISDbits.TRISD7 = 1;


  TRISFCLR = 0b1; // F0 an output
  TRISFbits.TRISF0 = 0;
  LATFbits.LATF0 = 1; // led0 off
  LATFSET = 0b1;


  int state = 0;
  int old_button_value = 1;




  while (1); {

    if (PORTDbits.RD7 == 0 && old_button_value == 1) {
      state = 1;

    }

    else{
      state = 0;

    }



    if(state == 0) {
      LATFbits.LATF0 = 0;

    }

    else {
      LATFbits.LATF0 = 1;
    }

    old_button_value = PORTDbits.RD7;

  }

}
