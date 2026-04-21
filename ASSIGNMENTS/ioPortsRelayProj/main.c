/* 
 * Title: ioPortsRelayProj
 * File:   main.c
 * Author: Maxim Reoutt
 * Purpose: To take two digit inputs from touchless photoresistor switches and 
 * if the code is correct turn on a motor. Display digit being entered on a 
 * seven segment display.
 *
 * Inputs: RB1 for PR1_PORT, RB2 for PR2_PORT, RB6 for CHECK_PORT, RB0 for EMG_PORT
 * Outputs: PORTD for DISPLAY_LED, RB3 for SYS_LED_LAT, RB4 for MOTOR_LAT, RB5 for BUZZER_LAT
 * 
 * Versions:
 * V0.1 (4/15/26)[No header files yet, and no functions, just body of main]
 * V0.2 (4/15/26)[Header files and funtions added, no emergency interrupt 
 * function yet]
 * V1.0 (4/15/26)[code with all functions and a working emergency switch now
 * ready for implementation to PIC and testing]
 * V1.1 (4/21/26)[code works to all specifications]
 */



#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "myCConfigHeader.h"
#include "init.h"
#include "functions.h"


//define seven seg digits
const unsigned char seg_num[10] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

//setup pins
void setup_pins(void)
{
    // Make PORTB digital
    ANSELB = 0x00;

    // Make PORTC and PORTD digital too
    ANSELC = 0x00;
    ANSELD = 0x00;

    // Set LED, motor, and buzzer as outputs
    SYS_LED_TRIS = 0;
    MOTOR_TRIS = 0;
    BUZZER_TRIS = 0;

    // Entire PORTC is output for 7-segment display
    SEG_TRIS = 0x00;

    // Set PR1, PR2, and emergency switch as inputs
    PR1_TRIS = 1;
    PR2_TRIS = 1;
    EMG_TRIS = 1;
    CHECK_TRIS = 1;

    // Enable weak pull-ups on the input pins
    //WPUBbits.WPUB1 = 1;   // pull-up for PR1
    //WPUBbits.WPUB2 = 1;   // pull-up for PR2
    //WPUBbits.WPUB0 = 1;   // pull-up for emergency input

    // Initial output states
    SYS_LED_LAT = 1;   // System LED ON when power is on
    MOTOR_LAT = 0;     // Motor OFF at startup
    BUZZER_LAT = 0;    // Buzzer OFF at startup
    SEG_LAT = 0x00;    // 7-seg OFF at startup
}


//display current digit
void display_digit(unsigned char num)
{
    // If digit is valid, show it
    if (num <= 9)
        SEG_LAT = seg_num[num];
    else
        SEG_LAT = 0x00;
}


//clear the display
void clear_display(void)
{
    // Turn off all segments
    SEG_LAT = seg_num[0];
}


//if code is right
void correct_code(void)
{
    // If code is correct, turn on motor
    MOTOR_LAT = 1;
    __delay_ms(3000);
}


//if code is wrong
void wrong_code(void)
{
    // If code is wrong, turn on buzzer for 2 seconds
    BUZZER_LAT = 1;
    __delay_ms(2000);
    BUZZER_LAT = 0;
}


//emergency buzz
void emergency_buzz(void)
{
    BUZZER_LAT = 1;
    __delay_ms(500);
    BUZZER_LAT = 0;
    __delay_ms(500);
    BUZZER_LAT = 1;
    __delay_ms(500);
    BUZZER_LAT = 0;
    __delay_ms(500);
}


//count digit 1 presses
unsigned char count_presses_pr1(void)
{
    // This variable stores how many covers were counted
    unsigned char count = 0;

    // This variable measures how long the sensor has been inactive
    // after at least one press has happened
    unsigned int idle = 0;
    
    while (1)
    {
        

        //check if emergency switch is pulled
        if(EMG_PORT == 1) {
            __delay_ms(80);
            if(EMG_PORT == 1){
                return 255;
            }
        }
        
        if(CHECK_PORT == 1)
        {
            __delay_ms(80);
            
            if(CHECK_PORT == 1)
            {
                while(CHECK_PORT == 1);
                return count;
            }
        }
        
        // PR1 is considered active when it becomes LOW
        // Example: hand covers sensor -> input goes low
        if (PR1_PORT == 0)
        {
            // Small delay for debounce / stability
            __delay_ms(80);

            // Check again after delay
            // If still low, it is a real press/cover
            if (PR1_PORT == 0)
            {
                // Count one cover
                count++;
                display_digit(count);
                

                // Wait until sensor becomes inactive again
                // This prevents one long cover from being counted many times
                while (PR1_PORT == 0);

                // Short delay after release
                __delay_ms(80);

                // Reset idle timer because activity just happened
                idle = 0;
            }
        }
    }
}


//count digit 2 presses
unsigned char count_presses_pr2(void)
{
    // Same idea as PR1, but for the second digit
    unsigned char count = 0;
    unsigned int idle = 0;

    while (1)
    {
        

        //check if emergency switch is pulled
        if(EMG_PORT == 1) {
            __delay_ms(80);
            if(EMG_PORT == 1){
                return 255;
            }
        }
        
        if(CHECK_PORT == 1)
        {
            __delay_ms(80);
            
            if(CHECK_PORT == 1)
            {
                while(CHECK_PORT == 1);
                return count;
            }
        }
        
        // Check whether PR2 has been activated
        if (PR2_PORT == 0)
        {
            // Debounce delay
            __delay_ms(80);

            // Confirm it is still active
            if (PR2_PORT == 0)
            {
                // Count one cover
                count++;
                display_digit(count);

                // Wait until the sensor is released
                while (PR2_PORT == 0);
                

                // Small delay after release
                __delay_ms(80);

                // Reset inactivity timer
                idle = 0;
            }
        }
    }
}



//main
void main(void) {
    
    //first digit entered
    unsigned char first;
    //second digit entered
    unsigned char second;
    
    //set up pins
    setup_pins();
    
    //display default of 0
    
    
    //main loop
    while(1)
    {
        //make sure motor is off
        MOTOR_LAT = 0;
        
        //clear seven seg
        clear_display();
        
        if(EMG_PORT == 1){
            __delay_ms(80);
            if(EMG_PORT == 1){
                emergency_buzz();
                continue;
            }
        }
        
        //take first digit from pr1
        first = count_presses_pr1();
        
        //display digit taken
        display_digit(first);
        
        //check if emergency switch was pulled
        if(first == 255){
            emergency_buzz();
            continue;
        }
        
        //delay
        __delay_ms(1000);
        
        
        //clear seven seg
        clear_display();
        
        
        //take second digit from pr2
        second = count_presses_pr2();
        
        //display digit taken
        display_digit(second);
        
        //check if emergency switch was pulled
        if(second == 255){
            emergency_buzz();
            continue;
        }
        
        //delay
        __delay_ms(1000);
        
        
        //check if digits match code
        if(first == secret1 && second == secret2)
        {
            correct_code();
        }
        else
        {
            wrong_code();
        }
        
        //wait before next attempt
        __delay_ms(1000);
                
    }
    
    return;
}
