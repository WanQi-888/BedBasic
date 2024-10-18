#include "stm32f10x.h"                  // Device header
#include "Sensor_BRT_RS485.h"

/*  将传感器返回的16进制的原始数据转换为10进制编码器值
    未来可以进一步优化为mm
*/
uint16_t Get_EncoderVal_Dec(uint8_t *rx_buff)
{
    return rx_buff[5] * 256 + rx_buff[6];
}

/*  将负载移动到aim_location指定的位置 */
void Move_To_Location(uint16_t aim_location)
{
    Current_Location = Get_EncoderVal_Dec(rx_buff);
    if( Current_Location < aim_location - error_location )
    {
        //实现负载上升函数
        H_Bridge_Set_PWM2(3);//上升
        H_Bridge_Set_PWM1(0);//下降
    }
    else if( Current_Location > aim_location + error_location )
    {
        //实现负载下降函数
        H_Bridge_Set_PWM1(3);
        H_Bridge_Set_PWM2(0);
    }
    else if( Current_Location >= aim_location-error_location && Current_Location <= aim_location + error_location )
    {
        //实现负载停止函数
//      H_BridgeDisable();
        H_Bridge_Set_PWM1(0);
        H_Bridge_Set_PWM2(0);
    }
}

/*  发送指定命令到BRT传感器 */
void SendCommandToSensorBRT(uint8_t *Array, uint16_t Length)
{
    Serial_SendArray( Array, Length );
}
