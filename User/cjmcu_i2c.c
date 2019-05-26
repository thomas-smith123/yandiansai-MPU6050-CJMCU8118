/**
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

/*
	应用说明：
	在访问I2C设备前，请先调用 i2c_CheckDevice() 检测I2C设备是否正常，该函数会配置GPIO
*/

#include "stm32f10x.h"
#include "cjmcu_i2c.h"
#include "bsp_SysTick.h"
u8 BUF[12];
u8 temperature[2];
u8 Information[10];
u8 temp=0x5a;
u8 MeasureMode,Status,Error_ID;
void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
		/* 使能与 I2C有关的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );  

	 /* PC3-I2C_SCL、PC5-I2C_SDA*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10| GPIO_Pin_11; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	SCL_H;
	SDA_H;
}
void delay_1us(u8 x)//粗略延时,iic_40K
{
	u8 i=20;
	x=i*x;
	while(x--);
}
////////IIC起始函数//////////
/*
IIC起始:当SCL处于高电平期间，SDA由高电平变成低电平出现一个下降沿，然后SCL拉低
*/
u8 I2C_Start(void)
{
		SDA_H; 
		delay_1us(5);	//延时保证时钟频率低于40K，以便从机识别
		SCL_H;
	
		delay_1us(5);//延时保证时钟频率低于40K，以便从机识别
		if(!SDA_read) return 0;//SDA线为低电平则总线忙,退出
		SDA_L;   //SCL处于高电平的时候，SDA拉低
		delay_1us(5);
	  if(SDA_read) return 0;//SDA线为高电平则总线出错,退出
		SCL_L;
	  delay_1us(5);
	  return 1;
}
//**************************************
//IIC停止信号
/*
IIC停止:当SCL处于高电平期间，SDA由低电平变成高电平出现一个上升沿
*/
//**************************************
void I2C_Stop(void)
{
    SDA_L;
		SCL_L;
		delay_1us(5);
		SCL_H;
		delay_1us(5);
		SDA_H;//当SCL处于高电平期间，SDA由低电平变成高电平             //延时
}
//**************************************
//IIC发送应答信号
//入口参数:ack (0:ACK 1:NAK)
/*
应答：当从机接收到数据后，向主机发送一个低电平信号
先准备好SDA电平状态，在SCL高电平时，主机采样SDA
*/
//**************************************
void I2C_SendACK(u8 i)
{
    if(1==i)
			SDA_H;	             //准备好SDA电平状态，不应答
    else 
			SDA_L;  						//准备好SDA电平状态，应答 	
	  SCL_H;                    //拉高时钟线
    delay_1us(5);                 //延时
    SCL_L ;                  //拉低时钟线
    delay_1us(5);    
} 
///////等待从机应答////////
/*
当本机(主机)发送了一个数据后，等待从机应答
先释放SDA，让从机使用，然后采集SDA状态
*/
/////////////////
u8 I2C_WaitAck(void) 	 //返回为:=1有ACK,=0无ACK
{
	uint16_t i=0;
	SDA_H;	        //释放SDA
	SCL_H;         //SCL拉高进行采样
	while(SDA_read)//等待SDA拉低
	{
		i++;      //等待计数
		if(i==500)//超时跳出循环
		break;
	}
	if(SDA_read)//再次判断SDA是否拉低
	{
		SCL_L; 
		return RESET;//从机应答失败，返回0
	}
  delay_1us(5);//延时保证时钟频率低于40K，
	SCL_L;
	delay_1us(5); //延时保证时钟频率低于40K，
	return SET;//从机应答成功，返回1
}
//**************************************
//向IIC总线发送一个字节数据
/*
一个字节8bit,当SCL低电平时，准备好SDA，SCL高电平时，从机采样SDA
*/
//**************************************
void I2C_SendByte(u8 dat)
{
  u8 i;
	SCL_L;//SCL拉低，给SDA准备
  for (i=0; i<8; i++)         //8位计数器
  {
		if(dat&0x80)//SDA准备
		SDA_H;  
		else 
		SDA_L;
    SCL_H;                //拉高时钟，给从机采样
    delay_1us(5);        //延时保持IIC时钟频率，也是给从机采样有充足时间
    SCL_L;                //拉低时钟，给SDA准备
    delay_1us(5); 		  //延时保持IIC时钟频率
		dat <<= 1;          //移出数据的最高位  
  }					 
}
//void I2C_Send2Byte(u16 dat)
//{
//	u16 i;
//}
//**************************************
//从IIC总线接收一个字节数据
//**************************************
u8 I2C_RecvByte()
{
    u8 i;
    u8 dat = 0;
    SDA_H;//释放SDA，给从机使用
    delay_1us(1);         //延时给从机准备SDA时间            
    for (i=0; i<8; i++)         //8位计数器
    { 
		  dat <<= 1;
			
      SCL_H;                //拉高时钟线，采样从机SDA
     
		  if(SDA_read) //读数据    
		   dat |=0x01;      
       delay_1us(5);     //延时保持IIC时钟频率		
       SCL_L;           //拉低时钟线，处理接收到的数据
       delay_1us(5);   //延时给从机准备SDA时间
    } 
    return dat;
}
//**************************************
//向IIC设备写入一个字节数据
//**************************************
u8 Single_WriteI2C_byte(u8 Slave_Address,u8 REG_Address,u8 data)
{
	  if(I2C_Start()==0)  //起始信号
		{I2C_Stop(); return RESET;}           

    I2C_SendByte(Slave_Address);   //发送设备地址+写信号
 	  if(!I2C_WaitAck()){I2C_Stop(); return RESET;}
   
		I2C_SendByte(REG_Address);    //内部寄存器地址，
 	  if(!I2C_WaitAck()){I2C_Stop(); return RESET;}
   
		I2C_SendByte(data);       //内部寄存器数据，
	  if(!I2C_WaitAck()){I2C_Stop(); return RESET;}
		
		I2C_Stop();   //发送停止信号
		
		return SET;
}

