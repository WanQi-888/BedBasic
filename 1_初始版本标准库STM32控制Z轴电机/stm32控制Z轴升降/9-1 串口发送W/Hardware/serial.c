#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

/* 串口初始化函数 */
void Serial_Init(void)
{
    /* 开启串口USART1和对应USART1的发送和接收数据引脚GPIOA的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
    /* 配置USART1的TX引脚，采用复用推挽输出模式 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    /* 配置串口的各个参数 */
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;  //波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Tx;  //串口模式，选择发送模式
	USART_InitStructure.USART_Parity = USART_Parity_No;  //校验模式，无校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //1位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //无校验，所以是8位字长
	USART_Init(USART1, &USART_InitStructure);
	
    /* 串口使能 */
	USART_Cmd(USART1, ENABLE);
}

/* 发送一个字节，写到TDR寄存器 */
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
    /* 等待数据是否发送完成 如果数据发送完成，则USART_FLAG_TXE的值会从RESET变成SET
       不需要手动清零，下次调用USART_SendData函数时，由硬件自动清0 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/* 发送一个数组 参数：数组首地址 长度 */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

/* 发送一个字符串 C语言中，字符串以'\0'结尾 */
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

/* 求X的Y次方 */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

/* 发送数字 参数：数字 数字的长度*/
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

///* print函数的移植方法，不是很懂 */
//int fputc(int ch, FILE *f)
//{
//	Serial_SendByte(ch);
//	return ch;
//}

//void Serial_Printf(char *format, ...)
//{
//	char String[100];
//	va_list arg;
//	va_start(arg, format);
//	vsprintf(String, format, arg);
//	va_end(arg);
//	Serial_SendString(String);
//}
