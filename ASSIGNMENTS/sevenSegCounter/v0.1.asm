    //-----------------------------------------------------
    // Titile: Seven Segment Display Counter
    //-----------------------------------------------------
    // Purpose: This code is meant to increment a counter up or down im a range
    // of 0 to 15 in hexadecimal depending on which of two buttons are pressed,
    // resetting if both are pressed.
    // Compiler: pic.as v3.10
    // Author: Maxim Reoutt
    // Outputs: PORTD
    // Inputs: PORTB
    // Versions:
    //	    v0.1 4/1/26 (pseudocode)
    
    
    // initialize display values in registers 40 to 50, setting up the PORTD
    // outputs for each digit. also sets up delay times and ports D and B
_INIT:
    
    
    // check each switch, calling the appropriate functions to increment or 
    // decrement the 0 pointer, make sure the pointer is in range, and then 
    // update PORTD with the digit to display. Then, call the delay function
_MAIN:
    
    
    // increment the pointer
_INCREMENT:
    
    
    // decrement the pointer
_DECREMENT:


    // sets pointer at the start of the range
_START:
    
    
    // sets pointer at the end of the range
_END:
    
    
    // delays the code for a time of about 0.2-1.0 seconds
_DELAY:
