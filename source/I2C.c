#include "LPC17xx.h"
#include "I2C.h"

#define SSD1780_address 0x3D
#define address_write (SSD1780_address<<1)|WRITE
#define address_read (SSD1780_address<<1)|READ
#define WRITE 0
#define READ 1


void I2C2_Initialise(void){
	LPC_SC->PCONP |= 1 <<26;//enable POWER I2C2
	LPC_SC->PCLKSEL1 |= 1<<20; //pclk = cclk
	LPC_PINCON->PINSEL0 |= 0x01<<20; //Pin P0.10 allocated to alternate function 2
	LPC_PINCON->PINSEL0 |= 0x01<<22; //Pin P0.11 allocated to alternate function 2
	LPC_GPIO0->FIODIR |= 1<<10;	//Bit P0.10 an output
	LPC_GPIO0->FIODIR |= 1<<11;	//Bit P0.11 an output
	LPC_PINCON->PINMODE0 &= ~(3<<20);	 
	LPC_PINCON->PINMODE0 |= (2<<20);	//P0.10 has no pull up/down resistor
	LPC_PINCON->PINMODE0 &= ~(3<<22);	 
	LPC_PINCON->PINMODE0 |= (2<<22);	//P0.10 has no pull up/down resistor
	LPC_PINCON->PINMODE_OD0 |= 1<<10;	//Bit P0.10 is open drain
	LPC_PINCON->PINMODE_OD0 |= 1<<11;	//Bit P0.11 is open drain
}
void I2C2_enable(void) 
{	
	LPC_I2C2->I2CONSET |= 1<< 6;	//enable I2C2
}

void I2C2_startTransmit(void){
	LPC_I2C2->I2CONSET |= 1<<5;
	WAIT_SI;
	LPC_I2C2->I2DAT = address_write;	//start bit transmitted,load slave address
	CLEAR_SI;
	}