/*
 * lab06_1.c
 *
 * Created: 10/25/2019 2:31:57 PM
 * Author : Administrator
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <timer.h>
#include <stdio.h>
#include <scheduler_2.h>
#include <string.h>
#include "io.c"

enum States2{ START2, RECEIVE };
enum States3{ START3, ON, OFF};


#define buttons ~(PINA & 0xE0)
const unsigned char Inc = (1 << PORTA5); // Which pin is the increment button
const unsigned char Dec = (1 << PORTA6); // Which pin is the decrement button
const unsigned char Reset = ( 1 << PORTA5) | (1<<PORTA6 );
const unsigned char LCD_B = (1 << PORTA4); 

unsigned char count;
enum IncDec_States { Start, Wait, IncHold, DecHold, ResetHold };
void updateCount(int);
const unsigned char InitialCount = 4;
unsigned char b1, b2, b4, b3, t_max, t_count; 

static task task2, task4;
task *tasks[] = {  &task2, &task4 };
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

int Tick2(int state) {
    b1 = (~( PINA &  Inc)) & Inc ;
    b2 = (~( PINA &  Dec)) & Dec ;
    b3 = (~( PINA &  LCD_B)) & LCD_B ;
    b4 = b3;
    if(b4){b4 = b2;}
   if (b3 == LCD_B) { task4.active = 0x01;}
   
    switch (state) { // Transitions
        case Start:
        count = InitialCount;
        state = Wait;
        break;
        case Wait:
        //if (buttons == Inc) {
            
          if( (b1 && (b2 == 0) ) ){  
            updateCount(1);
            state = IncHold;
           // } else if (buttons == Dec) {
           } else if ( (b1 == 0) && b2 ) {
            updateCount(-1);
            state = DecHold;
            //} else if (buttons == Reset) {
            }else if (b1 && b2) {
            count = 0;
            state = ResetHold;
            } else {
            state = Wait;
        }
        break;
        case IncHold:
        //if (buttons == Inc) {
        if( (b1 && (b2 == 0) ) ){
            state = IncHold;
            //} else if (buttons == Reset) { // Intentional bug. Leave in.
            } else if (b1 & b2) { // Intentional bug. Leave in.
            //PORTD = 0xFF;
            //for(;;);
            } else {
            state = Wait;
        }
        break;
        case DecHold:
        //if (buttons == Dec) {
         if( ( (b1 == 0) && b2) ){
            state = DecHold;
            } else {
            state = Wait;
        }
        break;
        case ResetHold:
        //if (buttons == Reset) {
        if (b1 && b2) {
            state = ResetHold;
            } else {
            state = Wait;
        }
        break;
        default:
        state = Wait;
        break;
    }
    switch (state) { // Actions
        case Start:
        case Wait:
        case IncHold:
        case DecHold:
        case ResetHold:
        break;
    }

    return state;
}

void updateCount(int incValue) {
    const char MAX_COUNT = 9;
    const char MIN_COUNT = 0;
    unsigned char newCount = count + incValue;
    if (MIN_COUNT <= newCount && newCount <= MAX_COUNT) {
        count = newCount;
    }
}


int Tick4(int state){
    
    switch(state){
        case START3:       
            t_max = 6;
            t_count = 0;
        break;
        case ON:
        LCD_ClearScreen();
        //LCD_DisplayString(1, count + '0');
        LCD_Cursor(2);
        LCD_WriteData(count + '0');
        
        LCD_Cursor(4);
        LCD_WriteData(b1 + '0');
        
        LCD_Cursor(6);
        LCD_WriteData(b2 + '0');
        
        LCD_Cursor(8);
        LCD_WriteData(b3 + '0');
        //LCD_DisplayString(1, "0123456789ABCDEF");
        //++count;
        ++t_count;
        break;
        default:
        break;
    }
    
    switch(state){
        case START3:
            state = ON;
        break;
        case ON:
            if(t_count > t_max){
                t_count = 0;
                LCD_ClearScreen();
                task4.active = 0x00;
            }
        break;
        case OFF:
        break;
        default:break;
    }
    return state;
}

int main(void)
{
    DDRA = 0x03; PORTA = 0x00;
    
    DDRC = 0xF0; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    TimerSet(50);
    TimerOn();
    
    LCD_init();
    LCD_ClearScreen();
    //LCD_DisplayString(1,"0123456789ABCDEF
    
    unsigned long t2 = 2;
    unsigned long t4 = 10;
    
    
    task2.state = 0;//Task initial state.
    task2.period = t2;//Task Period.
    task2.elapsedTime = t2; // Task current elapsed time.
    task2.TickFct = &Tick2; // Function pointer for the tick.
    task2.active = 0x01;   // Set task active.
    
    task4.state = 0;//Task initial state.
    task4.period = t4;//Task Period.
    task4.elapsedTime = t4; // Task current elapsed time.
    task4.TickFct = &Tick4; // Function pointer for the tick.
    task4.active = 0x00;   // Set task active.
    
    
    unsigned short i = 0; // Scheduler for-loop iterator
    while(1){
        // Scheduler code
        for ( i = 0; i < numTasks; i++ ) {
            // Task is ready to tick
            if ( (tasks[i]->elapsedTime >= tasks[i]->period) && tasks[i]->active ) {
                // Setting next state for task
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                // Reset the elapsed time for next tick.
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += 1;
        }

        while(!TimerFlag){}
        TimerFlag = 0;
    }
    return 0;
}

