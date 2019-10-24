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
enum LEDState {INIT,L0,L1,} led_state0, led_val, button ;

void LEDS_Init(){
	led_state0 = INIT;
}

void LEDS_Tick0(){
	//Actions
	switch(led_state0){
		case INIT:
            PORTD = 0;
            led_val = 1;
            break;
		case L0:
            led_val = (led_val << 1);
            break;
		case L1:
            led_val = (led_val >> 1);
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
            if(led_val == 0x80 || button){
                led_state0 = L1;
                button = 0;
            }                           
            break;
		case L1:
			 if(led_val == 0x01  || button){
                led_state0 = L0;
                button = 0;    
             }
            break;
		default:
			led_state0 = INIT;
            break;
	}
    PORTD = led_val;
}



void LedSecTask0() {
    LEDS_Init();
    for(;;) { 	
        LEDS_Tick0();
        vTaskDelay(500); 
    } 
}

void ButtonTask() {
    for(;;) {
        if( 
        (PINA & 0x01) == 0) button = 0xFF; 
        vTaskDelay(10);
    }
}



void StartSecPulse0(unsigned portBASE_TYPE Priority) {
    xTaskCreate(LedSecTask0, 
            (signed portCHAR *)"LedSecTask0", 
            configMINIMAL_STACK_SIZE, 
            NULL, 
            Priority, 
            NULL );
}	

void StartSecPulse1(unsigned portBASE_TYPE Priority) {
    xTaskCreate(ButtonTask,
    (signed portCHAR *)"ButtonTask",
    configMINIMAL_STACK_SIZE,
    NULL,
    Priority,
    NULL );
}
 
int main(void) { 
    DDRA = 0x00; PORTA=0xFF;
    DDRD = 0xFF;
    //Start Tasks  
    StartSecPulse0(1);
    StartSecPulse1(50);
    //RunSchedular 
    vTaskStartScheduler(); 
    return 0; 
}
