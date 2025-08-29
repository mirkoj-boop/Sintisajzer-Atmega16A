/*
 Naslov: MIKRORAÈUNALA - Programiranje mikrokontrolera porodice
 Atmel u programskom okruženju Atmel Studio 6
 Autori: Zoran Vrhovski, Marko Miletiæ
 
 lcd.c
 
 */ 

#include "lcd.h"
#include <stdio.h>

void EtoggleLCD(){
	
	set_port(LCD_E_PORT, LCD_E_PIN, 1);
	_delay_us(3600);
	set_port(LCD_E_PORT, LCD_E_PIN, 0);
	_delay_us(3600);
}

//-----------------------------------------------------------------------------------------
// FUNCTION: lcd_init
// PURPOSE:  Initialize LCD to 4 bit I/O mode
void lcd_init()
{
	// configure all port bits as output (LCD data and control lines on different ports
	LCD_DDR(LCD_RS_PORT)    |= _BV(LCD_RS_PIN);
	LCD_DDR(LCD_E_PORT)     |= _BV(LCD_E_PIN);
	LCD_DDR(LCD_DATA0_PORT) |= _BV(LCD_DATA0_PIN);
	LCD_DDR(LCD_DATA1_PORT) |= _BV(LCD_DATA1_PIN);
	LCD_DDR(LCD_DATA2_PORT) |= _BV(LCD_DATA2_PIN);
	LCD_DDR(LCD_DATA3_PORT) |= _BV(LCD_DATA3_PIN);

	//LCD // Naredbe
	set_port(LCD_RS_PORT, LCD_RS_PIN, 0);

	_delay_ms(100); //barem 100ms

	//1. Special case of "Function Set" (lower four bits are irrelevant)
	//0011
	set_port(LCD_DATA3_PORT, LCD_DATA3_PIN, 0);set_port(LCD_DATA2_PORT, LCD_DATA2_PIN, 0);set_port(LCD_DATA1_PORT, LCD_DATA1_PIN, 1);set_port(LCD_DATA0_PORT, LCD_DATA0_PIN, 1); //0011
	EtoggleLCD();
	_delay_us(10000); //barem 4.1ms
	
	//2. Special case of "Function Set" (lower four bits are irrelevant)
	//0011
	set_port(LCD_DATA3_PORT, LCD_DATA3_PIN, 0);set_port(LCD_DATA2_PORT, LCD_DATA2_PIN, 0);set_port(LCD_DATA1_PORT, LCD_DATA1_PIN, 1);set_port(LCD_DATA0_PORT, LCD_DATA0_PIN, 1); //0011
	EtoggleLCD();
	_delay_us(500); //barem 100us
	
	//3. Special case of "Function Set" (lower four bits are irrelevant)
	//0011
	set_port(LCD_DATA3_PORT, LCD_DATA3_PIN, 0);set_port(LCD_DATA2_PORT, LCD_DATA2_PIN, 0);set_port(LCD_DATA1_PORT, LCD_DATA1_PIN, 1);set_port(LCD_DATA0_PORT, LCD_DATA0_PIN, 1); //0011
	EtoggleLCD();
	_delay_us(500); //barem 100us

	//Initial "Funtion Set" to change interface (lower four bits are irrelevant)
	//0010
	set_port(LCD_DATA3_PORT, LCD_DATA3_PIN, 0);set_port(LCD_DATA2_PORT, LCD_DATA2_PIN, 0);set_port(LCD_DATA1_PORT, LCD_DATA1_PIN, 1);set_port(LCD_DATA0_PORT, LCD_DATA0_PIN, 0); //0010
	EtoggleLCD();
	_delay_us(500); //barem 100us
	
	//"Function Set" - I=1, N=1, F=0
	//0010 1000
	set_port(LCD_DATA3_PORT, LCD_DATA3_PIN, 0);set_port(LCD_DATA2_PORT, LCD_DATA2_PIN, 0);set_port(LCD_DATA1_PORT, LCD_DATA1_PIN, 1);set_port(LCD_DATA0_PORT, LCD_DATA0_PIN, 0); //0010
	EtoggleLCD();
	set_port(LCD_DATA3_PORT, LCD_DATA3_PIN, 1);set_port(LCD_DATA2_PORT, LCD_DATA2_PIN, 0);set_port(LCD_DATA1_PORT, LCD_DATA1_PIN, 0);set_port(LCD_DATA0_PORT, LCD_DATA0_PIN, 0); //1000
	EtoggleLCD();
	_delay_us(150); //barem 53us

	//Display OFF, D=0, C=0, B=0
	//0000 1000
	set_port(LCD_DATA3_PORT, LCD_DATA3_PIN, 0);set_port(LCD_DATA2_PORT, LCD_DATA2_PIN, 0);set_port(LCD_DATA1_PORT, LCD_DATA1_PIN, 0);set_port(LCD_DATA0_PORT, LCD_DATA0_PIN, 0); //0000
	EtoggleLCD();
	set_port(LCD_DATA3_PORT, LCD_DATA3_PIN, 1);set_port(LCD_DATA2_PORT, LCD_DATA2_PIN, 0);set_port(LCD_DATA1_PORT, LCD_DATA1_PIN, 0);set_port(LCD_DATA0_PORT, LCD_DATA0_PIN, 0); //1000
	EtoggleLCD();
	_delay_us(150); //barem 53us

	//"Clear Display"
	//0000 0001
	set_port(LCD_DATA3_PORT, LCD_DATA3_PIN, 0);set_port(LCD_DATA2_PORT, LCD_DATA2_PIN, 0);set_port(LCD_DATA1_PORT, LCD_DATA1_PIN, 0);set_port(LCD_DATA0_PORT, LCD_DATA0_PIN, 0); //0000
	EtoggleLCD();
	set_port(LCD_DATA3_PORT, LCD_DATA3_PIN, 0);set_port(LCD_DATA2_PORT, LCD_DATA2_PIN, 0);set_port(LCD_DATA1_PORT, LCD_DATA1_PIN, 0);set_port(LCD_DATA0_PORT, LCD_DATA0_PIN, 1); //0001
	EtoggleLCD();
	_delay_us(6000); //barem 3ms
	
	//"Entry mode set", I/D=1, S=0
	//0000 0110
	set_port(LCD_DATA3_PORT, LCD_DATA3_PIN, 0);set_port(LCD_DATA2_PORT, LCD_DATA2_PIN, 0);set_port(LCD_DATA1_PORT, LCD_DATA1_PIN, 0);set_port(LCD_DATA0_PORT, LCD_DATA0_PIN, 0); //0000
	EtoggleLCD();
	set_port(LCD_DATA3_PORT, LCD_DATA3_PIN, 0);set_port(LCD_DATA2_PORT, LCD_DATA2_PIN, 1);set_port(LCD_DATA1_PORT, LCD_DATA1_PIN, 1);set_port(LCD_DATA0_PORT, LCD_DATA0_PIN, 0); //0110
	EtoggleLCD();
	_delay_us(150); //barem 53us
	
	//KRAJ INICIJALIZACIJE

	//"Display ON", D=1, C=0, B=0
	//0000 1100
	set_port(LCD_DATA3_PORT, LCD_DATA3_PIN, 0);set_port(LCD_DATA2_PORT, LCD_DATA2_PIN, 0);set_port(LCD_DATA1_PORT, LCD_DATA1_PIN, 0);set_port(LCD_DATA0_PORT, LCD_DATA0_PIN, 0); //0000
	EtoggleLCD();
	set_port(LCD_DATA3_PORT, LCD_DATA3_PIN, 1);set_port(LCD_DATA2_PORT, LCD_DATA2_PIN, 1);set_port(LCD_DATA1_PORT, LCD_DATA1_PIN, 0);set_port(LCD_DATA0_PORT, LCD_DATA0_PIN, 0); //1100
	EtoggleLCD();
	_delay_us(150); //barem 53us
}

