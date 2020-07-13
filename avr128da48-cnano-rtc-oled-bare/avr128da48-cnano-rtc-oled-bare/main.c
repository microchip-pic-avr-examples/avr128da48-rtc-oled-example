/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
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



#include <avr/io.h>
#include <avr/builtins.h>
#include <avr/interrupt.h>

#define  F_CPU                           24000000UL

#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "OLEDC_Click.h"
#include "OLED_functions.h"
#include "CLOCK_functions.h"
#include "port.h"

#define BUTTON_PRESSED                  false
#define BUTTON_LONG_PRESS_THRESHOLD     1000
#define BUTTON_TIME_STEP                10
#define ONE_SECOND_PERIOD               0x3ff

typedef enum{
    BT_NOCHANGE,
    BT_SHORT_PRESS,
    BT_LONG_PRESS
} button_t;

void buttonInit(void);
void OSCSetup(void);
void RTCInit(void);
static button_t buttonCheck(void);


uint8_t volatile second_passed = 0;
button_t button_state = BT_NOCHANGE;

int main(void)
{
    OSCSetup();
    oledC_init();
    buttonInit();
    RTCInit();
    sei();
    oledC_setup();
    OLED_displayInit();

    while (1){
        button_state = buttonCheck();
        if(second_passed == 1)
        {
            CLOCK_updateTime();
            second_passed = 0;
        }
        if(button_state == BT_SHORT_PRESS)
        {
            CLOCK_updateSelectedUnit();
        }
        else if(button_state == BT_LONG_PRESS)
        {
            CLOCK_updateSelection();
        }
        
    }
}

void OSCSetup(void)
{
	ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA),CLKCTRL_FREQSEL_24M_gc);
}  

void buttonInit(void)  
{
    PORTC_set_pin_dir(PIN7_bp, PORT_DIR_IN);
    PORTC_set_pin_pull_mode(PIN7_bp, PORT_PULL_UP);
}

void RTCInit(void)
{
    RTC.CTRLA = 1 << RTC_RTCEN_bp;     
    RTC.PER = ONE_SECOND_PERIOD; 
    RTC.CLKSEL = RTC_CLKSEL_OSC1K_gc;
    RTC.INTCTRL = 1 << RTC_OVF_bp;
}

static button_t buttonCheck(void)
{
    button_t result = BT_NOCHANGE;
    uint8_t counter = 0;
    static bool old_button_state = !BUTTON_PRESSED;
    bool button_state = PORTC_get_pin_level(PIN7_bp);
    /* detecting only the button-press event */
    if( (button_state == BUTTON_PRESSED) && (old_button_state != BUTTON_PRESSED) )
    {
        /*  wait for debouncing time */
        _delay_ms(BUTTON_TIME_STEP);
        while( (PORTC_get_pin_level(PIN7_bp) == BUTTON_PRESSED) && \
                (counter < (BUTTON_LONG_PRESS_THRESHOLD/BUTTON_TIME_STEP)) )
        {
            /* then stay in the loop until either */
            /* is button released or long-press encountered*/
            counter++;
            _delay_ms(BUTTON_TIME_STEP);
        }
        if(counter)
        {
            result = BT_SHORT_PRESS;
            if(counter >= (BUTTON_LONG_PRESS_THRESHOLD/BUTTON_TIME_STEP))
                result = BT_LONG_PRESS;
        }
    }
    old_button_state = button_state;
    return result;
}

ISR(RTC_CNT_vect)
{
    second_passed = 1;
    RTC.INTFLAGS = RTC_OVF_bm;
}
