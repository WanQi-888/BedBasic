#ifndef __SERIAL_H_
#define __SERIAL_H_

#include <stdio.h>

//全局变量定义  
extern uint8_t rx_buff[];  //接收缓存  
extern uint8_t rx_done;    //接收完成标志  
extern uint8_t rx_cnt;     //接收数据长度

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);

void Serial_Printf(char *format, ...);

//uint8_t Serial_GetRxFlag(void);
//uint8_t Serial_GetRxData(void);
void USART1_IRQHandler(void);

#endif
