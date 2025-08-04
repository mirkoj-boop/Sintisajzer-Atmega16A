#include "AVR lib/AVR_lib.h"
#include <avr/io.h>
#include <util/delay.h>
#include "I2C/i2c.h"
#include "USART/USART.h"

// ISPRAVKA 1: Koristite 7-bit adrese, ne 8-bit
// Vaš scanner je pokazao 0x21, 0x22, 0x23
#define MCP23008_ADDR1 0x21  // Bilo: 0x42
#define MCP23008_ADDR2 0x22  // Bilo: 0x44
#define MCP23008_ADDR3 0x23  // Bilo: 0x48

// Registri MCP23008
#define IODIR   0x00
#define GPPU    0x06  // DODANO: Pull-up registar
#define GPIO    0x09

static const int note_freqs[24] = {
	262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, // C4 do B4
	523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988  // C5 do B5
};

// ISPRAVKA 2: Dodaj << 1 shift u funkciji, ne u #define
void mcp23008_init(uint8_t addr) {
	uint8_t write_addr = addr << 1;  // DODANO: Shift ovdje
	
	if (i2c_start(write_addr) != 0) {  // ISPRAVKA: Uklonjen |TW_WRITE
		usart_write_string("Alo maci ne radi\r\n");
		return;
	}
	i2c_write(IODIR);
	usart_write_string("2.\r\n");
	i2c_write(0xFF);
	usart_write_string("3.\r\n");
	i2c_stop();
	
	// DODANO: Inicijaliziraj pull-up otpornike
	if (i2c_start(write_addr) != 0) return;
	i2c_write(GPPU);
	i2c_write(0xFF);  // Uklju?i pull-up na svim pinovima
	i2c_stop();
	
	usart_write_string("4.\r\n");
}

// ISPRAVKA 3: Dodaj << 1 shift i u read funkciju
uint8_t mcp23008_read(uint8_t addr) {
	uint8_t write_addr = addr << 1;      // DODANO
	uint8_t read_addr = write_addr | 0x01;  // DODANO
	
	i2c_start(write_addr);               // ISPRAVKA: Uklonjen |TW_WRITE
	i2c_write(GPIO);
	i2c_rep_start(read_addr);            // ISPRAVKA: Uklonjen |TW_READ
	uint8_t data = i2c_read_nack();
	i2c_stop();
	return data;
}

int main(void) {
	// Inicijalizacija
	BUZZ(0.2, 440);
	usart_init(9600);
	usart_write_string("USART initialized.\r\n");
	
	i2c_init(100000);
	usart_write_string("I2C initialized.\r\n");
	
	mcp23008_init(MCP23008_ADDR1);
	usart_write_string("MCP 1 initialized.\r\n");
	mcp23008_init(MCP23008_ADDR2);
	usart_write_string("MCP 2 initialized.\r\n");
	mcp23008_init(MCP23008_ADDR3);
	usart_write_string("MCP 3 initialized.\r\n");
	
	uint8_t prev_state[3] = {0xFF, 0xFF, 0xFF};
	
	usart_write_string("System initialized. Monitoring MCP23008 states...\r\n");
	
	while (1) {
		uint8_t states[3] = {
			mcp23008_read(MCP23008_ADDR1),
			mcp23008_read(MCP23008_ADDR2),
			mcp23008_read(MCP23008_ADDR3)
		};
		
		// Debug ispis - zadržano kao u originalu
		usart_write("MCP1: 0x%02X, MCP2: 0x%02X, MCP3: 0x%02X\r\n",
		states[0], states[1], states[2]);
		
		for (uint8_t chip = 0; chip < 3; chip++) {
			for (uint8_t pin = 0; pin < 8; pin++) {
				if (!(states[chip] & (1 << pin)) && (prev_state[chip] & (1 << pin))) {
					uint8_t note_index = chip * 8 + pin;
					
					// ISPRAVKA 4: Provjeri da note_index nije izvan granica
					if (note_index < 24) {
						usart_write("Key pressed: Chip %d, Pin %d, Note %d Hz\r\n",
						chip + 1, pin, note_freqs[note_index]);
						BUZZ(0.2, note_freqs[note_index]);
					}
				}
			}
			prev_state[chip] = states[chip];
		}
		
		_delay_ms(100);  // ISPRAVKA 5: Bilo *delay*ms
	}
	
	return 0;
}