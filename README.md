# yandiansai-MPU6050-CJMCU8118
Based on STM32F103ZET6, using MPU6050&CJMCU-8118, including the use of HDC1080, work on very well.
All devices were connected in a same i2c bus->PB6(SCL),PB7(SDA). USART1 sends message to PC, while USART2 receives message from GPS.
