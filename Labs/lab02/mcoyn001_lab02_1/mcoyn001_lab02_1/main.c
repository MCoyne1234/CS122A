/*
 * mcoyn001_lab02_1.c
 *
 * Created: 10/4/2019 1:18:28 PM
 * Author : Administrator
 * 
 *    Partner(s) Name & E-mail: Matthew Coyne mcoyn001@ucr.edu
 *    Lab Section: 021
 *    Assignment: Lab #2  Exercise #1 
 *    Exercise Description: USART Lab with hardwired leader/follower.
 *    
 *    I acknowledge all content contained herein, excluding template 
 *     or example code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include <timer.h>
#include <stdio.h>
#include <scheduler.h>
#include "usart_ATmega1284.h"
#include "io.c"


enum States{ START, LISTEN, SEND };
enum States2{ START2, RECIEVE };
enum States3{ START3, ON};
char dataSend, dataRec, master, mask;


int Tick(int state){
    
    switch(state){
        case START:
        break;
        case LISTEN:
            PORTA = ( (dataSend & ~mask) | ( (master << 0) & mask) );
        break;
        case SEND:
            if(USART_IsSendReady(1)){
                USART_Send(dataSend, 1);
            }            
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
            }       
        break;
        case SEND:
            if(USART_HasTransmitted(1)){
                state = LISTEN;
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
        case RECIEVE:
            if(master != 0x01){
                if(USART_HasReceived(0)){
                    dataRec = USART_Receive(0);
                    USART_Flush(0);
                    PORTA = ( dataRec & 0x0E );
                }
            }
        break;
        default:
        break;
    }
    
    switch(state){
        case START2:
            state = RECIEVE;
        break;
        case RECIEVE:           
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
        master = (PINB & 0x01);
    }
    return 0;
}

