#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

uint8_t rx_buff[100];  //接收缓存  
uint8_t rx_done = 0; //接收完成标志
uint8_t rx_cnt = 0;//接收数据长度

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
    
    /* 配置USART1的RX引脚 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    /* 配置串口的各个参数 */
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;  //波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //串口模式，选择发送模式并且接收模式
	USART_InitStructure.USART_Parity = USART_Parity_No;  //校验模式，无校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //1位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //无校验，所以是8位字长
	USART_Init(USART1, &USART_InitStructure);
	
    
	/* 中断相关方面的配置 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //配置优先级，即2位，2位
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  //中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
    
    /* 使用中断去接收数据 开启USART_IT_RXNE到NVIC的中断 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//增加的代码，空闲中断使能  USART_IT_IDLE
    USART_Cmd(USART1, ENABLE);//使能串口
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

///* 接收标志位 即不想暴露静态变量 */
//uint8_t Serial_GetRxFlag(void)
//{
//	if (Serial_RxFlag == 1)
//	{
//		Serial_RxFlag = 0;
//		return 1;
//	}
//	return 0;
//}

///* 返回接收到的数据 即不想暴露静态变量 */
//uint8_t Serial_GetRxData(void)
//{
//	return Serial_RxData;
//}

/* 表示接收到数据了 Serial_RxFlag置1 并清除对应标志位 */
void USART1_IRQHandler(void)
{
    uint8_t temp;  
  
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//接收到一个字节，进入一次接收中断  
    {  
        rx_buff[rx_cnt++] = USART_ReceiveData(USART1); //将接收的数据存入rx_buff中  
        if(rx_cnt >= 100) rx_cnt = 0;                //每包数据不能超过接收buff的总长度  
  
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);//清除接收中断标志  
    }  
  
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)//接收完数据后进入空闲中断  
    {  
        //USART_ClearITPendingBit(DEBUG_USARTx, USART_IT_IDLE);//这条语句是无效的  
        temp = USART1->SR; //先读SR,再读DR才能完成idle中断的清零，否则会一直进入中断。  
        temp = USART1->DR; 
  
        rx_done = 1; //检测到空闲状态，置位接收完成位  
    }
}
