#include <avr/io.h>
#include <util/delay.h>

// MCP23008 registri
#define MCP_IODIR  0x00
#define MCP_GPIO   0x09

// I2C adrese ure?aja
#define MCP1_ADDR  0x21
#define MCP2_ADDR  0x22
#define MCP3_ADDR  0x23
#define DAC_ADDR   0x60

// Tablica DAC vrijednosti za 24 note (C3–B4)
const uint16_t notes[24] = {
	406,  431,  457,  483,  512,  542,  574,  608,  644,  683,  723,  766,
	812,  862,  914,  969, 1027, 1088, 1153, 1221, 1293, 1369, 1449, 1533
};
// Ove vrijednosti odgovaraju 12-bit DAC-u s Vref=3.3V i 1V/oct skali
// C3 ? 130.8Hz do B4 ? 493.9Hz

// ====== I2C ======
void TWI_init(void) {
	TWSR = 0x00;            // prescaler 1
	TWBR = 72;              // SCL ~100kHz @16MHz
	TWCR = (1<<TWEN);
}

void TWI_start(void) {
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}

void TWI_stop(void) {
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

void TWI_write(uint8_t data) {
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}

uint8_t TWI_read_nack(void) {
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}

// ====== MCP23008 ======
void MCP23008_init(uint8_t addr) {
	TWI_start();
	TWI_write((addr<<1) | 0);
	TWI_write(MCP_IODIR);
	TWI_write(0xFF);    // svi pinovi kao ulazi
	TWI_stop();
}

uint8_t MCP23008_read(uint8_t addr) {
	uint8_t val;
	TWI_start();
	TWI_write((addr<<1) | 0);
	TWI_write(MCP_GPIO);
	TWI_start();
	TWI_write((addr<<1) | 1);
	val = TWI_read_nack();
	TWI_stop();
	return val;
}

// ====== MCP4725 DAC ======
void MCP4725_write(uint16_t value) {
	TWI_start();
	TWI_write((DAC_ADDR<<1) | 0);
	TWI_write(0x40);                // write DAC register
	TWI_write(value >> 4);          // MSB
	TWI_write((value & 0x0F)<<4);  // LSB
	TWI_stop();
}

// ====== MAIN ======
int main(void) {
	TWI_init();
	MCP23008_init(MCP1_ADDR);
	MCP23008_init(MCP2_ADDR);
	MCP23008_init(MCP3_ADDR);

	while (1) {
		uint8_t keys[3];
		keys[0] = ~MCP23008_read(MCP1_ADDR);  // invert (aktivno LOW)
		keys[1] = ~MCP23008_read(MCP2_ADDR);
		keys[2] = ~MCP23008_read(MCP3_ADDR);

		uint8_t found = 0;

		for (uint8_t i = 0; i < 8; i++) {
			if (keys[0] & (1<<i)) {
				MCP4725_write(notes[i]);
				found = 1;
				break;
			}
		}
		if (!found) {
			for (uint8_t i = 0; i < 8; i++) {
				if (keys[1] & (1<<i)) {
					MCP4725_write(notes[i+8]);
					found = 1;
					break;
				}
			}
		}
		if (!found) {
			for (uint8_t i = 0; i < 8; i++) {
				if (keys[2] & (1<<i)) {
					MCP4725_write(notes[i+16]);
					found = 1;
					break;
				}
			}
		}

		if (!found) {
			// nijedna tipka pritisnuta
			MCP4725_write(0);
		}

		_delay_ms(10);
	}
}
