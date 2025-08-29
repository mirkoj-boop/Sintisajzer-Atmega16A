#include "i2c.h"
#include <avr/io.h>
#include <util/twi.h>

// Inicijalizacija I2C modula
void i2c_init(const uint32_t scl_freq) {
	// Postavi prescaler na 1
	TWSR = 0x00;
	
	TWBR = ((F_CPU / scl_freq) - 16) / 2;
	
	
	TWCR = (1 << TWEN);
}


uint8_t i2c_start(const uint8_t address) {
	// Slanje START uvjeta
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if ((TWSR & TW_STATUS_MASK) != TW_START) return 1; 
	
	// Slanje adrese
	TWDR = address;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	
	// Provjera ACK-a ovisno o tome je li čitanje ili pisanje
	uint8_t expected_status = (address & 0x01) ? TW_MR_SLA_ACK : TW_MT_SLA_ACK;
	if ((TWSR & TW_STATUS_MASK) != expected_status) return 1; // Greška
	
	return 0; // Uspjeh
}

// Slanje REPEATED START uvjeta
uint8_t i2c_rep_start(const uint8_t address) {
	// Slanje REPEATED START uvjeta
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if ((TWSR & TW_STATUS_MASK) != TW_REP_START) return 1; // Greška - različit status za REP_START
	
	// Slanje adrese
	TWDR = address;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	
	// Provjera ACK-a ovisno o tome je li čitanje ili pisanje
	uint8_t expected_status = (address & 0x01) ? TW_MR_SLA_ACK : TW_MT_SLA_ACK;
	if ((TWSR & TW_STATUS_MASK) != expected_status) return 1; // Greška
	
	return 0; // Uspjeh
}


void i2c_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while (TWCR & (1 << TWSTO));
}


uint8_t i2c_write(const uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))); // NEDOSTAJALO - čekanje da se završi transmisija
	
	if ((TWSR & TW_STATUS_MASK) != TW_MT_DATA_ACK) return 1; // Greška
	
	return 0; // Uspjeh
}


uint8_t i2c_read_ack(void) {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}


uint8_t i2c_read_nack(void) {
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}