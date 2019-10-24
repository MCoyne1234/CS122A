/*
 * xfern001_mcoyn001_lab5.c
 *
 * Created: 10/23/2019 2:51:48 PM
 * Author : Xander
 */ 

#include <avr/io.h>
#include <stdlib.h>

unsigned char D = 0x00;

enum States{Check, Debounce, Reset, Inc, Dec} state;
	
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
	
void Tick() {
	switch(state) { // state transitions
		case Check:
			if (!(PINA & 0x01)) {
				state = Inc;
			} else if (!(PINA & 0x02)) {
				state = Dec;
			} else {
				state = Check;
			}
		break;
		case Debounce:
			if (!(PINA & 0x01) && !(PINA & 0x02)) {
				state = Reset;
			} else if (!(PINA & 0x01) || !(PINA & 0x02)) {
				state = Debounce;
			} else {
				state = Check;
			}
		break;
		case Reset:
			if (!(PINA & 0x01) || !(PINA & 0x02)) {
				//nothin
			} else {
				state = Check;
			}
		break;
		case Inc:
			state = Debounce;
		break;
		case Dec:
			state = Debounce;
		break;
	}
	
	switch(state) { // state actions
		case Check:
			//
		break;
		case Debounce:
			//
		break;
		case Reset:
			D = 0x00;
			transmit_data(D);
		break;
		case Inc:
			if (D < 0xFF) {
				D++;
			}
			transmit_data(D);
		break;
		case Dec:
			if (D > 0x00) {
				D--;
			}
			transmit_data(D);
		break;
	}
}

int main(void) {
	// Initialize ports
	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRA = 0x00; PORTA = 0x00;
	
	while(1) {
		Tick();
	}
}