/*
 * mcoyn001_lab01.c
 *
 * Created: 10/2/2019 3:23:30 PM
 * Author : Administrator
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include <timer.h>
#include <stdio.h>
#include <scheduler.h>
#include <lcd.h>

 enum States{ START, ONE, TWO, THREE};
 enum States2{ START2, ON, OFF};
 int button; 

 int Tick(int state){
     
     switch(state){
         case START:
            PORTB = 0x00;
         break;
         case ONE:
            PORTB = PORTB & 0xF8;
            PORTB = PORTB | 0x01;
         break;
         case TWO:
            PORTB = PORTB & 0xF8;
            PORTB = PORTB | 0x02;
         break;
         case THREE:
            PORTB = PORTB & 0xF8;
            PORTB = PORTB | 0x04;
         break;
         default:
         break;
     }
     
     switch(state){
         case START:
            state = ONE;
         break;
         case ONE:
            state = TWO;
         break;
         case TWO:
            state = THREE;
         break;
         case THREE:
            state = START;
         break;
         default:break;
     }
     return state;
 }     
 
  int Tick2(int state){
      
      switch(state){
          case START2:
          PORTB = 0x00;
          break;
          case ON:
          PORTB = PORTB | 0x08;
          break;
          case OFF:
          PORTB = PORTB & 0xF7;
          break;
          default:
          break;
      }
      
      switch(state){
          case START2:
          state = ON;
          break;
          case ON:
          state = OFF;
          break;
          case OFF:
          state = ON;
          break;
          default:break;
      }
      return state;
  }
int main(void)
{   
   DDRA = 0x00; PORTA = 0x00;
   DDRB = 0xFF; PORTB = 0x00;
    
    TimerSet(500);
    TimerOn();
        
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
    task2.elapsedTime = t2; // Task current elasped time.
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
            butt = ( !(PINA & 0x04) );
            if( butt){
                button = 1;
                while(button == 1 ){
                    butt = ( !(PINA & 0x04) );
                    if( butt ) button = 0;
                }
            }
        };
        TimerFlag = 0;
    }
       return 0;
}

