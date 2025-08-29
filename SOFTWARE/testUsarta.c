#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "I2C/i2c.h"
#include "USART/USART.h"

// MCP23008 definicije
#define MCP23008_ADDR1   0x21  // Prvi MCP23008
#define MCP23008_ADDR2   0x22  // Drugi MCP23008
#define MCP23008_ADDR3   0x23  // Tre?i MCP23008

// MCP23008 registri
#define MCP23008_IODIR   0x00  // I/O Direction (1=input, 0=output)
#define MCP23008_IPOL    0x01  // Input Polarity (1=inverted)
#define MCP23008_GPINTEN 0x02  // Interrupt enable
#define MCP23008_DEFVAL  0x03  // Default value
#define MCP23008_INTCON  0x04  // Interrupt control
#define MCP23008_IOCON   0x05  // Configuration
#define MCP23008_GPPU    0x06  // Pull-up enable (1=pull-up ON)
#define MCP23008_INTF    0x07  // Interrupt flag
#define MCP23008_INTCAP  0x08  // Interrupt capture
#define MCP23008_GPIO    0x09  // GPIO port
#define MCP23008_OLAT    0x0A  // Output latch

// Funkcije za rad s MCP23008
uint8_t mcp23008_write_reg(uint8_t device_addr, uint8_t reg, uint8_t value) {
	uint8_t write_addr = device_addr << 1;
	
	if (i2c_start(write_addr) != 0) return 1;
	if (i2c_write(reg) != 0) return 1;
	if (i2c_write(value) != 0) return 1;
	i2c_stop();
	return 0;
}

uint8_t mcp23008_read_reg(uint8_t device_addr, uint8_t reg) {
	uint8_t write_addr = device_addr << 1;
	uint8_t read_addr = write_addr | 0x01;
	
	if (i2c_start(write_addr) != 0) return 0xFF;
	if (i2c_write(reg) != 0) return 0xFF;
	if (i2c_rep_start(read_addr) != 0) return 0xFF;
	
	uint8_t data = i2c_read_nack();
	i2c_stop();
	return data;
}

// Inicijalizacija MCP23008 za ?itanje tipki
void mcp23008_init_keypad(uint8_t device_addr) {
	// Postavke za ?itanje tipki:
	mcp23008_write_reg(device_addr, MCP23008_IODIR, 0xFF);  // Svi pinovi kao ulazi
	mcp23008_write_reg(device_addr, MCP23008_GPPU, 0xFF);   // Uklju?i pull-up otpornike
	mcp23008_write_reg(device_addr, MCP23008_IPOL, 0x00);   // Normalna polarnost
	
	usart_write_string("MCP23008 0x");
	print_hex(device_addr);
	usart_write_string(" inicijaliziran za tipke\r\n");
	
	_delay_ms(10);
}

// Pomo?ne funkcije za ispis
void print_hex(uint8_t value) {
	uint8_t high = (value >> 4) & 0x0F;
	uint8_t low = value & 0x0F;
	
	usart_write_char(high < 10 ? '0' + high : 'A' + high - 10);
	usart_write_char(low < 10 ? '0' + low : 'A' + low - 10);
}

void print_binary(uint8_t value) {
	for (int8_t i = 7; i >= 0; i--) {
		usart_write_char((value & (1 << i)) ? '1' : '0');
	}
}

// citanje i prikaz stanja tipki
void read_keypad(uint8_t device_addr) {
	uint8_t gpio_state = mcp23008_read_reg(device_addr, MCP23008_GPIO);
	uint8_t pressed_keys = ~gpio_state; 
	
	if (pressed_keys != 0) {
		usart_write_string("MCP 0x");
		print_hex(device_addr);
		usart_write_string(": ");
		
		
		for (uint8_t i = 0; i < 8; i++) {
			if (pressed_keys & (1 << i)) {
				usart_write_string("Tipka ");
				usart_write_char('0' + i);
				usart_write_string(" ");
			}
		}
		
		usart_write_string("(Binary: ");
		print_binary(pressed_keys);
		usart_write_string(")\r\n");
	}
}


void scan_all_keypads(void) {
	static uint8_t old_states[3] = {0, 0, 0}; // Pamti prethodna stanja
	uint8_t devices[] = {MCP23008_ADDR1, MCP23008_ADDR2, MCP23008_ADDR3};
	
	for (uint8_t i = 0; i < 3; i++) {
		uint8_t gpio_state = mcp23008_read_reg(devices[i], MCP23008_GPIO);
		uint8_t pressed_keys = ~gpio_state;
		
		// Provjeri ima li promjena (novi pritisak tipke)
		uint8_t new_presses = pressed_keys & (~old_states[i]);
		
		if (new_presses != 0) {
			usart_write_string(">>> NOVA TIPKA MCP 0x");
			print_hex(devices[i]);
			usart_write_string(": ");
			
			for (uint8_t j = 0; j < 8; j++) {
				if (new_presses & (1 << j)) {
					usart_write_string("Pin ");
					usart_write_char('0' + j);
					usart_write_string(" ");
				}
			}
			usart_write_string("\r\n");
		}
		
		old_states[i] = pressed_keys;
		_delay_ms(5);
	}
}

// citam samo jedan mcp23008 ako radi uopce
void scan_single_keypad(uint8_t device_addr) {
	static uint8_t old_state = 0;
	
	uint8_t gpio_state = mcp23008_read_reg(device_addr, MCP23008_GPIO);
	uint8_t pressed_keys = ~gpio_state; // Inverzija za active-low tipke
	
	// Provjeri na bridu da li se nešto doga?a
	uint8_t new_presses = pressed_keys & (~old_state);
	uint8_t releases = old_state & (~pressed_keys);
	
	if (new_presses != 0) {
		usart_write_string("PRITISNUT: ");
		for (uint8_t i = 0; i < 8; i++) {
			if (new_presses & (1 << i)) {
				usart_write_char('0' + i);
				usart_write_char(' ');
			}
		}
		usart_write_string("\r\n");
	}
	
	if (releases != 0) {
		usart_write_string("OTPUSTEN: ");
		for (uint8_t i = 0; i < 8; i++) {
			if (releases & (1 << i)) {
				usart_write_char('0' + i);
				usart_write_char(' ');
			}
		}
		usart_write_string("\r\n");
	}
	
	old_state = pressed_keys;
}


int main(void) {
	
	usart_init(9600);
	sei();
	i2c_init(100000UL);
	
	usart_write_string("MCP23008 Keypad Test\r\n");
	_delay_ms(500);
	
	
	mcp23008_init_keypad(MCP23008_ADDR1);
	mcp23008_init_keypad(MCP23008_ADDR2);
	mcp23008_init_keypad(MCP23008_ADDR3);
	
	usart_write_string("\r\nPritisnite tipke...\r\n\r\n");
	
	
	while (1) {
		
		scan_all_keypads();
		
	
		
		_delay_ms(50); a
	}
	
	return 0;
}