#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "serial.h"

int main(void)
{
	OLED_Init();
    Serial_Init();
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
//	GPIO_ResetBits(GPIOA, GPIO_Pin_0); //置低电平
	GPIO_SetBits(GPIOA, GPIO_Pin_1); //置高电平
//	GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET); //同理，第3个参数代表低电平
    
    Serial_SendByte(0x41);
    
    uint8_t MyArray[] = {0x42, 0x43, 0x44, 0x45};
	Serial_SendArray(MyArray, 4);
	
	Serial_SendString("\r\nNum1=");
	
	Serial_SendNumber(111, 3);
	
//    /* 注意在serial.h里面包含#include <stdio.h>头文件 */
////	printf("\r\nNum2=%d", 222);
//	
//    /* print函数的移植方法，不是很懂 */
//	char String[100];
//	sprintf(String, "\r\nNum3=%d", 333);
//	Serial_SendString(String);
//    
//	Serial_Printf("\r\nNum4=%d", 444);
//	Serial_Printf("\r\n");
	while(1)
	{
		
	}
}
