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
#include <string.h>
#include "io.c"
#include "spi.h"
#include "keypad.h"

enum States{ START, LISTEN, SEND };
enum States2{ START2, RECIEVE };
enum States3{ START3, ON};
char data, key, master, maskUp, maskLow;
char pattern, p_size, speed, slave, t_count, t_max; 
int iter;

char p1[2] = {0xF0, 0x0F};
char p2[2] = {0xAA, 0x55};
char p3[14] = {128,64,32,16,8,4,2,1,2,4,8,16,32,64};
char p4[18] = {0,1,3,7,15,31,63,127,255,127,63,31,15,7,5,3,1,0};        

char *paterns[4] = {p1,p2,p3,p4};   
char sizes[4] = {2,2,14,17}; 
char speeds[] = {40,20,10,5,2,1};
    
char message_1[9] = "Ptrn: 0 ";
char message_2[7] = "Spd: 0";
char message_3[6] = "uC: 0";


void keyAssign();
void slaveData();

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
            if(master) {
                key = GetKeypadKey();
                if(key != '\0' ){
                    keyAssign();
                 }
            }else  slaveData();           
            /*if(!master){                                   
                PORTA = ( (PORTA & ~mask) | ( (data) & mask) );
            }*/               
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

int Tick4(int state){
    
    switch(state){
        case START3:
            pattern =0; 
            p_size = 0; 
            speed=0; 
            slave = 0; 
            t_count=0 ;
            t_max=0;
            iter = 0;
            if(master) key = 10;
        break;
        case ON:
            if(t_count == t_max){
                if(master && (key != '\0' )) {
                    message_1[6] = (pattern +'1');
                    message_2[5] = (speed +'1');
                    message_3[4] = (slave +'0');
                    char line_1[17] = {};
                    strcat(line_1, message_1);
                    strcat(line_1, message_2);
                     
                    LCD_DisplayString( 1, (unsigned char*)line_1);
                    //LCD_DisplayString( 17, (unsigned char*)message_2);
                    t_count = 0;
                }
            }else ++t_count;                
        break;
        default:
        break;
    }
    
    switch(state){
        case START:
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
    DDRA = 0xFF; PORTA = 0x10;
    //DDRB = 0xFE; PORTB = 0x00;
    DDRC = 0xF0; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    
    TimerSet(50);
    TimerOn();
    
    LCD_init();
    LCD_ClearScreen();
    //LCD_Cursor(1);   
    //LCD_DisplayString( 1, (unsigned char*)"YO");
    
    maskUp = 0xF0;
    maskLow = 0x0F;
    
    unsigned long t1 = 6;
    unsigned long t2 = 2;
    unsigned long t3 = 1;
    unsigned long t4 = 1;
    
    static task task1, task2, task3, task4;
    task *tasks[] = { &task3, &task1, &task2, &task4};
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
    
    task4.state = 0;//Task initial state.
    task4.period = t4;//Task Period.
    task4.elapsedTime = t4; // Task current elapsed time.
    task4.TickFct = &Tick4; // Function pointer for the tick.
    
    data = 0;
    
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

void keyAssign(){
    
    switch(key){
        case 'A':
            pattern = 0;
            p_size = 2;
        break;
        case 'B':
            pattern = 1;
        p_size = 2;
        break;
        case 'C':
            pattern = 2;
        p_size = 14;
        break;
        case 'D':
            pattern = 3;
            p_size = 17;
        break;
        case '1':
            speed = 0;
        break;
        case '2':
            speed = 1;
        break;
            case '3':
            speed = 2;
        break;
        case '4':
            speed = 3;
        break;
        case '5':
            speed = 4;
        break;
        case '6':
            speed = 5;
        break;
        case '7':
            slave = 0;
        break;
        case '8':
            slave = 1;
        break;
        case '9':
            slave = 2;
        break;
        default:
        break;
    }
    
    data = ( (data & ~maskUp) | ( (pattern << 4) & maskUp) )
    | ( ( data & ~maskLow) | ( (speed) & maskLow) );
}

void slaveData(){ 
    pattern = (data >> 4);
    speed = (data & 0x0F);
    p_size = sizes[data];
}