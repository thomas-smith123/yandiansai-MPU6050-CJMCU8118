/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   用3.5.0版本库建的工程模板
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
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
//#include "cjmcu——i2c.h"

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
#define TASK_ENABLE 0
//extern unsigned int Task_Delay[NumOfTask];
//extern __IO uint16_t ADC_ConvertedValue;

// 局部变量，用于保存转换计算后的电压值 	 
//float ADC_ConvertedValueLocal;      
int main(void)
{
//	u8 id[2];
	SysTick_Init();
	/* 串口通信初始化 */
	USART1_Config();
//	NVIC_Configuration();
	//I2C初始化
	uart_init(9600);
	//MPU6050初始化
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
//		ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096*3.3; // 读取转换的AD值
	
//		printf("\r\n The current AD value = 0x%04X \r\n", ADC_ConvertedValue); 
	Delay_us(50000);
		parseGpsBuffer();
		printGpsBuffer();
		Delay_us(50000);
	}
	  /* add your code here ^_^. */
}

/*********************************************END OF FILE**********************/

