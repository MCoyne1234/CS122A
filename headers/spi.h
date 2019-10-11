#ifndef SPI_LIBRARY
#define SPI_LIBRARY

//#define ISR_DATA PORTD
#define DDR_SPI DDRB
#define DD_SS PORTB4 
#define DD_MOSI PORTB5
#define DD_MISO PORTB6
#define DD_SCK PORTB7

 //Straight from the datasheet
volatile unsigned char receivedData;
 
void SPI_MasterInit(void){
	/* Set MOSI and SCK output, all others input */
	DDR_SPI = (1<<DD_SS)|(1<<DD_MOSI) | (1<<DD_SCK);
	
	// Enable SPI, Master, set clock rate fck/16
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
}

void SPI_MasterTransmit(unsigned char cData){
	// Start transmission 
	SPDR = cData;
	
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
}

void SPI_ServantInit(void){
	/* Set MISO output, all others input */
	DDR_SPI = (1<<DD_MISO);
	
	/* Enable SPI */
	SPCR = (1<<SPE);
}

char SPI_ServantReceive(void)
{
/* Wait for reception complete */
while(!(SPSR & (1<<SPIF)))
;
/* Return Data Register */
return SPDR;
}

ISR(SPI_STC_vect){
  receivedData = SPI_ServantReceive();
}

#endif // SPI_LIBRARY