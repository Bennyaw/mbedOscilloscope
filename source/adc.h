
/* Bit positions of ADCR and ADGR registers */ 
#define SBIT_BURST      16u
#define SBIT_START      24u
#define SBIT_PDN        21u
#define SBIT_EDGE       27u 
#define SBIT_DONE       31u
#define SBIT_RESULT     4u
#define SBIT_CLCKDIV    8u

#define AD0      0u
#define AD1      1u
#define AD2      2u
#define AD3      3u
#define AD4      4u
#define AD5      5u
#define AD6      6u
#define AD7      7u

void adc_init( void ); 

