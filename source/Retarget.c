
// This file is available at C://Keil/ARM/Startup/Retarget.c
// Modified by Zaid on 29th January' 2016 for fputc( ) function

/******************************************************************************/
/* RETARGET.C: 'Retarget' layer for target-dependent low level functions      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005 Keil Software. All rights reserved.                     */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <rt_misc.h>
#include "lcd.h"

#pragma import(__use_no_semihosting_swi)

extern int uart_Tx(char ch);	/* in Serial.c */
extern int uart_Rx(void);			/* in Serial.c */
extern long timeval;          /* in Time.c   */
extern char current_lcd_line;
extern char PUTCHAR_SERIAL;

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f){
	unsigned int next_line;
	if (PUTCHAR_SERIAL)
		return (uart_Tx(ch));
	else {
		if (ch == '\n'){     // new line
			if (current_lcd_line == 3)
				lcd_locate(0,0);
			else if (current_lcd_line < 3){
				next_line = current_lcd_line + 1;
				lcd_locate(next_line, 0);
			}
		}
		else {
			lcd_write_char(ch);		// write char to LCD
		}		
		return (ch);
	}
}	
	
int fgetc(FILE *f) {
		return(uart_Tx(uart_Rx()));
}

int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}

void _ttywrch(int ch) {
  uart_Tx (ch);
}

void _sys_exit(int return_code) {
  while (1);    /* endless loop */
}
