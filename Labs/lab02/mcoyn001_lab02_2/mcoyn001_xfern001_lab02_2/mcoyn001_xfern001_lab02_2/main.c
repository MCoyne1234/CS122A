/*
 * mcoyn001_xfern_lab02_2.c
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


enum States{ START, SEND };
enum States2{ START2, RECEIVE };
enum States3{ START3, ON};
char dataSend, dataRec, master, mask;
int countNoMess, noMessMax;

int Tick(int state){
    
    switch(state){
        case START:
        break;
        case SEND:
            PORTA = ( (dataSend & ~mask) | ( (master << 0) & mask) );
            if(USART_IsSendReady(1)){
                USART_Send(dataSend, 1);
            }            
        break;
        default:
        break;
    }
    
    switch(state){
        case START:
                state = SEND;       
        break;
        case SEND:
            if(USART_HasTransmitted(1)){
                // don't think I actually need this right now.
            }
        break;
        default:break;
    }
    return state;
}

int Tick2(int state){
    
    switch(state){
        case START2:
        break;
        case RECEIVE:
                if(USART_HasReceived(0)){
                    master = 0x00;
                    countNoMess = 0;
                    dataRec = USART_Receive(0);
                    USART_Flush(0);
                    PORTA = ( dataRec & 0x0E );
                }else{
                    ++countNoMess;
                    if(countNoMess >= noMessMax){
                        master = 0xFF;
                        countNoMess = 0;
                    }
                }                    
        break;
        default:
        break;
    }
    
    switch(state){
        case START2:
            state = RECEIVE;
        break;
        case RECEIVE:           
        break;
        default:break;
    }
    return state;
}

int Tick3(int state){
    
    switch(state){
        case START3:
        break;
        case ON:
            if(master){
                dataSend = ~dataSend;
            }
        break;
        default:
        break;
    }
    
    switch(state){
        case START3:
            state = ON;
        break;
        case ON:
        //state = OFF;
        break;
        default:break;
    }
    return state;
}

int main(void)
{
    DDRA = 0xFF; PORTA = 0x00;
    DDRB = 0xFE; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFA; PORTD = 0x00;

    initUSART(0);    
    USART_Flush(0);
    initUSART(1);
    USART_Flush(1);
    
    TimerSet(10);
    TimerOn();
    
    mask = 0x01;
    master = 0x00;
    countNoMess = 0;
    noMessMax = 150;
    
    unsigned long t1 = 2;
    unsigned long t2 = 2;
    unsigned long t3 = 50;
    
    static task task1, task2, task3;
    task *tasks[] = { &task3, &task1, &task2};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    task1.state = 0;//Task initial state.
    task1.period = t1;//Task Period.
    task1.elapsedTime = t1; // Task current elasped time.
    task1.TickFct = &Tick; // Function pointer for the tick.
    
    task2.state = 0;//Task initial state.
    task2.period = t2;//Task Period.
    task2.elapsedTime = t2; // Task current elapsed time.
    task2.TickFct = &Tick2; // Function pointer for the tick.
    
    task3.state = 0;//Task initial state.
    task3.period = t3;//Task Period.
    task3.elapsedTime = t3; // Task current elapsed time.
    task3.TickFct = &Tick3; // Function pointer for the tick.
    
    dataSend = 0;
    dataRec = 0;
    
    unsigned short i = 0; // Scheduler for-loop iterator
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

        while(!TimerFlag){}
        TimerFlag = 0;
    }
    return 0;
}

