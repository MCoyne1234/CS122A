/*
 * mcoyn001_xfern001_lab04_2
 *
 * Created: 10/8/2019 8:56:27 PM
 * Author : Administrator
 * 
 *    Partner(s) Name & E-mail: Matthew Coyne mcoyn001@ucr.edu
 *    Lab Section: 021
 *    Assignment: Lab #3  Exercise #2 
 *    Exercise Description: RTOS 
 *      Challange: All exercises demoed 
 *    I acknowledge all content contained herein, excluding template 
 *     or example code, is my own original work.
 */ 

#include <stdint.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <stdbool.h> 
#include <string.h> 
#include <math.h> 
#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <avr/eeprom.h> 
#include <avr/portpins.h> 
#include <avr/pgmspace.h> 
 
//FreeRTOS include files 
#include "FreeRTOS.h" 
#include "task.h" 
#include "croutine.h" 
enum LEDState {INIT,L0,L1,L2,L3,L4,L5,L6,L7} led_state0,led_state1,led_state2 ;

void LEDS_Init(){
	led_state0 = INIT;
}

void LEDS_Tick0(){
	//Actions
	switch(led_state0){
		case INIT:
            PORTD = 0;
            break;
		case L0:
            PORTD = ( (1<<PORTD0)| PORTD);
            break;
		case L1:
            PORTD = ( ~(1<<PORTD0) & PORTD);
            break;
		default:
            PORTD = 0;
            break;
	}
	//Transitions
	switch(led_state0){
		case INIT:
			led_state0 = L0;
            break;
		case L0:
			led_state0 = L1;
            break;
		case L1:
			led_state0 = L0;
            break;
		default:
			led_state0 = INIT;
            break;
	}
}

void LEDS_Tick1(){
    //Actions
    switch(led_state1){
        case INIT:
        PORTD = 0;
        break;
        case L0:
        PORTD = ( (1<<PORTD2)| PORTD);
        break;
        case L1:
        PORTD = ( ~(1<<PORTD2) & PORTD);
        break;
        default:
        PORTD = 0;
        break;
    }
    //Transitions
    switch(led_state1){
        case INIT:
        led_state1 = L0;
        break;
        case L0:
        led_state1 = L1;
        break;
        case L1:
        led_state1 = L0;
        break;
        default:
        led_state1 = INIT;
        break;
    }
}

void LEDS_Tick2(){
    //Actions
    switch(led_state2){
        case INIT:
        PORTD = 0;
        break;
        case L0:
        PORTD = ( (1<<PORTD4)| PORTD);
        break;
        case L1:
        PORTD = ( ~(1<<PORTD4) & PORTD);
        break;
        default:
        PORTD = 0;
        break;
    }
    //Transitions
    switch(led_state2){
        case INIT:
        led_state2 = L0;
        break;
        case L0:
        led_state2 = L1;
        break;
        case L1:
        led_state2 = L0;
        break;
        default:
        led_state2 = INIT;
        break;
    }
}

void LedSecTask0() {
    LEDS_Init();
    for(;;) { 	
        LEDS_Tick0();
        vTaskDelay(500); 
    } 
}

void LedSecTask1() {
    LEDS_Init();
    for(;;) {
        LEDS_Tick1();
        vTaskDelay(1000);
    }
}

void LedSecTask2() {
    LEDS_Init();
    for(;;) {
        LEDS_Tick2();
        vTaskDelay(2500);
    }
}


void StartSecPulse(unsigned portBASE_TYPE Priority) {
    xTaskCreate(LedSecTask0, 
            (signed portCHAR *)"LedSecTask0", 
            configMINIMAL_STACK_SIZE, 
            NULL, 
            Priority, 
            NULL );
    xTaskCreate(LedSecTask1,
        (signed portCHAR *)"LedSecTask1",
        configMINIMAL_STACK_SIZE,
        NULL,
        Priority,
        NULL );
    
    xTaskCreate(LedSecTask2,
        (signed portCHAR *)"LedSecTask2",
        configMINIMAL_STACK_SIZE,
        NULL,
        Priority,
        NULL );
}	
 
int main(void) { 
    DDRA = 0x00; PORTA=0xFF;
    DDRD = 0xFF;
    //Start Tasks  
    StartSecPulse(1);
    //RunSchedular 
    vTaskStartScheduler(); 
    return 0; 
}
