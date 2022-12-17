#include "NU32.h" // config bits, constants, funcs for startup and UART
#include <stdio.h>
#include "encoder.h" // decoder
#include "utilities.h" // mode 
#include "ina219.h" // current sensor library
#include <stdlib.h> // absolute value
#define IDLE 0
#define PWM 1
#define ITEST 2
#define HOLD 3
#define TRACK 4
#define EINTMAX 200
// include other header files here
#define BUF_SIZE 200

volatile int i,duty,length=0;
volatile float u, u_pos, hold_ref, ref = 200.0, e, eint = .0, pe = .0, peint = .0, pedot = .0, peprev = .0, Ki = 0, Kp = 0, Kd = 0,  itest_real[100],itest_ref[100], pref[2000],preal[2000];// position controllers & reference arrays
volatile float Kpp = 0, Kip = 0, Kdp = 0;
int abs_duty;
char buffer[BUF_SIZE];



void __ISR(_TIMER_1_VECTOR, IPL4SOFT) PositionControl(void) {// position control ISR
    static int counter=0;
    switch(get_mode()){

        /*
        case 3:{ // HOLD

            
            WriteUART2("a");
            while(!get_encoder_flag()){} // wait for the new position while the flag is 0
            set_encoder_flag(0); // remember to clear the flag so it works again next time   
            int cur_pos = get_encoder_count(); 
            float ppreal = (cur_pos*360) / (4*334); // actual position

            pe = hold_ref - ppreal; // position error

            peint = peint + pe; // position integral error

            if (peint>EINTMAX){
                peint = EINTMAX;
            }
            if (peint<-EINTMAX){
                peint = -EINTMAX;
            }
            pedot = pe - peprev;
            peprev = pe; // set next eprev
            u_pos = Kpp*pe + Kip*peint + Kdp*pedot;
 
            break;
        }

        */

        case 4: { // TRACK

            
            float ppref = pref[counter]; // reference angle
            WriteUART2("a");
            while(!get_encoder_flag()){} // wait for the new position while the flag is 0
            set_encoder_flag(0); // remember to clear the flag so it works again next time   
            int cur_pos = get_encoder_count(); 
            float ppreal = (cur_pos*360) / (4*334); // actual position

            pe = ppref - ppreal; // position error

            peint = peint + pe; // position integral error

            if (peint>EINTMAX){
                peint = EINTMAX;
            }
            if (peint<-EINTMAX){
                peint = -EINTMAX;
            }
            pedot = pe - peprev;
            peprev = pe; // set next eprev
            u_pos = Kpp*pe + Kip*peint + Kdp*pedot;

        
            
            preal[counter] = ppreal;
            counter++;


            if(counter==length){
                counter = 0;
                peint = .0;
                peprev = .0;
                pedot = .0;
                eint = .0;
                set_mode(IDLE);
            }
            break;
        }

    }

    IFS0bits.T1IF = 0; // clear flag
}


