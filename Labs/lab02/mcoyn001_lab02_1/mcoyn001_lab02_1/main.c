/*
 * mcoyn001_lab02_1.c
 *
 * Created: 10/4/2019 1:18:28 PM
 * Author : Administrator
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include <timer.h>
#include <stdio.h>
#include <scheduler.h>
#include "usart_ATmega1284.h"
#include "io.c"


enum States{ START, LISTEN, SEND, RECIEVE};
enum States2{ START2, ON, OFF};
char dataSend, dataRec, master;


int Tick(int state){
    
    switch(state){
        case START:
        break;
        case LISTEN:
            //USART_
        break;
        case SEND:
            if(USART_IsSendReady(0)){
                USART_Send(dataSend, 0);
            }            
        break;
        case RECIEVE:
            dataRec = USART_Receive(0);
        break;
        default:
        break;
    }
    
    switch(state){
        case START:
            state = LISTEN;
        break;
        case LISTEN:
            if(master){
                state = SEND;
            }else if(USART_HasReceived(0)){
                state = RECIEVE;
            }        
        break;
        case SEND:
            if(USART_HasTransmitted(0)){
                state = LISTEN;
            }
        break;
        case RECIEVE:
            state = LISTEN;
        break;
        default:break;
    }
    return state;
}

int Tick2(int state){
    
    switch(state){
        case START2:
        break;
        case ON:
            if(master){
                
                dataSend = P
            }else(){
                
            }
        break;
        case OFF:
        break;
        default:
        break;
    }
    
    switch(state){
        case START2:
        state = ON;
        break;
        case ON:
        //state = OFF;
        break;
        case OFF:
        //state = ON;
        break;
        default:break;
    }
    return state;
}

int main(void)
{
    DDRA = 0x00; PORTA = 0x00;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFA; PORTD = 0x00;
    
    initUSART(0);    
    USART_Flush(0);
    
    unsigned long t1 = 1;
    unsigned long t2 = 2;
    
    
    static task task1, task2;
    task *tasks[] = { &task1, &task2};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    task1.state = 0;//Task initial state.
    task1.period = t1;//Task Period.
    task1.elapsedTime = t1; // Task current elasped time.
    task1.TickFct = &Tick; // Function pointer for the tick.
    
    task2.state = 0;//Task initial state.
    task2.period = t2;//Task Period.
    task2.elapsedTime = t2; // Task current elapsed time.
    task2.TickFct = &Tick2; // Function pointer for the tick.
    
    char butt = 0;
    unsigned short i; // Scheduler for-loop iterator
    while(1){
        // Scheduler code
        for ( i = 0; i < numTasks; i++ ) {
            // Task is ready to tick
            if ( tasks[i]->elapsedTime == tasks[i]->period ) {
                // Setting next state for task
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                // Reset the elapsed time for next tick.
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += 1;
        }

        while(!TimerFlag){
            master = ( !(PINA & 0x00) );
        };
        TimerFlag = 0;
        //PORTC = score;
        //LCD_ClearScreen();
        //LCD_Cursor(1);
        //LCD_WriteData(48 + score);
    }
    return 0;
}

