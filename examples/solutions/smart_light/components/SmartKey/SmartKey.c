#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "conn_mgr.h"
#include "SmartKey.h"
#include "SmartLED.h"
#include "LEDIndex.h"

static char *TAG = "SmartKey";

static KEY_FUN KeyFuns[CALLFUN_MAX];
static TimerHandle_t mTimer_TaskKEY = NULL;

static uint8_t SmartKey_GetValue(void)
{
    return !gpio_get_level(SmartKey_GPIO_NUM);
}


bool KeyFun_Register(uint8_t KeyValue, uint16_t PressTime, void (*KeyFun)(void))
{
    PressTime /= 10;

    for (int i = 0; i < CALLFUN_MAX; i++)
    {
        if ((KeyFuns[i].KeyValue == KeyValue) && (KeyFuns[i].PressTime == PressTime))
        {
            KeyFuns[i].KeyFun = KeyFun;
            KeyFuns[i].PressTime = PressTime;
            return true;
        }
    }

    for (int i = 0; i < CALLFUN_MAX; i++)
    {
        if (KeyFuns[i].KeyFun == NULL)
        {
            KeyFuns[i].KeyValue = KeyValue;
            KeyFuns[i].KeyFun = KeyFun;
            KeyFuns[i].PressTime = PressTime;
            return true;
        }
    }

    return false;
}


static void Timer_TaskKEY(TimerHandle_t xTimer)
{
    static uint16_t PressTime = 0;
    static int16_t LastKey = -1;

    int key = SmartKey_GetValue();
    if (LastKey == key)
    {   //去抖完成
        if (key != KVALUE_NONE)
        {   //有效按键
            PressTime++;

            for (int i = 0; i < CALLFUN_MAX; i++)
            {
                if ((KeyFuns[i].KeyValue == KVALUE_SET) && (KeyFuns[i].PressTime == PressTime))
                {
                    KeyFuns[i].KeyFun();
                    return;
                }
            }
        }else
        {   //没有键按下
			if(PressTime > 0)
			{
				for (int i = 0; i < CALLFUN_MAX; i++)
				{
					if ((KeyFuns[i].KeyValue == KVALUE_NONE) && (KeyFuns[i].PressTime <= PressTime))
	                {
						if(KeyFuns[i].KeyFun != NULL)
							KeyFuns[i].KeyFun();
	                }
				}
			}
			PressTime = 0;
        }
    }
    else
    {
        LastKey = key;
    }
}

void Smart_LEDSwitch_100ms(void)
{   
    SMARTLED_COLOR color = LED_Status.color;

    if(++color >= LC_MAX)
       color = LC_BLACK;

    ESP_LOGI(TAG, "===ledSwitch[%d]===", color);
    SmartLED_Color_Set(color);
}

void Smart_LEDIndex_2s(void)
{
	LED_Mode_Set(LM_ConfigWifi);
}

void Smart_WifiConfig_2s(void)
{
    ESP_LOGI(TAG, "===SmartConfig===");

	conn_mgr_reset_wifi_config();

	esp_restart();
}

void SmartKey_Init(void)
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO15/16
    io_conf.pin_bit_mask = (1ULL << SmartKey_GPIO_NUM);
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE,
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
	
 	for (int i = 0; i < CALLFUN_MAX; i++)
 	{
		KeyFuns[i].KeyFun = NULL;
 	}

	mTimer_TaskKEY = xTimerCreate("SmartKey_Scan", (10 / portTICK_PERIOD_MS), pdTRUE, NULL, (TimerCallbackFunction_t)Timer_TaskKEY);
	if(mTimer_TaskKEY)
	{
		xTimerStart(mTimer_TaskKEY, portMAX_DELAY);
		
		KeyFun_Register(Key_Press, 	 100,  Smart_LEDSwitch_100ms);
		KeyFun_Register(Key_Press, 	 2000, Smart_LEDIndex_2s);
		KeyFun_Register(Key_Release, 2000, Smart_WifiConfig_2s);	
	}	

}



