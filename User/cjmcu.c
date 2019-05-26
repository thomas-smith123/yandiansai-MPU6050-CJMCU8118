 #include "stm32f10x.h"
 #include "cjmcu_i2c.h"
 #include "cjmcu.h"
 u8 BUF[12];
u8 Information[10];
u8 temp=0x5a;
u8 MeasureMode,Status,Error_ID;
u8 temperature[4];
u8 HDC1080_config[2]={0x36,0x00};
typedef struct {
u16 eco2;
u16 tvoc;
u8 status;
u8 error_id;
u16 raw_data;
} ccs811_measurement_t;
ccs811_measurement_t CCS;
void Single_ReadI2C(u8 Slave_Address,u8 REG_Address,u8 *REG_data,u8 length)
{
		unsigned char i;
	
	i2c2_Start();
	i2c2_SendByte(Slave_Address);
	i2c2_WaitAck();
	i2c2_SendByte(REG_Address);
	i2c2_WaitAck();
	
	i2c2_Start();
	i2c2_SendByte(Slave_Address);
	i2c2_WaitAck();
	
	for(i=0;i<(length-1);i++){
		*REG_data=i2c2_ReadByte(1);
		REG_data++;
	}
	*REG_data=i2c2_ReadByte(0);
	i2c2_Stop();

}
void Single_WriteI2C_byte(u8 Slave_Address,u8 REG_Address,u8 data)
{
	i2c2_Start();
	i2c2_SendByte(Slave_Address);
	i2c2_WaitAck();
	i2c2_SendByte(REG_Address);
	i2c2_WaitAck();
	i2c2_SendByte(data);
	i2c2_WaitAck();
	i2c2_Stop();
}
void Single_MWriteI2C_byte(u8 Slave_Address,u8 REG_Address,u8 const *data,u8 length)
{
	  i2c2_Start();         

    i2c2_SendByte(Slave_Address);
 	  i2c2_WaitAck();
   
		i2c2_SendByte(REG_Address);
		i2c2_WaitAck();
   
	while(length)
	{
		i2c2_SendByte(*data++);
		i2c2_WaitAck();		length--;
		}
	//	I2C_SendByte(*data);       //内部寄存器数据，
 	//	if(!I2C_WaitAck()){I2C_Stop(); return RESET;}
		i2c2_Stop();
}
void cjmcu_operate(void)
{
		Single_ReadI2C(CCS811_Add,0x20,Information,1);  //Read CCS's information  ,ID
    Single_ReadI2C(CCS811_Add,0x23,&Information[1],2);	//FW_Boot_Version  
	Single_ReadI2C(CCS811_Add,0x24,&Information[3],2); 	//FW_App_Version
	Single_WriteI2C_byte(CCS811_Add,0xF3,0xF0);	//Application Verify
  	Single_ReadI2C(CCS811_Add,0x00,&Status,1);	   //Firstly the status register is read and the APP_VALID flag is checked.
	if(Status&0x10)	 Single_MWriteI2C_byte(CCS811_Add,0xF4,&temp,0);	//Used to transition the CCS811 state from boot to application mode, a write with no data is required.
	Single_ReadI2C(CCS811_Add,0x00,&Status,1);
	Single_ReadI2C(CCS811_Add,0x01,&MeasureMode,1);
    Single_WriteI2C_byte(CCS811_Add,0x01,0x10); //Write Measure Mode Register,sensor measurement every second,no interrupt
//	Single_MWriteI2C_byte(0x40,0x02,HDC1080_config,2);
}
void cjmcu_readall(void)
{
	u16 a=0;
	u8 i;
		   Single_ReadI2C(CCS811_Add,0x00,&Status,1);
	   Single_ReadI2C(CCS811_Add,0xE0,&Error_ID,1);
	   Single_ReadI2C(CCS811_Add,0x02,BUF,8);
	   	Single_ReadI2C(CCS811_Add,0x20,Information,1);  //Read CCS's information  ,ID
//			Single_ReadI2C(0x40,0x00,temperature,2);
	for(i=0;i<2;i++)
	{
		a|=temperature[i];
		a<<8;
	}
//	   OFF_CS(); 
	   CCS.eco2= (u16)BUF[0]*256+BUF[1];
	   CCS.tvoc= (u16)BUF[2]*256+BUF[3];
	   printf("eco2=%d  tvoc=%d\r\n",CCS.eco2,CCS.tvoc);
	   
	   printf("ID=%d\r\n",Information[0]);
//			printf("temperature=%x\r\n",a);
	   Information[0]=0;

}
