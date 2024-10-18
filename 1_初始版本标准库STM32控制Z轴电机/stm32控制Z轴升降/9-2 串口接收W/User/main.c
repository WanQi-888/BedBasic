#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "KEY.h"
#include "Timer.h"
#include "serial.h"

#define init_location 5000
#define error_location 20

static uint8_t array1[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B };
uint8_t col = 1;
uint8_t i = 0;

uint16_t Current_Location = 0;
uint8_t KeyNum = 0;

int main(void)
{
	OLED_Init();
    Key_Init();
    Timer_Init();
    Serial_Init();
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_ResetBits(GPIOB, GPIO_Pin_13); //置低电平
    GPIO_ResetBits(GPIOB, GPIO_Pin_14); //置低电平
    
	while(1)
	{
//        Serial_SendArray(array1, 8);
//        Delay_ms(20);
		if(1 == rx_done) //检测数据是否接收完成  
        {  
            rx_done = 0; //清零标志位 
            //此处添加相应的数据处理代码吧
            OLED_Clear_Column(1);
            for( i = 3; i <= 6; i++ )
            {
                OLED_ShowNum(1, 1 + 4*i - 12, rx_buff[i], 3);
            }
            
            Current_Location = rx_buff[5] * 256 + rx_buff[6];
            OLED_ShowNum(4, 5, Current_Location, 5);
            if( Current_Location < init_location - error_location )
            {
                GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
                GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
            }
            else if( Current_Location > init_location + error_location )
            {
                GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
                GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
            }
            else if( Current_Location > init_location-error_location && Current_Location < init_location + error_location )
            {
                GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
                GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
                OLED_ShowString(4, 11, "OK");
            }
            
            OLED_Clear_Column(2);
            OLED_Clear_Column(3);
            for( i = 0; i < rx_cnt; i++ )
            {
                if(1 + 2*i > 16)
                {
                    OLED_ShowHexNum(3, 1 + 2*i - 16, rx_buff[i], 2);
                }
                else
                {
                    OLED_ShowHexNum(2, 1 + 2*i, rx_buff[i], 2);
                }
            }
            OLED_ShowNum(4, 1, rx_cnt, 2);
            rx_cnt = 0;
        }
//        OLED_ShowNum(2, 5, TIM_GetCounter(TIM2), 5);
//		OLED_ShowNum(3, 5, TIM_GetPrescaler(TIM2), 5);
        /* 按键控制IO口的翻转 */
        KeyNum = Key_GetNum();
        if(KeyNum != 0)
        {
            if(KeyNum == 1)
            {
                // 假设要翻转 GPIOB 的第 13 个引脚
                if ( GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_13) ) 
                {
                    GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
                } 
                else 
                {
                    GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
                } 
            }
            else if(KeyNum == 2)
            {
                // 假设要翻转 GPIOB 的第 14 个引脚
                if ( GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_14) ) 
                {
                    GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
                } 
                else 
                {
                    GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
                }
            }
            else
            {
                
            }
        }
	}
}

void TIM2_IRQHandler(void)
{
	if( TIM_GetITStatus(TIM2, TIM_IT_Update) == SET )
	{
		Serial_SendArray(array1, 8);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
