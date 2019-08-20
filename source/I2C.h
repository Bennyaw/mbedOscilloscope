#ifndef __I2C_H
#define __I2C_H

#include "LPC17xx.h"

#define SSD1780_address 0x3D

#define  WAIT_SI		while (!(LPC_I2C2->I2CONSET & (1<<3)))
#define  CLEAR_SI		LPC_I2C2->I2CONCLR = 1<< 3

#define  ACTION     LPC_GPIO0->FIOSET0 = 1
#define  ACTION_E   LPC_GPIO0->FIOCLR0 = 1
#define  ADDRESS    LPC_GPIO0->FIOSET0 = 2
#define  ADDRESS_E  LPC_GPIO0->FIOCLR0 = 2

void I2C2_Initialise(void);
void I2C2_enable(void);
void I2C2_startTransmit(void);

#endif
