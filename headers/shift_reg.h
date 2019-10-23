#include <avr/io.h>

#define SHIFT_REG PORTA
#define SER PORTA4 
#define RCLK PORTA2
#define SRCLK PORTA1
#define SRCLR PORTA0

void transmit_data(unsigned char data) {
	// SER (PB0), RCLK (PB1), SRCLK (PB2), SRCLR (PB3)
	// when ready to begin transmitting, set RCLK (PB1) low and SRCLR (PB3) high
	// for each bit of data, set SRCLR (PB3) high and RCLK (PB1) Low,
	// Then for each bit, set SRCLK (PB2) low, then set SER (PB0) to the value of the
	// bit being transfered (1 or 0), then set SRCLK (PB2) high
	unsigned char i = 0, bit;
	unsigned char mask = (1 << SER);
	while(i < 8) {
		bit = ( (data >> i) & 0x01) << SER;
		SHIFT_REG = (1 << SRCLR);
		//SHIFT_REG |= ((data >> i) & SER);
		SHIFT_REG &= ~(1 << SER);
		SHIFT_REG |= bit;
		
		SHIFT_REG |= (1 << SRCLK);
		i++;
	}
	
	SHIFT_REG |= (1 << RCLK);
	SHIFT_REG = 0x00;
	
}