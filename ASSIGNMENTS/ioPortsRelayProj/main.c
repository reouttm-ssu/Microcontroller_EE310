/* 
 * Title: ioPortsRelayProj
 * File:   main.c
 * Author: Maxim Reoutt
 * Purpose: To take two digit inputs from touchless photoresistor switches and 
 * if the code is correct turn on a motor. Display digit being entered on a 
 * seven segment display.
 * 
 * 
 * Versions:
 * V0.1 (4/15/26)[No header files yet, and no functions, just body of main]
 * 
 */


#include <xc.h>
#include "myCConfigHeader.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define _XTAL_FREQ 4000000                 // Fosc  frequency for _delay()  library
#define FCY    _XTAL_FREQ/4

void main(void) {
    
    //first digit entered
    unsigned char first;
    //second digit entered
    unsigned char second;
    
    //set up pins
    setup_pins();
    
    //main loop
    while(1)
    {
        //make sure motor is off
        MOTOR_LAT = 0;
        
        //clear seven seg
        clear_display();
        
        //take first digit from pr1
        first = count_presses_pr1();
        
        //display digit taken
        display_digit(first);
        
        //delay
        __delay_ms(1000);
        
        
        //clear seven seg
        clear_display();
        
        
        //take second digit from pr2
        second = count_presses_pr2();
        
        //display digit taken
        display_digit(second);
        
        //delay
        __delay_ms(1000);
        
        
        //check if digits match code
        if(first == seceret1 && second == seceret2)
        {
            correct_code();
        }
        else()
        {
            wrong_code();
        }
        
        //wait before next attempt
        __delay_ms(1000);
                
    }
    
    return;
}
