/*
 * SPI_SLAVE.c
 *
 * Created: 10/9/2019 5:33:08 PM
 * Author : Administrator
 */ 

#ifndef F_CPU
#define F_CPU 20000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include "io.c"
#include "lcd.h"

#define ACK 0x7E

void spi_init_slave (void)
{
    DDRB= (1<<PORTB6);                                  //MISO as OUTPUT
    SPCR=(1<<SPE);                                //Enable SPI
}

//Function to send and receive data
unsigned char spi_tranceiver (unsigned char data)
{
    SPDR = data;                                  //Load data into buffer
    while(!(SPSR & (1<<SPIF) )) {              //Wait until transmission complete
    }            
    return SPDR;                                 //Return received data
}

int main(void)
{
    //lcd_init(LCD_DISP_ON_CURSOR_BLINK);           //Initialize LCD
    spi_init_slave();                             //Initialize slave SPI
    unsigned char data, buffer[10];
    DDRA  = 0xFF;                                 //Initialize PORTA as INPUT
    PORTA = 0xFF;                                 //Enable Pull-Up Resistors
    
    char doot = 0x10; 
    while(1)
    {
        doot = spi_tranceiver(ACK);
        PORTA = doot;
        /*
        lcd_clrscr();                             //LCD Clear screen
        lcd_home();                               //LCD move cursor to home
        lcd_puts("Testing");
        lcd_gotoxy(0,1);
        data = spi_tranceiver(ACK);               //Receive data, send ACK
        itoa(data, buffer, 10);                   //Convert integer into string
        lcd_puts(buffer);                         //Display received data
        */
        _delay_ms(5);                            //Wait
    }
}
