
#include <avr/io.h>
#include <util/delay.h>
#include "USART/USART.h"


// Define F_CPU once, matching USART.h (8 MHz assumed)
#define F_CPU 100000UL

int main(void) {
	// Initialize USART at 9600 baud
	usart_init(9600);
	
	// Send startup message
	usart_write_string("===== USART Test Start =====\r\n");
	usart_write_string("F_CPU: 8000000 Hz, Baud: 9600. Ensure Realterm is Open on COM5!\r\n");
	
	uint8_t counter = 0;
	while (1) {
		usart_write_string("Upaljeno radi, brojac: ");
		if (counter < 10) {
			usart_write_char('0' + counter);
			} else {
			usart_write_char('0' + (counter / 10));
			usart_write_char('0' + (counter % 10));
		}
		usart_write_string("\r\n");
		counter++;
		if (counter >= 100) counter = 0;
		
		_delay_ms(1000); // Send every 1 second
	}
	
	return 0;
}
