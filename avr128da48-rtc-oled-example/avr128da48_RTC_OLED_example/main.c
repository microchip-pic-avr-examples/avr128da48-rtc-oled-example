/* 
    (c) 2019 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip software and any
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party
    license terms applicable to your use of third party software (including open source software) that
    may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
    FOR A PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
    SOFTWARE.
*/

#define F_CPU                           16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "OLEDC_shapes.h"
#include "OLEDC_Click.h"
#include "OLEDC_colors.h"

#define BUFF_SIZE                       10
#define FONT_SIZE_X                     2
#define FONT_SIZE_Y                     2
#define UPPER                           96
#define LOWER                           0
#define TIME_TEXT_X                     25
#define TIME_TEXT_Y                     25
#define BOX_SIZE_X                      100
#define BOX_SIZE_Y                      18
#define TIME_VALUE_X                    3
#define TIME_VALUE_Y                    45
#define LINE_VALUE_X                    3
#define LINE_VALUE_Y                    70

#define PORT_CHECKER                    0x80
#define PRESSED_ONE_SECOND              0x15
#define ENABLE_TCB                      0x03
#define DISABLE_TCB                     0x02
#define INT_ON_LOW_LEVEL                0x05
#define THOUSAND_HZ_CLOCK               0x01
#define ENABLE_RTC                      0x01
#define SECOND_PERIOD                   0x03C4
#define ENABLE_OVF_INT                  0x01
#define CLOCK_PRE_TWO                   0x02
#define PERIODIC_INT_MODE               0x00
#define SIXTY_MILISECOND_PERIOD         0x1D4C
#define PULL_UP                         0x08
#define SECONDS                         0
#define MINUTES                         1
#define HOURS                           2
#define RETURN_TO_SECONDS               3

static void RTC_Init(void);
static void TMR_Init(void);
static void PORT_init(void);
static void CLK_init(void);
static void SPI_init(void);
static void clearScreen(void);
static void setBackground(uint16_t color);
static void displayInit(void);
static void displayTime(uint8_t h, uint8_t m, uint8_t s);
static void Start_Button_Timer(void);
static void Stop_Button_Timer(void);


uint16_t background_color;
uint16_t time_test = 0;
uint8_t volatile second_passed = 0;
uint8_t sec = 0;
uint8_t min = 0;
uint8_t hour = 0;
uint8_t volatile button = 0;
uint8_t button_timer_started = 0;
uint8_t volatile timer = 0;
uint8_t volatile time_pressed = 0;
uint8_t selection = 0;


int main(void)
{
	PORT_init();
	CLK_init();
	SPI_init();
	TMR_Init();
    RTC_Init();
    oledC_setup();
    displayInit();
    
    sei();
    
    while (1)
    {
        if(second_passed == 1)
        {
            sec++;
            if(sec >= 60)
            {
                sec = 0;
                min++;
                if(min >= 60)
                {
                    min = 0;
                    hour++;
                    if(hour >= 24)
                    {
                        hour = 0;
                    }
                }
            }
            displayTime(hour,min,sec);
            second_passed = 0;
        }
        if(button == 1)
        {
            if( !button_timer_started)
            {
                Start_Button_Timer();
                button_timer_started = 1;
            }
            if( timer && ((PORTC.IN & PORT_CHECKER) == 0) )
            {
                time_pressed++;
                timer = 0;
            }
            if(timer && ( (PORTC.IN & PORT_CHECKER) != 0) )
            {
                if(selection == SECONDS)
                {
                    sec++;
                    if(sec >= 60)
                    sec = 0;
                }
                if(selection == MINUTES)
                {
                    min++;
                    if(min >= 60)
                    min = 0;
                }
                if(selection == HOURS)
                {
                    hour++;
                    if(hour >= 24)
                    hour = 0;
                }
                time_pressed = 0;
                timer = 0;
                button = 0;
                Stop_Button_Timer();
                button_timer_started = 0;
            }
            if(time_pressed > PRESSED_ONE_SECOND)
            {
                selection++;
                if(selection == RETURN_TO_SECONDS)
                {
                    selection = SECONDS;
                }
                time_pressed = 0;
                button = 0;
                Stop_Button_Timer();
                button_timer_started = 0;
                _delay_ms(1000);
            }
        }
    }
    
}

static void PORT_init(void)
{
    /* Configure PC7 as input */
    PORTC.DIR &= ~PIN7_bm;
	PORTC.PIN7CTRL = INT_ON_LOW_LEVEL | PULL_UP;
	
    /* Configure OLED CS pin - PA7 as output low */
	PORTA.DIRSET |= PIN7_bm;
    PORTA.OUT &= ~PIN7_bm;

	/* Configure OLED DC pin - PD0 as output low */
	PORTD.DIRSET |= PIN0_bm;
    PORTD.OUT &= ~PIN0_bm;

    /* Configure OLED RW pin - PD3 as output low */
	PORTD.DIRSET |= PIN3_bm;
    PORTD.OUT &= ~PIN3_bm;
	
    /* Configure OLED EN pin - PD6 as output low */
	PORTD.DIRSET |= PIN6_bm;
    PORTD.OUT &= ~PIN6_bm;

    /* Configure OLED RST pin - PD7 as output low */	
	PORTD.DIRSET |= PIN7_bm;
    PORTD.OUT &= ~PIN7_bm;
}

static void CLK_init (void)
{
    /* Configure OSCHF to run at 16 MHz */
    _PROTECTED_WRITE (CLKCTRL.OSCHFCTRLA, CLKCTRL_FREQSEL_16M_gc);	
}

