#ifndef __SERIAL_H_
#define __SERIAL_H_

#include <stdio.h>
#include <stdarg.h>

#define USART2_REC_LEN 100
#define USART1_REC_LEN 100
#define Usart1ARRAY_DEC_LEN  100

//全局变量定义  
extern uint8_t Usart2_rx_buff[];  //接收缓存，485传感器的数据通过串口2接收
extern uint8_t Usart2_rx_done;    //串口2接收完成标志  
extern uint8_t Usart2_rx_cnt;     //串口2接收数据长度

//全局变量定义  
extern uint8_t Usart1_rx_buff[];  //接收缓存，用户发送的命令由串口1接收
extern uint8_t Usart1_rx_done;    //串口1接收完成标志  
extern uint8_t Usart1_rx_cnt;     //串口1接收数据长度
extern uint32_t Usart1_decimalArray[];  //用于存储用户从串口1输入的16进制数据转换为10进制后的数据 

void Serial_2_Init(uint32_t baudrate);
void Serial_1_Init(uint32_t baudrate);
void Serial_SendByte(USART_TypeDef* USARTx, uint8_t Byte);
void Serial_SendArray(USART_TypeDef* USARTx, uint8_t *Array, uint16_t Length);
void Serial_SendString(USART_TypeDef* USARTx, char *String);
void Serial_SendNumber(USART_TypeDef* USARTx, uint32_t Number, uint8_t Length);

void Serial_Printf(USART_TypeDef* USARTx, char *format, ...);

void USART2_IRQHandler(void);
void USART1_IRQHandler(void);

#endif
