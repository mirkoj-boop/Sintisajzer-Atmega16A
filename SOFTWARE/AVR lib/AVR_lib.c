/*
 Naslov: MIKRORA?UNALA - Programiranje mikrokontrolera porodice 
 Atmel u programskom okruženju Atmel Studio 6
 Autori: Zoran Vrhovski, Marko Mileti?
 
AVR_lib.c
 */ 
#include "AVR_lib.h"
#include <util/delay.h>

// debounce 
// funkcija prima tri parametra, prvi je adresa porta, drugi je pozicija pina, a tre?i je logi?ka varijabla (0 ili 1, false ili true)
// funkcija vra?a vrijednost tre?eg parametra ako je i stanje pina jednako tre?em parametru i nakon ?ekanja od DEBOUNCE_TIME ms
bool debounce(uint8_t* port, uint8_t pin, bool _value){
    if (get_pin(*port,pin) == _value){
        int a = 0;
        int brojac = 0;
        while(a++ < DEBOUNCE_TIME){
            _delay_ms(1);
            if (get_pin(*port,pin) == _value) brojac++;
        }
        if (brojac >= DEBOUNCE_TIME * 9 /10){
            return _value;
        }
    }
    return !_value;
}

// debounce2
// funkcija prima ?etiri parametra, prvi je adresa porta, drugi je pozicija pina, tre?i je logi?ka varijabla (0 ili 1, false ili true),
// a ?etvrti je funkcija koja se poziva ukoliko je stanje pina jednako tre?em parametru i nakon ?ekanja od DEBOUNCE_TIME ms
void debounce2(uint8_t* port, uint8_t pin, bool _value, void (*f)()){
    if (get_pin(*port,pin) == _value){
        int a = 0;
        int brojac = 0;
        while(a++ < DEBOUNCE_TIME){
            _delay_ms(1);
            if (get_pin(*port,pin) == _value) brojac++;
        }
        if (brojac >= DEBOUNCE_TIME * 9 /10){
            f();
        }
    }
}

// funkcija za zujalicu
// duration - trajanje zvuka u sekundama
// freq - frekvencija
void BUZZ(double duration, int freq) 
{
    long int i,cycles;
    cycles = duration * freq; // Compute the number of cycles to loop toggling the pin
    long int period = 1.0/freq * 100000; // Compute a half cycle period
    long int half_period =  period / 2;
    BUZZER_DDR = (1 << BUZZER_PIN) | BUZZER_DDR;  // Set the port for the buzzer output
    
    for (i=0; i < cycles; i++)   // Toggle the speaker the appropriate number of cycles
    {
        half_period = period/2;
        while(half_period--) {
            _delay_us(10);
        }                       
        
        BUZZER_PORT = (1 << BUZZER_PIN) | BUZZER_PORT;    // Set the port pin
        
        half_period = period/2;
        while(half_period--) {
            _delay_us(10);
        }                       // Wait a half cycle to clear the port pin
        BUZZER_PORT = ~(1 << BUZZER_PIN) & BUZZER_PORT;   // Clear the port pin
    }
}

void SAWTOOTH(double duration, int freq) 
{
    int steps = 20;  // Broj koraka u sawtooth-u
    double step_duration = duration / steps;  // Trajanje svakog koraka
    int freq_step = freq / 4;  // Korak frekvencije
    
    output_port(BUZZER_DDR, BUZZER_PIN);  // Postavi pin kao izlaz
    
    // Generiraj sawtooth kroz steps koraka
    for (int step = 0; step < steps; step++) {
        // Trenutna frekvencija - raste linearno
        int current_freq = (freq / 2) + (step * freq_step / steps);
        if (current_freq < 50) current_freq = 50;  // Minimum frekvencija
        
        // Broj ciklusa za ovaj korak
        int cycles = current_freq * step_duration;
        if (cycles < 1) cycles = 1;
        
        // Generiraj tonove za ovaj korak
        for (int i = 0; i < cycles; i++) {
            set_port(BUZZER_PORT, BUZZER_PIN, true);   // HIGH
            
            // Koristi petlju umesto _delay_us za varijabilno kašnjenje
            volatile long int delay_count = (F_CPU / current_freq) / 4;
            if (delay_count < 10) delay_count = 10;
            
            while(delay_count--) {
                asm("nop");  // No operation - kratka pauza
            }
            
            set_port(BUZZER_PORT, BUZZER_PIN, false);  // LOW
            
            // Isto kašnjenje za LOW dio
            delay_count = (F_CPU / current_freq) / 4;
            if (delay_count < 10) delay_count = 10;
            
            while(delay_count--) {
                asm("nop");
            }
        }
    }
    
    set_port(BUZZER_PORT, BUZZER_PIN, false); // Završi na LOW
}