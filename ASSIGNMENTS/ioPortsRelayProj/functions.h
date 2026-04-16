//functions header file for ioPortsRelayProj V1.0
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Display one digit on the 7-segment display
void display_digit(unsigned char num);

// Turn off the 7-segment display
void clear_display(void);

// Count how many times PR1 is covered
unsigned char count_presses_pr1(void);

// Count how many times PR2 is covered
unsigned char count_presses_pr2(void);

// What happens if code is correct
void correct_code(void);

// What happens if code is wrong
void wrong_code(void);

#endif
