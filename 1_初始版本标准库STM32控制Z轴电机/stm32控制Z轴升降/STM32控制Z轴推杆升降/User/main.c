#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "KEY.h"
#include "Timer.h"
#include "PWM.h"
#include "serial.h"

#include "H_Bridge.h"
#include "Sensor_BRT_RS485.h"

/*  init_location:代表推杆的初始位置，以拉绳编码器的值作为单位，一圈4096
    error_location：代表误差的范围，经过计算，误差范围与定时器的频率有关
    定时器的频率不能太快，即不能快于100Hz，每10ms发送一次问询
*/
#define init_location 6610
#define error_location 0
/* RS485发送命令的数组，stm32使用串口发送出去 */
static uint8_t array1[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B };
/* 用于在OLED屏幕上显示传感器返回的相应的数值 */
uint8_t col = 1;
uint8_t i = 0;

uint16_t Current_Location = 0;
uint8_t KeyNum = 0;

int main(void)
{
	OLED_Init();
    Key_Init();
    Timer_Init();
    PWM_Init();
    Serial_Init();
    H_Bridge_Init();
    
    /*  按键的函数，IO口PB13和PB14连接到继电器和推杆上，实现推杆的升降
        PB13和PB14连接到继电器上面，分别被按键PB1和PB11控制。
    */
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
            //第一行显示拉绳传感器的编码器数值
            OLED_Clear_Column(1);
            for( i = 3; i <= 6; i++ )
            {
                OLED_ShowNum(1, 1 + 4*i - 12, rx_buff[i], 3);
            }
            
            /*  可以使推杆上升到init_location对应的位置，误差为error_location */
            Current_Location = rx_buff[5] * 256 + rx_buff[6];
            OLED_ShowNum(4, 5, Current_Location, 5);
            if( Current_Location < init_location - error_location )
            {
                H_Bridge_Set_PWM2(3);
                H_Bridge_Set_PWM1(0);
            }
            else if( Current_Location > init_location + error_location )
            {
                H_Bridge_Set_PWM1(3);
                H_Bridge_Set_PWM2(0);
            }
            else if( Current_Location >= init_location-error_location && Current_Location <= init_location + error_location )
            {
//                H_BridgeDisable();
                H_Bridge_Set_PWM1(0);
                H_Bridge_Set_PWM2(0);
                OLED_ShowString(4, 11, "OK");
            }
            
            //第二 三行显示拉绳传感器返回的原始数值
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
        
        /* 按键通过控制IO口PB13 PB14的翻转，来完成控制推杆的升降 */
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

/* 定时器中断，每隔固定时间就向拉绳传感器报告位置 */
void TIM2_IRQHandler(void)
{
	if( TIM_GetITStatus(TIM2, TIM_IT_Update) == SET )
	{
		Serial_SendArray(array1, 8);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
