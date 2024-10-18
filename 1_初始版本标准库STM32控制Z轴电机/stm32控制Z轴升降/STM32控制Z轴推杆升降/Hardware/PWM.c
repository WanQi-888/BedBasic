#include "stm32f10x.h"                  // Device header

/* 使用定时器3来完成PWM波的输出,与H桥配合，实现对推杆的定时输出 */
void PWM_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
//	//引脚重映射AFIO，将TIM2的CH1即PA0重映射到PA15
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//	GPIO_PinRemapConfig(GPIO_PartialRemap2_TIM2, ENABLE);//手册里有对应的TIM2重映射模式
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//解除PA15为默认调试端口
	
	TIM_InternalClockConfig(TIM3);//使用内部72mhz时钟
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//滤波器的滤波频率，由内部时钟分频而来，TIM_CKD_DIV1代表1分频，即72MHZ
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStruct.TIM_Period = 100 - 1;//ARR
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;//PSC
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;//高级计数器才有的
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);//在调用初始化函数后，会立即生成一个更新事件
	
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;//PWM模式1，冻结，cnt>=ccr时，置有效、无效电平等
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平为高电平
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//输出使能
	TIM_OCInitStruct.TIM_Pulse = 10;//设置CCR的值
	TIM_OC1Init(TIM3, &TIM_OCInitStruct);//TIM3_OC1引脚复用在了PA6引脚
    TIM_OC2Init(TIM3, &TIM_OCInitStruct);//TIM3_OC2引脚复用在了PA7引脚
	
	//初始化GPIOA_PIN_0引脚，复用推挽输出
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出，这样才能输出PWM波形
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//初始化PA6 PA7引脚
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	TIM_Cmd(TIM3, ENABLE);//启动定时器
}

void PWM_SetCompare1(uint16_t compare)
{
	TIM_SetCompare1(TIM3, compare);//更改定时器3的CH1通道的CCR的值
}

void PWM_SetCompare2(uint16_t compare)
{
	TIM_SetCompare2(TIM3, compare);//更改定时器3的CH2通道的CCR的值
}
