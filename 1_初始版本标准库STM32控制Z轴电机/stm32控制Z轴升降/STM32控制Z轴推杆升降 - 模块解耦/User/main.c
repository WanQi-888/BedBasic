#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"
#include "PWM.h"
#include "serial.h"

#include "H_Bridge.h"
#include "Sensor_BRT_RS485.h"

/*  init_location:代表推杆的初始位置，以拉绳编码器的值作为单位，一圈4096
    error_location：代表误差的范围，经过计算，误差范围与定时器的频率有关
    定时器的频率不能太快，即不能快于100Hz，每10ms发送一次问询
*/

/* 用于在OLED屏幕上显示传感器返回的相应的数值 */
uint8_t col = 1;
uint8_t i = 0;

uint16_t Current_Location1 = 0;

int main(void)
{
	OLED_Init();
    Timer_Init();
    PWM_Init();
    Serial_Init();
    H_Bridge_Init();
    
    H_Bridge_Set_PWM1(10);

    /*  这2句话用于设置自动回传时间 自动回传模式 调用一次就行了 现在已经设置完毕 暂定70ms */
//    SendCommandToSensorBRT(Set_AutoBack, 8);
//    SendCommandToSensorBRT(Set_WorkMode, 8);
    
	while(1)
	{
		if(1 == rx_done) //检测数据是否接收完成  
        {  
            rx_done = 0; //清零标志位
            
            //第一行显示拉绳传感器的编码器的原始数值
            OLED_Clear_Column(1);
            for( i = 3; i <= 6; i++ )
            {
                OLED_ShowHexNum(1, 1 + 4*i - 12, rx_buff[i], 2);
            }
            
            /*  可以使推杆上升到Move_To_Location对应的位置，误差为error_location */
            Current_Location1 = Get_EncoderVal_Dec(rx_buff);
            OLED_ShowNum(4, 5, Current_Location1, 5);
            Move_To_Location(6500);
            
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
    }
}

/* 定时器中断，每隔固定时间就向拉绳传感器查询位置 */
void TIM2_IRQHandler(void)
{
	if( TIM_GetITStatus(TIM2, TIM_IT_Update) == SET )
	{
        //定时器2更新，触发中断，在中断里面写你想要处理的事情
//		SendCommandToSensorBRT( Get_EncoderVal, sizeof(Get_EncoderVal) / sizeof(Get_EncoderVal[0]) );
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
