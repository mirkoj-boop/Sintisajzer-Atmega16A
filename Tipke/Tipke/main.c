#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define MCP4725_ADDR 0x61
#define MCP23008_BASE_ADDR 0x21

#include "i2c.h"

volatile uint16_t current_frequency = 0;

// Lookup table: frekvencije za 24 tipke (C4 ? B5)
const uint16_t notes[24] = {
	262, 277, 294, 311, 330, 349, 370, 392,   // C4 - G4
	415, 440, 466, 494, 523, 554, 587, 622,   // G#4 - D#5
	659, 698, 740, 784, 831, 880, 932, 988    // E5 - B5
};

// Funkcija za slanje DAC vrijednosti (0-4095)
void dac_output(uint16_t value) {
	i2c_start();
	i2c_write((MCP4725_ADDR << 1) | 0);
	i2c_write(0x40); // Fast mode
	i2c_write(value >> 4);
	i2c_write((value & 0x0F) << 4);
	i2c_stop();
}

// Timer1 ISR: toggla square wave na DAC
ISR(TIMER1_COMPA_vect) {
	static uint8_t state = 0;
	if (state) {
		dac_output(4095);
		} else {
		dac_output(0);
	}
	state = !state;
}

// Postavi Timer1 za zadanu frekvenciju
void set_frequency(uint16_t freq) {
	if (freq == 0) {
		TCCR1B = 0; // zaustavi timer
		dac_output(0);
		return;
	}

	uint32_t ocr = (F_CPU / (2UL * freq)) - 1;
	cli();
	OCR1A = (uint16_t)ocr;
	TCNT1 = 0;
	TCCR1B = (1 << WGM12) | (1 << CS10); // CTC, no prescaler
	sei();
}

// Inicijaliziraj MCP23008 na zadanoj adresi
void mcp23008_init(uint8_t addr) {
	i2c_start();
	i2c_write((addr << 1) | 0);
	i2c_write(0x00); // IODIR
	i2c_write(0xFF); // svi ulazi
	i2c_stop();

	i2c_start();
	i2c_write((addr << 1) | 0);
	i2c_write(0x06); // GPPU
	i2c_write(0xFF); // pull-up enable
	i2c_stop();
}

// ?itaj GPIO s MCP23008
uint8_t mcp23008_read(uint8_t addr) {
	uint8_t data;

	i2c_start();
	i2c_write((addr << 1) | 0);
	i2c_write(0x09); // GPIO
	i2c_start();
	i2c_write((addr << 1) | 1);
	data = i2c_read_nack();
	i2c_stop();

	return data;
}

int main(void) {
	i2c_init();

	// Init MCP23008 ekspandere
	for (uint8_t i = 0; i < 3; i++) {
		mcp23008_init(MCP23008_BASE_ADDR + i);
	}

	// Init Timer1
	TCCR1A = 0;
	TCCR1B = 0;
	TIMSK1 = (1 << OCIE1A);

	sei();

	uint8_t last_state[3] = {0xFF, 0xFF, 0xFF}; // za debouncing
	uint8_t stable_state[3] = {0xFF, 0xFF, 0xFF};

	while (1) {
		for (uint8_t exp = 0; exp < 3; exp++) {
			uint8_t state = mcp23008_read(MCP23008_BASE_ADDR + exp);

			// Debounce
			if (state != last_state[exp]) {
				_delay_ms(5); // debounce delay
				state = mcp23008_read(MCP23008_BASE_ADDR + exp);
				last_state[exp] = state;
			}

			if (state != stable_state[exp]) {
				stable_state[exp] = state;

				// Provjeri tipke
				for (uint8_t bit = 0; bit < 8; bit++) {
					if (!(state & (1 << bit))) {
						// Tipka pritisnuta
						current_frequency = notes[exp * 8 + bit];
						set_frequency(current_frequency);
						goto next_scan;
					}
				}
			}
		}
		// Ako nijedna tipka nije pritisnuta
		set_frequency(0);

		next_scan:
		;
	}
}
