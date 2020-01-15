//======================Copyright (c)=====================================
// �ļ�����: LEDIndex.c
// ��������:
//
// �޸ļ�¼:
//           2019-11-24, luckx �����ļ�
//
//
//========================================================================
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "LEDIndex.h"


//= Start ================================================================

static TimerHandle_t mTimer_TaskLED;
static LED_CTRL LED_Ctrl;
static uint8_t g_Led_mode_change;


void Bsp_LED_Set(uint8_t en)
{
	if(en)
		gpio_set_level(GPIO_PIN_LEDINDEX, 0);
	else
		gpio_set_level(GPIO_PIN_LEDINDEX, 1);
}

void LED_BackTo_LastMode(void)
{
    LED_Ctrl.Mode = 0xff;
    LED_Mode_Set(LED_Ctrl.LastMode);
}

//............................................................................
//0:��������·����  ���1.25��˸
//1:�޷����ӵ�·��  ���1.25��˸
//2:���ӵ�·��   	��ɫ 0.5��˸
//3:���ӵ�����		��ɫ����
//4:HUB��������   	��ɫ 2.5��˸
//5:HUB��λ      	��ɫ2.5��˸5��
//6:HUB����豸  	��ɫ1.25��˸
//7:����         	��ɫ1.25��˸��������ָ���ԭ��״̬
//10: ����			1s��˸
//............................................................................
void LED_Mode_Set(LED_MODE mode)
{
#if LOG_DEBUG_EN == 1
    printf("START:mode:[ %d ],LastMode:[ %d ],Mode:[ %d ]\r\n",mode,LED_Ctrl.LastMode,LED_Ctrl.Mode);
    printf("flash_time:[ %d ],flash_count:[ %d ],Clour:[ %d ]\r\n",LED_Ctrl.flash_time,LED_Ctrl.flash_count,LED_Ctrl.Clour);
#endif

    if (LED_Ctrl.Mode == mode)
        return;

	g_Led_mode_change = 1;
    LED_Ctrl.Mode = mode;
    LED_Ctrl.flash_time = 0;
    LED_Ctrl.flash_count = 0xff;

   	switch(mode)
	{
		case LM_ConnectSuccess:
		case LM_CloudFail:
				//����
				LED_Ctrl.flash_count = 0;
				break;
		case LM_CloudConnected:
				//Ϩ��
     			LED_Ctrl.flash_count = 0;
				break;
		case LM_ConfigWifi:
				LED_Ctrl.flash_time_set = 10;
				break;
		case LM_Reset:
				LED_Ctrl.flash_time_set = 40;
				break;
		case LM_Update:
		case LM_Connectting:
		case LM_ConnectFail:
				LED_Ctrl.flash_time_set = 80;
				break;
		default:
				break;
		
	}


#if LOG_DEBUG_EN == 1
    printf("END:mode:[ %d ],LastMode:[ %d ],Mode:[ %d ]\r\n",mode,LED_Ctrl.LastMode,LED_Ctrl.Mode);
    printf("flash_time:[ %d ],flash_count:[ %d ],Clour:[ %d ]\r\n",LED_Ctrl.flash_time,LED_Ctrl.flash_count,LED_Ctrl.Clour);
#endif
}

void Timer_TaskLED(TimerHandle_t xTimer)
{
	//LEDģʽ�������ģ����¼�ʱ
	if(g_Led_mode_change)
	{
		LED_Ctrl.flash_time = 0;
		g_Led_mode_change = 0;
	}

	if(LED_Ctrl.Mode != 0xff)
	{
	    if (LED_Ctrl.flash_count > 0)
	    {
	        LED_Ctrl.flash_time++;

	        if (LED_Ctrl.flash_time < LED_Ctrl.flash_time_set)
	        {
	            Bsp_LED_Set(1);
	        }
	        else if (LED_Ctrl.flash_time < (LED_Ctrl.flash_time_set + LED_Ctrl.flash_time_set))
	        {
	            Bsp_LED_Set(0);
	        }
	        else
	        {
	            LED_Ctrl.flash_time = 0;

	            if (LED_Ctrl.flash_count != 0xff)
	                LED_Ctrl.flash_count--;
	        }
	    }
	    else
	    {
			if(LED_Ctrl.Mode == LM_CloudConnected)
				Bsp_LED_Set(0);
			else 
				Bsp_LED_Set(1);
		}
	}
}


void LEDIndex_Init(void)
{
	gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO15/16
    io_conf.pin_bit_mask = (1ULL << GPIO_PIN_LEDINDEX);
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE,
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

	gpio_set_level(GPIO_PIN_LEDINDEX, 0);
	
	LED_Ctrl.Mode = 0xff;
	
	mTimer_TaskLED = xTimerCreate("LEDIndex_Scan", (10 / portTICK_PERIOD_MS), pdTRUE, NULL, (TimerCallbackFunction_t)Timer_TaskLED);
	if(mTimer_TaskLED)
	{
		xTimerStart(mTimer_TaskLED, portMAX_DELAY);	
	}	

}