//-----------------------------------------------------------------------------------------
// FUNCTION: _auxToggleE
// PURPOSE: flush channel E
void lcd_e_toggle(void)
{
    lcd_e_high();
	_delay_us(600);	//sensitive when changing CPU MHz!!!!!!!!!!!!!
    lcd_e_low();
}

//-----------------------------------------------------------------------------------------
// FUNCTION: lcd_write
// PURPOSE: send a character or an instruction to the LCD
void lcd_write(uint8_t data,uint8_t rs) 
{
	//check write type
    if (rs)    
       lcd_rs_high(); //write data
    else     
       lcd_rs_low();  //write instruciton
        
    // output high nibble first 
	// prvo se šalje gornjih 4 bita instrukcije, zatim donjih 4 bita instruukcije
	
    LCD_DATA3_PORT &= ~_BV(LCD_DATA3_PIN);
    LCD_DATA2_PORT &= ~_BV(LCD_DATA2_PIN);
    LCD_DATA1_PORT &= ~_BV(LCD_DATA1_PIN);
    LCD_DATA0_PORT &= ~_BV(LCD_DATA0_PIN);
	if(data & 0x80) LCD_DATA3_PORT |= _BV(LCD_DATA3_PIN);
	if(data & 0x40) LCD_DATA2_PORT |= _BV(LCD_DATA2_PIN);
	if(data & 0x20) LCD_DATA1_PORT |= _BV(LCD_DATA1_PIN);
	if(data & 0x10) LCD_DATA0_PORT |= _BV(LCD_DATA0_PIN);   
    lcd_e_toggle();
 
    // output low nibble 
    LCD_DATA3_PORT &= ~_BV(LCD_DATA3_PIN);
    LCD_DATA2_PORT &= ~_BV(LCD_DATA2_PIN);
    LCD_DATA1_PORT &= ~_BV(LCD_DATA1_PIN);
    LCD_DATA0_PORT &= ~_BV(LCD_DATA0_PIN);
	if(data & 0x08) LCD_DATA3_PORT |= _BV(LCD_DATA3_PIN);
	if(data & 0x04) LCD_DATA2_PORT |= _BV(LCD_DATA2_PIN);
	if(data & 0x02) LCD_DATA1_PORT |= _BV(LCD_DATA1_PIN);
	if(data & 0x01) LCD_DATA0_PORT |= _BV(LCD_DATA0_PIN);
    lcd_e_toggle();     
  
    
    // all data pins high (inactive) 
    LCD_DATA0_PORT |= _BV(LCD_DATA0_PIN);
    LCD_DATA1_PORT |= _BV(LCD_DATA1_PIN);
    LCD_DATA2_PORT |= _BV(LCD_DATA2_PIN);
    LCD_DATA3_PORT |= _BV(LCD_DATA3_PIN);
}