u8 Single_MWriteI2C_byte(u8 Slave_Address,u8 REG_Address,u8 const *data,u8 length)
{
	  if(I2C_Start()==0)  //起始信号
		{I2C_Stop(); return RESET;}           

    I2C_SendByte(Slave_Address);   //发送设备地址+写信号
 	  if(!I2C_WaitAck()){I2C_Stop(); return RESET;}
   
		I2C_SendByte(REG_Address);    //内部寄存器地址，
 	  if(!I2C_WaitAck()){I2C_Stop(); return RESET;}
   
	while(length)
	{
		I2C_SendByte(*data++);       //内部寄存器数据，
	   if(!I2C_WaitAck()){I2C_Stop(); return RESET;}           //应答
		length--;
	}
	//	I2C_SendByte(*data);       //内部寄存器数据，
 	//	if(!I2C_WaitAck()){I2C_Stop(); return RESET;}
		I2C_Stop();   //发送停止信号		
		return SET;
}

//**************************************
//从IIC设备读取一个字节数据
//**************************************
u8 Single_ReadI2C(u8 Slave_Address,u8 REG_Address,u8 *REG_data,u8 length)
{
 if(I2C_Start()==0)  //起始信号
		{I2C_Stop(); return RESET;}          
	 
	I2C_SendByte(Slave_Address);    //发送设备地址+写信号
 	if(!I2C_WaitAck()){I2C_Stop(); return RESET;} 
	
	I2C_SendByte(REG_Address);     //发送存储单元地址
 	if(!I2C_WaitAck()){I2C_Stop(); return RESET;} 
	
	if(I2C_Start()==0)  //起始信号
			{I2C_Stop(); return RESET;}            

	I2C_SendByte(Slave_Address+1);  //发送设备地址+读信号
 	if(!I2C_WaitAck()){I2C_Stop(); return RESET;}
	
	while(length-1)
	{
		*REG_data++=I2C_RecvByte();       //读出寄存器数据
		I2C_SendACK(0);               //应答
		length--;
	}
	*REG_data=I2C_RecvByte();  
	I2C_SendACK(1);     //发送停止传输信号
	I2C_Stop();                    //停止信号
	return SET;
}
u8 HDC1080_read(u8 Slave_Address,u8 REG_Address,u8 *REG_data,u8 length)
{
 if(I2C_Start()==0)  //起始信号
		{I2C_Stop(); return RESET;}          
	 
	I2C_SendByte(Slave_Address);    //发送设备地址+写信号
 	if(!I2C_WaitAck()){I2C_Stop(); return RESET;} 
	
	I2C_SendByte(REG_Address);     //发送存储单元地址
 	if(!I2C_WaitAck()){I2C_Stop(); return RESET;} 
	
	if(I2C_Start()==0)  //起始信号
			{I2C_Stop(); return RESET;}            

	I2C_SendByte(Slave_Address+1);  //发送设备地址+读信号
			/********************************************************************/
//		Delay_us(650);
 	if(!I2C_WaitAck()){I2C_Stop(); return RESET;}
	
	while(length-1)
	{
		*REG_data++=I2C_RecvByte();       //读出寄存器数据
		I2C_SendACK(0);               //应答
		length--;
	}
	*REG_data=I2C_RecvByte();  
	I2C_SendACK(1);     //发送停止传输信号
	I2C_Stop();                    //停止信号
	return SET;
}
u8 HDC1080_trigger(u8 Slave_Address,u8 REG_Address,u8 *REG_data1,u8 length)
{
	u8 length3=2;
	u8 i=100;
//	u8 data9[2]={0x30,0x60};
 if(I2C_Start()==0)  //起始信号
		{I2C_Stop(); return RESET;}          
	 
	I2C_SendByte(Slave_Address);    //发送设备地址+写信号
 	if(!I2C_WaitAck()){I2C_Stop(); return RESET;} 
	
	I2C_SendByte(REG_Address);     //发送存储单元地址
 	if(!I2C_WaitAck()){I2C_Stop(); return RESET;} 
	while(i--);
	I2C_Stop();
	
	Delay_us(1300);
	////////////////
	 if(I2C_Start()==0)  //起始信号
		{I2C_Stop(); return RESET;}          
	 
	I2C_SendByte(Slave_Address+1);    //发送设备地址+写信号
 	if(!I2C_WaitAck()){I2C_Stop(); return RESET;} 
		
	while(length-1)
	{
		*REG_data1++=I2C_RecvByte();       //读出寄存器数据
		I2C_SendACK(0);               //应答
		length--;
	}
	*REG_data1=I2C_RecvByte();  
	I2C_SendACK(1);     //发送停止传输信号
	I2C_Stop();                    //停止信号
	return SET;
}
void cjmcu_operate(void)
{
	u8 device_id[2];
	u8 manufacturer_ID[2];
	u8 HDC1080_config[2]={0x10,0x00};
	u8 HDC1080_config_read[2];
	u8 Receive[4];
	u8 i=100;
	Single_ReadI2C(CCS811_Add,0x20,Information,1);  //Read CCS's information  ,ID
	Single_ReadI2C(CCS811_Add,0x23,&Information[1],2);	//FW_Boot_Version  
	Single_ReadI2C(CCS811_Add,0x24,&Information[3],2); 	//FW_App_Version
	Single_WriteI2C_byte(CCS811_Add,0xF3,0xF0);	//Application Verify
	Single_ReadI2C(CCS811_Add,0x00,&Status,1);	   //Firstly the status register is read and the APP_VALID flag is checked.
	if(Status&0x10)	 Single_MWriteI2C_byte(CCS811_Add,0xF4,&temp,0);	//Used to transition the CCS811 state from boot to application mode, a write with no data is required.
	Single_ReadI2C(CCS811_Add,0x00,&Status,1);
	Single_ReadI2C(CCS811_Add,0x01,&MeasureMode,1);
	Single_WriteI2C_byte(CCS811_Add,0x01,0x10); //Write Measure Mode Register,sensor measurement every second,no interrupt
	/**************************************************/
	Single_MWriteI2C_byte(0x80,0x02,HDC1080_config,2);//normal operation,temperatureand humidity are acquired in sequence
	while(i--);
	Single_ReadI2C(0x80,0x02,HDC1080_config_read,2);
	printf("HDC1080 config read=0x%x%x\r\n",HDC1080_config_read[0],HDC1080_config_read[1]);
	
	//14bit,14bit
	while(1)
	{
		Single_ReadI2C(0x80,0xff,device_id,2);
		Single_ReadI2C(0x80,0xfe,manufacturer_ID,2);
	//Single_MWriteI2C_byte(CCS811_Add,0x02,&HDC1080_config,2);
		printf("Manufacturer ID=0x%x%x\r\n",manufacturer_ID[0],manufacturer_ID[1]);
		printf("Device ID=0x%x%x\r\n",device_id[0],device_id[1]);
		if(device_id[0]==0x10&&device_id[1]==0x50&&manufacturer_ID[0]==0x54&&manufacturer_ID[1]==0x49)
			break;
	}
//	Single_MWriteI2C_byte(0x80,0x00,HDC1080_config,2);
//	HDC1080_trigger(0x80,0x00,Receive,4);
}
void cjmcu_readall(void)
{
	u16 a=0;
	u8 i;
	u8 temp[2];
	u8 humidity[2];
	u8 Receive[4];
	float x,y,x1,y1;
	ccs811_measurement_t CCS;
	Single_ReadI2C(CCS811_Add,0x00,&Status,1);
	Single_ReadI2C(CCS811_Add,0xE0,&Error_ID,1);
	Single_ReadI2C(CCS811_Add,0x02,BUF,8);
	Single_ReadI2C(CCS811_Add,0x20,Information,1);  //Read CCS's information  ,ID
	//	   OFF_CS(); 
	CCS.eco2= (u16)BUF[0]*256+BUF[1];
	CCS.tvoc= (u16)BUF[2]*256+BUF[3];
	printf("eco2=%d  tvoc=%d\r\n",CCS.eco2,CCS.tvoc);
	HDC1080_read(0x80,Temper,temp,2);//wrong
//	HDC1080_read(0x80,Humidity,humidity,2);//wrong
	//			printf("temperature=%x\r\n",a);
	HDC1080_trigger(0x80,Temper,Receive,4);
	
	
	x=Receive[0]*256+Receive[1];
	y=Receive[2]*256+Receive[3];
	x1=x/65536*165-45;
	y1=y/65536;
	printf("Tem&Humidity=%f   %f\r\n",x1,y1);
	Information[0]=0;

}