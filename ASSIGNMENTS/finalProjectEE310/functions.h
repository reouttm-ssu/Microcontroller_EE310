/* 
 * File:   functions.h
 * Author: Maxim Reoutt & John Etheridge
 *
 * Created on May 15, 2026, 5:55 PM
 */

#define _XTAL_FREQ 4000000UL   // must match oscillator setting

//libraries
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//definitions of variables
#define LCD_RS LATDbits.LATD0
#define LCD_E  LATDbits.LATD1
#define LCD_D4 LATDbits.LATD4
#define LCD_D5 LATDbits.LATD5
#define LCD_D6 LATDbits.LATD6
#define LCD_D7 LATDbits.LATD7

#ifndef PWM_H
#define	PWM_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* PWM_H */

//definitions of functions
void OSC_Init(void)
{
    OSCCON1 = 0x60; //HIFINTOSC selected
    OSCFRQ = 0x02;   // set internal oscillator to 4 MHz
}

void GPIO_Init(void)
{
    ANSELD = 0x00;   // digital mode
    TRISD = 0x00;    // outputs
    LATD  = 0x00;    // clear pins
    
    ANSELBbits.ANSELB5 = 0;
    TRISBbits.TRISB5 = 1;
    PORTBbits.RB5 = 0;
    
    ANSELBbits.ANSELB2 = 0;   // RB2 digital
    TRISBbits.TRISB2 = 0;     // RB2 output
    LATBbits.LATB2 = 0;
    
}

// generate enable pulse
void LCD_PulseEnable(void)
{
    LCD_E = 1;
    __delay_us(10);
    LCD_E = 0;
    __delay_us(100);
}

// send 4 bits to LCD
void LCD_SendNibble(uint8_t nibble)
{
    LCD_D4 = (nibble >> 0) & 1;
    LCD_D5 = (nibble >> 1) & 1;
    LCD_D6 = (nibble >> 2) & 1;
    LCD_D7 = (nibble >> 3) & 1;

    LCD_PulseEnable();
}

// send command (RS = 0)
void LCD_Command(uint8_t cmd)
{
    LCD_RS = 0;

    LCD_SendNibble(cmd >> 4);
    LCD_SendNibble(cmd & 0x0F);

    if (cmd == 0x01 || cmd == 0x02)
        __delay_ms(2);   // longer delay for clear/home
    else
        __delay_us(50);
}

// send data (RS = 1)
void LCD_Data(uint8_t data)
{
    LCD_RS = 1;

    LCD_SendNibble(data >> 4);
    LCD_SendNibble(data & 0x0F);
    
    __delay_us(50);
}

// LCD initialization sequence (4-bit mode)
void LCD_Init(void)
{
    __delay_ms(50);

    LCD_RS = 0;
    LCD_E  = 0;

    LCD_SendNibble(0x03);
    __delay_ms(5);

    LCD_SendNibble(0x03);
    __delay_ms(150);

    LCD_SendNibble(0x03);
    __delay_ms(150);

    LCD_SendNibble(0x02);
    __delay_ms(150);

    LCD_Command(0x28);

    LCD_Command(0x08);

    LCD_Command(0x01);

    LCD_Command(0x06);

    LCD_Command(0x0C);
}

// position cursor
void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t address;

    if (row == 1)
        address = col;
    else
        address = 0x40 + col;

    LCD_Command(0x80 | address);
}

// print string
void LCD_Print(const char *text)
{
    while (*text)
        LCD_Data(*text++);
}

//clear lcd
void LCD_Clear(void)
{
    LCD_Command(0x01);   // clear display command
    __delay_ms(2);       // required delay
}

void ADC_Init(void)
{
    //------Setup the ADC
    //DO: using ADCON0 set right justify
    ADCON0bits.FM = 1;
    //DO: Using ADCON0 set ADCRC Clock
    ADCON0bits.CS = 1;

    //DO: Set RA0 to input
    TRISAbits.TRISA0 = 1;
    //DO: Set RA0 to analog
    ANSELAbits.ANSELA0 = 1;

    //DO: Set RA0 as Analog channel in ADC ADPCH
    ADPCHbits.PCH = 0b000000;
    //DO: set ADC CLOCK Selection register to zero
    ADCLK = 0b000000;

    //DO: Clear ADC Result registers
    ADRESH = 0;
    ADRESL = 0;

    //DO: set precharge select to 0 in register ADPERL & ADPERH
    ADPREL = 0;
    ADPREH = 0;

    //DO: Set acquisition LOW and HIGH bytes to zero
    ADACQL = 0;
    ADACQH = 0;

    //DO: Turn ADC On on register ADCON0   
    ADCON0bits.ON = 1;
}

