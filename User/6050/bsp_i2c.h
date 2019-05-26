#ifndef _BSP_I2C_H
#define _BSP_I2C_H
#include "stm32f10x.h"
#include <inttypes.h>

#define I2C_WR	0		/* Ð´¿ØÖÆbit */
#define I2C_RD	1		/* ¶Á¿ØÖÆbit */

void i2c1_Start(void);
void i2c1_Stop(void);
void i2c1_SendByte(uint8_t _ucByte);
uint8_t i2c1_ReadByte(u8 ack);
uint8_t i2c1_WaitAck(void);
void i2c1_Ack(void);
void i2c1_NAck(void);
uint8_t i2c1_CheckDevice(uint8_t _Address);
void i2c1_GPIO_Config(void);

#endif
