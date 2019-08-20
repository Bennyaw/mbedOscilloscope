/* Library for the mbed Application Board's  C12832 LCD
 * 
 * by Ahmad Zaid bin Noh
 * Elvira Systems Sdn Bhd
 *
 * The Software is provided "as is", without warranty of any kind, express or
 * implied, including but not limited to the warranties of merchantability,
 * fitness for a particular purpose and noninfringement.
 * 15.11.2014  initial design
 * 05.02.2016  updated, include spi_init
 */
 
#include  "LPC17xx.h"						// Include this as part of CMSIS
#include  <stdio.h>
#include  "small_font.h"
#include  "lcd.h"

// Global Variables
	unsigned char current_lcd_line;	// LCD line or page

// Initialize SPI
void	spi_init( ) {
	LPC_SC->PCONP |= 1 <<10; 			// Enable POWER to SSP1 
	LPC_SC->PCLKSEL0 |= 1<<20; 		// Set Peripheral Clock Selection for SSP1 pclk = 12MHz
	LPC_SSP1->CPSR |= 6; 					// Set Clock Prescale to divide SSP1 clock by 6 = 2MHz
	LPC_SSP1->CR0 |= 3<<6; 				// Set clock phase (CPOL=1 and CPHA=1)
	LPC_SSP1->CR0 |= 7<<0; 				// Set SSP1 as 8 bits data transfer (DSS)
	LPC_SSP1->CR1 |= 1<<1;				// Enable SSP1
}

// Setting up the LCD Interfaces
void lcd_interface( ){
	LPC_GPIO0->FIODIR |= (1<<18); // Output chip select (CS) (P0.18)
	LPC_GPIO0->FIODIR |= (1<<8); 	// Output Reset (P0.8)
	LPC_GPIO0->FIODIR |= (1<<6); 	// Output A0 (P0.6)
	LPC_PINCON->PINSEL0 |= 2<<14;	// Output SCLK1, Alternate Function 2 (P0.7)
	LPC_PINCON->PINSEL0 |= 2<<18; // Output MOSI1, Alternate Function 2 (P0.9)
	// Initialise CS, Reset and A0 to high //
	LPC_GPIO0->FIOSET  = (1<<18) | (1<<8) | (1<<6); 	
}

// Transfer data to LCD - MOSI and SCK operation
void	spi_lcd_tfr(char mosi_data) {	
	LPC_GPIO0->FIOCLR = 1<<18; 						// Select LCD (CS=0)
	LPC_SSP1->DR = mosi_data;	 
	while ((LPC_SSP1->SR & (1<<4)) != 0);	// Wait if SSP1 Busy
	LPC_GPIO0->FIOSET = 1<<18; 						// Release LCD (CS=1)
}	 

// Write Command to LCD controller
void	wr_cmd(char cmd) {
	LPC_GPIO0->FIOCLR = 1<<6; 	// Assert Write Command Operation (ie A0 = 0)
	spi_lcd_tfr(cmd);						// Transfer 8 bit cmd through SPI/SSP1
	LPC_GPIO0->FIOSET = 1<<6; 	// Relase A0: A0 = 1 Write Data Operation
} 

// Write Data to LCD
void wr_dat(char dat) {
	spi_lcd_tfr(dat);								// Transfer 8 bit data through SPI/SSP1	
}

void 	delay_us(int dd ){
	int i;
	dd= 3*dd;
	for (i=0; i< dd; i++);
}

void 	delay_ms(int dd ){
	int i;
	for (i=0; i< dd; i++) delay_us(1000);
}

// Reset LCD 
void lcd_reset( ){
	LPC_GPIO0->FIOCLR = 1<<8; 	// Reset set to LOW
	delay_us(50);
	LPC_GPIO0->FIOSET = 1<<8; 	// release Reset
	delay_ms(5);
}

// Initialize LCD
void 	lcd_init( ){
	spi_init( );
	lcd_interface( ); // Set up LCD Interface		
	lcd_reset( );  	// Reset LCD 
	// Initialize LCD Controller sequence
	wr_cmd(0xAE); 	// display off
	wr_cmd(0xA2); 	// bias voltage
	wr_cmd(0xA0);	  // sets the display RAM address SEG output normal
	wr_cmd(0xC8); 	// column normal
	wr_cmd(0x22); 	// voltage resistor ratio
	wr_cmd(0x2F); 	// power on
	wr_cmd(0x40); 	// start line = 0
	wr_cmd(0xAF); 	// display ON
	wr_cmd(0x81); 	// set contrast
	wr_cmd(0x17); 	// set contrast
	wr_cmd(0xA6); 	// display normal 
	lcd_clear( );
} 

// Locate the LCD's cursor
char lcd_locate(char line, char col){
	if (line > 3) line = 0; 			// out of range errors
	if (col > 127) col = 0;
	wr_cmd(0x00 + (col & 0x0F));	// set column low nibble 0
	wr_cmd(0x10 + ((col & 0x70) >> 4)); 	// set column hi nibble 0
	wr_cmd(0xB0 + line); 				 	// set page address 0
	current_lcd_line = line;
	return current_lcd_line;
}

// Clear LCD Screen
void	lcd_clear( ){
	int i,j;
	for (i=0;i<4;i++){
		lcd_locate(i,0);
		for (j=0; j<128; j++) spi_lcd_tfr(0x00);	//clear all pixels
	}
	lcd_locate(0,0);	
} 

void lcd_line_clr(int line){
	int i;
	
	lcd_locate((line-1),0);
	for(i=0; i<128; i++) spi_lcd_tfr(0x00);  // Clear line 
	lcd_locate((line-1),0); // return back to line
}

// Write character to LCD
int 	lcd_write_char(unsigned char ch){
	int i, j, k;	
	j = (ch - (' ')) * 8;			// Adjust Starting Char 'Space' = 0x20
	//j = (ch - (0x20)) * 8;		// or use this 'Space' = 0x20	
	
	if (ch == 0x20){	 					// If Char is 'space'
		for (k=0; k<4; k++) 
			spi_lcd_tfr(0x00);					// Set space bar = 4 columns
	}
	
	else {
		for (i=0; i<8; i++) {
			if (Small_font[j] == 0x00)	
				j++;									// Don't write to column if data is 0x00
			else { 
				spi_lcd_tfr(Small_font[j]);// Write data to column
				j++; } }	
		spi_lcd_tfr(0x00);						// Set 1 column space between Char
	}	
	return ch;
}

// Write string to LCD
void	lcd_disp(int line, int col, char * s) {
	lcd_locate(line,8*col);					// Locate the LCD cursor
	while (*s)
	lcd_write_char(*s++);								// Write character until end of string
}

/******************************************************************************/
/* 'Retarget' layer for target-dependent low level functions      						*/
/******************************************************************************/
/* Edited by Ahmad Zaid Noh from file Retarget.c															*/
/* Retarget.c template available at C:\Keil\ARM\Startup												*/
/* Modified due to printf() calls fputc()																			*/
/* NOTE: You must define fputc() so it calls function that will output  			*/
/*       a single character to the LCD, ie: lcd_write_char( )											*/
/******************************************************************************/
/*
struct __FILE { int handle;};
FILE __stdout;
FILE __stdin;

int fputc(int value, FILE *f){
	unsigned int next_line;
	
  if (value == '\n'){     // new line
		if (current_lcd_line == 3)
			lcd_locate(0,0);
		else if (current_lcd_line < 3){
			next_line = current_lcd_line + 1;
			lcd_locate(next_line, 0);
		}
	}
	else {
    lcd_write_char(value);		// write char to LCD
  }		
  return value;
}
*/
