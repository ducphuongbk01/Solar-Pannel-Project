/**********
4 chan ADC PA0 - above; PA1 - Below; PA2 - Right; PA3 - Left
PWM_1 TIM 9 - PE5
PWM_2 TIM 12 - PB14
***********/
#include "stm32f4xx.h"                  // Device header
#include "system_timetick.h"
#include "fuzzyController.h"

#define BUFF_SIZE 6
#define ADC_BUFFSIZE	2

void init_main(void);
void Deg2Pulse_Servo1(uint32_t b);
void Deg2Pulse_Servo2(uint32_t b);

uint8_t 	rxbuff[BUFF_SIZE];
uint8_t 	data[4*BUFF_SIZE];
uint16_t indexx = 0;
uint16_t count = 0;
double below = 0, above = 0, left = 0, right = 0;
double down[10], up[10], lf[10], rgt[10];
uint32_t a1, a2;
uint16_t ADC1_Data[ADC_BUFFSIZE], ADC2_Data[ADC_BUFFSIZE];
int i = 0, j = 0;
double sumAbove = 0, sumBelow = 0;
double sumRight = 0, sumLeft = 0;
double result, result_2;
int8_t flag = 0, flag_2=0;


int main(void)
{
	init_main();
	ADC_SoftwareStartConv(ADC1);
	ADC_SoftwareStartConv(ADC2);
	SysTick_Config(SystemCoreClock/200); // ---> thoi gian lay mau 5ms
	while(1)
	{
		if(tick_flag)
		{
			tick_flag = 0;
			if( (flag == 0) && (flag_2 == 0))
				{
					DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
					DMA_Cmd(DMA2_Stream0,ENABLE);
					DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE);
					DMA_Cmd(DMA2_Stream3,ENABLE);
				}
				else if( (flag == 1) && (flag_2 == 1) )
				{
					
					result = controller(above,below,0.005);
					result_2 = controller2(right,left,0.005);
					Deg2Pulse_Servo1(result);
					Deg2Pulse_Servo2(result_2);
					flag_2 = 0;
					flag = flag_2;
				}		
//				Deg2Pulse_Servo1(180);
		}
	}
}

