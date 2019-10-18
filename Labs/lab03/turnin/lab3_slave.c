/*
 * mcoyn001_xfern001_lab03
 *
 * Created: 10/8/2019 8:56:27 PM
 * Author : Administrator
 * 
 *    Partner(s) Name & E-mail: Matthew Coyne mcoyn001@ucr.edu
 *    Lab Section: 021
 *    Assignment: Lab #3  Exercise #1 
 *    Exercise Description: SPI Lab: Slave code 
 *		There are some artifacts from the master code that isn't used.
 *      Challange: Slave select has been demoed. 
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
#include <string.h>
#include "spi.h"
#include "keypad.h"

enum States2{ START2, RECEIVE };
enum States3{ START3, ON};
unsigned char data, key, master, maskUp, maskLow, trigger;
char pattern, p_size, speed, slave, t_count, t_max; 
int iter;

char p1[2] = {0xF0, 0x0F};
char p2[2] = {0xAA, 0x55};
char p3[14] = {128,64,32,16,8,4,2,1,2,4,8,16,32,64};
char p4[18] = {0,1,3,7,15,31,63,127,255,127,63,31,15,7,3,1};        

char *patterns[4] = {p1,p2,p3,p4};   
char sizes[4] = {2,2,14,16}; 
char speeds[] = {39,19,9,4,1,0};
    
char message_1[9] = "Ptrn: 0 ";
char message_2[7] = "Spd: 0";
char message_3[6] = "uC: 0";


void slaveData();



int Tick2(int state){
    
    switch(state){
        case START2:
            pattern =0;
            p_size = 0;
            speed=0;
            slave = 0;
            t_count=0 ;
            t_max=2;
            iter = 0;
        break;
        case RECEIVE:
                 //if( (PINB & 0x10) == 0){
                    data = SPI_ServantReceive();
                    slaveData();
                // }                    
                //PORTA = data; 
                // PORTA = ( (PORTA & ~mask) | ( (receivedData) & mask) );         
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


int Tick4(int state){
    
    switch(state){
        case START3:
        break;
        case ON:
           // PORTA = ~PORTA;
            if(t_count >= t_max){
                    t_count = 0;
                    PORTA = patterns[pattern][iter];
                    ++iter;
                    if(iter >= p_size) iter = 0;
            }else ++t_count;    
     
        break;
        default:
        break;
    }
    
    switch(state){
        case START3:
            state = ON;
        break;
        case ON:
        break;
        default:break;
    }
    return state;
}

int main(void)
{
    DDRA = 0xFF; PORTA = 0xAA;
    
    DDRC = 0xF0; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    TimerSet(50);
    TimerOn();
    
    maskUp = 0xF0;
    maskLow = 0x0F;
    
    unsigned long t2 = 1;
    unsigned long t4 = 2;
    
    static task task2, task4;
    task *tasks[] = {  &task2, &task4};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    
    task2.state = 0;//Task initial state.
    task2.period = t2;//Task Period.
    task2.elapsedTime = t2; // Task current elapsed time.
    task2.TickFct = &Tick2; // Function pointer for the tick.
    
    task4.state = 0;//Task initial state.
    task4.period = t4;//Task Period.
    task4.elapsedTime = t4; // Task current elapsed time.
    task4.TickFct = &Tick4; // Function pointer for the tick.
    
    data = 0;
    
     SPI_ServantInit();
    
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

void slaveData(){ 
    pattern = (data >> 4);
    speed = (data & 0x0F);
    t_max = speeds[speed];
    p_size = sizes[pattern];
}
