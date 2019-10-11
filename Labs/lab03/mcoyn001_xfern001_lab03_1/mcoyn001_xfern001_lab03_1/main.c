/*
 * mcoyn001_xfern001_lab03_1.c
 *
 * Created: 10/8/2019 8:56:27 PM
 * Author : Administrator
 * 
 *    Partner(s) Name & E-mail: Matthew Coyne mcoyn001@ucr.edu
 *    Lab Section: 021
 *    Assignment: Lab #3  Exercise #1 
 *    Exercise Description: SPI Lab: LCD KEYPAD LCD.
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
#include "io.c"
#include "spi.h"


enum States{ START, LISTEN, SEND };
enum States2{ START2, RECIEVE };
enum States3{ START3, ON};
char data, dataRec, master, mask;


int Tick(int state){
    
    switch(state){
        case START:
        break;
        case LISTEN:
        break;
        case SEND:    
            if(master) SPI_MasterTransmit(data);       
        break;
        default:
        break;
    }
    
    switch(state){
        case START:
            state = SEND;
        break;
        case LISTEN:  
        break;
        case SEND:
            //state = LISTEN;
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
            if(!master){
                PORTA = SPI_ServantReceive();//receivedData;
                //PORTA = ( (PORTA & ~mask) | ( (receivedData) & mask) );
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
                if(data == 0x10) data = 0x00;
                else data = 0x10;                               
                PORTA = ( (PORTA & ~mask) | ( (data) & mask) );
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
    DDRA = 0xFF; PORTA = 0x10;
    //DDRB = 0xFE; PORTB = 0x00;
    DDRC = 0xF0; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    
    TimerSet(2);
    TimerOn();
    
    //sei();
    
    mask = 0x10;
    
    unsigned long t1 = 50;
    unsigned long t2 = 3;
    unsigned long t3 = 250;
    
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
    
    data = 0;
    dataRec = 0;
    
    if(PINB & 0x01) master = 0xFF;
    else master = 0x00;
    
    if(master) SPI_MasterInit();
    else SPI_ServantInit();
    
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

