#pragma once
/*
 Naslov: MIKRORA?UNALA - Programiranje mikrokontrolera porodice 
 Atmel u programskom okruženju Atmel Studio 6
 Autori: Zoran Vrhovski, Marko Mileti?
 
AVR_lib.h
 */ 

#include <avr/io.h>
#include "stdbool.h"

// manipulacija s ulazima i izlazima
#define set_port(port, position, value)  (value == true) ? (port |= (1 << position)) : (port &= ~(1 << position))
#define get_pin(port, position) ((port & (1 << position))>>position)
#define TOGGLE_PORT(port, position) port ^= (1 << position)

// postavljanje registra za ulazne i izlazne pinove
#define output_port(x,y) x |= (1 << y)
#define input_port(x,y) x &= ~(1 << y)

// debounce
#define DEBOUNCE_TIME 25 // vrijeme u ms
bool debounce(uint8_t* port, uint8_t pin, bool _value);
void debounce2(uint8_t* port, uint8_t pin, bool _value, void (*f)());

// buzzer
#define BUZZER_PORT PORTD
#define BUZZER_DDR  DDRD
#define BUZZER_PIN  PD2 // Promijenjeno s PD2 na PD7

void BUZZ(double duration, int freq);
void SAWTOOTH(double duration, int freq);
