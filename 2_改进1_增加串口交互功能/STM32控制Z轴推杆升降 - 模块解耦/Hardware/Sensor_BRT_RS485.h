#ifndef __Sensor_BRT_RS485_H_
#define __Sensor_BRT_RS485_H_

#include "serial.h"
#include "H_Bridge.h"

/*  移动到指定位置的误差范围 */
#define error_location 1

/*  RS485发送命令的数组，stm32使用串口发送出去 */
/*  代表希望获取拉绳传感器的原始数值 */
static uint8_t Get_EncoderVal[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B };
/*  代表希望设置传感器的自动回传时间 0x64代表100ms */
static uint8_t Set_AutoBack[]   = { 0x01, 0x06, 0x00, 0x07, 0x00, 0x46, 0xB9, 0xF9 };
/*  代表希望设置传感器的模式：
    0x00：查询模式
    0x01：自动回传编码器值
    0x05：自动回传编码器角速度值) 
*/
static uint8_t Set_WorkMode[] = { 0x01, 0x06, 0x00, 0x06, 0x00, 0x01, 0xA8, 0x0B };


static uint16_t Current_Location = 0;


uint16_t Get_EncoderVal_Dec(uint8_t *rx_buff);
void Move_To_Location(uint16_t aim_location);
void SendCommandToSensorBRT(USART_TypeDef* USARTx, uint8_t *Array, uint16_t Length);

#endif
