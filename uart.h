
#include "lpc17xx.h"
void uart_init(uint32_t baudrate);
void uart_Tx(char ch);
char uart_Rx(void);
void uart_Tx_str(char *s);
