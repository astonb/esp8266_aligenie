//======================Copyright (c)=====================================
// 文件名称: LEDIndex.h
// 功能描述:
//
// 修改记录:
//           2019-11-24, luckx 创建文件
//
//
//========================================================================
#ifndef LEDIndex_H__
#define LEDIndex_H__


#define GPIO_PIN_LEDINDEX	5

typedef struct
{
//	uint8_t Clour;
    uint8_t Mode;
    uint8_t LastMode;
    uint16_t flash_time;
    uint16_t flash_time_set;
    uint8_t flash_count;
	
} LED_CTRL;


typedef enum
{
	LM_ConfigWifi =0,	//1:配网状态
    LM_Connectting, 	//2:正在连接路由器
    LM_ConnectFail,     //3:无法连接到路由器
    LM_ConnectSuccess,  //4:连接到路由器
    LM_CloudFail,		//5:服务器连接失败
    LM_CloudConnected,  //6:服务器连接成功
    LM_Update,          //7:升级
    LM_Reset           	//8:复位
    
} LED_MODE;

void LED_Mode_Set(LED_MODE mode);
void LEDIndex_Init(void);

#endif
