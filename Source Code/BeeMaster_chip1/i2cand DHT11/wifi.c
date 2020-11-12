#define F_CPU 8000000UL     // define cpu frquency
#include<avr/io.h>
#include<util/delay.h>
#include <stdlib.h>
#include <stdio.h>

///////////////////////LCD display/////////////////
//#include "display/i2c_lcd.h"
//#include "common/i2cmaster.h"

#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)     // convert to baudrate dec value

#define STRING_SIZE 16

void UART_init(long USART_BAUDRATE)
{
	UCSRB |= (1 << RXEN) | (1 << TXEN);         // Turn on transmission and reception by setting RX Tx bits
	UCSRC |= (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);    // Use 8-bit character sizes
	UBRRL = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value
	UBRRH = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value
}

void UART_TxChar(char c)
{
	while (! (UCSRA & (1<<UDRE))); // Wait for empty transmit buffer
	UDR = c;
}

void UART_sendString(char *str)
{
	unsigned char s=0;
	
	while (str[s]!=0) // string till null
	{
		UART_TxChar(str[s]);    // send s to UART_TxChar(s) function
		s++;
	}
}
unsigned char UART_RxChar()									/* Data receiving function */
{
	while (!(UCSRA & (1 << RXC)));					/* Wait until new data receive */
	return UDR;									/* Get and return received data */
}

char* receiveStringFromNode(){
	static char str[STRING_SIZE];
	char num;
	while (UART_RxChar() != 255);
	for (char i = 0; i < STRING_SIZE; i++)
	{
		if(num = UART_RxChar())
			str[i] = num;
		if(num == '\0')
			break;
	}
	return str;
}

unsigned char readCharFromNode()	{
	return UART_RxChar();
}

void sendStringToNode(char * str){
	_delay_ms(50);
	UART_sendString(str);      // send string
	_delay_ms(1200);
}

#define SUCCESS_CHAR 'Z'

void waitTillChar(char ch){
	while(readCharFromNode() != ch);
}

void sendCharToNode(char c){
	UART_sendString(c);
}

