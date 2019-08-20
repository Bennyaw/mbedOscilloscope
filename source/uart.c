// LPC1768:UART Programming

#include "lpc17xx.h"
#include <stdio.h>

#define WordLenght 		0x0u
#define DLAB					0x7u
#define En_FIFO				0x0u
#define Rst_RxFIFO		0x1u
#define Rst_TxFIFO		0x2u

#define SBIT_RDR			0x0u
#define SBIT_THRE			0x5u

// Function to initialize UART0 at specified baud rate

void uart_init(uint32_t baudrate) {
	uint32_t var_UartPclk_u32, var_Pclk_u32, var_RegValue_u32;
	
	// Select Pin Alternate Function
  LPC_PINCON->PINSEL0 |= (1<<4) | (1<<6); // Set pin sel to TXD0 and RXD0	
	
	// Setup UART0 FIFO Control Register
	LPC_UART0->FCR = (1<<En_FIFO) | (1<<Rst_RxFIFO) | (1<<Rst_TxFIFO); // Enable FIFO and reset Rx/Tx buffers
	
	// Setup UART0 Line Control Register
	LPC_UART0->LCR = (0x03<<WordLenght) | (1<<DLAB); 	// 8 bit data, 1 Stop bit, No Parity, Enable DLAB
	
	/*** Baud Rate Calculation:
	PCLKSELx registers contains the PCLK info for all the clock dependant peripherals.
	Bit6, Bit7 contain the Uart Clock(ie. UART_PCLK)information.
	The UART_PCLK and the actual Peripheral Clock(PCLK) is calculated as below.
	
	UART_PCLK			  PCLK
		0x00			SystemCoreClock/4
		0x01			SystemCoreClock
		0x02			SystemCoreClock/2
		0x03			SystemCoreClock/8
	
	LPC1768 generates the baud rate depending on the values of DLM,DLL
	Baudrate = PCLK/(16*(256*DLM+DLL)*(1+DivAddVal/MulVal)
	
	DivAddVal/MulVal==0
	Hence, DLL/DLM is calculate as below
	(256*DLL+DLM) = PCLK/(16*Baudrate).
	
	***/
	
	var_UartPclk_u32 = (LPC_SC->PCLKSEL0 >> 6) & 0x03;  // Read UART0 Clock Selection
	
	switch(var_UartPclk_u32) {
		case 0x0: var_Pclk_u32 = SystemCoreClock/4; break;
		case 0x1: var_Pclk_u32 = SystemCoreClock; break;
		case 0x2: var_Pclk_u32 = SystemCoreClock/2; break;
		case 0x3: var_Pclk_u32 = SystemCoreClock/8; break;
	}	
	var_RegValue_u32 = (var_Pclk_u32 / (16*baudrate));	
	LPC_UART0->DLL = var_RegValue_u32 & 0xff;	
	LPC_UART0->DLM = (var_RegValue_u32 >> 0x08) & 0xFF;	
		
	LPC_UART0->LCR ^= 1<<DLAB;		// Clear DLAB after setting DLL, DLM
}

int uart_Tx(char ch) {
	while ((LPC_UART0->LSR & (1<<SBIT_THRE)) == 0); // Wait for Previous Transmission
	LPC_UART0->THR = ch;		// Load data to be transmitted
	return (ch);
}
	
void uart_Tx_str(char *s) {
	while (*s)
		uart_Tx(*s++);				// Write character until end of string
}

char uart_Rx() {
	char ch;
	  while ((LPC_UART0->LSR & (1<<SBIT_RDR)) == 0); // Wait till data is received
		ch = LPC_UART0->RBR;	// Read received data
	return ch;
}	 
