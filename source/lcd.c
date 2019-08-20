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
	unsigned int draw_mode;
	enum {NORMAL,XOR};
	unsigned char buffer[512];
	int auto_up;
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
	auto_up=1;
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
void lcd_copy_to_lcd(void)
{
#ifndef TARGET_LPC1768
    int i;
#endif
    //page 0
    wr_cmd(0x00);      // set column low nibble 0
    wr_cmd(0x10);      // set column hi  nibble 0
    wr_cmd(0xB0);      // set page address  0
    LPC_GPIO0->FIOSET = 1<<6;
#if defined TARGET_LPC1768
    LPC_GPIO0->FIOCLR = 1<<18;
    // start 128 byte DMA transfer to SPI1
    LPC_GPDMACH0->DMACCDestAddr = (uint32_t)&LPC_SSP1->DR; // we send to SSP1
    LPC_SSP1->DMACR = 0x2;  // Enable SSP1 for DMA.
    LPC_GPDMA->DMACIntTCClear = 0x1;
    LPC_GPDMA->DMACIntErrClr = 0x1;
    LPC_GPDMACH0->DMACCSrcAddr = (uint32_t) (buffer);
    LPC_GPDMACH0->DMACCControl = 128 | (1UL << 31) |  DMA_CHANNEL_SRC_INC ; // 8 bit transfer , address increment, interrupt
    LPC_GPDMACH0->DMACCConfig  = DMA_CHANNEL_ENABLE | DMA_TRANSFER_TYPE_M2P | DMA_DEST_SSP1_TX;
    LPC_GPDMA->DMACSoftSReq = 0x1;
    do {
    } while ((LPC_GPDMA->DMACRawIntTCStat & 0x01) == 0); // DMA is running
    do {
    } while ((LPC_SSP1->SR & 0x10) == 0x10); // SPI1 not idle
    LPC_GPIO0->FIOSET = 1<<18; 
#else  // no DMA
    for(i=0; i<128; i++) {
        wr_dat(buffer[i]);
    }
#endif

    // page 1
    wr_cmd(0x00);      // set column low nibble 0
    wr_cmd(0x10);      // set column hi  nibble 0
    wr_cmd(0xB1);      // set page address  1
    LPC_GPIO0->FIOSET = 1<<6;
#if defined TARGET_LPC1768
    LPC_GPIO0->FIOCLR = 1<<18;
    // start 128 byte DMA transfer to SPI1
    LPC_GPDMA->DMACIntTCClear = 0x1;
    LPC_GPDMA->DMACIntErrClr = 0x1;
    LPC_GPDMACH0->DMACCSrcAddr = (uint32_t) (buffer + 128);
    LPC_GPDMACH0->DMACCControl = 128 | (1UL << 31) |  DMA_CHANNEL_SRC_INC ; // 8 bit transfer , address increment, interrupt
    LPC_GPDMACH0->DMACCConfig  = DMA_CHANNEL_ENABLE | DMA_TRANSFER_TYPE_M2P | DMA_DEST_SSP1_TX;
    LPC_GPDMA->DMACSoftSReq = 0x1;
    do {
    } while ((LPC_GPDMA->DMACRawIntTCStat & 0x01) == 0); // DMA is running
    do {
    } while ((LPC_SSP1->SR & 0x10) == 0x10); // SPI1 not idle
    LPC_GPIO0->FIOSET = 1<<18; 
#else // no DMA
    for(i=128; i<256; i++) {
        wr_dat(buffer[i]);
    }
#endif

    //page 2
    wr_cmd(0x00);      // set column low nibble 0
    wr_cmd(0x10);      // set column hi  nibble 0
    wr_cmd(0xB2);      // set page address  2
    LPC_GPIO0->FIOSET = 1<<6;
#if defined TARGET_LPC1768
    LPC_GPIO0->FIOCLR = 1<<18;
    // start 128 byte DMA transfer to SPI1
    LPC_GPDMA->DMACIntTCClear = 0x1;
    LPC_GPDMA->DMACIntErrClr = 0x1;
    LPC_GPDMACH0->DMACCSrcAddr = (uint32_t) (buffer + 256);
    LPC_GPDMACH0->DMACCControl = 128 | (1UL << 31) |  DMA_CHANNEL_SRC_INC ; // 8 bit transfer , address increment, interrupt
    LPC_GPDMACH0->DMACCConfig  = DMA_CHANNEL_ENABLE | DMA_TRANSFER_TYPE_M2P | DMA_DEST_SSP1_TX ;
    LPC_GPDMA->DMACSoftSReq = 0x1;
    do {
    } while ((LPC_GPDMA->DMACRawIntTCStat & 0x01) == 0); // DMA is running
    do {
    } while ((LPC_SSP1->SR & 0x10) == 0x10); // SPI1 not idle
    LPC_GPIO0->FIOSET = 1<<18; 
#else // no DMA
    for(i=256; i<384; i++) {
        wr_dat(buffer[i]);
    }
#endif

    //page 3
    wr_cmd(0x00);      // set column low nibble 0
    wr_cmd(0x10);      // set column hi  nibble 0
    wr_cmd(0xB3);      // set page address  3
    LPC_GPIO0->FIOSET = 1<<6;

    LPC_GPIO0->FIOCLR = 1<<18;
#if defined TARGET_LPC1768
    // start 128 byte DMA transfer to SPI1
    LPC_GPDMA->DMACIntTCClear = 0x1;
    LPC_GPDMA->DMACIntErrClr = 0x1;
    LPC_GPDMACH0->DMACCSrcAddr = (uint32_t) (buffer + 384);
    LPC_GPDMACH0->DMACCControl = 128  | (1UL << 31) |  DMA_CHANNEL_SRC_INC ; // 8 bit transfer , address increment, interrupt
    LPC_GPDMACH0->DMACCConfig  = DMA_CHANNEL_ENABLE | DMA_TRANSFER_TYPE_M2P | DMA_DEST_SSP1_TX;
    LPC_GPDMA->DMACSoftSReq = 0x1;
    do {
    } while ((LPC_GPDMA->DMACRawIntTCStat & 0x01) == 0); // DMA is running
    do {
    } while ((LPC_SSP1->SR & 0x10) == 0x10); // SPI1 not idle
    LPC_GPIO0->FIOSET = 1<<18; 
#else // no DMA
    for(i=384; i<512; i++) {
        wr_dat(buffer[i]);
    }
#endif
}


