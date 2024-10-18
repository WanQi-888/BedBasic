#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"
#include "PWM.h"
#include "serial.h"

#include "H_Bridge.h"
#include "Sensor_BRT_RS485.h"
#include "Process_Data.h"

/*  init_location:代表推杆的初始位置，以拉绳编码器的值作为单位，一圈4096
    error_location：代表误差的范围，经过计算，误差范围与定时器的频率有关
    定时器的频率不能太快，即不能快于100Hz，每10ms发送一次问询
*/

/* 用于在OLED屏幕上显示传感器返回的相应的数值 */
uint8_t col = 1;
uint8_t i = 0;

uint16_t Current_Location1 = 0;

uint32_t Processed_Data = 0;

int main(void)
{
	OLED_Init();
    Timer_Init();
    PWM_Init();
    Serial_2_Init(9600);
    Serial_1_Init(9600);
    H_Bridge_Init();
    
//    H_Bridge_Set_PWM1(10);
    Serial_SendByte(USART1, 0x23);
    uint8_t arr1[] = {0x23, 0x45, 0x56};
    Serial_SendArray(USART1, arr1, 3);
    char *str1 = "hello,world";
    Serial_SendString(USART1, str1);
    Serial_SendNumber(USART1, 13450, 5);
    /*  这2句话用于设置自动回传时间 自动回传模式 调用一次就行了 现在已经设置完毕 暂定70ms */
//    SendCommandToSensorBRT(Set_AutoBack, 8);
//    SendCommandToSensorBRT(Set_WorkMode, 8);
    
	while(1)
	{
//		if(1 == Usart2_rx_done) //检测数据是否接收完成  
//        {  
//            Usart2_rx_done = 0; //清零标志位
//            
//            //第一行显示拉绳传感器的编码器的原始数值
//            OLED_Clear_Column(1);
//            for( i = 3; i <= 6; i++ )
//            {
//                OLED_ShowHexNum(1, 1 + 4*i - 12, Usart2_rx_buff[i], 2);
//            }
//            
//            /*  可以使推杆上升到Move_To_Location对应的位置，误差为error_location */
//            Current_Location1 = Get_EncoderVal_Dec(Usart2_rx_buff);
//            OLED_ShowNum(4, 5, Current_Location1, 5);
//            Move_To_Location(13000);
//            
//            //第二 三行显示拉绳传感器返回的原始数值
//            OLED_Clear_Line(2);
//            OLED_Clear_Line(3);
//            for( i = 0; i < Usart2_rx_cnt; i++ )
//            {
//                if(1 + 2*i > 16)
//                {
//                    OLED_ShowHexNum(3, 1 + 2*i - 16, Usart2_rx_buff[i], 2);
//                }
//                else
//                {
//                    OLED_ShowHexNum(2, 1 + 2*i, Usart2_rx_buff[i], 2);
//                }
//            }
//            OLED_ShowNum(4, 1, Usart2_rx_cnt, 2);
//            Usart2_rx_cnt = 0;
//        }
        
        if(1 == Usart1_rx_done) //检测用户输入的数据是否接收完成  
        {  
            Usart1_rx_done = 0; //清零标志位
            
            /*  写自己的处理代码 */
            OLED_ShowNum(4, 11, Usart1_rx_cnt, 2);
            OLED_Clear_Line(1);
            for( i = 0; i < Usart1_rx_cnt; i++ )
            {
                OLED_ShowHexNum(1, 1 + 2*i, Usart1_rx_buff[i], 2);
            }
            
            if( ConvertCommandToNumber(Usart1_rx_buff, Usart1_rx_cnt, &Processed_Data) )
            {
                OLED_ShowNum(3, 1, Processed_Data, 5);
                Move_To_Location(16000);
            }
            else
            {
                OLED_ShowString(2, 1, "failed");
            }
            Usart1_rx_cnt = 0;
        }  
    }
}

/* 定时器2中断 */
void TIM2_IRQHandler(void)
{
	if( TIM_GetITStatus(TIM2, TIM_IT_Update) == SET )
	{
        //定时器2更新，触发中断，在中断里面写你想要处理的事情
//		SendCommandToSensorBRT( Get_EncoderVal, sizeof(Get_EncoderVal) / sizeof(Get_EncoderVal[0]) );
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
