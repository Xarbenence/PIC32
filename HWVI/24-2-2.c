
#include "NU32.h"
#define NUMSAMPS 1000 // number of points in waveform
static volatile int Waveform[NUMSAMPS]; // waveform
void makeWaveform();


void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) { // _TIMER_2_VECTOR = 8
    static int counter = 0; // initialize counter once
    // insert line(s) to set OC1RS
    OC1RS = Waveform[counter];

    counter++; // add one to counter every time ISR is entered
    if (counter == NUMSAMPS) {
    counter = 0; // roll the counter over when needed
    }
    IFS0bits.T2IF = 0 ;// insert line to clear interrupt flag
}


int main(void) {
    NU32_Startup();
    makeWaveform();
    OC1CONbits.OCTSEL = 1; // Select Timer 3 as timer for OC1

    __builtin_disable_interrupts();   // step 2: disable interrupts at CPU

    PR2 = 1249; // 
    TMR2 = 0; // intialize 
    T2CONbits.TCKPS = 0b110 ; // prescaler of 64
    // T2CONbits.TGATE = 0; // not gated
    // T2CONbits.TCS = 0; // PBCLK input
    T2CONbits.ON = 1; // timer 2 on

    IPC2bits.T2IP = 5;            // step 3: interrupt priority 5
    IPC2bits.T2IS = 0;            // step 4: interrupt sub priority 0
    IFS0bits.T2IF = 0;            // step 5: clear the int flag
    IEC0bits.T2IE = 1;            // step 6: enable 
    __builtin_enable_interrupts();    // step 7: CPU interrupts enabled

    
    // T3CONbits.TCKPS = 1 ; // Timer 3 prescaler N = 1
    PR3 = 3999;  // period = (PR3 +1) * N * 12.5 ns = 50 us
    TMR3 = 0 ; // initial TMR3 count is 0
    OC1CONbits.OCM = 0b110; // PWM mode without fault pin
    OC1RS = 3000; // duty cycle = OC1RS /(PR3+1) = 75%
    OC1R = 3000; // intialize
    T3CONbits.ON = 1; // turn on Timer 3
    OC1CONbits.ON = 1; // turn on OC1


    while(1) {
        ;
    }

    //IFS0bits.T3IF IEC0bits.T3IE IPC3bits.T3IP IPC3bits.T3IS
}






void makeWaveform() {
    int i = 0, center = 2000, A = 2000; // square wave, center = PR3 + 1/ 2, A = amplitude
    for (i = 0; i < NUMSAMPS; ++i) {
    if ( i < NUMSAMPS/2) {
    Waveform[i] = center + A;
        } 
    else {
    Waveform[i] = center - A;
        }
    }
}
