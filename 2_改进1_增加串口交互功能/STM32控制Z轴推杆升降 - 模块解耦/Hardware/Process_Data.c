#include "stm32f10x.h"                  // Device header
#include "Process_Data.h" 

/*  ����ģ�飬��ʱ���ڴ����û��Ӵ���1���͵����� */

uint8_t checkArrayForMove(const uint8_t *array1) 
{
    // �Ƚ������MOVE��ASCII��
    for (int i = 0; i < 5; ++i) 
    {
        if (array1[i] != moveCode[i]) 
        {
            // ������κβ�ƥ�䣬����0
            return 0;
        }
    }
    
    // �����ַ���ƥ�䣬����1
    return 1;
}

uint8_t checkArrayForScan(const uint8_t *array) 
{
    // �Ƚ������SCAN��ASCII��
    for (int i = 0; i < 5; ++i) 
    {
        if (array[i] != scanCode[i]) 
        {
            // ������κβ�ƥ�䣬����0
            return 0;
        }
    }
    
    // �����ַ���ƥ�䣬����1
    return 1;
}

uint8_t ConvertCommandToNumber(const uint8_t *array, uint32_t arrayLength, uint32_t *Processed_Data) 
{
    // ������鳤���Ƿ��㹻
    if (arrayLength < 5) 
    {
        return 0;
    }

    // ���ǰ����ַ��Ƿ���"MOVE "
    if (!checkArrayForMove(array)) 
    {
        return 0;
    }

    // ��ʼ��ת���������Ϊ0
    uint32_t number = 0;

    // ������ĵ�����Ԫ�ؿ�ʼת������
    for (uint32_t i = 5; i < arrayLength; ++i) 
    {
        // ��ASCII��ת��Ϊ��Ӧ�����֣����ۼӵ�number��
        if (array[i] >= '0' && array[i] <= '9') 
        {
            number = number * 10 + (array[i] - '0');
        } 
        else 
        {
            // ��������������ַ�������ʧ��
            return 0;
        }
    }

    // ��ת��������ִ洢��Processed_Data��
    *Processed_Data = number;
    return 1;
}
