/*
 * Title: Assignment 8 - Sensor Safe Box
 * Purpose: Uses two photoresistors as touchless inputs to enter a 2-digit code.
 *          If the code is correct, the relay turns the motor on.
 *          If the code is incorrect, the buzzer sounds.
 *          The emergency switch forces an alarm routine.
 * Compiler: XC8 / MPLAB X
 * MCU: PIC18F47K42
 */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#define _XTAL_FREQ 1000000UL

// =====================
// Configuration bits
// =====================
#pragma config FEXTOSC = OFF
#pragma config RSTOSC = HFINTOSC_1MHZ
#pragma config CLKOUTEN = OFF
#pragma config CSWEN = ON
#pragma config FCMEN = OFF

// =====================
// User settings
// =====================

// Secret code: PR1 = 1, PR2 = 2
#define SECRET_DIGIT_1   1
#define SECRET_DIGIT_2   2

// 1 = sensor active when input is LOW (covered / dark)
// 0 = sensor active when input is HIGH
#define SENSOR_ACTIVE_LOW    1

// 1 = relay module turns ON with logic HIGH at IN pin
// 0 = relay module turns ON with logic LOW at IN pin
#define RELAY_ACTIVE_HIGH    1

// =====================
// Pin macros
// =====================

// Inputs
#define PR1_PORT        PORTAbits.RA7
#define PR2_PORT        PORTAbits.RA6
#define EMERGENCY_PORT  PORTCbits.RC5

// Outputs
#define RELAY_LAT       LATAbits.LATA2
#define BUZZER_LAT      LATCbits.LATC0
#define SYSLED_LAT      LATEbits.LATE0

// 7-segment pins (common anode)
#define SEG_A_LAT       LATDbits.LATD0
#define SEG_B_LAT       LATDbits.LATD1
#define SEG_C_LAT       LATDbits.LATD2
#define SEG_D_LAT       LATDbits.LATD3
#define SEG_E_LAT       LATDbits.LATD4
#define SEG_F_LAT       LATDbits.LATD5
#define SEG_G_LAT       LATDbits.LATD6

// =====================
// Function prototypes
// =====================
static void init_system(void);
static void display_blank(void);
static void display_digit(uint8_t digit);
static void set_segments(bool a, bool b, bool c, bool d, bool e, bool f, bool g);
static bool pr1_active(void);
static bool pr2_active(void);
static bool emergency_pressed(void);
static void wait_for_pr1_release(void);
static void wait_for_pr2_release(void);
static void relay_on(void);
static void relay_off(void);
static void buzzer_on(void);
static void buzzer_off(void);
static void error_beep(void);
static void emergency_alarm(void);
static void run_motor_sequence(void);

// =====================
// Main
// =====================
void main(void)
{
    uint8_t code[2];
    uint8_t count = 0;

    init_system();

    while (1)
    {
        SYSLED_LAT = 1;      // system LED always on while enabled
        relay_off();

        // emergency check
        if (emergency_pressed())
        {
            emergency_alarm();
            display_blank();
            count = 0;
        }

        // wait for first / next touchless input
        if (pr1_active())
        {
            __delay_ms(40);   // debounce / settle
            if (pr1_active())
            {
                code[count] = 1;
                display_digit(1);
                count++;
                wait_for_pr1_release();
            }
        }
        else if (pr2_active())
        {
            __delay_ms(40);
            if (pr2_active())
            {
                code[count] = 2;
                display_digit(2);
                count++;
                wait_for_pr2_release();
            }
        }

        // if two digits entered, evaluate code
        if (count >= 2)
        {
            __delay_ms(250);

            if ((code[0] == SECRET_DIGIT_1) && (code[1] == SECRET_DIGIT_2))
            {
                run_motor_sequence();
            }
            else
            {
                error_beep();
            }

            display_blank();
            count = 0;
        }
    }
}

