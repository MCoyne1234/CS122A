#include <avr/io.h>
#include <timer.h>
#include <lcd.h>

enum States{START, B0, B1, B2} state;
enum States2{START2, ON1, ON2, OFF1, OFF2} state2;

int light1 = 0x00;
int light2 = 0x00;

void Tick() {
    switch(state) { //state transitions
        case START:
        state = B0;
        break;
        case B0:
        state = B1;
        break;
        case B1:
        state = B2;
        break;
        case B2:
        state = B0;
        break;
    }
    
    switch(state) { // state actions
        case START:
        // PORTB = 0x00;
        break;
        case B0:
        //PORTB = ((PORTB & 0x08) | 0x01);
        light1 = 0x01;
        break;
        case B1:
        //PORTB = ((PORTB & 0x08) | 0x02);
        light1 = 0x02;
        break;
        case B2:
        //PORTB = ((PORTB & 0x08) | 0x04);
        light1 = 0x04;
        break;
    }
}

void Tick2() {
    switch(state2) {
        case START2:
        state2 = ON1;
        break;
        case ON1:
        state2 = ON2;
        break;
        case ON2:
        state2 = OFF1;
        break;
        case OFF1:
        state2 = OFF2;
        break;
        case OFF2:
        state2 = ON1;
        break;
    }
    
    switch(state2) {
        case START2:
        //PORTB = 0x00;
        break;
        case ON1:
        //PORTB = PORTB | 0x08;
        light2 = 0x08;
        break;
        case ON2:
        //PORTB = PORTB | 0x08;
        light2 = 0x08;
        break;
        case OFF1:
        //PORTB = PORTB & 0xF7; // 1111 0111
        light2 = 0x00;
        break;
        case OFF2:
        //PORTB = PORTB & 0xF7;
        light2 = 0x00;
        break;
    }
}

int main(void) {
    DDRA = 0x00; PORTA = 0X00;
    DDRB = 0xFF; PORTB = 0X00;
    
    TimerSet(500);
    TimerOn();
    
    state = START;
    state2 = START2;

    while (1) {
        Tick();
        Tick2();
        PORTB = light1 + light2;
        while(!TimerFlag);
        TimerFlag = 0;
    }
}
