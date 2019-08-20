#include <lpc17xx.h>
#include  "adc.h"

void adc_init( void ) {
	LPC_SC->PCONP |= (1U << 12U); // Enable CLOCK for internal ADC controller
  LPC_ADC->ADCR = ((1<<SBIT_PDN) | (10<<SBIT_CLCKDIV));  //Set the clock and Power ON ADC module
  
	// Select the P1.31 Pin Function Select to ADC5 
	LPC_PINCON->PINSEL3 |= ( 3UL << 30 ); 	
	LPC_ADC->ADCR |= (1 << AD5);			// Select ADC5, Connected to MBED P20(POT 2) P1.31
	
	// Interrupt Setting for ADC
	//LPC_ADC->ADINTEN |= (1<<5);				// Set Interrupt Enable for Completion of Conversion for ADC5
	NVIC_SetPriority(ADC_IRQn, 0x1F);
	NVIC_EnableIRQ(ADC_IRQn);					// Enable IDC Interrupt
}


