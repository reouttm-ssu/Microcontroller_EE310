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
    //	    v0.2 4/1/26 (code compiles, yet to be tested on simulator or
    //	    PIC [Does not work])
    //	    v1.0 4/2/26 (Final product, code works as specified)
    
#include ".\myConfigFile.inc"
#include <xc.inc>
    
in_loop 	equ	0xFF
out_loop2	equ	0x05
out_loop	equ	0xFF

REG10   equ     10h
REG11   equ     11h
REG12	equ	12h
;---------------------
; Program Organization
;---------------------
    PSECT absdata,abs,ovrld        ; Do not change

    ORG          0                ;Reset vector
    GOTO        _INIT

    ORG          0020H           ; Begin assembly at 0020H
    
    
    // initialize display values in registers 40 to 50, setting up the PORTD
    // outputs for each digit. also sets up delay times and ports D and B
_INIT:
    //start with PORTB (This section of code was brought from a previous project)
    BANKSEL	PORTB ;
    CLRF	PORTB ;Init PORTB
    BANKSEL	LATB ;Data Latch
    CLRF	LATB ;
    BANKSEL	ANSELB ;
    CLRF	ANSELB ;digital I/O
    BANKSEL	TRISB ;
    MOVLW	0b00000011 ;Set RD[1:0] as outputs
    MOVWF	TRISB
    
    
    //now PORTD (This section of code was brought from a previous project)
    BANKSEL	PORTD ;
    CLRF	PORTD ;Init PORTA
    BANKSEL	LATD ;Data Latch
    CLRF	LATD ;
    BANKSEL	ANSELD ;
    CLRF	ANSELD ;digital I/O
    BANKSEL	TRISD ;
    MOVLW	0b00000000 ;Set RD[7:1] as outputs
    MOVWF	TRISD ;and set RD0 as ouput
    
    
    // load 7-segment codes into RAM 0x30 to 0x3F
    // common-cathode patterns
    movlw   0x3F    // 0
    movwf   0x40,0
    movlw   0x06    // 1
    movwf   0x41,0
    movlw   0x5B    // 2
    movwf   0x42,0
    movlw   0x4F    // 3
    movwf   0x43,0
    movlw   0x66    // 4
    movwf   0x44,0
    movlw   0x6D    // 5
    movwf   0x45,0
    movlw   0x7D    // 6
    movwf   0x46,0
    movlw   0x07    // 7
    movwf   0x47,0
    movlw   0x7F    // 8
    movwf   0x48,0
    movlw   0x6F    // 9
    movwf   0x49,0
    movlw   0x77    // A
    movwf   0x4A,0
    movlw   0x7C    // B
    movwf   0x4B,0
    movlw   0x39    // C
    movwf   0x4C,0
    movlw   0x5E    // D
    movwf   0x4D,0
    movlw   0x79    // E
    movwf   0x4E,0
    movlw   0x71    // F
    movwf   0x4F,0
    
    clrf    FSR0H
    movlw   0x40
    movwf   FSR0L //set pointer
    call    _DELAY //first delay
    
    movf   INDF0,W //set display to 0
    movwf   LATD,0
    goto    _MAIN
    
    
    // check each switch, calling the appropriate functions to increment or 
    // decrement the 0 pointer, make sure the pointer is in range, and then 
    // update PORTD with the digit to display. Then, call the delay function
_MAIN:
    btfsc   PORTB,0 //check if switch a is on
    call    _INCREMENT
    btfsc   PORTB,1 //check if switch b is on
    call    _DECREMENT
    
    movlw   0x50 //check if pointer is below 0x50
    cpfslt  FSR0L
    call    _START
    
    movlw   0x3F //check if pointer is above 0x3F
    cpfsgt  FSR0L
    call    _END
    
    movlw   0x0 //update segement with value being pointed to
    movf   INDF0,W
    movwf   LATD,0
    
    call    _DELAY //delay the program
    
    goto    _MAIN //restart program loop
    
    
    // increment the pointer, but checks if b is pressed first
_INCREMENT:
    btfsc   PORTB,1 //check if b is on
    goto    _START
    incf    FSR0L,1 //increment pointer
    return
    
    
    // decrement the pointer, but checks if a is pressed first
_DECREMENT:
    btfsc   PORTB,0 //check if a is on
    goto    _START
    decf    FSR0L,1 //decrement pointer
    return


    // sets pointer at the start of the range
_START:
    movlw   0x40 //go to start of digit loop
    movwf   FSR0L
    return
    
    
    // sets pointer at the end of the range
_END:
    movlw   0x4F //go to end of digit loop
    movwf   FSR0L
    return
    
    
    // delays the code for a time of about 1.0 seconds
_DELAY:
    MOVLW   in_loop
    MOVWF   REG10
    MOVLW   out_loop
    MOVWF   REG11
    MOVLW   out_loop2
    MOVWF   REG12
    
loop1:
    DECF    REG10,1
    BNZ	    loop1
    MOVLW   in_loop
    MOVWF   REG10
    DECF    REG11
    BNZ	    loop1
    MOVLW   out_loop
    MOVWF   REG11
    DECF    REG12
    BNZ	    loop1
    
    RETURN
