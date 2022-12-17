#include "NU32.h"           // constants, funcs for startup and UART
#define CORE_TICKS 40000000 // 40 M ticks (one second)

volatile int state = 0;
volatile int j, t1, t2;
volatile int t3;
float te;




void __ISR(_CORE_TIMER_VECTOR, IPL6SRS) CoreTimerISR(void) {  // step 1: the ISR
  IFS0bits.CTIF = 0;                // clear CT int flag IFS0<0>, same as IFS0CLR=0x0001
  LATFINV = 0x2;                    // invert pin RF1 only
  //_CP0_SET_COUNT(0);                // set core timer counter to 0
  _CP0_SET_COMPARE(CORE_TICKS);     // must set CP0_COMPARE again after interrupt
}


void __ISR(_EXTERNAL_0_VECTOR, IPL2SOFT) Ext0ISR(void) { // step 1: the ISR
  IFS0bits.INT0IF = 0;            // clear interrupt flag IFS0<3>

  // read the core timer
  // while core timer less than 10 ms 
  // read the button again
      char m[100]; // initialize a message string
      j = 0;
      while (j<400000){
        j = j+1;
      }

      //sprintf(m,"state = %d \r\n",state);
      //NU32_WriteUART3(m); // print time elapsed to screen

      if (PORTDbits.RD7 == 0) { // after the delay, if RD7 is still low (pressed) then execute code below
          if (state==0){ // button hasn't been pushed yet
            
            t1 = _CP0_GET_COUNT();
            sprintf(m,"time 1: %d\r\n",t1);
            NU32_WriteUART3(m); // print time elapsed to screen
            // record the core timer for the first time
            state = 1; // to note that the button has been pushed and t1 has been recorded

          }

          else if (state==1) { // button has alread been pressed and t1 has been recorded

            t2 = _CP0_GET_COUNT(); // record core timer upon second button press
            t3 = t2 - t1;
            te = ((t3) / (40000000.0)) ; // find difference
            
            sprintf(m,"seconds elapsed: %f \r\n time 1: %d \r\n time 2:%d \r\n",te,t1,t2);
            NU32_WriteUART3(m); // print time elapsed to screen

            state = 0; // reset 

          }
      
      }

      

  
}






int main(void) {
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init

  __builtin_disable_interrupts();   // step 2: disable interrupts at CPU
  _CP0_SET_COMPARE(CORE_TICKS);     // step 3: CP0_COMPARE register set to 40 M
  IPC0bits.CTIP = 6;                // step 4: interrupt priority
  IPC0bits.CTIS = 0;                // step 4: subp is 0, which is the default
  IFS0bits.CTIF = 0;                // step 5: clear CT interrupt flag
  IEC0bits.CTIE = 1;                // step 6: enable core timer interrupt

  INTCONbits.INT0EP = 0;          // step 3: INT0 triggers on falling edge
  IPC0bits.INT0IP = 2;            // step 4: interrupt priority 2
  IPC0bits.INT0IS = 1;            // step 4: interrupt priority 1
  IFS0bits.INT0IF = 0;            // step 5: clear the int flag
  IEC0bits.INT0IE = 1;            // step 6: enable INT0 by setting IEC0<3>
  __builtin_enable_interrupts();    // step 7: CPU interrupts enabled

  _CP0_SET_COUNT(0);                // set core timer counter to 0

  while(1) { ;
  }
  return 0;

}
