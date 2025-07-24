
#include "AVR lib/AVR_lib.h"
#include <avr/io.h>
#include <util/delay.h>
#include "I2C/i2c.h"
#define MCP23008_ADDR1 0x42 // 0x21 << 1
#define MCP23008_ADDR2 0x44 // 0x22 << 1
#define MCP23008_ADDR3 0x46 // 0x23 << 1

// Registri MCP23008
#define IODIR   0x00 // Smjer pina (0 = izlaz, 1 = ulaz)
#define GPIO    0x09 // GPIO registar (?itanje/pisanje stanja pinova)

// Frekvencije za 24 tipke (C4 do B5, 2 oktave)
static const int note_freqs[24] = {
	262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, // C4 do B4
	523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988  // C5 do B5
};

// Inicijalizacija MCP23008
void mcp23008_init(uint8_t addr) {
	i2c_start(addr | I2C_WRITE);
	i2c_write(IODIR); // Postavi registar IODIR
	i2c_write(0xFF);  // Svi pinovi su ulazi
	i2c_stop();
}

// ?itanje stanja tipki s MCP23008
uint8_t mcp23008_read(uint8_t addr) {
	i2c_start(addr | I2C_WRITE);
	i2c_write(GPIO); // Postavi GPIO registar
	i2c_rep_start(addr | I2C_READ);
	uint8_t data = i2c_read_nack();
	i2c_stop();
	return data;
}

int main(void) {
	// Inicijalizacija
	i2c_init(100000); // I2C na 100 kHz
	mcp23008_init(MCP23008_ADDR1);
	mcp23008_init(MCP23008_ADDR2);
	mcp23008_init(MCP23008_ADDR3);
	
	// Polje za pra?enje stanja tipki
	uint8_t prev_state[3] = {0xFF, 0xFF, 0xFF}; // Pretpostavljamo pull-up (1 = tipka nije pritisnuta)
	
	while (1) {
		// ?itaj stanje tipki s tri MCP23008 ?ipa
		uint8_t states[3] = {
			mcp23008_read(MCP23008_ADDR1),
			mcp23008_read(MCP23008_ADDR2),
			mcp23008_read(MCP23008_ADDR3)
		};
		
		// Provjeri svaku tipku
		for (uint8_t chip = 0; chip < 3; chip++) {
			for (uint8_t pin = 0; pin < 8; pin++) {
				// Ako je tipka pritisnuta (0) i prije nije bila (1)
				if (!(states[chip] & (1 << pin)) && (prev_state[chip] & (1 << pin))) {
					uint8_t note_index = chip * 8 + pin; // Indeks note (0-23)
					BUZZ(0.2, note_freqs[note_index]); // Sviraj notu 200 ms
				}
			}
			prev_state[chip] = states[chip]; // Spremi trenutno stanje
		}
		
		_delay_ms(10); // Mali delay za smanjenje optere?enja
	}
	
	return 0;
}