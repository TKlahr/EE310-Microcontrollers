#include <xc.h>
#include <stdint.h>
#include "PWM.h"
#include "Configfile.h"

#define _XTAL_FREQ 4000000

#define SW_LEFT   PORTBbits.RB6
#define SW_RIGHT  PORTBbits.RB7

#define SERVO_MIN     22   // about 1.0 ms
#define SERVO_CENTER  51   // about 1.5 ms
#define SERVO_MAX     81   // about 2.0 ms

_Bool pwmStatus;
uint8_t servoPosition = SERVO_CENTER;
uint8_t moveCounter = 0;

void main(void)
{
    OSCSTATbits.HFOR = 1;
    OSCFRQ = 0x02;          // 4 MHz

    ANSELB = 0x00;          // PORTB digital

    TRISBbits.TRISB2 = 0;   // RB2 servo PWM output
    TRISBbits.TRISB6 = 1;   // SW1 input
    TRISBbits.TRISB7 = 1;   // SW2 input

    WPUBbits.WPUB6 = 1;     // pull-up for SW1
    WPUBbits.WPUB7 = 1;     // pull-up for SW2

    TMR2_Initialize();
    T2PR = 155;             // about 20 ms period
    //T2CON = 0x70;           // servo Timer2 setup
    TMR2_StartTimer();

    PWM_Output_D8_Enable();
    PWM2_Initialize();
    PWM2_LoadDutyValue(servoPosition);

    while(1)
    {
        // Keep software PWM constantly updated
        pwmStatus = PWM2_OutputStatusGet();
        PORTBbits.RB2 = pwmStatus;

        // Only change servo position once every PWM period
        if(PIR4bits.TMR2IF == 1)
        {
            PIR4bits.TMR2IF = 0;

            moveCounter++;

            if(moveCounter >= 2)   // increase number for slower movement
            {
                moveCounter = 0;

                if(SW_LEFT == 0 && servoPosition > SERVO_MIN)
                {
                    servoPosition--;
                }
                else if(SW_RIGHT == 0 && servoPosition < SERVO_MAX)
                {
                    servoPosition++;
                }

                PWM2_LoadDutyValue(servoPosition);
            }
        }
    }
}
