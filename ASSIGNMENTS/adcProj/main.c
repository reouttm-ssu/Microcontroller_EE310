/*
 * Purpose:
 *   This program initializes a PIC microcontroller to read an analog sound
 *   sensor input using the ADC, convert the digital ADC result into a voltage,
 *   and display the sound level category and voltage on a 16x2 LCD.
 *
 * Inputs:
 *   - Analog signal on RA0: Sound sensor voltage input
 *   - Vref: 3.3 V reference voltage used for ADC conversion
 *
 * Outputs:
 *   - LCD display on PORTD showing:
 *       1. Sound level category: Quiet, Normal, Loud, or Obnoxious
 *       2. Measured voltage value from the ADC
 *
 * Date:
 *   May 5, 2026
 *
 * Author:
 *   Maxim
 * 
 * Version:
 * V1 (too little delay between checks)
 * V2 (too wide a voltage range)
 * V3 (working version)
 * V4 (actual working version, more accurate sampling)
 */
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ---- CONFIG BITS (unchanged) ----
#pragma config FEXTOSC = OFF
#pragma config RSTOSC = HFINTOSC_1MHZ
#pragma config CLKOUTEN = OFF
#pragma config PR1WAY = ON
#pragma config CSWEN = ON
#pragma config FCMEN = ON
#pragma config MCLRE = EXTMCLR
#pragma config PWRTS = PWRT_OFF
#pragma config MVECEN = ON
#pragma config IVT1WAY = ON
#pragma config LPBOREN = OFF
#pragma config BOREN = SBORDIS
#pragma config BORV = VBOR_2P45
#pragma config ZCD = OFF
#pragma config PPS1WAY = ON
#pragma config STVREN = ON
#pragma config DEBUG = OFF
#pragma config XINST = OFF
#pragma config WDTCPS = WDTCPS_31
#pragma config WDTE = OFF
#pragma config WDTCWS = WDTCWS_7
#pragma config WDTCCS = SC
#pragma config BBSIZE = BBSIZE_512
#pragma config BBEN = OFF
#pragma config SAFEN = OFF
#pragma config WRTAPP = OFF
#pragma config WRTB = OFF
#pragma config WRTC = OFF
#pragma config WRTD = OFF
#pragma config WRTSAF = OFF
#pragma config LVP = ON
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL   // must match oscillator setting

// ---- LCD pin mapping (PORTD) ----
#define LCD_RS LATDbits.LATD0
#define LCD_E  LATDbits.LATD1
#define LCD_D4 LATDbits.LATD4
#define LCD_D5 LATDbits.LATD5
#define LCD_D6 LATDbits.LATD6
#define LCD_D7 LATDbits.LATD7
#define Vref 5 // voltage reference 
int digital; // holds the digital value 
float voltage1; // hold the analog value (volt))
float voltage2;
float voltage3;
float voltage4;
float voltage5;
float vfinal;
char data[10];



void ADC_Init(void);

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

void main(void)
{
    OSC_Init();     // ensure 4 MHz clock
    GPIO_Init();
    ADC_Init();
    
    
    __delay_ms(200);
    
    LCD_Init();
    
    __delay_ms(200);

    LCD_SetCursor(1, 0);
    LCD_Print("Hello,");

    LCD_SetCursor(2, 0);
    LCD_Print("Maxim!");
    
    __delay_ms(5000);

    while (1)
    {
        ADCON0bits.GO = 1;
        
        while (ADCON0bits.GO); //Wait for conversion done
        
        digital = (ADRESH*256) | (ADRESL);/*Combine 8-bit LSB and 2-bit MSB*/
        // DO: define voltage = Vref/4096 (note that voltage is float type
        voltage1 = digital*((float)Vref/(float)4096);
        
        ADCON0bits.GO = 1;
        
        while (ADCON0bits.GO); //Wait for conversion done
        
        digital = (ADRESH*256) | (ADRESL);/*Combine 8-bit LSB and 2-bit MSB*/
        // DO: define voltage = Vref/4096 (note that voltage is float type
        voltage2 = digital*((float)Vref/(float)4096);
        
        ADCON0bits.GO = 1;
        
        while (ADCON0bits.GO); //Wait for conversion done
        
        digital = (ADRESH*256) | (ADRESL);/*Combine 8-bit LSB and 2-bit MSB*/
        // DO: define voltage = Vref/4096 (note that voltage is float type
        voltage3 = digital*((float)Vref/(float)4096);
        
        ADCON0bits.GO = 1;
        
        while (ADCON0bits.GO); //Wait for conversion done
        
        digital = (ADRESH*256) | (ADRESL);/*Combine 8-bit LSB and 2-bit MSB*/
        // DO: define voltage = Vref/4096 (note that voltage is float type
        voltage4 = digital*((float)Vref/(float)4096);
        
        ADCON0bits.GO = 1;
        
        while (ADCON0bits.GO); //Wait for conversion done
        
        digital = (ADRESH*256) | (ADRESL);/*Combine 8-bit LSB and 2-bit MSB*/
        // DO: define voltage = Vref/4096 (note that voltage is float type
        voltage5 = digital*((float)Vref/(float)4096);
        
        vfinal = (voltage1 + voltage2 + voltage3 + voltage4 + voltage5) / 5;
        
        /*This is used to convert integer value to ASCII string*/
        sprintf(data,"%.4f",vfinal);
        strcat(data,"V Sound Lv");	/*Concatenate result and unit to print*/
        
        LCD_SetCursor(1, 0);
        LCD_Print("");

        LCD_SetCursor(2, 0);
        LCD_Print("");
        
        if (vfinal <= 0.1999) {
            LCD_SetCursor(1, 0);
            LCD_Print("Sound: Quiet    ");

            LCD_SetCursor(2, 0);
            LCD_Print(data);
            __delay_ms(500);
        }
        else if (vfinal <= 0.2006) {
            LCD_SetCursor(1, 0);
            LCD_Print("Sound: Normal");

            LCD_SetCursor(2, 0);
            LCD_Print(data);
            __delay_ms(500);
        }
        else if (vfinal <= 0.201) {
            LCD_SetCursor(1, 0);
            LCD_Print("Sound: Loud     ");

            LCD_SetCursor(2, 0);
            LCD_Print(data);
            __delay_ms(500);
        }
        else{
            LCD_SetCursor(1, 0);
            LCD_Print("Sound: Obnoxious");

            LCD_SetCursor(2, 0);
            LCD_Print(data);
            __delay_ms(500);
        }
        
        /*
        LCD_SetCursor(1, 0);
        LCD_Print("The input volt:");

        LCD_SetCursor(2, 0);
        LCD_Print(data);
         */
    }
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
