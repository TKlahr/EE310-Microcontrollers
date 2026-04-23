//-----------------------------
// Title: Interfacing with Sensors
//-----------------------------
// Purpose: This program uses two photoresistors as touchless inputs to enter a two-digit code. 
// If the correct code is entered, a relay is activated to run a motor and an LED turns on. 
// If the code is incorrect, a buzzer sounds and the LED turns on. An external interrupt resets the system and turns all outputs off.
// Dependencies: config.h, pic18f47k42.h, xc.h
// Compiler: MPLAB X IDE v6.30 (XC8 v3.10)
// Author: Tyler Klahr
// OUTPUTS: 
//   RA5 → Relay (Motor control)
//   RA3 → LED
//   RB2 → Buzzer
//   RD0–RD6 → 7-segment display
// INPUTS:
//   RA0 → Photoresistor input (Digit 2)
//   RA1 → Photoresistor input (Digit 1)
//   RB0 → Emergency interrupt switch (INT0)
//  	V1.0: 4/21/26 - First version 
//  	V1.1: 4/22/26 - Second version with refference

#include <xc.h>
#include <stdint.h>
#include "config.h"
#include "pic18f47k42.h"

int numTable[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
int passCode1 = 2;
int passCode2 = 3;

volatile int resetFlag = 0;

void init(void){
   
    // Buzzer pin RB2
    ANSELBbits.ANSELB2 = 0; 
    LATBbits.LATB2 = 0;
    TRISBbits.TRISB2 = 0;
     
    // RA0, RA1 as input
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;

    // RA3 LED, RA5 Motor as output
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA5 = 0;
  
    // Digital mode
    ANSELAbits.ANSELA0 = 0;
    ANSELAbits.ANSELA1 = 0;
    ANSELAbits.ANSELA3 = 0;
    ANSELAbits.ANSELA5 = 0;

    // Outputs OFF initially
    LATAbits.LATA3 = 0;
    LATAbits.LATA5 = 0;
   
    // 7-segment pins
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    TRISDbits.TRISD4 = 0;
    TRISDbits.TRISD5 = 0;
    TRISDbits.TRISD6 = 0;
    
    ANSELDbits.ANSELD0 = 0;
    ANSELDbits.ANSELD1 = 0;
    ANSELDbits.ANSELD2 = 0;
    ANSELDbits.ANSELD3 = 0;
    ANSELDbits.ANSELD4 = 0;
    ANSELDbits.ANSELD5 = 0;
    ANSELDbits.ANSELD6 = 0;
    
    LATDbits.LATD0 = 0;
    LATDbits.LATD1 = 0;
    LATDbits.LATD2 = 0;
    LATDbits.LATD3 = 0;
    LATDbits.LATD4 = 0;
    LATDbits.LATD5 = 0;
    LATDbits.LATD6 = 0;
    
    // RB0 as interrupt input
    TRISBbits.TRISB0 = 1;
    ANSELBbits.ANSELB0 = 0;

    // INT0 interrupt setup
    INTCON0bits.INT0EDG = 1;   // rising edge
    PIR1bits.INT0IF = 0;       // clear flag
    PIE1bits.INT0IE = 1;       // enable INT0
    INTCON0bits.GIE = 1;       // global interrupt enable
}

void Sevenseg_Disp(int number){
    switch (number)
    {
        case 0: LATD = numTable[0]; break;
        case 1: LATD = numTable[1]; break;
        case 2: LATD = numTable[2]; break;
        case 3: LATD = numTable[3]; break;
        case 4: LATD = numTable[4]; break;
        case 5: LATD = numTable[5]; break;
        case 6: LATD = numTable[6]; break;
        case 7: LATD = numTable[7]; break;
        case 8: LATD = numTable[8]; break;
        case 9: LATD = numTable[9]; break;
        default: LATD = numTable[0]; break;
    }
}

void __interrupt(__irq(IRQ_INT0), __high_priority) ISR(void)
{
    LATAbits.LATA5 = 0;   // Motor OFF
    LATBbits.LATB2 = 0;   // Buzzer OFF
    LATAbits.LATA3 = 0;   // LED OFF

    resetFlag = 1;
    PIR1bits.INT0IF = 0;
}

int isDigit2_pressed(){
    return(PORTAbits.RA0);
}

int isDigit1_pressed(){
    return(PORTAbits.RA1);
}

void Turn_On_Motor(){
    LATAbits.LATA5 = 1;
}

void Turn_Off_Motor(){
    LATAbits.LATA5 = 0;
}

void Turn_On_Buzzer(){
    LATBbits.LATB2 = 1;
}

void Turn_Off_Buzzer(){
    LATBbits.LATB2 = 0;
}

void Turn_On_Led(){
    LATAbits.LATA3 = 1;
}

void Turn_Off_Led(){
    LATAbits.LATA3 = 0;
}

void main(void)
{
    int digit1 = 0;
    int digit2 = 0;
    int TimeStamp = 0;
    int DigitFlag = 0;

    init();

    while(1)
    {
        if(resetFlag)
        {
            digit1 = 0;
            digit2 = 0;
            TimeStamp = 0;
            DigitFlag = 0;

            Turn_Off_Motor();
            Turn_Off_Buzzer();
            Turn_Off_Led();

            Sevenseg_Disp(0);

            resetFlag = 0;
        }

        if(isDigit1_pressed())
        {
            digit1++;
            DigitFlag = 0;
            TimeStamp = 0;
            while(isDigit1_pressed());
        }
        
        if(isDigit2_pressed())
        {
            digit2++;
            DigitFlag = 1;
            TimeStamp = 0;
            while(isDigit2_pressed());
        }

        if(DigitFlag == 0){
            Sevenseg_Disp(digit1);
        }
        else if(DigitFlag == 1){
            Sevenseg_Disp(digit2);
        }
        
        if(TimeStamp == 50){
            if((passCode1 == digit1) && (passCode2 == digit2))
            {
                Turn_On_Motor();
                Turn_On_Led();

//                for(int i = 0; i < 50; i++)
//                {
//                    if(resetFlag) break;
//                    __delay_ms(100);
//                }
//
//                Turn_Off_Motor();
//                Turn_Off_Led();
            }
            else
            {
                Turn_On_Buzzer();
                Turn_On_Led();

                for(int i = 0; i < 50; i++)
                {
                    if(resetFlag) break;
                    __delay_ms(100);
                }

                Turn_Off_Buzzer();
                Turn_Off_Led();
            }

            digit1 = 0;
            digit2 = 0;
            TimeStamp = 0;
            Sevenseg_Disp(0);
        }

        __delay_ms(100);  
        TimeStamp++;
    }
}
