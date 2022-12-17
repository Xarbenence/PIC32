










void blink() {
    LATFbits.LATF0 = 0; // led on

    // delay

    int j;
    for(j=0; J<1000000; j =  j+1) {
        // nothing

    }

    sprintf(message, "%f\r\n")

    // _CP0_SET_COUNT(0); // this is a blocking function. PIC can't do anything else until this function is fulfilled

    unsigned int t = _CP0_GET_COUNT(); // have to worry about roll over. largest delay you could have is largest unsigned int 

    while(_CP0_GET_COUNT()-t < 40000000){ // this takes about ~80 mil clock cycles ~= 1 sec
        // 40000000  counts is 1 sec, so 40 mil / 10 = 1/10 of a sec 
        // nothing

        // can put stuff in here so that the function doesn't have to do nothing (i.e. checking buttons)
        // or use interrupts
    }

    LATFbits.LATF0 = 1; // led off



}