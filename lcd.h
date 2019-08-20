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