void __ISR(_TIMER_2_VECTOR, IPL5SOFT) CurrentControl(void) { // current control ISR

    static int counter,ref = 200.0;;
    switch(get_mode()){
        case 0 : // IDLE case
            {
                OC1RS = 0; // set duty cycle to 0 (break mode)
                break;
            }

        case 1 : // PWM case
        {
            if(duty<0)
            {
            LATDbits.LATD1 = 0;// if negative duty cycle, RD1 is low, clockwise rotation
            }
            else {
            LATDbits.LATD1 = 1; // if positive duty cycle, RD1 is high, ccw rotation
            }

            abs_duty = abs(duty);

            unsigned int OCR = (abs_duty/100.0)*(PR3+1);
            OC1RS = OCR;  // calculate and set OC1RS to fulfill the desired duty cycle
            break;
        }

        case 2: // ITEST
        {

            
            if(counter==25 || counter == 50 || counter ==75){
                ref = -ref;// change sign
                }
            
            float current = INA219_read_current();
            e = ref - current;
            eint = eint + e; // integral error
            if (eint>EINTMAX){
                eint = EINTMAX;
            }
            if (eint<-EINTMAX){
                eint = -EINTMAX;
            }

            // edot = e - eprev;
            // eprev = e; // set next eprev
            u = Kp*e + Ki*eint;

            if(u>100){
                u = 100;
            }
            if(u<-100){
                u = -100;
            }
            if(u<0)
            {
                LATDbits.LATD1 = 0;// if negative duty cycle, RD1 is low, clockwise rotation
            }
            else {
                LATDbits.LATD1 = 1; // if positive duty cycle, RD1 is high, ccw rotation
            }

            itest_real[counter] = current;
            itest_ref[counter] = ref;

            if(u<0){
                u = -u;
            }
            unsigned int OCR = (u/100.0)*(PR3+1);
            OC1RS = OCR;
            counter++;
            if(counter==99){
                counter = 0;
                eint = .0;
                // eprev = .0;
                set_mode(IDLE); // set mode to IDLE
            }

            break;
        }


        /*
        case 3: { // HOLD

            ref = u_pos;
            e = ref - INA219_read_current(); 
            eint = eint + e; // integral error
            if (eint>EINTMAX){
                eint = EINTMAX;
            }
            if (eint<-EINTMAX){
                eint = -EINTMAX;
            }

            u = Kp*e + Ki*eint;

            if(u>100){
                u = 100;
            }
            if(u<-100){
                u = -100;
            }
            if(u<0)
            {
                LATDbits.LATD1 = 0;// if negative duty cycle, RD1 is low, clockwise rotation
            }
            else {
                LATDbits.LATD1 = 1; // if positive duty cycle, RD1 is high, ccw rotation
            }

    
            if(u<0){
                u = -u;
            }

            unsigned int OCR = (u/100.0)*(PR3+1);
            OC1RS = OCR;
            break;


        }

        */

        case 4: { // TRACK

            ref = u_pos;
            e = ref - INA219_read_current(); 
            eint = eint + e; // integral error
            if (eint>EINTMAX){
                eint = EINTMAX;
            }
            if (eint<-EINTMAX){
                eint = -EINTMAX;
            }

            u = Kp*e + Ki*eint;

            if(u>100){
                u = 100;
            }
            if(u<-100){
                u = -100;
            }
            if(u<0)
            {
                LATDbits.LATD1 = 0;// if negative duty cycle, RD1 is low, clockwise rotation
            }
            else {
                LATDbits.LATD1 = 1; // if positive duty cycle, RD1 is high, ccw rotation
            }

    
            if(u<0){
                u = -u;
            }

            unsigned int OCR = (u/100.0)*(PR3+1);
            OC1RS = OCR;
            break;

        }


    }
    // OC1RS = 1000; // set duty cycle to 25% : duty cycle = OC1RS / (PR3+1) * 100%
    // LATDINV = 0b10;  // invert RD1
    
    IFS0bits.T2IF = 0;            // clear the int flag
}





