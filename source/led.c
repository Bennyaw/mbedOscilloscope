#include "LPC17xx.h"

#define NUM_LEDS 4

void LED_Init (void)				
{
 LPC_GPIO1->FIODIR2 |= 0xb4;	// // Set the FIODIR = Output (to drive the LED)
}

void LED_On(uint32_t num){
	switch(num) {
	case 1: 
					LPC_GPIO1->FIOSET2 |= 0x04; 
					break;
	case 2: 
					LPC_GPIO1->FIOSET2 |= 0x10; 
					break;
	case 3: 
					LPC_GPIO1->FIOSET2 |= 0x20; 
					break;
	case 4: 
					LPC_GPIO1->FIOSET2 |= 0x80; 
					break;
	default:
					LPC_GPIO1->FIOSET2 |= 0x0; 
					break;
	}
}

void LED_Off(uint32_t num){
	switch(num) {
	case 1: 
					LPC_GPIO1->FIOCLR2 |= 0x04; 
					break;
	case 2: 
					LPC_GPIO1->FIOCLR2 |= 0x10; 
					break;
	case 3: 
					LPC_GPIO1->FIOCLR2 |= 0x20; 
					break;
	case 4: 
					LPC_GPIO1->FIOCLR2 |= 0x80; 
					break;
	default:
					LPC_GPIO1->FIOCLR2 |= 0x0; 
					break;
	}
}

/*-----------------------------------------------------------------------------
 *      LED_Out: Write value to LEDs
 *
 * Parameters:  val - value to be displayed on LEDs
 * Return:     (none)
 *----------------------------------------------------------------------------*/
void LED_Out (uint32_t val) {
  uint32_t n;

  for (n = 0; n < NUM_LEDS; n++) {
    if (val & (1<<n)) {
      LED_On(n+1);
    } else {
      LED_Off(n+1);
    }
  }
}

