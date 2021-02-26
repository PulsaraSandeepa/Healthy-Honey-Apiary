/*
 * Group34 BeeMaster 
*/
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "i2cmaster.h"
#include "i2c_lcd.h"
#define DHT11_PIN 5
#define  Trigger_pin	PA0	/* Trigger pin */
int TimerOverflow = 0;
//#include "24cXX.h"
//#define  Trigger_pin	PA0	/* Trigger pin */
//#define LED1 PB0

ISR(TIMER1_OVF_vect)
{
	TimerOverflow++;	/* Increment Timer Overflow count */
}
//void placeEmptyCharacters (char charsNumber[8], unsigned char emptySpacesNumber);

//--------------------------------------------------------------------

//--------------------------------------------------------------------
uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;

void Request()				/* Microcontroller send start pulse/request */
{
	DDRD |= (1<<DHT11_PIN);
	PORTD &= ~(1<<DHT11_PIN);	/* set to low pin */
	_delay_ms(20);	/* wait for 20ms */
	PORTD |= (1<<DHT11_PIN);	/* set to high pin */
	//_delay_ms(20);	/* wait for 20ms */
}

void Response()				/* receive response from DHT11 */
{
	DDRD &= ~(1<<DHT11_PIN);
	while(PIND & (1<<DHT11_PIN));
	while((PIND & (1<<DHT11_PIN))==0);
	while(PIND & (1<<DHT11_PIN));
	
}

uint8_t Receive_data()			/* receive data */
{
	for (int q=0; q<8; q++)
	{
		while((PIND & (1<<DHT11_PIN)) == 0);  /* check received bit 0 or 1 */
		_delay_us(30);
		if(PIND & (1<<DHT11_PIN))/* if high pulse is greater than 30ms */
		c = (c<<1)|(0x01);	/* then its logic HIGH */
		else			/* otherwise its logic LOW */
		c = (c<<1);
		while(PIND & (1<<DHT11_PIN));
	}
	return c;
}

int main(void)
{
	char data[5];
	long count;
	double distance;
	lcd_init(LCD_BACKLIGHT_ON);			/* Initialize LCD */
	lcd_clear();			/* Clear LCD */
	lcd_goto_xy(0,0);		/* Enter column and row position */
	DDRA = 0x01;		/* Make trigger pin as output */
	PORTD = 0xFF;		/* Turn on Pull-up */


	sei();			/* Enable global interrupt */
	TIMSK = (1 << TOIE1);	/* Enable Timer1 overflow interrupts */
	TCCR1A = 0;		/* Set all bit to zero Normal operation */

	while(1)
	{	
		
		
	////////////////////////Security system/////////////////////////////////////////////////////	
		/* Give 10us trigger pulse on trig. pin to HC-SR04 */
		PORTA |= (1 << Trigger_pin);
		_delay_us(10);
		PORTA &= (~(1 << Trigger_pin));
		
		TCNT1 = 0;	/* Clear Timer counter */
		TCCR1B = 0x41;	/* Capture on rising edge, No prescaler*/
		TIFR = 1<<ICF1;	/* Clear ICP flag (Input Capture flag) */
		TIFR = 1<<TOV1;	/* Clear Timer Overflow flag */

		/*Calculate width of Echo by Input Capture (ICP) */
		
		while ((TIFR & (1 << ICF1)) == 0);/* Wait for rising edge */
		TCNT1 = 0;	/* Clear Timer counter */
		TCCR1B = 0x01;	/* Capture on falling edge, No prescaler */
		TIFR = 1<<ICF1;	/* Clear ICP flag (Input Capture flag) */
		TIFR = 1<<TOV1;	/* Clear Timer Overflow flag */
		TimerOverflow = 0;/* Clear Timer overflow count */

		while ((TIFR & (1 << ICF1)) == 0);/* Wait for falling edge */
		count = ICR1 + (65535 * TimerOverflow);	/* Take count */
		/* 16MHz Timer freq, sound speed =343 m/s */
		distance = (double)count / 466.47;

		/*dtostrf(distance, 2, 2, string); distance to string 
		strcat(string, " cm   ");  Concat unit i.e.cm 
		lcd_goto_xy(0, 0);
		lcd_puts("Dist= ");
		lcd_goto_xy(5, 0);
		lcd_puts(string);	Print distance 
		_delay_ms(200);
		*/
		if(distance<=20)
		{
			DDRB=0b00000100;;
			PORTB=0b00000100;///////port B2 high
			_delay_ms(1000);
			PORTB=0b00000000;
		}

/////////////////////Security System//////////////////////////////	
		
/////////////////Fan system/////////////////////////////////

lcd_puts("Humidity = ");
lcd_goto_xy(0,1);
lcd_puts("Temp = ");
		/*lcd_goto_xy(0,0);		Enter column and row position 
		lcd_puts("Humidity =");
		_delay_ms(1000);
		lcd_clear();
		_delay_ms(1000);*/
		Request();		/* send start pulse */
		Response();		/* receive response */
		I_RH=Receive_data();	/* store first eight bit in I_RH */
		D_RH=Receive_data();	/* store next eight bit in D_RH */
		I_Temp=Receive_data();	/* store next eight bit in I_Temp */
		D_Temp=Receive_data();	/* store next eight bit in D_Temp */
		CheckSum=Receive_data();/* store next eight bit in CheckSum */
		
		if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum)
		{
			lcd_goto_xy(0,0);
			lcd_puts("Error");
		}
		
		else
		{
			itoa(I_RH,data,10);
			lcd_goto_xy(11,0);
			lcd_puts(data);
			sendStringToNode(data);
			lcd_puts(".");
			
			itoa(D_RH,data,10);
			lcd_puts(data);
			lcd_puts("%");

			itoa(I_Temp,data,10);
			lcd_goto_xy(7,1);
			lcd_puts(data);
			sendStringToNode(data);
			lcd_puts(".");
			
			itoa(D_Temp,data,10);
			lcd_puts(data);
			//lcd_sendData(0xDF);
			lcd_puts("C ");
			
			/*itoa(CheckSum,data,10);
			lcd_puts(data);
			lcd_puts(" ");*/
			
		}
		_delay_ms(1000);
		lcd_clear();
		_delay_ms(1000);
		

		if(I_Temp>=32)/////////////////////////
		{
			DDRB=0b00000010;
			PORTB |=(1<<PB1);///port B1 high
		}
		else
		{
			DDRB=0b00000010;
			PORTB &=~(1<<PB1);
		}
		if(I_Temp<=30)//////////////////////////////
		{
			DDRD=0b00000001;
			PORTD |=(1<<PD0);///port D0 high{
			}
			else
			{
				DDRD=0b00000001;
				PORTD &=~(1<<PD0);
			}
/////////////////Fan system/////////////////////////////////

	
	}
}
		
