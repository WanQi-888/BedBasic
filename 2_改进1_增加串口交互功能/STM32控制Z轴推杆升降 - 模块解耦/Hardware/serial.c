#include "stm32f10x.h"                  // Device header
#include "Serial.h"

uint8_t Usart2_rx_buff[USART2_REC_LEN];  //接收缓存，485传感器的数据通过串口2接收  
uint8_t Usart2_rx_done = 0;   //接收完成标志
uint8_t Usart2_rx_cnt = 0;    //接收数据长度

uint8_t Usart1_rx_buff[USART1_REC_LEN];  //接收缓存，用户发送的命令由串口1接收
uint8_t Usart1_rx_done = 0;   //串口1接收完成标志
uint8_t Usart1_rx_cnt = 0;    //串口1接收数据长度
uint32_t Usart1_DecArray[Usart1ARRAY_DEC_LEN];//用于存储用户从串口1输入的16进制数据转换为10进制后的数据 

/* 串口初始化函数 */
/* 串口2初始化,用于接收485传感器的数据 */
void Serial_2_Init(uint32_t baudrate)
{
    /* 开启串口USART2和对应USART2的发送和接收数据引脚GPIOA的时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
    /* 配置USART2的TX引脚，PA2 采用复用推挽输出模式 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;/* 复用推挽输出，该I/O口由片上外设即串口2接管 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 配置USART2的RX引脚，PA3 采用上拉输入模式 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//采用上拉输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    /* 配置串口的各个参数 */
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = baudrate;  //波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //硬件流控制，这里选择不采用
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //串口模式，选择发送模式并且接收模式
	USART_InitStructure.USART_Parity = USART_Parity_No;  //校验模式，无校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  //1位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //无校验，所以是8位字长
	USART_Init(USART2, &USART_InitStructure);
    
	/* 中断相关方面的配置 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //配置优先级，即2位，2位
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;  //打开串口2的中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
    
    /* 使用中断去接收数据 开启USART_IT_RXNE到NVIC的中断 */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//增加的代码，空闲中断使能  USART_IT_IDLE
    USART_Cmd(USART2, ENABLE);//使能串口
}

/* 串口1初始化,用于接收用户的数据 */
void Serial_1_Init(uint32_t baudrate)
{
    /* 开启串口USART1和对应USART1的发送和接收数据引脚GPIOA的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
    /* 配置USART1的TX引脚，PA9 采用复用推挽输出模式 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;/* 复用推挽输出，该I/O口由片上外设即串口1接管 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 配置USART1的RX引脚 PA10 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//采用上拉输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    /* 配置串口的各个参数 */
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = baudrate;  //波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //硬件流控制，这里禁用
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

/*
    * 函数名：Serial_SendByte
    * 描述  ：USARTx对应的串口发送出去一个字节Byte的数据      
    * 输入  ：USARTx：选择对应哪个串口
              Byte：要发送的字节
    * 输出  ：无
    * 调用  ：USART_SendData，标准库自带的函数
    * 返回值：无
*/
void Serial_SendByte(USART_TypeDef* USARTx, uint8_t Byte)
{
	USART_SendData(USARTx, Byte);
    /* 等待数据是否发送完成 如果数据发送完成，则USART_FLAG_TXE的值会从RESET变成SET
       不需要手动清零，下次调用USART_SendData函数时，由硬件自动清0 */
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

/*
    * 函数名：Serial_SendArray
    * 描述  ：USARTx对应的串口发送出去一个数组    
    * 输入  ：USARTx：选择对应哪个串口
              Array：要发送的数组
              Length：要发送的数组长度
    * 输出  ：无
    * 调用  ：Serial_SendByte
    * 返回值：无
*/
void Serial_SendArray(USART_TypeDef* USARTx, uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(USARTx, Array[i]);
	}
}

/*
    * 函数名：Serial_SendString
    * 描述  ：USARTx对应的串口发送出去一个字符串    
    * 输入  ：USARTx：选择对应哪个串口
              String：要发送的字符串
    * 输出  ：无
    * 调用  ：Serial_SendByte
    * 返回值：无
*/
void Serial_SendString(USART_TypeDef* USARTx, char *String)
{
	uint16_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		Serial_SendByte(USARTx, String[i]);
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

/*
    * 函数名：Serial_SendNumber
    * 描述  ：USARTx对应的串口发送出去一个数字   
    * 输入  ：USARTx：选择对应哪个串口
              String：要发送的数字
              Length：要发送的数字的长度
    * 输出  ：无
    * 调用  ：Serial_SendByte  Serial_Pow
    * 返回值：无
*/
void Serial_SendNumber(USART_TypeDef* USARTx, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(USARTx, Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

///* print函数的重定向方法，不是很懂原理 */
//int fputc(int ch, FILE *f)
//{
//	Serial_SendByte(ch);
//	return ch;
//}

/*  使用sprintf可以同时使用多个串口打印 C语言可变参数*/
void Serial_Printf(USART_TypeDef* USARTx, char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(USARTx, String);
}

/* 串口2：使用空闲中断，来使串口接收不定长的数据包 */
void USART2_IRQHandler(void)
{
    uint8_t temp;  
  
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到一个字节，进入一次接收中断  
    {  
        Usart2_rx_buff[Usart2_rx_cnt++] = USART_ReceiveData(USART2); //将接收的数据存入Usart2_rx_buff中  
        if(Usart2_rx_cnt >= 100) Usart2_rx_cnt = 0;                //每包数据不能超过接收buff的总长度  
  
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);//清除接收中断标志  
    }  
  
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)//接收完数据后进入空闲中断  
    {  
        //USART_ClearITPendingBit(DEBUG_USARTx, USART_IT_IDLE);//这条语句是无效的  
        temp = USART2->SR; //先读SR,再读DR才能完成idle中断的清零，否则会一直进入中断。  
        temp = USART2->DR; 
  
        Usart2_rx_done = 1; //检测到空闲状态，置接收完成位为1  
    }
}

/* 串口1：使用空闲中断，来使串口接收不定长的数据包 */
void USART1_IRQHandler(void)
{
    uint8_t temp;  
  
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//接收到一个字节，进入一次接收中断  
    {  
        Usart1_rx_buff[Usart1_rx_cnt++] = USART_ReceiveData(USART1); //将接收的数据存入rx_buff中  
        if(Usart1_rx_cnt >= 100) Usart1_rx_cnt = 0;                //每包数据不能超过接收buff的总长度  
  
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);//清除接收中断标志  
    }  
  
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)//接收完数据后进入空闲中断  
    {  
        //USART_ClearITPendingBit(DEBUG_USARTx, USART_IT_IDLE);//这条语句是无效的  
        temp = USART1->SR; //先读SR,再读DR才能完成idle中断的清零，否则会一直进入中断。  
        temp = USART1->DR; //先读SR,再读DR才能完成idle中断的清零，否则会一直进入中断。

        Usart1_rx_done = 1; //检测到空闲状态，置位接收完成位  
    }
}
