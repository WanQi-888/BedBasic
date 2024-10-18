#include "stm32f10x.h"                  // Device header
#include "PWM.h"

#define H_BridgeEnable_port GPIOB
#define H_BridgeEnable_pin  GPIO_Pin_12

void H_Bridge_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIOInitStructure;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIOInitStructure.GPIO_Pin = H_BridgeEnable_pin;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(H_BridgeEnable_port, &GPIOInitStructure);
    
    GPIO_WriteBit(H_BridgeEnable_port, H_BridgeEnable_pin, Bit_SET);
}

void H_BridgeEnable(void)
{
    GPIO_WriteBit(H_BridgeEnable_port, H_BridgeEnable_pin, Bit_SET);
}

void H_BridgeDisable(void)
{
    GPIO_WriteBit(H_BridgeEnable_port, H_BridgeEnable_pin, Bit_RESET);
}

void H_Bridge_Set_PWM1(uint16_t compare)
{
    PWM_SetCompare1(compare);
}

void H_Bridge_Set_PWM2(uint16_t compare)
{
    PWM_SetCompare2(compare);
}