void lcd_pixel(int x, int y, int color)
{
    // first check parameter
    if(x > 128 || y > 32 || x < 0 || y < 0) return;

    if(draw_mode == NORMAL) {
        if(color == 0)
            buffer[x + ((y/8) * 128)] &= ~(1 << (y%8));  // erase pixel
        else
            buffer[x + ((y/8) * 128)] |= (1 << (y%8));   // set pixel
    } else { // XOR mode
        if(color == 1)
            buffer[x + ((y/8) * 128)] ^= (1 << (y%8));   // xor pixel
    }
}

void lcd_line(int x0, int y0, int x1, int y1, int color)
{
    int   dx = 0, dy = 0;
    int   dx_sym = 0, dy_sym = 0;
    int   dx_x2 = 0, dy_x2 = 0;
    int   di = 0;

    dx = x1-x0;
    dy = y1-y0;

    //  if (dx == 0) {        /* vertical line */
    //      if (y1 > y0) vline(x0,y0,y1,color);
    //      else vline(x0,y1,y0,color);
    //      return;
    //  }

    if (dx > 0) {
        dx_sym = 1;
    } else {
        dx_sym = -1;
    }
    //  if (dy == 0) {        /* horizontal line */
    //      if (x1 > x0) hline(x0,x1,y0,color);
    //      else  hline(x1,x0,y0,color);
    //      return;
    //  }

    if (dy > 0) {
        dy_sym = 1;
    } else {
        dy_sym = -1;
    }

    dx = dx_sym*dx;
    dy = dy_sym*dy;

    dx_x2 = dx*2;
    dy_x2 = dy*2;

    if (dx >= dy) {
        di = dy_x2 - dx;
        while (x0 != x1) {

            lcd_pixel(x0, y0, color);
            x0 += dx_sym;
            if (di<0) {
                di += dy_x2;
            } else {
                di += dy_x2 - dx_x2;
                y0 += dy_sym;
            }
        }
        lcd_pixel(x0, y0, color);
    } else {
        di = dx_x2 - dy;
        while (y0 != y1) {
            lcd_pixel(x0, y0, color);
            y0 += dy_sym;
            if (di < 0) {
                di += dx_x2;
            } else {
                di += dx_x2 - dy_x2;
                x0 += dx_sym;
            }
        }
        lcd_pixel(x0, y0, color);
    }
    if(auto_up) lcd_copy_to_lcd();
}

#define NUMHORIZPIXELS		128
#define NUMVERTPIXELS		32
#define FIRSTVERT			21
#define SECONDVERT			42
#define THIRDVERT			63
#define FOURTHVERT			84
#define FIFTHVERT			105
#define FIRSTHORIZ			11
#define SECONDHORIZ			22
void place_grid(void)
{
	lcd_line(FIRSTVERT, 0, FIRSTVERT, NUMVERTPIXELS-1, 1);
	lcd_line(SECONDVERT, 0, SECONDVERT, NUMVERTPIXELS-1, 1);
	lcd_line(THIRDVERT, 0, THIRDVERT, NUMVERTPIXELS-1, 1);
	lcd_line(FOURTHVERT, 0, FOURTHVERT, NUMVERTPIXELS-1, 1);
	lcd_line(FIFTHVERT, 0, FIFTHVERT, NUMVERTPIXELS-1, 1);
	lcd_line(0, FIRSTHORIZ, NUMHORIZPIXELS-1, FIRSTHORIZ, 1);
	lcd_line(0, SECONDHORIZ, NUMHORIZPIXELS-1, SECONDHORIZ, 1);
}

void place_border(void){
	lcd_line(0,0,NUMHORIZPIXELS-1,0,1);//low border line
	lcd_line(0,NUMVERTPIXELS-1,NUMHORIZPIXELS-1,NUMVERTPIXELS-1,1);//top border line
	lcd_line(0,0,0,NUMVERTPIXELS-1,1);//left border line
	lcd_line(NUMHORIZPIXELS-1,0,NUMHORIZPIXELS-1,NUMVERTPIXELS-1,1);
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
