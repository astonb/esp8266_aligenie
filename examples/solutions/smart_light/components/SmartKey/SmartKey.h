#ifndef _SMARTKEY_H__
#define _SMARTKEY_H__




#define CALLFUN_MAX			5
#define SmartKey_GPIO_NUM	16

#define KVALUE_NONE			0x00
#define KVALUE_SET			0x01

#define Key_Release			0x00
#define Key_Press			0x01



typedef struct
{
    uint8_t KeyValue;
    uint16_t PressTime;
    void (*KeyFun)(void);
} KEY_FUN;



bool KeyFun_Register(uint8_t KeyValue, uint16_t PressTime, void (*KeyFun)(void));

void SmartKey_Init(void);




#endif
