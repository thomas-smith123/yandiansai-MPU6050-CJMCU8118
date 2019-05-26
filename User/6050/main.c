/**
  ******************************************************************************
  * @file    main.c
  * @author  LYC
  * @version V1.0
  * @date    2014-04-22
  * @brief   ��ֲҰ�������I2C
  ******************************************************************************
  * @attention
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "bsp_SysTick.h"
#include "bsp_led.h"
#include "bsp_usart1.h"
#include "mpu6050.h"
#include "bsp_i2c.h"

#define TASK_ENABLE 0
extern unsigned int Task_Delay[NumOfTask];
/*
 * t : ��ʱʱ�� 
 * Ticks : ���ٸ�ʱ�����ڲ���һ���ж� 
 * f : ʱ��Ƶ�� 72000000
 * t = Ticks * 1/f = (72000000/100000) * (1/72000000) = 10us 
 */ 

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
	short Accel[3];
	short Gyro[3];
	short Temp;
  
  //��ʼ��systick
	SysTick_Init();
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();
	/* ����ͨ�ų�ʼ�� */
	USART1_Config();
	NVIC_Configuration();
	//I2C��ʼ��
	i2c_GPIO_Config();
  //MPU6050��ʼ��
	MPU6050_Init();

	while(1)
	{
		if(Task_Delay[0]==TASK_ENABLE)
		{
			LED1_TOGGLE;
			Task_Delay[0]=1000;
		}
		
		if(Task_Delay[1]==0)
		{
			MPU6050ReadAcc(Accel);			
			printf("\r\n���ٶȣ� %8d%8d%8d    ",Accel[0],Accel[1],Accel[2]);
			MPU6050ReadGyro(Gyro);
			printf("�����ǣ� %8d%8d%8d    ",Gyro[0],Gyro[1],Gyro[2]);
      
			MPU6050_ReturnTemp(&Temp);
			printf("�¶ȣ� %d",Temp);
      
			Task_Delay[1]=100;//��ֵÿ1ms���1������0�ſ������½����������ִ�е�������100ms
		}

		//*************************************	��������������ĸ�ʽ************************************//
//		if(Task_Delay[i]==0)
//		{
//			Task(i);
//			Task_Delay[i]=;
//		}

	}     
}

/*********************************************END OF FILE**********************/