int main() {
    
    NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
    UART2_Startup(); // initialize UART 
    INA219_Startup(); // call ina219
    set_mode(IDLE); // initialize mode to IDLE


    OC1CONbits.OCTSEL = 1; // Select Timer 3 as timer for OC1
    TRISDbits.TRISD1 = 0; // make RD1 a digital output
    LATDbits.LATD1 = 1; // intialize D1 output to be high, which leads to CCW (positive) rotation of the motor

    __builtin_disable_interrupts();   // step 2: disable interrupts at CPU


    PR1 = 6249; // 80*10^6 / 400 = (PR1+1)*N if N = 64
    TMR1 = 0; // initialize
    T1CONbits.TCKPS = 0b10; //prescaler of 64
    T1CONbits.ON = 1; // timer 1 on

    IPC1bits.T1IP = 4; // priority 4
    IPC1bits.T1IS = 0; // sub 0
    IFS0bits.T1IF = 0; // clear flag
    IEC0bits.T1IE = 1; // enable


    PR2 = 249; // 80*10^6 / 5000 = (PR2+1)*N if N = 64
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
    OC1RS = 1000; // duty cycle = OC1RS /(PR3+1) = 75%
    OC1R = 3000; // intialize
    T3CONbits.ON = 1; // turn on Timer 3
    OC1CONbits.ON = 1; // turn on OC1


    NU32_LED1 = 1; // turn off the LEDs
    NU32_LED2 = 1;
    
    while(1)    {
        NU32_ReadUART3(buffer,BUF_SIZE); // we expect the next character to be a menu command
        NU32_LED2 = 1; // clear the error LED
        switch (buffer[0]) { // what does switch do ?

    

        case 'b': // read current
            {
                sprintf(buffer,"%f \n", INA219_read_current());
                NU32_WriteUART3(buffer);
                break;
            }
        

        case 'c': // get position in encoder counts
            {     
                
                WriteUART2("a");
                while(!get_encoder_flag()){} // wait for the new position while the flag is 0
                set_encoder_flag(0); // remember to clear the flag so it works again next time       
                sprintf(buffer,"%d \n", get_encoder_count()); // get encoder count
                NU32_WriteUART3(buffer); // send encoder count to client
                // NU32_WriteUART3("\r\n");
                        
                break;
            }


        case 'd': // get position in degrees
        {
            WriteUART2("a");
            while(!get_encoder_flag()){} // wait for the new position while the flag is 0
            set_encoder_flag(0); // remember to clear the flag so it works again next time      
            int count_pos = get_encoder_count(); 
            float deg = (count_pos*360) / (4*334);
            sprintf(buffer,"%f \n",deg);
            NU32_WriteUART3(buffer);
            break;
        }



        case 'e': // reset position

            {
                WriteUART2("b"); // reset encoder
                //NU32_WriteUART3("Position Reset.\r\n");
                
                break;
            }

        case 'f' : // set pwm duty cycle
            {
                set_mode(PWM);
                // NU32_WriteUART3("Set PWM -100 to 100 percent\r\n");
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%d\r\n", &duty);
                sprintf(buffer,"%d \n", duty);
                // sprintf(buffer,"Duty cycle set to %d perecent. Mode is %d\r\n", duty, get_mode());
                NU32_WriteUART3(buffer);
               
                break;
            }
        
        case 'g': // set current gains
        {
            // NU32_WriteUART3("Enter Kp\r\n");
            NU32_ReadUART3(buffer,BUF_SIZE);
            sscanf(buffer,"%f", &Kp);
            // NU32_WriteUART3("Enter Ki\r\n");
            NU32_ReadUART3(buffer,BUF_SIZE);
            sscanf(buffer,"%f", &Ki);
            
            // sprintf(buffer,"%f %f\n",Kp,Ki);
           

            //sprintf(buffer,"%f\n",Ki);
            //NU32_WriteUART3(buffer);

            break;

        }

        case 'h': // get gains
        {
            //sprintf(buffer,"%f %f \n",Kp,Ki);
            //NU32_WriteUART3(buffer);
            break;

        }

        case 'i': { // Set Position Gains
            NU32_ReadUART3(buffer,BUF_SIZE);
            sscanf(buffer,"%f",&Kpp);

            NU32_ReadUART3(buffer,BUF_SIZE);
            sscanf(buffer,"%f",&Kip);

            NU32_ReadUART3(buffer,BUF_SIZE);
            sscanf(buffer,"%f",&Kdp);
            break;

        }

        case 'j': { // Get Position Gains
            // sprintf(buffer,"%f %f %f\n",Kpp,Kip,Kdp);
            // NU32_WriteUART3(buffer);
            break;

        }



        case 'k': // ITEST (test current gains)
        {
            
            set_mode(ITEST);
            int i = 0;
            while(get_mode() == ITEST) {
                // delay
            }
            NU32_WriteUART3("100\n"); // send number of data points
            

            for(i=0; i<100; i++){
                sprintf(buffer,"%f %f\n",itest_ref[i],itest_real[i]);
                NU32_WriteUART3(buffer); // send reference and real data points
            }

            break;
        }

        case 'l': // Go to angle
        { 
            
            int angle;
            NU32_ReadUART3(buffer,BUF_SIZE);
            sscanf(buffer,"%f",&angle);

            break;
        }

        case 'm': // Load Step Trajectory
        { 
        


            break;
        }

        case 'n': // Load Cubic Trajectory
        {

            int i,degree;
            NU32_ReadUART3(buffer,BUF_SIZE);// read number of data points
            sscanf(buffer,"%d\n",&length);

            for(i=0;i<length;i++){ // store all angles
                NU32_ReadUART3(buffer,BUF_SIZE);
                sscanf(buffer,"%d",&degree);
                pref[i] = degree;
            }

        



            break;
        }
      

        case 'o': // Execute
        {
            set_mode(TRACK);

            while(get_mode()==TRACK){
                // wait while tracking
            }

            sprintf(buffer,"%d\n",length); //
            NU32_WriteUART3(buffer); // send number of data points
            
            for(i=0; i<length; i++){
                
                sprintf(buffer,"%f %f\n",pref[i],preal[i]);
                NU32_WriteUART3(buffer); // send reference and real data points
            }
            hold_ref = pref[length-1];
            //set_mode(HOLD);
            

            break;
        }


        
        case 'p': // set mode to IDLE
        {
            set_mode(IDLE); 
            // NU32_WriteUART3("Motor powered off\r\n");
            break; 
        }

        case 'r': // get mode
        {
            sprintf(buffer,"%d \n",get_mode());
            NU32_WriteUART3(buffer);

            break;
        }
                
                
            
        case 's' : // dummy command for demonstration purposes
            {
                int n = 0;
                NU32_ReadUART3(buffer,BUF_SIZE);
                sscanf(buffer, "%d", &n);
                sprintf(buffer,"%d\r\n", n + 1); // return the number + 1
                NU32_WriteUART3(buffer);
                break;
            }

        case 'q':
            {
                set_mode(IDLE);
            // handle q for quit. Later you may want to return to IDLE mode here.
                break;
            }

        case 'x': // tester case
        {
           int num1, num2;

           NU32_ReadUART3(buffer,BUF_SIZE);
           sscanf(buffer, "%d %d", &num1,&num2);
           sprintf(buffer,"%d\r\n", num1+num2); // return sum
           NU32_WriteUART3(buffer);
            break ;
        }

    default:
            {
        NU32_LED2 = 0; // turn on LED2 to indicate an error
        break;
            }
        }
    }
    return 0;
}