// =====================
// Initialization
// =====================
static void init_system(void)
{
    // All digital
    ANSELA = 0x00;
    ANSELC = 0x00;
    ANSELD = 0x00;
    ANSELE = 0x00;

    // Input directions
    TRISAbits.TRISA7 = 1;   // PR1
    TRISAbits.TRISA6 = 1;   // PR2
    TRISCbits.TRISC5 = 1;   // Emergency switch

    // Output directions
    TRISAbits.TRISA2 = 0;   // Relay IN
    TRISCbits.TRISC0 = 0;   // Buzzer
    TRISEbits.TRISE0 = 0;   // System LED

    TRISDbits.TRISD0 = 0;   // 7-seg A
    TRISDbits.TRISD1 = 0;   // 7-seg B
    TRISDbits.TRISD2 = 0;   // 7-seg C
    TRISDbits.TRISD3 = 0;   // 7-seg D
    TRISDbits.TRISD4 = 0;   // 7-seg E
    TRISDbits.TRISD5 = 0;   // 7-seg F
    TRISDbits.TRISD6 = 0;   // 7-seg G

    // Start states
    relay_off();
    buzzer_off();
    SYSLED_LAT = 1;
    display_blank();
}

// =====================
// Input helpers
// =====================
static bool pr1_active(void)
{
#if SENSOR_ACTIVE_LOW
    return (PR1_PORT == 0);
#else
    return (PR1_PORT == 1);
#endif
}

static bool pr2_active(void)
{
#if SENSOR_ACTIVE_LOW
    return (PR2_PORT == 0);
#else
    return (PR2_PORT == 1);
#endif
}

static bool emergency_pressed(void)
{
    return (EMERGENCY_PORT == 1);   // switch to VCC with pulldown
}

static void wait_for_pr1_release(void)
{
    while (pr1_active())
    {
        if (emergency_pressed())
        {
            emergency_alarm();
            break;
        }
    }
    __delay_ms(80);
}

static void wait_for_pr2_release(void)
{
    while (pr2_active())
    {
        if (emergency_pressed())
        {
            emergency_alarm();
            break;
        }
    }
    __delay_ms(80);
}

// =====================
// Output helpers
// =====================
static void relay_on(void)
{
#if RELAY_ACTIVE_HIGH
    RELAY_LAT = 1;
#else
    RELAY_LAT = 0;
#endif
}

static void relay_off(void)
{
#if RELAY_ACTIVE_HIGH
    RELAY_LAT = 0;
#else
    RELAY_LAT = 1;
#endif
}

static void buzzer_on(void)
{
    BUZZER_LAT = 1;
}

static void buzzer_off(void)
{
    BUZZER_LAT = 0;
}

// =====================
// System actions
// =====================
static void run_motor_sequence(void)
{
    relay_on();
    __delay_ms(3000);
    relay_off();
}

static void error_beep(void)
{
    buzzer_on();
    __delay_ms(300);
    buzzer_off();
    __delay_ms(150);
    buzzer_on();
    __delay_ms(300);
    buzzer_off();
}

static void emergency_alarm(void)
{
    relay_off();

    for (uint8_t i = 0; i < 8; i++)
    {
        buzzer_on();
        __delay_ms(120);
        buzzer_off();
        __delay_ms(120);
    }
}

// =====================
// 7-segment display
// Common anode:
// segment ON  = 0
// segment OFF = 1
// =====================
static void set_segments(bool a, bool b, bool c, bool d, bool e, bool f, bool g)
{
    SEG_A_LAT = a ? 0 : 1;
    SEG_B_LAT = b ? 0 : 1;
    SEG_C_LAT = c ? 0 : 1;
    SEG_D_LAT = d ? 0 : 1;
    SEG_E_LAT = e ? 0 : 1;
    SEG_F_LAT = f ? 0 : 1;
    SEG_G_LAT = g ? 0 : 1;
}

static void display_blank(void)
{
    SEG_A_LAT = 1;
    SEG_B_LAT = 1;
    SEG_C_LAT = 1;
    SEG_D_LAT = 1;
    SEG_E_LAT = 1;
    SEG_F_LAT = 1;
    SEG_G_LAT = 1;
}

static void display_digit(uint8_t digit)
{
    switch (digit)
    {
        case 0: set_segments(1,1,1,1,1,1,0); break;
        case 1: set_segments(0,1,1,0,0,0,0); break;
        case 2: set_segments(1,1,0,1,1,0,1); break;
        case 3: set_segments(1,1,1,1,0,0,1); break;
        case 4: set_segments(0,1,1,0,0,1,1); break;
        case 5: set_segments(1,0,1,1,0,1,1); break;
        case 6: set_segments(1,0,1,1,1,1,1); break;
        case 7: set_segments(1,1,1,0,0,0,0); break;
        case 8: set_segments(1,1,1,1,1,1,1); break;
        case 9: set_segments(1,1,1,1,0,1,1); break;
        default: display_blank(); break;
    }
}
