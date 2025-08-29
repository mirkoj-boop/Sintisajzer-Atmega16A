#include "AVR lib/AVR_lib.h"
#include <avr/io.h>
#include <util/delay.h>
#include "I2C/i2c.h"
#include "LCD/lcd.h"


#define MCP23008_ADDR1 0x21
#define MCP23008_ADDR2 0x22
#define MCP23008_ADDR3 0x23


#define IODIR   0x00
#define GPPU    0x06  
#define GPIO    0x09

static const int note_freqs[24] = {
	262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, // C4 do B4
	523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988  // C5 do B5
};


uint8_t display_mode = 0;  // 0 = PWM, 1 = sawtooth
uint8_t pb0_prev_state = 1;  

void mcp23008_init(uint8_t addr) {
	uint8_t write_addr = addr << 1;  // Shift za I2C write adresu
	
	
	if (i2c_start(write_addr) != 0) {
		return; 
	}
	i2c_write(IODIR);
	i2c_write(0xFF);  
	i2c_stop();
	
	
	if (i2c_start(write_addr) != 0) return;
	i2c_write(GPPU);
	i2c_write(0xFF);  
	i2c_stop();
}

uint8_t mcp23008_read(uint8_t addr) {
	uint8_t write_addr = addr << 1;
	uint8_t read_addr = write_addr | 0x01;
	
	i2c_start(write_addr);
	i2c_write(GPIO);
	i2c_rep_start(read_addr);
	uint8_t data = i2c_read_nack();
	i2c_stop();
	return data;
}

void inicijalizacija() {
	input_port(DDRB, PB0);  
	set_port(PORTB, PB0, 1); 
	
	lcd_init(); 
}

void update_display() {
	lcd_clrscr();
	lcd_home();
	
	if (display_mode == 0) {
		lcd_print("PWM");
		} else {
		lcd_print("sawtooth");
	}
}

int main(void) {
	inicijalizacija();
	lcd_clrscr();
	lcd_home();
	
	lcd_print("Welcome");
	_delay_ms(1000);  
	
	
	update_display();
	
	
	SAWTOOTH(0.2, 440);  
	
	i2c_init(100000);  
	
	
	mcp23008_init(MCP23008_ADDR1);
	mcp23008_init(MCP23008_ADDR2);
	mcp23008_init(MCP23008_ADDR3);
	
	
	_delay_ms(100);
	
	
	SAWTOOTH(0.1, 523);
	_delay_ms(100);
	SAWTOOTH(0.1, 659);
	_delay_ms(100);
	SAWTOOTH(0.1, 784);
	
	uint8_t prev_state[3] = {0xFF, 0xFF, 0xFF};  
	
	while (1) {
		
		uint8_t pb0_current_state = get_pin(PINB, PB0);
		
	
		if (pb0_current_state == 0 && pb0_prev_state == 1) {
			
			display_mode = !display_mode;
			update_display();
			
			
			BUZZ(0.1, 880);
			
			
			_delay_ms(200);
		}
		
		
		pb0_prev_state = pb0_current_state;
		
		
		uint8_t states[3] = {
			mcp23008_read(MCP23008_ADDR1),
			mcp23008_read(MCP23008_ADDR2),
			mcp23008_read(MCP23008_ADDR3)
		};
		
		
		for (uint8_t chip = 0; chip < 3; chip++) {
			for (uint8_t pin = 0; pin < 8; pin++) {
				
				if (!(states[chip] & (1 << pin)) && (prev_state[chip] & (1 << pin))) {
					uint8_t note_index = chip * 8 + pin;
					
				
					if (note_index < 24) {
					
						BUZZ(0.2, note_freqs[note_index]);
					}
				}
			}
		
			prev_state[chip] = states[chip];
		}
		
		_delay_ms(50);  
	}
	
	return 0;
}