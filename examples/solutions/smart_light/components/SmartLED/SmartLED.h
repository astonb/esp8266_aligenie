/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#ifndef _SMARTLED_H_
#define _SMAERLED_H_

typedef enum{

	LC_BLACK = 0,	//0.0.0
	LC_BLUE,		//0.0.255
	LC_GREEN,		//0.255.0
	LC_CYAN,		//0.255.255
	LC_PURPLE,		//80.00.80
	LC_RED, 		//255.0.0
	LC_ORANGE,		//255.165.0	
	LC_YELLOW,		//255.255.0
	LC_WHITE,		//255.255.255
	
	LC_INVAILD,
	LC_MAX = LC_INVAILD

}SMARTLED_COLOR;

typedef struct{

	uint8_t powerstate;
	SMARTLED_COLOR color;
	uint32_t color_value;
	
}LED_STATE;

extern LED_STATE LED_Status;;

void SmartLED_Color_SetByValue32(uint32_t color_32bit);

void SmartLED_Color_Set(SMARTLED_COLOR color);

void SmartLED_Color_Parse(uint32_t color_32bit);

void SmartLED_Init(SMARTLED_COLOR color);

#endif /* _SMARTLED_H_ */
