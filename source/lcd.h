// Function Prototype for LCD 
int  lcd_write_char(unsigned char ch);
void lcd_next_line(void);
void lcd_init(void);
void lcd_clear(void);
char lcd_locate(char line, char col);
void lcd_disp(int line, int column, char *);
void lcd_reset(void);
void delay_ms(int dd);
void lcd_line_clr(int line);
void place_grid(void);
void lcd_line(int x0, int y0, int x1, int y1, int color);
void lcd_pixel(int x, int y, int color);

//defines for DMA

#define DMA_CHANNEL_ENABLE      1
#define DMA_TRANSFER_TYPE_M2P   (1UL << 11)
#define DMA_CHANNEL_TCIE        (1UL << 31)
#define DMA_CHANNEL_SRC_INC     (1UL << 26)
#define DMA_MASK_IE             (1UL << 14)
#define DMA_MASK_ITC            (1UL << 15)
#define DMA_SSP1_TX             (1UL << 2)
#define DMA_SSP0_TX             (0)
#define DMA_DEST_SSP1_TX        (2UL << 6)
#define DMA_DEST_SSP0_TX        (0UL << 6)

