//-----------------------------
// Title: ADC Voltage Measurement with LCD Display
//-----------------------------
// Purpose: This program reads an analog voltage input from a potentiometer using the PIC18F47K42 ADC. 
// The digital value is converted into a voltage and displayed on a 16x2 LCD. This demonstrates ADC 
// operation, voltage conversion, and real-time display of sensor data.
// Dependencies: xc.h, pic18f47k42.h
// Compiler: MPLAB X IDE v6.30 (XC8 v3.10)
// Author: Tyler Klahr
// OUTPUTS:
//   RD0 → LCD RS (Register Select)
//   RD1 → LCD E (Enable)
//   RB0–RB7 → LCD Data (D0–D7)
// INPUTS:
//   RA1 / ANA1 → Analog voltage input (from potentiometer)
// VERSIONS:
//   V1.0: 5/3/26 - Initial version with LCD interface
//   V1.1: 5/4/26 - Added ADC input and voltage display functionality

#include <xc.h>
#include <stdio.h>

// CONFIG (keep yours if already working)
#pragma config FEXTOSC = OFF
#pragma config RSTOSC = HFINTOSC_1MHZ
#pragma config CLKOUTEN = OFF
#pragma config MCLRE = EXTMCLR
#pragma config LVP = ON

#define _XTAL_FREQ 4000000

// LCD pin definitions
#define RS LATD0
#define EN LATD1
#define ldata LATB

#define LCD_Port TRISB
#define LCD_Control TRISD

// Function Prototypes
void LCD_Init(void);
void LCD_Command(char);
void LCD_Char(char);
void LCD_String(const char *);
void LCD_String_xy(char, char, const char *);
void LCD_Clear(void);
void MSdelay(unsigned int);

void ADC_Init(void);
unsigned int ADC_Read(void);
void Display_Voltage(void);

//-------------------- MAIN --------------------
void main(void)
{
    // LCD digital pins
    ANSELB = 0x00;
    ANSELD = 0x00;

    // ADC pin (RA1)
    TRISAbits.TRISA1 = 1;
    ANSELAbits.ANSELA1 = 1;

    LATB = 0x00;
    LATD = 0x00;

    LCD_Init();
    LCD_Clear();

    ADC_Init();

    while(1)
    {
        Display_Voltage();
        MSdelay(500);
    }
}

//-------------------- ADC --------------------
void ADC_Init(void)
{
    ADCON0bits.ADFM = 1;   // Right justified
    ADCON0bits.CS = 1;     // Internal clock
    ADPCH = 0x01;          // ANA1 (RA1)
    ADCON0bits.ADON = 1;   // Turn on ADC
}

unsigned int ADC_Read(void)
{
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO);

    return ((unsigned int)ADRESH << 8) | ADRESL;
}

void Display_Voltage(void)
{
    unsigned int adcValue;
    float voltage;
    char buffer[16];
    char *soundLevel;

    adcValue = ADC_Read();
    voltage = ((float)adcValue / 4095.0) * 5.0;

    // --- Sound classification ---
    if (voltage < 1.5)
        soundLevel = "quiet";
    else if (voltage < 2.5)
        soundLevel = "normal";
    else if (voltage < 3.5)
        soundLevel = "loud";
    else
        soundLevel = "obnox";

    // --- Display ---
    LCD_String_xy(1, 0, "Sound:        ");
    LCD_String_xy(1, 7, soundLevel);

    sprintf(buffer, "%.2fV Level   ", voltage);
    LCD_String_xy(2, 0, buffer);
}

//-------------------- LCD --------------------
void LCD_Init()
{
    MSdelay(15);
    LCD_Port = 0x00;
    LCD_Control = 0x00;

    LCD_Command(0x01);
    LCD_Command(0x38);
    LCD_Command(0x0C);
    LCD_Command(0x06);
}

void LCD_Clear()
{
    LCD_Command(0x01);
}

void LCD_Command(char cmd)
{
    ldata = cmd;
    RS = 0;
    EN = 1;
    NOP();
    EN = 0;
    MSdelay(3);
}

void LCD_Char(char dat)
{
    ldata = dat;
    RS = 1;
    EN = 1;
    NOP();
    EN = 0;
    MSdelay(1);
}

void LCD_String(const char *msg)
{
    while(*msg)
    {
        LCD_Char(*msg++);
    }
}

void LCD_String_xy(char row, char pos, const char *msg)
{
    char location;

    if(row <= 1)
        location = 0x80 | (pos & 0x0F);
    else
        location = 0xC0 | (pos & 0x0F);

    LCD_Command(location);
    LCD_String(msg);
}

//-------------------- DELAY --------------------
void MSdelay(unsigned int val)
{
    unsigned int i, j;
    for(i = 0; i < val; i++)
        for(j = 0; j < 165; j++);
}
