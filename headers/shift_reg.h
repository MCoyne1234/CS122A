#include <avr/io.h>

#define SHIFT_REG PORTA
#define SER 	  PORTA4 
#define OE 		  PORTA3
#define RCLK 	  PORTA2
#define SRCLK 	  PORTA1
#define SRCLR 	  PORTA0

#define OE2		  PORTA5
#define RCLK2	  PORTA6
#define SRCLR2	  PORTA7

enum disp_state{DISP_OFF, DISP_ON};
    
sr_display(unsigned char reg, int state){
	if(reg = 0){
		PORTA = (PORTA & ~(1 << OE)) | (state << OE);	
	}else{
		PORTA = (PORTA & ~(1 << OE2)) | (state << OE2);
	}
}  

void transmit_data(unsigned char data) {
	// SER, RCLK, SRCLK, SRCLR
	// when ready to begin transmitting, set RCLK low and SRCLR high
	// for each bit of data, set SRCLR high and RCLK Low,
	// Then for each bit, set SRCLK low, then set SER to the value of the
	// bit being transfered (1 or 0), then set SRCLK high
	unsigned char i = 0, bit = 0;
	unsigned char disp = ((SHIFT_REG >> OE) & 0x01) << OE;
	while(i < 8) {
		bit = ( (data >> i) & 0x01) << SER;
		SHIFT_REG = (1 << SRCLR);
		//SHIFT_REG |= ((data >> i) & SER);
		SHIFT_REG &= ~(1 << SER);
		SHIFT_REG |= bit;
		
		SHIFT_REG |= (1 << SRCLK);
		i++;
	}
	
	SHIFT_REG |= (1 << RCLK);
	SHIFT_REG = (0x00 | disp);
}

void transmit_data_2(unsigned char data, unsigned char reg) {
	// SER, RCLK, SRCLK, SRCLR
	// when ready to begin transmitting, set RCLK low and SRCLR high
	// for each bit of data, set SRCLR high and RCLK Low,
	// Then for each bit, set SRCLK low, then set SER to the value of the
	// bit being transfered (1 or 0), then set SRCLK high
	unsigned char output = SHIFT_REG;
	unsigned char out_en, reg_clk, sreg_clr;
	
	if(!reg){
		out_en = OE;
		reg_clk = RCLK;
		sreg_clr = SRCLR;
	}else{
		out_en = OE2;
		reg_clk = RCLK2;
		sreg_clr = SRCLR2;
	}
	
	unsigned char i = 0, bit = 0;
	unsigned char disp = ((output >> out_en) & 0x01) << out_en;
	while(i < 8) {
		bit = ( (data >> i) & 0x01) << SER;
		output = (1 << sreg_clr);
		//SHIFT_REG |= ((data >> i) & SER);
		output &= ~(1 << SER);
		output |= bit;
		
		output |= (1 << SRCLK);
		i++;
	}
	
	output |= (1 << reg_clk);
	output = (0x00 | disp);
	
	SHIFT_REG = output;
}