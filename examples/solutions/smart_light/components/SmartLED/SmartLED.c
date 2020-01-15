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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"

#include "SmartLED.h"
#include "ws2812_driver.hpp"

LED_STATE LED_Status;
extern SemaphoreHandle_t Device_DP_Report;
xSemaphoreHandle LEDColor_SetUp_Mutex = NULL;

void SmartLED_Color_SetByValue32(uint32_t color_32bit)
{
	uint8_t color_r = 0;
	uint8_t color_g = 0;
	uint8_t color_b = 0;

	if(color_32bit > 0xffffff)
		return;

	color_r = (color_32bit&0xf00)>>16;
	color_g = (color_32bit&0xf0)>>8;
	color_b = (uint8_t)color_32bit;

	ws2812_setColor(color_r, color_g, color_b);
}

void SmartLED_Color_Set(SMARTLED_COLOR color)
{
	xSemaphoreTake(LEDColor_SetUp_Mutex, portMAX_DELAY);
	if(LED_Status.color == color || color >= LC_MAX)
		return;
	
	switch(color)
	{
		case LC_BLACK:
			LED_Status.color_value = 16777215;
			ws2812_setColor(0,0,0);
			break;
		case LC_BLUE:
			LED_Status.color_value = 255;
			ws2812_setColor(0,0,255);
			break;
		case LC_GREEN:
			LED_Status.color_value = 65280;
			ws2812_setColor(0,255,0);
			break;
		case LC_CYAN:
			LED_Status.color_value = 65535;
			ws2812_setColor(0,255,255);
			break;
		case LC_PURPLE:
			LED_Status.color_value = 8388736;
			ws2812_setColor(80,0,80);
			break;
		case LC_RED:
			LED_Status.color_value = 16711680;
			ws2812_setColor(255,0,0);
			break;
		case LC_ORANGE:
			LED_Status.color_value = 16753920;
			ws2812_setColor(255,165,0);
			break;
		case LC_YELLOW:
			LED_Status.color_value = 16776960;
			ws2812_setColor(255,255,0);
			break;
		case LC_WHITE:
			LED_Status.color_value = 16777215;
			ws2812_setColor(255,255,255);
			break;
		default:
			break;	
	}

	LED_Status.color = color;
	if(LED_Status.color != LC_BLACK)
		LED_Status.powerstate = true;
	else
		LED_Status.powerstate = false;

	if(Device_DP_Report != NULL)
	{//数据点上报
		xSemaphoreGive(Device_DP_Report);
	}

	xSemaphoreGive(LEDColor_SetUp_Mutex);
}

void SmartLED_Color_Parse(uint32_t color_32bit)
{
	switch(color_32bit)
	{
		case 255:
			SmartLED_Color_Set(LC_BLUE);
			break;
		case 65280:
			SmartLED_Color_Set(LC_GREEN);
			break;
		case 65535:
			SmartLED_Color_Set(LC_CYAN);
			break;
		case 8388736:
			SmartLED_Color_Set(LC_PURPLE);
			break;
		case 16711680:
			SmartLED_Color_Set(LC_RED);
			break;
		case 16753920:
			SmartLED_Color_Set(LC_ORANGE);
			break;
		case 16776960:
			SmartLED_Color_Set(LC_YELLOW);
			break;
		case 16777215:
			SmartLED_Color_Set(LC_WHITE);
			break;
		default:
			break;
	}
}


void SmartLED_Init(SMARTLED_COLOR color)
{
	LEDColor_SetUp_Mutex = xSemaphoreCreateMutex();
	if(LEDColor_SetUp_Mutex)
	{
		WS2812_Init();

		//初始化智能灯状态
		LED_Status.color = LC_INVAILD;
		LED_Status.powerstate = false;
		
		SmartLED_Color_Set(color);
	}
	else
	{
		ESP_LOGE("SmartLED", "===SmartLED_Init===");
	}
}



