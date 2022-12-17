// Make F1 an output and turn it on

TRISFbits.TRISF1 = 0; // happens slower in real time, but helps us 

// will see this format 99% of the time 

LATFbits.LAT1 = 1; 

// Want to invert F1

LATFINV = 0b10;

// Turn on F1

LATFSET = 0b10;

// Turn off F1

LATFCLR = 0b10;


// Could also do

LATF = LATF&(ob1<<2); // == 0b100 make all values 0, except for shifted value? 





