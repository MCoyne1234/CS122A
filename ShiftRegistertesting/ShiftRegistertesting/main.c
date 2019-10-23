#include <avr/io.h>
#include <stdlib.h>



void transmit_data(unsigned char data) {
	// SER (PB0), RCLK (PB1), SRCLK (PB2), SRCLR (PB3)
	// when ready to begin transmitting, set RCLK (PB1) low and SRCLR (PB3) high
	// for each bit of data, set SRCLR (PB3) high and RCLK (PB1) Low,
	// Then for each bit, set SRCLK (PB2) low, then set SER (PB0) to the value of the
	// bit being transfered (1 or 0), then set SRCLK (PB2) high
	unsigned char i = 0;
	while(i < 8) {
		PORTB = 0x08;
		PORTB |= ((data >> i) & 0x01);
		PORTB |= 0x04;
		i++;
	}
	
	PORTB |= 0x02;
	PORTB = 0x00;
	
}

int main(void) {
	// Initialize ports
	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRD = 0xFF; PORTD = 0x00; // LCD data lines
	DDRA = 0xFF; PORTA = 0x00; // LCD control lines
	
	char D = 0xF0;
	transmit_data(D);
}