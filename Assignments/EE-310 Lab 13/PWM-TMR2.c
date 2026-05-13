#include <xc.h>
#include "PWM.h"
#include "Configfile.h"

#define _XTAL_FREQ 4000000

#define SERVO_LEFT    31
#define SERVO_CENTER  47
#define SERVO_RIGHT   63

_Bool pwmStatus;

void main(void)
{
    OSCSTATbits.HFOR = 1;
    OSCFRQ = 0x02;          // 4 MHz internal oscillator

    ANSELB = 0x00;          // PORTB digital

    TRISBbits.TRISB2 = 0;   // RB2 servo signal output
    TRISBbits.TRISB6 = 1;   // top button input
    TRISBbits.TRISB7 = 1;   // bottom button input

    WPUBbits.WPUB6 = 1;     // weak pull-up for RB6
    WPUBbits.WPUB7 = 1;     // weak pull-up for RB7

    PORTB = 0x00;

    TMR2_Initialize();
    T2PR = 155;             // about 20 ms servo period
    //T2CON = 0x70;           // Timer2 prescale for servo PWM
    TMR2_StartTimer();

    PWM_Output_D8_Enable();
    PWM2_Initialize();

    PWM2_LoadDutyValue(SERVO_CENTER);

    while(1)
    {
        if(PORTBbits.RB6 == 0)
        {
            PWM2_LoadDutyValue(SERVO_LEFT);
        }
        else if(PORTBbits.RB7 == 0)
        {
            PWM2_LoadDutyValue(SERVO_RIGHT);
        }
        else
        {
            PWM2_LoadDutyValue(SERVO_CENTER);
        }

        pwmStatus = PWM2_OutputStatusGet();
        PORTBbits.RB2 = pwmStatus;
    }
}
