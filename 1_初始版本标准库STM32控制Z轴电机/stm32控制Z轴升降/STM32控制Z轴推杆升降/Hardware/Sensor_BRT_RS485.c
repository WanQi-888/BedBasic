#include "stm32f10x.h"                  // Device header
#include "serial.h"

static uint8_t Get_EncoderVal[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B };
static uint8_t Set_AutoBack[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B };

uint16_t Get_EncoderVal_Dec(uint8_t *rx_buff)
{
    return rx_buff[5] * 256 + rx_buff[6];
}
