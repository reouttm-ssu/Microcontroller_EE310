/******************************************************************************
 * Project Title: Voltage Memory Game
 * File: main.c
 *
 * Description:
 * A microcontroller-based memory game that uses analog voltage input,
 * PWM control, timers, push buttons, and an LCD display. The player
 * must match or remember voltage values within a timed game sequence.
 *
 * Inputs:
 * - Analog voltage input (ADC) (RA0)
 * - Push button input (RB5 / CHECK_BUTTON)
 * - Timer0 interrupt events
 * - Internal clock oscillator
 *
 * Outputs:
 * - LCD display messages and game information (PORTD 0-1, 4-7)
 * - PWM signal output (RB3)
 * - Calculated voltage readings
 * - Score and game result display
 *
 * Main Features:
 * - Reads analog voltage values
 * - Displays values and prompts on LCD
 * - Uses PWM for signal control
 * - Timed gameplay using Timer0 interrupts
 * - Win/Lose game logic with scoring system
 *
 * Author: Maxim Reoutt & John Etheridge
 * Created: May 18, 2026
 ******************************************************************************/

#include "myCConfigFile.h"
#include "functions.h"
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>


#define _XTAL_FREQ 4000000UL   // must match oscillator setting
#define FCY    _XTAL_FREQ/4     // system clock is 1 usec

// ---- LCD pin mapping (PORTD) ----
#define LCD_RS LATDbits.LATD0
#define LCD_E  LATDbits.LATD1
#define LCD_D4 LATDbits.LATD4
#define LCD_D5 LATDbits.LATD5
#define LCD_D6 LATDbits.LATD6
#define LCD_D7 LATDbits.LATD7
#define CHECK_BUTTON PORTBbits.RB5
#define Vref 5 // voltage reference

#define PWM2_INITIALIZE_DUTY_VALUE 32
#define TMR0_PRELOAD 0xFF9E

int digital; // holds the digital value 
float voltage; // hold the analog value (volt)
char data[10];
char str0[10];
char str1[10];
char str2[10];
char str3[10];
char str4[10];
double min = 0.00;
double max = 5.00;
uint16_t dutyVal = 50;
uint16_t checkdutyCycle;
char preScale;
uint16_t score = 0;
char scoreText[16];

volatile uint16_t timeout_ticks_remaining = 250;
volatile bool timeout_25s = false;

void OSC_Init(void);
void GPIO_Init(void);
void ADC_Init(void);
void LCD_Init(void);
void LCD_PulseEnable(void);
void LCD_SendNibble(uint8_t nibble);
void LCD_Command(uint8_t cmd);
void LCD_Data(uint8_t data);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Print(const char *text);
void LCD_Clear(void);
void TMR2_Initialize(void);
void TMR0_Reset25s(void);
void TMR0_Initialize(void);
void __interrupt(irq(IRQ_TMR0)) TMR0_ISR(void);
void TMR0_Reload(void);



