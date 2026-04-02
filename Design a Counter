;----------------------------------------------------
; Title: Hex Counter with 7-Segment Display
;----------------------------------------------------
; Purpose: This program uses two push buttons to control a hexadecimal counter displayed on a 7-segment display. One button increments the count, the other decrements it, and pressing both resets the count to zero.
; Dependencies: AssemblyConfig.inc, xc.inc
; Compiler: MPLAB X IDE v6.30 (XC8 Assembler)
; Author: Tyler Klahr
; Inputs: RB6 - Switch A (Increment), RB7 - Switch B (Decrement)
; Outputs: RD0–RD6 - 7-segment display (segments A–G)
; Versions: 
;     v1.0: 4/1/26 - Initial version
;----------------------------------------------------
#include "AssemblyConfig.inc"
#include <xc.inc>

;---------------------
; Delay constants
;---------------------
innerLoop  EQU 125
outerLoop  EQU 100
thirdLoop  EQU 12

;---------------------
; Register locations
;---------------------
REG10      EQU 0x0A     ; start of 7-seg table (digit 0)
REG25      EQU 0x19     ; end of table (digit F)
REG31      EQU 0x31     ; delay counter 1
REG32      EQU 0x32     ; delay counter 2
REG33      EQU 0x33     ; delay counter 3

;---------------------
; I/O definitions
;---------------------
#define BUTTONA PORTB, 6   ; increment button
#define BUTTONB PORTB, 7   ; decrement button
#define DISPLAY LATD       ; 7-segment output

;---------------------
; Reset vector
;---------------------
PSECT resetVec,class=CODE,reloc=2
resetVec:
    GOTO Start

;---------------------
; Main program
;---------------------
PSECT code

Start:
    ;-----------------------------
    ; Initialize PORTB (buttons)
    ; RB6, RB7 = inputs
    ;-----------------------------
    BANKSEL PORTB
    CLRF PORTB
    BANKSEL LATB
    CLRF LATB
    BANKSEL ANSELB
    CLRF ANSELB
    BANKSEL TRISB
    MOVLW 0xC0          ; RB6 and RB7 inputs
    MOVWF TRISB

    ;-----------------------------
    ; Initialize PORTD (display)
    ;-----------------------------
    BANKSEL PORTD
    CLRF PORTD
    BANKSEL LATD
    CLRF LATD
    BANKSEL ANSELD
    CLRF ANSELD
    BANKSEL TRISD
    CLRF TRISD          ; all outputs

    ;-----------------------------
    ; Load 7-segment codes into RAM
    ;-----------------------------
    MOVLW 0xED
    MOVWF REG10      ; 0
    MOVLW 0x21
    MOVWF 0x0B       ; 1
    MOVLW 0xF4
    MOVWF 0x0C       ; 2
    MOVLW 0xF1
    MOVWF 0x0D       ; 3
    MOVLW 0x39
    MOVWF 0x0E       ; 4
    MOVLW 0xD9
    MOVWF 0x0F       ; 5
    MOVLW 0xDD
    MOVWF 0x10       ; 6
    MOVLW 0x61
    MOVWF 0x11       ; 7
    MOVLW 0xFD
    MOVWF 0x12       ; 8
    MOVLW 0xF9
    MOVWF 0x13       ; 9
    MOVLW 0x7D
    MOVWF 0x14       ; A
    MOVLW 0x9D
    MOVWF 0x15       ; b
    MOVLW 0xCC
    MOVWF 0x16       ; C
    MOVLW 0xB5
    MOVWF 0x17       ; d
    MOVLW 0xDC
    MOVWF 0x18       ; E
    MOVLW 0x5C
    MOVWF REG25      ; F

    ;-----------------------------
    ; Initialize pointer to 0
    ;-----------------------------
    LFSR 0, REG10
    MOVF INDF0, W
    MOVWF DISPLAY

;====================================================
; Main loop
;====================================================
MainLoop:

    ; Check increment button (active-high)
    BTFSC BUTTONA
    GOTO CheckB_WithA

    ; If A not pressed, check B
    BTFSC BUTTONB
    GOTO Decrease

    ; No buttons pressed → hold value
    GOTO MainLoop

CheckB_WithA:
    ; If both buttons pressed → reset
    BTFSC BUTTONB
    GOTO Zero

    ; Only A pressed → increase
    GOTO Increase

;====================================================
; Increase / decrease logic
;====================================================
Increase:
    ; If at F, wrap to 0
    MOVF INDF0, W
    CPFSEQ REG25
    GOTO Increment
    GOTO Zero

Decrease:
    ; If at 0, wrap to F
    MOVF INDF0, W
    CPFSEQ REG10
    GOTO Decrement
    GOTO Max

Increment:
    ; Move pointer to next digit
    INCF FSR0L, F
    MOVF INDF0, W
    MOVWF DISPLAY
    CALL loopDelay
    GOTO MainLoop

Decrement:
    ; Move pointer to previous digit
    DECF FSR0L, F
    MOVF INDF0, W
    MOVWF DISPLAY
    CALL loopDelay
    GOTO MainLoop

Max:
    ; Set pointer to F
    LFSR 0, REG25
    MOVF INDF0, W
    MOVWF DISPLAY
    CALL loopDelay
    GOTO MainLoop

Zero:
    ; Set pointer to 0
    LFSR 0, REG10
    MOVF INDF0, W
    MOVWF DISPLAY
    CALL loopDelay
    GOTO MainLoop

;====================================================
; Delay subroutine
;====================================================
loopDelay:
    MOVLW innerLoop
    MOVWF REG31
    MOVLW outerLoop
    MOVWF REG32
    MOVLW thirdLoop
    MOVWF REG33

loop1:
    DECF REG31, F
    BNZ loop1

    MOVLW innerLoop
    MOVWF REG31

    DECF REG32, F
    BNZ loop1

    MOVLW outerLoop
    MOVWF REG32

    DECF REG33, F
    BNZ loop1

    RETURN
