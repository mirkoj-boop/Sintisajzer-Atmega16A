/*
 Naslov: MIKRORAÈUNALA - Programiranje mikrokontrolera porodice 
 Atmel u programskom okruženju Atmel Studio 6
 Autori: Zoran Vrhovski, Marko Miletiæ
 
 Vježba 4.1.3
 */ 


#include "AVR lib/AVR_lib.h"
#include <avr/io.h>
#include <util/delay.h>

void inicijalizacija(){
	
	DDRB |= (1 << PB7); // pin PB7 postavljen kao izlazni port
	PORTB |= (1 << PB7); // postavljanje pina PB7 u logièku jedinicu

}
		


int main(void){
		
	
	inicijalizacija(); // inicijalizacija mikrokontrolera
	

return 0;
	
}