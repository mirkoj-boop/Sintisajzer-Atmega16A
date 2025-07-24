/*
 Naslov: MIKRORAÈUNALA - Programiranje mikrokontrolera porodice 
 Atmel u programskom okruženju Atmel Studio 6
 Autori: Zoran Vrhovski, Marko Miletiæ
 
 Vježba 4.1.6
 */ 

#include "AVR lib/AVR_lib.h"
#include <avr/io.h>

void inicijalizacija(){
	
	DDRB |= (1 << PB7) | (1 << PB6) | (1 << PB5) | (1 << PB4); // PB7, PB6, PB5 i PB4 izlazni pinovi
	PORTB |= (1 << PB7); // postavljanje PB7 u visoko stanje
}


int main(void){
	
	
	inicijalizacija(); // inicijalizacija mikrokontrolera
	
	for (char i = 0; i < 50; i++){ // petlja koja se izvodi 50 puta
	// ovdje kopirati blok naredaba while petlje iz datoteke vjezba414.c	
	}
	
	// ovdje ugasiti crvenu LED diodu

	return 0;	
}