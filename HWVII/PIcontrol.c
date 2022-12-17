
#include "NU32.h"
#include <stdio.h>

#define NUMSAMPS 1000 // number of points in waveform
#define PLOTPTS 200 // number of data points to plot
#define DECIMATION 10 // plot every 10th point
static volatile int Waveform[NUMSAMPS]; // waveform
static volatile int ADCarray[PLOTPTS]; // measured values to plot
static volatile int REFarray[PLOTPTS]; // reference values to plot
static volatile int StoringData = 0; // if this flag = 1, currently storing 
// plot data
static volatile float Kp = 0, Ki = 0; // control gains
static volatile int eprev = 0; // initial "previous error" is zero
static volatile int eint = 0; // initial error integral is zero



void makeWaveform();
unsigned int adc_sample_convert(int pin);

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void) { // _TIMER_2_VECTOR = 8
    static int counter = 0; // initialize counter once
    static int plotind = 0; // index for data array: counts up to PLOTPTS
    static int decctr = 0; // counts to store datat once every DECIMATION
    static int adcval = 0; // ADC reading
    int unew = 0; // saturated u 
    int u = 0; // calculated control signal
    int r = 0; // reference signal
    int e = 0; // error

   
    adcval = adc_sample_convert(15); // our sensor voltage reading

    r = Waveform[counter]; // waveform is reference signal
    e = r - adcval; // error is reference minus output
    eint =  eint + e; // integral error
    u = Kp*e + Ki*eint; // controller output

   


    unew = u + 50.0; // saturate u at 0% and 100%
    if (unew > 100.0) {
        unew = 100.0;
    } 
    else if (unew < 0.0) {
        unew = 0.0;
    }

    OC1RS = (unsigned int) ((unew/100.0) * PR3);

    

    if (StoringData) {
        decctr++;
        if (decctr == DECIMATION) { // after DECIMATION control loops,
            decctr = 0; // reset decimation counter
            ADCarray[plotind] = adcval; // store data in global arrays
            REFarray[plotind] = Waveform[counter];
            plotind++; // increment plot data index
        }
        if (plotind == PLOTPTS) { // if max number of plot points plot is reached,
            plotind = 0; // reset the plot index
            StoringData = 0; // tell main data is ready to be sent to MATLAB
        }
        
        }

    counter++; // add one to counter every time ISR is entered
    if (counter == NUMSAMPS) {
    counter = 0; // roll the counter over when needed
    }
    IFS0bits.T2IF = 0 ;//  clear interrupt flag
}


int main(void) {
    NU32_Startup();
    makeWaveform();
    char message[100]; // message to and from python
    float kptemp = 0, kitemp = 0; // temporary local gains
    int i = 0; // plot data loop counter
    AD1PCFGbits.PCFG15 = 0;                 // AN15 is an adc pin
    AD1CON3bits.ADCS = 2;                   // ADC clock period is Tad = 2*(ADCS+1)*Tpb =
                                            //                           2*3*12.5ns = 75ns
    AD1CON1bits.ADON = 1;                   // turn on A/D converter

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


    while (1) {
        
        NU32_ReadUART3(message, sizeof(message)); // wait for a message from python
        sscanf(message, "%f %f" , &kptemp, &kitemp);
        __builtin_disable_interrupts(); // keep ISR disabled as briefly as possible
        Kp = kptemp; // copy local variables to globals used by ISR
        Ki = kitemp;
        eint = 0;
        __builtin_enable_interrupts(); // only 2 simple C commands while ISRs disabled
        StoringData = 1; // message to ISR to start storing data

        while (StoringData) { // wait until ISR says data storing is done
            ; // do nothing
            }   

        for (i=0; i<PLOTPTS; i++) { // send plot data to python
        // when first number sent = 1, python knows we’re done
            sprintf(message, "%d %d %d\r\n", PLOTPTS-i, ADCarray[i], REFarray[i]);
            NU32_WriteUART3(message);
            }
        }

}


    
/*
    int s[100]; // array for values
    int j = 0; // index
    char m[100];

    while(1) {

        for (j=0; j<100;j++) { //
            s[j] = adc_sample_convert(15);    // sample and convert pin 15 and store in the s[i] position
            _CP0_SET_COUNT(0);
            while(CP0_GET_COUNT()<40000000/100){ // 100 hz
                ; // 1 second to get 100 data points
            }
            for(j=0;j<100;j++){
                sprintf(m,"%d\r\n",s[j]);
                NU32_WriteUART3(m);
            }

        }
        
    }

    //IFS0bits.T3IF IEC0bits.T3IE IPC3bits.T3IP IPC3bits.T3IS
}
*/



unsigned int adc_sample_convert(int pin) { // sample & convert the value on the given 
                                           // adc pin the pin should be configured as an 
                                           // analog input in AD1PCFG
    unsigned int elapsed = 0, finish_time = 0;
    AD1CHSbits.CH0SA = pin;                // connect chosen pin to MUXA for sampling
    AD1CON1bits.SAMP = 1;                  // start sampling
    elapsed = _CP0_GET_COUNT();
    finish_time = elapsed + 25;
    while (_CP0_GET_COUNT() < finish_time) { 
      ;                                   // sample for more than 250 ns
    }
    AD1CON1bits.SAMP = 0;                 // stop sampling and start converting
    while (!AD1CON1bits.DONE) {
      ;                                   // wait for the conversion process to finish
    }
    return ADC1BUF0;                      // read the buffer with the result
}



void makeWaveform() {
    int i = 0, center = 500, A = 300; // square wave, center = PR3 + 1/ 2, A = amplitude
    for (i = 0; i < NUMSAMPS; ++i) {
    if ( i < NUMSAMPS/2) {
    Waveform[i] = center + A;
        } 
    else {
    Waveform[i] = center - A;
        }
    }
}

