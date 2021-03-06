#include "mpu6050.h"
#include "bsp_usart1.h"
#include "bsp_i2c.h"
	short Accel[3];
	short Gyro[3];
	short Temp;
void PMU6050_WriteReg(u8 reg_add,u8 reg_dat)
{
	i2c1_Start();
	i2c1_SendByte(MPU6050_SLAVE_ADDRESS);
	i2c1_WaitAck();
	i2c1_SendByte(reg_add);
	i2c1_WaitAck();
	i2c1_SendByte(reg_dat);
	i2c1_WaitAck();
	i2c1_Stop();
}

void PMU6050_ReadData(u8 reg_add,unsigned char*Read,u8 num)
{
	unsigned char i;
	
	i2c1_Start();
	i2c1_SendByte(MPU6050_SLAVE_ADDRESS);
	i2c1_WaitAck();
	i2c1_SendByte(reg_add);
	i2c1_WaitAck();
	
	i2c1_Start();
	i2c1_SendByte(MPU6050_SLAVE_ADDRESS+1);
	i2c1_WaitAck();
	
	for(i=0;i<(num-1);i++){
		*Read=i2c1_ReadByte(1);
		Read++;
	}
	*Read=i2c1_ReadByte(0);
	i2c1_Stop();
}

void MPU6050_Init(void)
{
  int i=0,j=0;
  //在初始化之前要延时一段时间，若没有延时，则断电后再上电数据可能会出错
  for(i=0;i<1000;i++)
  {
    for(j=0;j<1000;j++)
    {
      ;
    }
  }
	PMU6050_WriteReg(MPU6050_RA_PWR_MGMT_1, 0x00);	    //解除休眠状态
	PMU6050_WriteReg(MPU6050_RA_SMPLRT_DIV , 0x07);	    //陀螺仪采样率，1KHz
	PMU6050_WriteReg(MPU6050_RA_CONFIG , 0x06);	        //低通滤波器的设置，截止频率是1K，带宽是5K
	PMU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG , 0x00);	  //配置加速度传感器工作在2G模式，不自检
	PMU6050_WriteReg(MPU6050_RA_GYRO_CONFIG, 0x18);     //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
}
void MPU6050ReadID(void)
{
	unsigned char Re = 0;
    PMU6050_ReadData(MPU6050_RA_WHO_AM_I,&Re,1);    //读器件地址
     printf("%d\r\n",Re);
}
void MPU6050ReadAcc(short *accData)
{
    u8 buf[6];
    PMU6050_ReadData(MPU6050_ACC_OUT, buf, 6);
    accData[0] = (buf[0] << 8) | buf[1];
    accData[1] = (buf[2] << 8) | buf[3];
    accData[2] = (buf[4] << 8) | buf[5];
}
void MPU6050ReadGyro(short *gyroData)
{
    u8 buf[6];
    PMU6050_ReadData(MPU6050_GYRO_OUT,buf,6);
    gyroData[0] = (buf[0] << 8) | buf[1];
    gyroData[1] = (buf[2] << 8) | buf[3];
    gyroData[2] = (buf[4] << 8) | buf[5];
}

void MPU6050ReadTemp(short *tempData)
{
	u8 buf[2];
    PMU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2);     //读取温度值
    *tempData = (buf[0] << 8) | buf[1];
}

void MPU6050_ReturnTemp(short*Temperature)
{
	short temp3;
	u8 buf[2];
	
	PMU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2);     //读取温度值
  temp3= (buf[0] << 8) | buf[1];
	*Temperature=(((double) (temp3 + 13200)) / 280)-13;
}
void MPU6050_ReadAll(void)
{
				MPU6050ReadAcc(Accel);			
			printf("\r\n加速度： %8d%8d%8d    ",Accel[0],Accel[1],Accel[2]);
			MPU6050ReadGyro(Gyro);
			printf("陀螺仪： %8d%8d%8d    ",Gyro[0],Gyro[1],Gyro[2]);
      
			MPU6050_ReturnTemp(&Temp);
			printf("%d\r\n",Temp);
	printf("温度： %d\r\n",Temp);

}