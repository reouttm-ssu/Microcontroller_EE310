/*
 * init file for ioPortsRelayProj V1.0
 */
#ifndef INIT_H
#define INIT_H

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// This tells __delay_ms() what clock speed to use
#define _XTAL_FREQ 4000000

// Secret code:
// First number is entered with PR1
// Second number is entered with PR2
// Example here: cover PR1 three times, then cover PR2 two times
#define secret1 3
#define secret2 2

// ===================== OUTPUT PINS =====================
// System LED: turns on when system has power
#define SYS_LED_LAT   LATDbits.LATD0
// Motor output: turns on if code is correct
#define MOTOR_LAT     LATDbits.LATD1
// Buzzer output: turns on if code is wrong or emergency happens
#define BUZZER_LAT    LATDbits.LATD2

// TRIS bits for outputs
#define SYS_LED_TRIS  TRISDbits.TRISD0
#define MOTOR_TRIS    TRISDbits.TRISD1
#define BUZZER_TRIS   TRISDbits.TRISD2

// ===================== INPUT PINS =====================
// PR1 and PR2 are the touchless switch inputs
// These are read as DIGITAL inputs
#define PR1_PORT      PORTBbits.RB1
#define PR2_PORT      PORTBbits.RB2
#define EMG_PORT      PORTBbits.RB0

// TRIS bits for input pins
#define PR1_TRIS      TRISBbits.TRISB1
#define PR2_TRIS      TRISBbits.TRISB2

// Emergency switch input
#define EMG_TRIS      TRISBbits.TRISB0

// ===================== 7-SEGMENT DISPLAY =====================
// Entire 7-segment display connected to PORTC
#define SEG_LAT       LATC
#define SEG_TRIS      TRISC

// Function prototypes
void setup_pins(void);
//void setup_interrupt(void);

#endif