static void SPI_init(void)
{
    /* Configure MISO - PA5 as input with pull-up disabled*/
	PORTA.DIR &= ~PIN5_bm;
	PORTA.PIN5CTRL &= ~PORT_PULLUPEN_bm;
	
    /* Configure MOSI - PA4 as output low */
	PORTA.DIRSET |= PIN4_bm;
    PORTA.OUT &= ~PIN4_bm;

    /* Configure SCK - PA6 as output low */
	PORTA.DIRSET |= PIN6_bm;
    PORTA.OUT &= ~PIN6_bm;

	SPI0.CTRLA = 0 << SPI_CLK2X_bp    /* Enable Double Speed: disabled */
	| 0 << SPI_DORD_bp   /* Data Order Setting: disabled */
	| 1 << SPI_ENABLE_bp /* Enable Module: enabled */
	| 1 << SPI_MASTER_bp /* SPI module in master mode */
	| SPI_PRESC_DIV4_gc; /* System Clock / 4 */
}

static void RTC_Init()
{
	RTC.CLKSEL = THOUSAND_HZ_CLOCK; //1.024 kHz.
	RTC.INTCTRL = ENABLE_OVF_INT;  //interrupt en
	RTC.PER = SECOND_PERIOD; //1sec period
	RTC.CTRLA = ENABLE_RTC; //enable
}

static void TMR_Init()
{
	TCB0.CTRLA |= CLOCK_PRE_TWO; //clk_per/2 source
	TCB0.CTRLB |= PERIODIC_INT_MODE; //periodic interrupt
	TCB0.INTCTRL |= TCB_OVF_bm; // OVF interrupt
	TCB0.CNT = SIXTY_MILISECOND_PERIOD; //60 ms period
}

static void clearScreen(void)
{
    oledC_setColumnAddressBounds(LOWER,UPPER);
    oledC_setRowAddressBounds(LOWER,UPPER);
    for(uint8_t x = LOWER; x < UPPER; x++){
        for(uint8_t y = LOWER; y < UPPER; y++){
            oledC_sendColorInt(background_color);
        }
    }
}

static void setBackground(uint16_t color)
{
    background_color = color;
    clearScreen();
}

static void displayInit(void)
{
    setBackground(OLEDC_COLOR_BLACK);
    
    oledC_DrawString(
    TIME_TEXT_X,
    TIME_TEXT_Y,
    FONT_SIZE_X,
    FONT_SIZE_Y,
    "TIME",
    OLEDC_COLOR_WHITE);
}

static void displayTime(uint8_t h, uint8_t m, uint8_t s)
{
    char tempString[BUFF_SIZE];
    char selectionLine[BUFF_SIZE];
    if(h < 10)
    {
        if(m < 10)
        {
            if(s < 10)
            {
                sprintf(tempString, "0%d:0%d:0%d \0", h,m,s);
            }
            else
            {
                sprintf(tempString, "0%d:0%d:%d \0", h,m,s);
            }
        }
        else
        {
            if(s < 10)
            {
                sprintf(tempString, "0%d:%d:0%d \0", h,m,s);
            }
            else
            {
                sprintf(tempString, "0%d:%d:%d \0", h,m,s);
            }
        }
    }
    else
    {
        if(m < 10)
        {
            if(s < 10)
            {
                sprintf(tempString, "%d:0%d:0%d \0", h,m,s);
            }
            else
            {
                sprintf(tempString, "%d:0%d:%d \0", h,m,s);
            }
        }
        else
        {
            if(s < 10)
            {
                sprintf(tempString, "%d:%d:0%d \0", h,m,s);
            }
            else
            {
                sprintf(tempString, "%d:%d:%d \0", h,m,s);
            }
        }
    }

    
    oledC_DrawRectangle(
    TIME_VALUE_X,
    TIME_VALUE_Y,
    TIME_VALUE_X + BOX_SIZE_X,
    TIME_VALUE_Y + BOX_SIZE_Y,
    OLEDC_COLOR_BLACK);
    oledC_DrawString(
    TIME_VALUE_X,
    TIME_VALUE_Y,
    FONT_SIZE_X,
    FONT_SIZE_Y,
    tempString,
    OLEDC_COLOR_WHITE);
    if(selection == SECONDS)
    {
        sprintf(selectionLine,"      __ \0");
    }
    if(selection == MINUTES)
    {
        sprintf(selectionLine,"   __     \0");
    }
    if(selection == HOURS)
    {
        sprintf(selectionLine,"__        \0");
    }
    oledC_DrawRectangle(
    LINE_VALUE_X,
    LINE_VALUE_Y,
    LINE_VALUE_X + BOX_SIZE_X,
    LINE_VALUE_Y + BOX_SIZE_Y,
    OLEDC_COLOR_BLACK);
    oledC_DrawString(
    LINE_VALUE_X,
    LINE_VALUE_Y,
    FONT_SIZE_X,
    FONT_SIZE_Y,
    selectionLine,
    OLEDC_COLOR_WHITE);
}

static void Start_Button_Timer()
{
    TCB0.CTRLA = ENABLE_TCB; //enable timer
}

static void Stop_Button_Timer()
{
    TCB0.CTRLA = DISABLE_TCB; //disable timer
}

ISR(PORTC_PORT_vect)
{
    button = 1;
    PORTC.INTFLAGS = PIN7_bm;
}

ISR(RTC_CNT_vect)
{
    second_passed = 1;
    RTC.INTFLAGS = RTC_OVF_bm;
}

ISR(TCB0_INT_vect)
{
    timer = 1;
    TCB0.INTFLAGS = TCB_OVF_bm;
}
