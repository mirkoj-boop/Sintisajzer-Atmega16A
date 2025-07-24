#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <stdint.h>

// Makroi za I2C operacije
#define I2C_READ    1
#define I2C_WRITE   0

// Inicijalizacija I2C modula
void i2c_init(uint32_t scl_freq);

// Slanje START uvjeta
uint8_t i2c_start(uint8_t address);

// Slanje REPEATED START uvjeta
uint8_t i2c_rep_start(uint8_t address);

// Slanje STOP uvjeta
void i2c_stop(void);

// Slanje bajta
uint8_t i2c_write(uint8_t data);

// Čitanje bajta s ACK-om
uint8_t i2c_read_ack(void);

// Čitanje bajta s NACK-om
uint8_t i2c_read_nack(void);

#endif 