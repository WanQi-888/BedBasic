#include "stm32f10x.h"                  // Device header
#include "Process_Data.h" 

/*  辅助模块，暂时用于处理用户从串口1发送的数据 */

uint8_t checkArrayForMove(const uint8_t *array1) 
{
    // 比较数组和MOVE的ASCII码
    for (int i = 0; i < 5; ++i) 
    {
        if (array1[i] != moveCode[i]) 
        {
            // 如果有任何不匹配，返回0
            return 0;
        }
    }
    
    // 所有字符都匹配，返回1
    return 1;
}

uint8_t checkArrayForScan(const uint8_t *array) 
{
    // 比较数组和SCAN的ASCII码
    for (int i = 0; i < 5; ++i) 
    {
        if (array[i] != scanCode[i]) 
        {
            // 如果有任何不匹配，返回0
            return 0;
        }
    }
    
    // 所有字符都匹配，返回1
    return 1;
}

uint8_t ConvertCommandToNumber(const uint8_t *array, uint32_t arrayLength, uint32_t *Processed_Data) 
{
    // 检查数组长度是否足够
    if (arrayLength < 5) 
    {
        return 0;
    }

    // 检查前五个字符是否是"MOVE "
    if (!checkArrayForMove(array)) 
    {
        return 0;
    }

    // 初始化转换后的数字为0
    uint32_t number = 0;

    // 从数组的第六个元素开始转换数字
    for (uint32_t i = 5; i < arrayLength; ++i) 
    {
        // 将ASCII码转换为对应的数字，并累加到number中
        if (array[i] >= '0' && array[i] <= '9') 
        {
            number = number * 10 + (array[i] - '0');
        } 
        else 
        {
            // 如果遇到非数字字符，返回失败
            return 0;
        }
    }

    // 将转换后的数字存储在Processed_Data中
    *Processed_Data = number;
    return 1;
}
