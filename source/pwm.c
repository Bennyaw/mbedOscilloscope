/****************************************************************************
 *   03/02/2016  // dd/mm/yyyy
 *   Description:
 *     This file contains PWM code example which include PWM initialization,
 *     PWM interrupt handler, and APIs for PWM access.
****************************************************************************/
#include "lpc17xx.h"
#include "pwm.h"

volatile uint32_t match_counter0, match_counter1;

/******************************************************************************
** Function name:		PWM1_IRQHandler
** Descriptions:		PWM1 interrupt handler
**									For now, it only deals with PWM1 match 0
** Parameters:			None
** Returned value:	None
******************************************************************************/
void PWM1_IRQHandler (void) {
  uint32_t regVal;

  regVal = LPC_PWM1->IR;
  if ( regVal & MR0_INT ) {
		match_counter1++;
  }
  LPC_PWM1->IR |= regVal;		// clear interrupt flag on match 0 
  return;
}

/******************************************************************************
** Function name:		PWM_Init
** Descriptions:		PWM initialization, setup all GPIOs to PWM0~6,
**									reset counter, all latches are enabled, interrupt
**									on PWMMR0, install PWM interrupt to the VIC table.
** parameters:			PortNum.
** Returned value:	True or False, if VIC table is full, return false
******************************************************************************/
uint32_t PWM_Init(uint32_t PWMPortNum) {
	
	match_counter1 = 0;
	
	LPC_SC->PCONP |= (1 << 6);	// Make sure PWM is enabled

	switch (PWMPortNum) {
		case 1:
			LPC_PINCON->PINSEL4 |= (1 << 0);
		break;
		case 2:
			LPC_PINCON->PINSEL4 |= (1 << 2);
		break;
		case 3:
			LPC_PINCON->PINSEL4 |= (1 << 4);
		break;
		case 4:
			LPC_PINCON->PINSEL4 |= (1 << 6);
		break;
		case 5:
			LPC_PINCON->PINSEL4 |= (1 << 8);
		break;
		case 6:
			LPC_PINCON->PINSEL4 |= (1 << 10);
		break;
	}

	LPC_PWM1->TCR = TCR_RESET;	// Counter Synchronously Reset with PCLK 
	LPC_PWM1->PR  = 0;					// TC incremented every PR+1 cycle  of PCLK
	LPC_PWM1->MCR = PWMMR0I;		// interrupt on PWMMR0
	
	// all PWM latch enabled 
	LPC_PWM1->LER = LER0_EN | LER1_EN | LER2_EN | LER3_EN | LER4_EN | LER5_EN | LER6_EN;
  
  NVIC_EnableIRQ(PWM1_IRQn);	//Enable PWM Interrupt 
  return (1);
}

/******************************************************************************
** Function name:		PWM_Frequency
** Descriptions:		Set the PWM frequency in Hertz**									
** parameters:			PWMFrequency(Hz)
** Returned value:	None
******************************************************************************/
void PWM_Frequency(uint32_t PWMFrequency) {
	
	uint32_t cycle, PWM1PclkSel, PWM1Pclk;
	
	PWM1PclkSel = (LPC_SC->PCLKSEL0 >> 12) & 0x03;  // Read UART0 Clock Selection
	switch(PWM1PclkSel) {
		case 0x0: PWM1Pclk = SystemCoreClock/4; break;
		case 0x1: PWM1Pclk = SystemCoreClock; break;
		case 0x2: PWM1Pclk = SystemCoreClock/2; break;
		case 0x3: PWM1Pclk = SystemCoreClock/8; break;
	}	
	cycle = (PWM1Pclk/PWMFrequency);
	LPC_PWM1->MR0 = cycle;			// set PWM cycle 	
}

/******************************************************************************
** Function name:		PWM_DutyCycle
** Descriptions:		PWM cycle setup
** parameters:			Port Number and 
**									DutyCycle(value 0.0-->1.0)
** Returned value:	None
******************************************************************************/

void PWM_DutyCycle( uint32_t PWMPortNum, float DutyCycle ) {
	
	uint32_t MR0cycle, offset;
	
	MR0cycle = LPC_PWM1->MR0;				// Read the frequency	
	offset = MR0cycle * DutyCycle;

  switch (PWMPortNum) {
		case 1:
			LPC_PWM1->MR1 = offset;
			LPC_PWM1->LER = LER1_EN;
		break;

		case 2:
			LPC_PWM1->MR2 = offset;
			LPC_PWM1->LER = LER2_EN;
		break;

		case 3:
			LPC_PWM1->MR3 = offset;
			LPC_PWM1->LER = LER3_EN;
		break;

		case 4:
			LPC_PWM1->MR4 = offset;
			LPC_PWM1->LER = LER4_EN;
		break;

		case 5:
			LPC_PWM1->MR5 = offset;
			LPC_PWM1->LER = LER5_EN;
		break;

		case 6:
			LPC_PWM1->MR6 = offset;
			LPC_PWM1->LER = LER6_EN;
		break;
	}
  return;
}

/******************************************************************************
** Function name:		PWM_Start
** Descriptions:		Reset Counter, Enable counter & PWM
** parameters:			None
** Returned value:	None
******************************************************************************/
void PWM_Start( void ) {
	LPC_PWM1->TCR = TCR_RESET;	//* Counter Synchronously Reset with PCLK*/ 
  /* All single edge, all enable */
	LPC_PWM1->PCR = PWMENA1 | PWMENA2 | PWMENA3 | PWMENA4 |PWMENA5 | PWMENA6;
	LPC_PWM1->TCR = TCR_CNT_EN | TCR_PWM_EN;	/* counter enable, PWM enable */
	} 

/******************************************************************************
** Function name:		PWM_Stop
** Descriptions:		Stop all PWM channels
** parameters:			channel number
** Returned value:	None
******************************************************************************/
void PWM_Stop( uint32_t channelNum )	{
  if ( channelNum == 1 ) {
		LPC_PWM1->PCR = 0;
		LPC_PWM1->TCR = 0x00;		/* Stop all PWMs */
  }
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