//-----------------------------------------------------------------------------------------
// FUNCTION: lcd_char
// PURPOSE:  send a character to the LCD
void lcd_char(uint8_t data)
{
	if (data =='\n')
	{
		if (g_nCurrentLine >= LCD_LINES - 1)
			lcd_setline(0);
		else
			lcd_setline(g_nCurrentLine+1);
	}
	else
	lcd_write(data,1);
}

//-----------------------------------------------------------------------------------------
// FUNCTION: lcd_instr
// PURPOSE:  send an instruction to the LCD
void lcd_instr(uint8_t instr)
{
	lcd_write(instr,0);
}



//-----------------------------------------------------------------------------------------
// FUNCTION: lcd_newline
// PURPOSE:  Move cursor on specified line
void lcd_setline(uint8_t line)
{
    uint8_t addressCounter = 0;
	switch(line)
	{
		case 0: addressCounter = LCD_START_LINE1; break;
		case 1: addressCounter = LCD_START_LINE2; break;
		case 2: addressCounter = LCD_START_LINE3; break;
		case 3: addressCounter = LCD_START_LINE4; break;
		default:addressCounter = LCD_START_LINE1; break;
	}
	g_nCurrentLine = line;

    lcd_instr((1<<LCD_DDRAM)+addressCounter);
}

