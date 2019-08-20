/*-------------------------------------------------------------------------
// Mini CMSIS RTX Project
// Name:
// Date:
// Description:
*------------------------------------------------------------------------*/
 
/*------------------------------------------------------------------------
// 1. Include File
*------------------------------------------------------------------------*/
#include "LPC17xx.h"						// Include this as part of CMSIS
#include "cmsis_os.h"						// CMSIS_OS include file
//Add necessary include files here
#include <stdio.h>
#include "lcd.h"
#include "uart.h"
#include "adc.h"

/*------------------------------------------------------------------------
// 2. Signal Declaration
*------------------------------------------------------------------------*/
uint8_t PUTCHAR_SERIAL = 1;			// If 1, used to display on UART
uint32_t adc_result;


/*-------------------------------------------------------------------------
// 3. Thread function prototype Declaration
*------------------------------------------------------------------------*/

void lcd_thread (const void *);
void adc_thread (const void *);

/*-------------------------------------------------------------------------
// 4. Define the thread handles and thread parameters
*------------------------------------------------------------------------*/	
osThreadId lcd_id, adc_id;

osThreadDef(lcd_thread, osPriorityNormal, 1, 0);
osThreadDef(adc_thread, osPriorityHigh, 1, 0);

/*-------------------------------------------------------------------------
// 5. Create and define Message Queue.
*------------------------------------------------------------------------*/	
osMessageQId msg_id;
osMessageQDef (msg, 10, uint32_t);


float calculateVoltFromAdcVal(uint32_t adcVal){
	float voltage = 0;
	voltage = (float)(adcVal) * (float)(0.0008);
	return voltage;
}



/*-------------------------------------------------------------------------
// 6. ADC interrupt handler. On Conversion set ADC thread signal
*------------------------------------------------------------------------*/
void ADC_IRQHandler (void)
{ 
	LPC_GPIO1->FIODIR2 |= 0xb4;	// // Set the FIODIR = Output (to drive the LED)
	osSignalSet(adc_id, 0x01);
	LPC_ADC->ADGDR = LPC_ADC->ADGDR; // clr
		osDelay(1000);
	LPC_GPIO1->FIOSET2 = 0x04; 
	LPC_GPIO1->FIOCLR2 = 0x10;
}




/*----------------------------------------------------------------------------
// 7. Thread to Service the ADC interrupt. Print the ADC value to UART
*---------------------------------------------------------------------------*/

void adc_thread(const void *arg){
	 LPC_GPIO1->FIODIR2 |= 0xb4;	// // Set the FIODIR = Output (to drive the LED)
	adc_init( );						// Initialize ADC
	
	LPC_ADC->ADCR |= 1U << SBIT_START ; 	
	
	while(1){
		osSignalWait(0x01, osWaitForever);
		osDelay(1000);
		LPC_GPIO1->FIOSET2 = 0x10; 
		LPC_GPIO1->FIOCLR2 = 0x04; 
		adc_result = (LPC_ADC->ADDR5 >> SBIT_RESULT) & 0xfff; 
		osMessagePut(msg_id, adc_result, osWaitForever);
		LPC_ADC->ADCR |= 1U << SBIT_START ; 						// Start ADC conversion	
	  osDelay(100);
	}
}
 /*----------------------------------------------------------------------------
// 8. Thread to display on LCD
*---------------------------------------------------------------------------*/
void lcd_thread(const void *args){
	float volts;
	osEvent  result;
	char str[20];
	
	lcd_init( );	// Initialise LCD
			
	for (;;) {
		result = osMessageGet(msg_id, osWaitForever);
		lcd_line_clr(1);
		lcd_line_clr(2);
		lcd_line_clr(3);
		lcd_line_clr(4);						// Point & Clear line 4
		volts = calculateVoltFromAdcVal(result.value.v);
		sprintf((char *)str, "ADC value is: %d",	result.value.v);
		lcd_disp(4, 0, str);
		osDelay(1000);										// Wait 1 second
	}
}



/*-------------------------------------------------------------------------
// 9.  Main Thread function
*------------------------------------------------------------------------*/
int main( ) {	 
		
	osKernelInitialize ();                    							// Initialize CMSIS-RTOS
	
	/*-------------------------------------------------------------------------
	// 10.  Create Thread, Message Queue, Pool, Mail etc 
	*------------------------------------------------------------------------*/
																													

	lcd_id = osThreadCreate(osThread(lcd_thread), NULL);
	adc_id = osThreadCreate(osThread(adc_thread), NULL);
	
	msg_id = osMessageCreate(osMessageQ(msg), NULL);
	osKernelStart ();                         							// Start thread execution
	for(;;); 
} 	 