void main(void){
    OSC_Init();     // ensure 4 MHz clock
    GPIO_Init();
    ADC_Init();
    TMR0_Initialize();
    TMR0_Reset25s();
    
    TMR2_Initialize();
    TMR2_StartTimer();        
    
    PWM_Output_D8_Enable();
    PWM2_Initialize();
    PWM2_LoadDutyValue(PWM2_INITIALIZE_DUTY_VALUE); // initialize CCPR2H/L
    
    // Duty Cycle in percentage 
    checkdutyCycle =(uint16_t)((100UL*PWM2_INITIALIZE_DUTY_VALUE)/(4*(T2PR+1)));
    // binary value of Register T2CON.PRESCALE
    preScale = ((T2CON >> 4) & (0x0F)); 
    
    bool allChecksMet = false;
    uint16_t pulseAmount = 1000;
    
    int count = 0;
    
    
    __delay_ms(200);
    
    LCD_Init();
    
    __delay_ms(200);

    LCD_SetCursor(1, 0);
    LCD_Print("Hello");

    LCD_SetCursor(2, 0);
    LCD_Print("Player!");
    
    TMR0_Reset25s();
    
    __delay_ms(3000);
    
    
    
    LCD_SetCursor(1, 0);
    LCD_Print("Prepare To");

    LCD_SetCursor(2, 0);
    LCD_Print("Memorize!");
    
    TMR0_Reset25s();
    
    __delay_ms(3000);
    
    
    srand(12556);
    
    double arr[5];
    
    arr[0] = min+((double)rand()/RAND_MAX)*(max - min);
    arr[1] = min+((double)rand()/RAND_MAX)*(max - min);
    arr[2] = min+((double)rand()/RAND_MAX)*(max - min);
    arr[3] = min+((double)rand()/RAND_MAX)*(max - min);
    arr[4] = min+((double)rand()/RAND_MAX)*(max - min);
    
    
    sprintf(str0,"%.1f",arr[0]);
    sprintf(str1,"%.1f",arr[1]);
    sprintf(str2,"%.1f",arr[2]);
    sprintf(str3,"%.1f",arr[3]);
    sprintf(str4,"%.1f",arr[4]);
    
    char *answers[5] = {str0, str1, str2, str3, str4};
    
    LCD_Clear();
    
    LCD_SetCursor(1,0);
    LCD_Print("Values:");
    
    TMR0_Reset25s();
    
    LCD_SetCursor(2, 0);
    LCD_Print(str0);
    __delay_ms(2000);
    
    LCD_SetCursor(2, 0);
    LCD_Print(str1);
    __delay_ms(2000);
    
    TMR0_Reset25s();
    
    LCD_SetCursor(2, 0);
    LCD_Print(str2);
    __delay_ms(2000);
    
    LCD_SetCursor(2, 0);
    LCD_Print(str3);
    __delay_ms(2000);
    
    TMR0_Reset25s();
    
    LCD_SetCursor(2, 0);
    LCD_Print(str4);
    __delay_ms(2000);
    
    TMR0_Reset25s();
    
    while (count < 5) {
        
        if (timeout_25s) {
            count = 5;
            allChecksMet = false;
        }
        
        ADCON0bits.GO = 1;
        
        char number[50] = "Value :";
        
        int num = count + 1;
        
        sprintf(number + strlen(number), "%d", num);
        
        while (ADCON0bits.GO); //Wait for conversion done
        
        digital = (ADRESH*256) | (ADRESL);
        // DO: define voltage = Vref/4096 (note that voltage is float type
        voltage = digital*((float)Vref/(float)4096);
        sprintf(data,"%.1f",voltage);
        
        dutyVal = ((voltage/Vref)*30)+32;
        PWM2_LoadDutyValue(dutyVal);
        checkdutyCycle = (uint16_t)((100UL * dutyVal) / (4 * (T2PR + 1)));
        
        LCD_Clear();
        
        LCD_SetCursor(1,0);
        LCD_Print(number);
        
        LCD_SetCursor(2,0);
        LCD_Print(data);
        
        __delay_ms(50);
        
        
        if (CHECK_BUTTON) {
            __delay_ms(50);

            if (CHECK_BUTTON) {
                while (CHECK_BUTTON);

                if (strcmp(data, answers[count]) == 0) {
                    count++;

                    if (count == 5) {
                        allChecksMet = true;
                    }
                    __delay_ms(200);
                } else {
                    count = 5;
                    __delay_ms(200);
                }
            }
        }
    }
    
    
    score = timeout_ticks_remaining * 10;
    TMR0_Reset25s();
        
        //with count at five, check if all checks were met
    if (count == 5)
    {
        if (allChecksMet)
        {
            LCD_Clear();
                
            LCD_SetCursor(1,0);
            LCD_Print("Congrats!");

            sprintf(scoreText, "Score: %u", score);

            LCD_SetCursor(2,0);
            LCD_Print(scoreText);
                
            __delay_ms(2000);
        }
        else
        {
            LCD_Clear();
                
            LCD_SetCursor(1,0);
            LCD_Print("Oh No!");
                
            LCD_SetCursor(2,0);
            LCD_Print("You Lost!");
                
                __delay_ms(2000);
        }
    }
}

void TMR0_Reset25s(void)
{
    timeout_ticks_remaining = 250;
    timeout_25s = false;

    TMR0_Reload();
}

void __interrupt(irq(IRQ_TMR0)) TMR0_ISR(void)
{
    PIR3bits.TMR0IF = 0;

    TMR0_Reload();

    if (timeout_ticks_remaining > 0)
    {
        timeout_ticks_remaining--;
    }

    if (timeout_ticks_remaining == 0)
    {
        timeout_25s = true;
    }
}

void TMR0_Reload(void)
{
    TMR0H = (TMR0_PRELOAD >> 8);
    TMR0L = (TMR0_PRELOAD & 0xFF);
}

void TMR0_Initialize(void)
{
    T0CON1 = 0x4A;          // Fosc/4, 1:1024 prescaler
    T0CON0 = 0x90;          // Timer0 ON, 16-bit, 1:1 postscaler

    TMR0_Reload();

    PIR3bits.TMR0IF = 0;
    PIE3bits.TMR0IE = 1;

    INTCON0bits.GIE = 1;    // global interrupts
}
