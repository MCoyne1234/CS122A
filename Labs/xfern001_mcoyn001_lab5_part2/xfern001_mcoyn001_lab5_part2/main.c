/*
 * xfern001_mcoyn001_lab5.c
 *
 * Created: 10/23/2019 2:51:48 PM
 * Author : Xander
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

unsigned char D = 0x00;
unsigned int j, k = 0;
unsigned int iter = 0;

char p0[1] = {0x00};
char p1[2] = {0xAA, 0x55};
char p2[14] = {128,64,32,16,8,4,2,1,2,4,8,16,32,64};
char p3[17] = {0,1,3,7,15,31,63,127,255,127,63,31,15,7,3,1,0};
char p4[8] = {0, 0x18, 0x24, 0x42, 0x81, 0x42, 0x24, 0x18};
	
char *patterns[4] = {p1, p2, p3, p4};
	
char sizes[4] = {2, 14, 17, 8};

enum States{Debounce, Debounce2, Debounce3, On, Off, Inc, Dec} state;
	
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
		case On:
			if (!(PINA & 0x01)) {
				state = Inc;
			} else if (!(PINA & 0x02)) {
				state = Dec;
			} else {
				state = On;
			}
		break;
		case Off:
			if (!(PINA & 0x01) && !(PINA & 0x02)) {
				state = Debounce3;
				} else {
				state = Off;
			}
		break;
		case Debounce:
			if (!(PINA & 0x01) && !(PINA & 0x02)) {
				state = Debounce2;
			} else if (!(PINA & 0x01) || !(PINA & 0x02)) {
				state = Debounce;
			} else {
				state = On;
			}
		break;
		case Debounce2:
			if (!(PINA & 0x01) || !(PINA & 0x02)) {
				state = Debounce2;
			} else {
				state = Off;
			}
		break;
		case Debounce3:
		if (!(PINA & 0x01) || !(PINA & 0x02)) {
			state = Debounce3;
		} else {
			state = On;
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
		case On:
			if (iter < sizes[j]) {
				D = patterns[j][iter];
				transmit_data(D);
				iter++;
			} else {
				iter = 0;
				D = patterns[j][iter];
				transmit_data(D);
				iter++;
			} _delay_ms(900);
			
		break;
		case Off:
			D = 0x00;
			transmit_data(D);
		case Debounce:
			//
		break;
		case Debounce2:
		//
		break;
		case Debounce3:
		//
		break;
		case Inc:
			if (j < 3) {
				j++;
			} else { j = 0; } 
			while(k < 100) { k++; } // added debounce delay
			k = 0;
		break;
		case Dec:
			if (j < 0) {
				j--;
			} else { j = 0; }
			while(k < 100) { k++; } // added debounce delay
			k = 0;
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