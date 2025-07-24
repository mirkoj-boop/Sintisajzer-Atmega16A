/*
 Naslov: MIKRORAÈUNALA - Programiranje mikrokontrolera porodice 
 Atmel u programskom okruženju Atmel Studio 6
 Autori: Zoran Vrhovski, Marko Miletiæ
 
 Vježba 4.1.5
 */ 

#include "AVR lib/AVR_lib.h"
#include <avr/io.h>
#include <util/delay.h>


void inicijalizacija(){
		
}


int main(void){
	
	
	inicijalizacija(); // inicijalizacija mikrokontrolera
	
	
	while (1) // beskonaèna petlja 
	{
	
		for (char i = 7; i >= 4; i--){
			
			_delay_ms(100);
			
			PORTB &= ~(1 << i);
			
			if (i > 4){
				PORTB |= (1 << (i-1));
			}
			else{
				PORTB |= (1 << PB7);
			}
			
		}
		
	}
	
	return 0;
	
}