void init_main(void)
	{
		ADC_InitTypeDef					ADC_InitStructure;
		ADC_CommonInitTypeDef		ADC_CommonInitStructure;
		GPIO_InitTypeDef				GPIO_InitStructure;
		USART_InitTypeDef				USART_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		TIM_OCInitTypeDef 		  TIM_OCInitStructure;
		DMA_InitTypeDef  				DMA_InitStructure;
		NVIC_InitTypeDef  			NVIC_InitStructure;
		
		/******USART*****/
	/*RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
		
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4); 
		
		USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(UART4, &USART_InitStructure);
		
		USART_Cmd(UART4, ENABLE);
		
		USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE); 
		
		DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&UART4->DR;
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)rxbuff;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
		DMA_InitStructure.DMA_BufferSize = BUFF_SIZE;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		DMA_Init(DMA1_Stream2, &DMA_InitStructure);
		DMA_Cmd(DMA1_Stream2, ENABLE);
	
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	
		DMA_ITConfig(DMA1_Stream2, DMA_IT_TC, ENABLE); */
		
		/*******PWM_Servo1******/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 											
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;										
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;							
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;									
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;										
		GPIO_Init(GPIOE, &GPIO_InitStructure); 													
		GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_TIM9);
		
		TIM_TimeBaseStructure.TIM_Period = 1000-1;											
		TIM_TimeBaseStructure.TIM_Prescaler = 3360;																
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);
		
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;								
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		
		TIM_OCInitStructure.TIM_Pulse = 0;																
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;				
		TIM_OC1Init(TIM9, &TIM_OCInitStructure);												
		TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);							 

		TIM_Cmd(TIM9, ENABLE);
		
		/******PWM_Servo2*******/
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; 											
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;										
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;							
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;									
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;										
		GPIO_Init(GPIOB, &GPIO_InitStructure); 													
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_TIM12);
		
		TIM_TimeBaseStructure.TIM_Period = 1000-1;											
		TIM_TimeBaseStructure.TIM_Prescaler = 1680;																
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);
		
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;								
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		
		TIM_OCInitStructure.TIM_Pulse = 0;																
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;				
		TIM_OC1Init(TIM12, &TIM_OCInitStructure);												
		TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Enable);							 

		TIM_Cmd(TIM12, ENABLE);
		
		/*******ADC*******/
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_2 | GPIO_Pin_3; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN; 
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init (GPIOA, &GPIO_InitStructure);

		ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);
	
	
		ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
		ADC_InitStructure.ADC_ScanConvMode = ENABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
		ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfConversion = 2;
		ADC_Init(ADC1, &ADC_InitStructure);
		ADC_Init(ADC2, &ADC_InitStructure);
		
		ADC_Cmd(ADC1, ENABLE);
		ADC_Cmd(ADC2, ENABLE);
		
		ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_3Cycles);
		ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_3Cycles);
		ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 1, ADC_SampleTime_3Cycles);
		ADC_RegularChannelConfig(ADC2, ADC_Channel_3, 2, ADC_SampleTime_3Cycles);
		
		ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
		
		DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ADC1_Data;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
		DMA_InitStructure.DMA_BufferSize = ADC_BUFFSIZE;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		DMA_Init(DMA2_Stream0, &DMA_InitStructure);
		DMA_Cmd(DMA2_Stream0, ENABLE);
		ADC_DMACmd(ADC1, ENABLE);
		
		NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	
		DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
		
		ADC_DMARequestAfterLastTransferCmd(ADC2, ENABLE);
		
		DMA_InitStructure.DMA_Channel = DMA_Channel_1;  
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC2->DR;
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ADC2_Data;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
		DMA_InitStructure.DMA_BufferSize = ADC_BUFFSIZE;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		DMA_Init(DMA2_Stream3, &DMA_InitStructure);
		DMA_Cmd(DMA2_Stream3, ENABLE);
		ADC_DMACmd(ADC2, ENABLE);
		
		NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	
		DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE);
		
		
	}
	
void DMA2_Stream0_IRQHandler(void)
{
  /* Clear the DMA1_Stream2 TCIF2 pending bit */
  DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC);
	up[i] = (double)ADC1_Data[0]*3.3/0x0FFF;
	down[i]=(double)ADC1_Data[1]*3.3/0x0FFF;
	i++;
	if(i > 9)
		{
			i = 0;
			sumAbove = 0, sumBelow = 0;
			for (int h = 0; h<10; h++)
				{
					sumAbove+=up[h];
					sumBelow+=down[h];
					}
			above = sumAbove/10;
			below = sumBelow/10;
			flag = 1;
			DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, DISABLE);
			}
	else
			DMA_Cmd(DMA2_Stream0, ENABLE);
	
	DMA2_Stream0->NDTR = ADC_BUFFSIZE;
}

void DMA2_Stream3_IRQHandler(void)
{
   /* Clear the DMA1_Stream2 TCIF2 pending bit */
  DMA_ClearITPendingBit(DMA2_Stream3, DMA_IT_TC);
	rgt[j] = (double)ADC2_Data[0]*3.3/0x0FFF;
	lf[j] = (double)ADC2_Data[1]*3.3/0x0FFF;
	j++;
	if(j > 9)
		{
			j = 0;
			sumRight = 0, sumLeft = 0;
			for (int h = 0; h<10; h++)
				{
					sumRight+=rgt[h];
					sumLeft+=lf[h];
					}
			right = sumRight/10;
			left = sumLeft/10;
			flag_2 = 1;
			DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, DISABLE);		
			}
		else 
			DMA_Cmd(DMA2_Stream3, ENABLE);
	
			DMA2_Stream3->NDTR = ADC_BUFFSIZE;		
}

void Deg2Pulse_Servo1(uint32_t b)
{
	a1 = 0.54*b+20;
	TIM9->CCR1 = a1;
}	

void Deg2Pulse_Servo2(uint32_t b)
{
	a2 = 0.54*b+20;
	TIM12->CCR1 = a2;
}

