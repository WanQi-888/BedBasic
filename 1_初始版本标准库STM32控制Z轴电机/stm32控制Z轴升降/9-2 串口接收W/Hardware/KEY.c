#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIOInitStructure;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_IPU;//in pull up
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIOInitStructure);
}

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	if( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0 )
	{
		Delay_ms(20);//消抖
		while( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0 );
		Delay_ms(20);
		KeyNum = 1;
	}
	
	if( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0 )
	{
		Delay_ms(20);
		while( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0 );
		Delay_ms(20);
		KeyNum = 2;
	}
	
	return KeyNum;
}
