#include <xc.h>
#include <stdint.h>
#include "PWM.h"
#include "Configfile.h"

#define _XTAL_FREQ 4000000

#define SW_CENTER PORTBbits.RB5
#define SW_LEFT   PORTBbits.RB6
#define SW_RIGHT  PORTBbits.RB7

#define SERVO_MIN     22
#define SERVO_CENTER  51
#define SERVO_MAX     81

_Bool pwmStatus;
uint8_t servoPosition = SERVO_CENTER;
uint8_t oldPosition = SERVO_CENTER;
uint8_t moveCounter = 0;

void main(void)
{
    OSCSTATbits.HFOR = 1;
    OSCFRQ = 0x02;          // 4 MHz

    ANSELB = 0x00;          // PORTB digital

    TRISBbits.TRISB2 = 0;   // RB2 servo PWM output
    TRISBbits.TRISB5 = 1;   // center button input
    TRISBbits.TRISB6 = 1;   // left button input
    TRISBbits.TRISB7 = 1;   // right button input

    WPUBbits.WPUB5 = 1;
    WPUBbits.WPUB6 = 1;
    WPUBbits.WPUB7 = 1;

    TMR2_Initialize();
    T2PR = 155;             // about 20 ms period
    TMR2_StartTimer();

    PWM_Output_D8_Enable();
    PWM2_Initialize();
    PWM2_LoadDutyValue(servoPosition);

    while(1)
    {
        pwmStatus = PWM2_OutputStatusGet();
        PORTBbits.RB2 = pwmStatus;

        if(PIR4bits.TMR2IF == 1)
        {
            PIR4bits.TMR2IF = 0;

            moveCounter++;

            if(moveCounter >= 2)
            {
                moveCounter = 0;

                if(SW_CENTER == 0)
                {
                    servoPosition = SERVO_CENTER;
                }
                else if(SW_LEFT == 0 && servoPosition > SERVO_MIN)
                {
                    servoPosition--;
                }
                else if(SW_RIGHT == 0 && servoPosition < SERVO_MAX)
                {
                    servoPosition++;
                }

                if(servoPosition != oldPosition)
                {
                    PWM2_LoadDutyValue(servoPosition);
                    oldPosition = servoPosition;
                }
            }
        }
    }
}
