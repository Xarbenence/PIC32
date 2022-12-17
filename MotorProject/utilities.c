#include "NU32.h"
#include "utilities.h"
#include <stdio.h>
int mode;
char m[100];

int get_mode() {
    return mode;
    
}

const char* set_mode(int i) {
    
        if(i == 0){
            mode = 0;
            return "Mode set to IDLE\r\n"; // IDLE
        }
        else if(i==1){
            mode = 1;
            return "Mode set to PWM\r\n"; // PWM
        }
        else if(i==2){
            mode = 2;
            return "Mode set to ITEST\r\n"; // ITEST
        }
        else if(i==3){
            mode = 3;
            return "Mode set to HOLD\r\n"; // HOLD
        }
        else if(i==4){
            mode = 4;
            return "Mode set to TRACK\r\n"; // TRACK
        }

}