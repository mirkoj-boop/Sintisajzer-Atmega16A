#include "AVR lib/AVR_lib.h"
#include <avr/io.h>
#include <util/delay.h>
#include "I2C/i2c.h"
#include "LCD/lcd.h"

// 7-bit adrese MCP23008 ?ipova
#define MCP23008_ADDR1 0x21
#define MCP23008_ADDR2 0x22
#define MCP23008_ADDR3 0x23

// Registri MCP23008
#define IODIR   0x00
#define GPPU    0x06  // Pull-up registar
#define GPIO    0x09

static const int note_freqs[24] = {
	262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, // C4 do B4
	523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988  // C5 do B5
};

// Globalne varijable za toggle funkcionalnost
uint8_t display_mode = 0;  // 0 = PWM, 1 = sawtooth
uint8_t pb0_prev_state = 1;  // Prethodno stanje PB0 (pull-up = 1)

void mcp23008_init(uint8_t addr) {
	uint8_t write_addr = addr << 1;  // Shift za I2C write adresu
	
	// Postavi IODIR registar - svi pinovi kao input
	if (i2c_start(write_addr) != 0) {
		return;  // Greška u komunikaciji
	}
	i2c_write(IODIR);
	i2c_write(0xFF);  // Svi pinovi kao input
	i2c_stop();
	
	// Uklju?i pull-up otpornike na svim pinovima
	if (i2c_start(write_addr) != 0) return;
	i2c_write(GPPU);
	i2c_write(0xFF);  // Pull-up na svim pinovima
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
	input_port(DDRB, PB0);  // pin PB0 postavljen kao ulaz
	set_port(PORTB, PB0, 1); // uklju?en pritezni otpornik na PB0
	
	lcd_init(); // inicijalizacija lcd displeja
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
	_delay_ms(1000);  // Duži delay da se vidi welcome poruka
	
	// Postavi po?etni display
	update_display();
	
	// Inicijalizacija sistema
	SAWTOOTH(0.2, 440);  // Kratki beep da signalizira po?etak
	
	i2c_init(100000);  // Inicijalizuj I2C na 100kHz
	
	// Inicijalizuj sve tri MCP23008 ?ipa
	mcp23008_init(MCP23008_ADDR1);
	mcp23008_init(MCP23008_ADDR2);
	mcp23008_init(MCP23008_ADDR3);
	
	// Kratka pauza nakon inicijalizacije
	_delay_ms(100);
	
	// Tri kratka beep-a da signalizira uspešnu inicijalizaciju
	SAWTOOTH(0.1, 523);
	_delay_ms(100);
	SAWTOOTH(0.1, 659);
	_delay_ms(100);
	SAWTOOTH(0.1, 784);
	
	uint8_t prev_state[3] = {0xFF, 0xFF, 0xFF};  // Prethodno stanje pinova
	
	while (1) {
		// ?itaj trenutno stanje PB0
		uint8_t pb0_current_state = get_pin(PINB, PB0);
		
		// Detektuj pritisak PB0 (prelazak sa 1 na 0)
		if (pb0_current_state == 0 && pb0_prev_state == 1) {
			// Toggle display mode
			display_mode = !display_mode;
			update_display();
			
			// Kratki beep za potvrdu
			BUZZ(0.1, 880);
			
			// Debounce delay
			_delay_ms(200);
		}
		
		// Ažuriraj prethodno stanje PB0
		pb0_prev_state = pb0_current_state;
		
		// ?itaj trenutno stanje sva tri ?ipa
		uint8_t states[3] = {
			mcp23008_read(MCP23008_ADDR1),
			mcp23008_read(MCP23008_ADDR2),
			mcp23008_read(MCP23008_ADDR3)
		};
		
		// Provjeri svaki ?ip i pin za promjene
		for (uint8_t chip = 0; chip < 3; chip++) {
			for (uint8_t pin = 0; pin < 8; pin++) {
				// Detektuj pritisak tipke (prelazak sa 1 na 0 zbog pull-up)
				if (!(states[chip] & (1 << pin)) && (prev_state[chip] & (1 << pin))) {
					uint8_t note_index = chip * 8 + pin;
					
					// Provjeri da note_index nije izvan granica niza
					if (note_index < 24) {
						// Sviraj odgovaraju?u notu
						BUZZ(0.2, note_freqs[note_index]);
					}
				}
			}
			// Ažuriraj prethodno stanje
			prev_state[chip] = states[chip];
		}
		
		_delay_ms(50);  // Kratka pauza izme?u ?itanja
	}
	
	return 0;
}