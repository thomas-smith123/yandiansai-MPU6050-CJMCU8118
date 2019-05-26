/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ��3.5.0�汾�⽨�Ĺ���ģ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
//#include "cjmcu.h"
#include "cjmcu_i2c.h"

#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "bsp_SysTick.h"
#include "GPS.h"
#include "bsp_usart1.h"
#include "bsp_usart3.h"
#include "mpu6050.h"
#include "bsp_i2c.h"
//#include "cjmcu����i2c.h"

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
#define TASK_ENABLE 0
//extern unsigned int Task_Delay[NumOfTask];
//extern __IO uint16_t ADC_ConvertedValue;

// �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
//float ADC_ConvertedValueLocal;      
int main(void)
{
//	u8 id[2];
	SysTick_Init();
	/* ����ͨ�ų�ʼ�� */
	USART1_Config();
//	NVIC_Configuration();
	//I2C��ʼ��
	uart_init(9600);
	//MPU6050��ʼ��
	i2c1_GPIO_Config();
	I2C_GPIO_Config();
	MPU6050_Init();

//	I2C2_Config();

//	Delay_us(50000);
	cjmcu_operate();
//	Delay_us(1000);
//	cjmcu_operate();
	Delay_us(1000);
	printf("Initiated completed\r\n");
	while(1)
	{

		MPU6050_ReadAll();

		Delay_us(50000);//Delay_us(50000);
		cjmcu_readall();
//		cjmuc_ReadAll();

//		Delay_us(50000);//Delay_us(50000);
//		cjmuc_ReadHDC1080();
//		ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096*3.3; // ��ȡת����ADֵ
	
//		printf("\r\n The current AD value = 0x%04X \r\n", ADC_ConvertedValue); 
	Delay_us(50000);
		parseGpsBuffer();
		printGpsBuffer();
		Delay_us(50000);
	}
	  /* add your code here ^_^. */
}

/*********************************************END OF FILE**********************/

