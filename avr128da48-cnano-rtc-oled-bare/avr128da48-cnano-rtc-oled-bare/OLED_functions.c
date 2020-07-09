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
#include "OLEDC_colors.h"
#include "OLEDC_Click.h"
#include "OLEDC_shapeHandler.h"
#include "OLED_functions.h"
#include "CLOCK_functions.h"


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
#define SELECTION_VALUE_X               3
#define SELECTION_VALUE_Y               65

uint16_t background_color;

void OLED_displayTime(uint8_t h, uint8_t m, uint8_t s)
{
    char tempString[BUFF_SIZE];
    if(h < 10)
    {
        if(m < 10)
        {
            if(s < 10)
            {
                sprintf(tempString, "0%d:0%d:0%d", h,m,s);
            }
            else
            {
                sprintf(tempString, "0%d:0%d:%d", h,m,s);
            }
        }
        else
        {
            if(s < 10)
            {
                sprintf(tempString, "0%d:%d:0%d", h,m,s);
            }
            else
            {
                sprintf(tempString, "0%d:%d:%d", h,m,s);
            }
        }
    }
    else
    {
        if(m < 10)
        {
            if(s < 10)
            {
                sprintf(tempString, "%d:0%d:0%d", h,m,s);
            }
            else
            {
                sprintf(tempString, "%d:0%d:%d", h,m,s);
            }
        }
        else
        {
            if(s < 10)
            {
                sprintf(tempString, "%d:%d:0%d", h,m,s);
            }
            else
            {
                sprintf(tempString, "%d:%d:%d", h,m,s);
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
    (uint8_t *) tempString,
    OLEDC_COLOR_WHITE);
}

void OLED_clearScreen(void)
{
    oledC_setColumnAddressBounds(LOWER,UPPER);
    oledC_setRowAddressBounds(LOWER,UPPER);
    for(uint8_t x = LOWER; x < UPPER; x++){
        for(uint8_t y = LOWER; y < UPPER; y++){
            oledC_sendColorInt(background_color);
        }
    }
}

void OLED_setBackground(uint16_t color)
{
    background_color = color;
    OLED_clearScreen();
}

void OLED_displayInit(void)
{
    OLED_setBackground(OLEDC_COLOR_BLACK);
    char timeString[BUFF_SIZE];
    sprintf(timeString, "TIME");
    
    oledC_DrawString(
    TIME_TEXT_X,
    TIME_TEXT_Y,
    FONT_SIZE_X,
    FONT_SIZE_Y,
    (uint8_t *) timeString,
    OLEDC_COLOR_WHITE);
    OLED_displaySelection(SECONDS_SELECTED);
}

void OLED_displaySelection(selection_t selection)
{
    char tempString[BUFF_SIZE];
    if(selection == SECONDS_SELECTED)
        sprintf(tempString, "      --");
    else if (selection == MINUTES_SELECTED)
        sprintf(tempString, "   --   ");
    else
        sprintf(tempString, "--      ");
    
    oledC_DrawRectangle(
    SELECTION_VALUE_X,
    SELECTION_VALUE_Y,
    SELECTION_VALUE_X + BOX_SIZE_X,
    SELECTION_VALUE_Y + BOX_SIZE_Y,
    OLEDC_COLOR_BLACK);
    oledC_DrawString(
    SELECTION_VALUE_X,
    SELECTION_VALUE_Y,
    FONT_SIZE_X,
    FONT_SIZE_Y,
    (uint8_t *) tempString,
    OLEDC_COLOR_WHITE);
}