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

#include <stdio.h>
#include "CLOCK_functions.h"
#include "OLED_functions.h"

uint8_t sec = 0;
uint8_t min = 0;
uint8_t hour = 0;
selection_t selection = SECONDS_SELECTED;

void CLOCK_updateTime(void)
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
    OLED_displayTime(hour,min,sec);
}

void CLOCK_updateSelectedUnit(void)
{
    if(selection == SECONDS_SELECTED)
    {
        sec++;
        if(sec >= 60){
            sec = 0;
        }
    }
    if(selection == MINUTES_SELECTED)
    {
        min++;
        if(min >= 60){
            min = 0;
        }
    }
    if(selection == HOURS_SELECTED)
    {
        hour++;
        if(hour >= 24){
            hour = 0;
        }
    }
    OLED_displayTime(hour,min,sec);
}

void CLOCK_updateSelection(void)
{
    selection++;
    if(selection == NO_SELECTION)
    {
        selection = SECONDS_SELECTED;
    }
    OLED_displaySelection(selection);
    OLED_displayTime(hour,min,sec);
}