int g_nCurrentLine = 0;

//-----------------------------------------------------------------------------------------
// FUNCTION: lcd_print
// PURPOSE:  send a null terminated string to the LCD eg. char x[10]="hello!";
void lcd_print2(char *text)
{
	char c;
    while ( (c = *text++) )
	lcd_char(c);	
}

void lcd_print(char *szFormat, ...)
{	
    char szBuffer[80]; //in this buffer we form the message
    va_list pArgs;
    va_start(pArgs, szFormat);
    vsprintf(szBuffer, szFormat, pArgs);
    va_end(pArgs);
	
	lcd_print2(szBuffer);

}
//-----------------------------------------------------------------------------------------
// FUNCTION: lcd_gotoxy
// PURPOSE:  Set cursor to specified position
//           Input:    x  redak
//                     y  stupac
void lcd_gotoxy(uint8_t y, uint8_t x)
{
#if LCD_LINES==1
    lcd_instr((1<<LCD_DDRAM)+LCD_START_LINE1+x);
#elif LCD_LINES==2
   	switch (y) 
	{
		case 0:lcd_instr((1<<LCD_DDRAM)+LCD_START_LINE1+x);break;
    	case 1:lcd_instr((1<<LCD_DDRAM)+LCD_START_LINE2+x);break;
		default: break;
	}
#elif LCD_LINES==4
   	switch (y) 
	{
		case 0:lcd_instr((1<<LCD_DDRAM)+LCD_START_LINE1+x);break;
    	case 1:lcd_instr((1<<LCD_DDRAM)+LCD_START_LINE2+x);break;
    	case 2:lcd_instr((1<<LCD_DDRAM)+LCD_START_LINE3+x);break;
    	case 3:lcd_instr((1<<LCD_DDRAM)+LCD_START_LINE4+x);break;
		default: break;
	}
#endif
}


//-----------------------------------------------------------------------------------------
// FUNCTION: lcd_clrscr
// PURPOSE:  Clear display and set cursor to home position
void lcd_clrscr(void)
{
	g_nCurrentLine = 0;
   	lcd_instr(1<<LCD_CLR);
	_delay_us(10000);
}

//-----------------------------------------------------------------------------------------
// FUNCTION: lcd_home
// PURPOSE:  Set cursor to home position
void lcd_home(void)
{
	g_nCurrentLine = 0;
    lcd_instr(1<<LCD_HOME);
	_delay_us(10000);
}



void lcd_define_char(void){
	
	//definiranje vlastitih simbola/karaktera

	const uint8_t symbol[8][8]  = {//definirajte 8 znakova

		/* 0x00 */	0x0A, 0x04, 0x0E, 0x11, 0x10, 0x11, 0x0E, 0x00, //è
		/* 0x01 */	0x02, 0x04, 0x0E, 0x11, 0x10, 0x11, 0x0E, 0x00, //æ
		/* 0x02 */	0x02, 0x07, 0x02, 0x0E, 0x12, 0x12, 0x0E, 0x00, //ð		
		/* 0x03 */	0x0A, 0x04, 0x0E, 0x10, 0x0E, 0x01, 0x1E, 0x00, //š	
		/* 0x04 */	0x0A, 0x04, 0x1F, 0x02, 0x04, 0x08, 0x1F, 0x00, //ž
		/* 0x05 */	0x02, 0x05, 0x04, 0x04, 0x04, 0x14, 0x08, 0x00, // integral
		/* 0x06 */	0x15, 0x1B, 0x11, 0x0E, 0x0F, 0x0E, 0x11, 0x1F, //è negativ
		/* 0x07 */	0x15, 0x1B, 0x11, 0x0F, 0x11, 0x1E, 0x01, 0x1F, //š negativ

	};


	//
	for (uint8_t i = 0; i < 8; i++)
	{
		_delay_us(200);
		lcd_instr((1<<LCD_CGRAM) + (i << 3));
		for (uint8_t j = 0; j < 8; j++)
		{
			lcd_write(symbol[i][j],1);
		}
		
	}
}