void TMR2_Initialize(void)
{
    // Set TMR2 to the options selected in the User Interface

    // T2CS FOSC/4; 
    T2CLKCON = 0x01;

    // T2PSYNC Not Synchronized; T2MODE Software control; T2CKPOL Rising Edge; T2CKSYNC Not Synchronized; 
    T2HLT = 0x00;

    // T2RSEL T2CKIPPS pin; 
    T2RST = 0x00;

    // PR2 255; 
    T2PR = 0x9B;

    // TMR2 8; 
    T2TMR = 0x00;

    // Clearing IF flag.
    PIR4bits.TMR2IF = 0;

    // T2CKPS 1:1; T2OUTPS 1:1; TMR2ON on; 
    T2CONbits.CKPS = 0b111;   // 1:128 prescaler
    T2CONbits.ON = 1;
}

void TMR2_Start(void)
{
    // Start the Timer by writing to TMRxON bit
    T2CONbits.TMR2ON = 1;
}

void TMR2_StartTimer(void)
{
    TMR2_Start();
}

void TMR2_Stop(void)
{
    // Stop the Timer by writing to TMRxON bit
    T2CONbits.TMR2ON = 0;
}

void TMR2_StopTimer(void)
{
    TMR2_Stop();
}

uint8_t TMR2_Counter8BitGet(void)
{
    uint8_t readVal;

    readVal = TMR2;

    return readVal;
}

uint8_t TMR2_ReadTimer(void)
{
    return TMR2_Counter8BitGet();
}

void TMR2_Counter8BitSet(uint8_t timerVal)
{
    // Write to the Timer2 register
    TMR2 = timerVal;
}

void TMR2_WriteTimer(uint8_t timerVal)
{
    TMR2_Counter8BitSet(timerVal);
}

void TMR2_Period8BitSet(uint8_t periodVal)
{
   PR2 = periodVal;
}

void TMR2_LoadPeriodRegister(uint8_t periodVal)
{
   TMR2_Period8BitSet(periodVal);
}

void PWM_Output_D8_Enable (void){
    PPSLOCK = 0x55; 
    PPSLOCK = 0xAA; 
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS

    // Set D8 as the output of CCP2
    RB2PPS = 0x0A;

    PPSLOCK = 0x55; 
    PPSLOCK = 0xAA; 
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS
}

void PWM_Output_D8_Disable (void){
    PPSLOCK = 0x55; 
    PPSLOCK = 0xAA; 
    PPSLOCKbits.PPSLOCKED = 0x00; // unlock PPS

    // Set D8 as GPIO pin
    RB3PPS = 0x00;

    PPSLOCK = 0x55; 
    PPSLOCK = 0xAA; 
    PPSLOCKbits.PPSLOCKED = 0x01; // lock PPS
    
    TRISBbits.TRISB3 = 0;
}

void PWM2_Initialize(void)
{
    // Set the PWM2 to the options selected in the User Interface
	
	// MODE PWM; EN enabled; FMT left_aligned; 
	CCP2CON = 0x8C;  // enable / Right-aligned format  
	
	// RH 127; 
	CCPR2H = 0x0;    
	
	// RL 192; 
	CCPR2L = 0x0f;    

	// Selecting Timer 2
	CCPTMRS0bits.C2TSEL = 0x1;
    
}
void PWM2_LoadDutyValue(uint16_t dutyValue)
{
    dutyValue &= 0x03FF;

    // Load duty cycle value /Right-aligned format by default FMT=0
    if(CCP2CONbits.FMT)
    {
        dutyValue <<= 6;
        CCPR2H = dutyValue >> 8;
        CCPR2L = dutyValue;
    }
    else
    {
        CCPR2H = dutyValue >> 8;
        CCPR2L = dutyValue;
    }
}

 _Bool PWM2_OutputStatusGet(void)
{
    // Returns the output status
    return(CCP2CONbits.OUT);
}
