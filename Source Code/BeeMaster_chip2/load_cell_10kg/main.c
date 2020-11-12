
//Beehive_chip2
//hx711 lib example
#define F_CPU 8000000UL

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//include uart
#define UART_BAUD_RATE 4800
#include "uart/uart.h"
#include "hx711/hx711.h"
#include "hx711/hx711.c"
#include "i2cmaster.h" //header files
#include "i2c_lcd.h"
#include "i2cmaster.c"
#include "i2c_lcd.c"


//defines running modes
#define HX711_MODERUNNING 1
#define HX711_MODECALIBRATION1OF2 2
#define HX711_MODECALIBRATION2OF2 3
#define HX711_MODECURRENT HX711_MODERUNNING

//2 step calibration procedure
//set the mode to calibration step 1
//read the calibration offset leaving the load cell with no weight
//set the offset to value read
//put a know weight on the load cell and set calibrationweight value
//run the calibration stes 2 of 2
//read the calibration scale 
//set the scale to value read

//set the gain
int8_t gain = HX711_GAINCHANNELA128;

#if HX711_MODECURRENT == HX711_MODERUNNING
//set the offset
int32_t offset = 8390371;
//set the scale
double scale = 5173;
#elif HX711_MODECURRENT == HX711_MODECALIBRATION1OF2
//set the offset
int32_t offset = HX711_OFFSETDEFAULT;
//set the scale
double scale = HX711_SCALEDEFAULT;
#elif HX711_MODECURRENT == HX711_MODECALIBRATION2OF2
//set the offset
int32_t offset = 8390371;
//set the scale
double scale = HX711_SCALEDEFAULT;
//set the calibration weight
double calibrationweight = 0.119;
#endif
	//init uart
	//uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));

	#define LTHRES 500

	void adc_init()
	{
		// AREF = AVcc
		ADMUX = (1<<REFS0);

		// ADC Enable and prescaler of 128
		// 16000000/128 = 125000
		ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	}

	uint16_t adc_read(uint8_t ch)
	{
		// select the corresponding channel 0~7
		// ANDing with '7' will always keep the value
		// of 'ch' between 0 and 7
		ch &= 0b00000111;  // AND operation with 7
		ADMUX = (ADMUX & 0xF8)|ch;     // clears the bottom 3 bits before ORing

		// start single conversion
		// write '1' to ADSC
		ADCSRA |= (1<<ADSC);

		// wait for conversion to complete
		// ADSC becomes '0' again
		// till then, run loop continuously
		while(ADCSRA & (1<<ADSC));

		return (ADC);
	}


int main(void) {
	
	DDRC |= (1<<PC7); //Nakes first pin of PORTC as Output
	// OR DDRC = 0x01;
	//DDRD &= ~(1<<PD0);//Makes firs pin of PORTD as Input
	// OR DDRD = 0x00; //Makes all pins of PORTD input
	
	
	
uint16_t adc_result0;
// adc_result1;
//char int_buffer[10];
//DDRC = 0x01;           // to connect led to PC0

adc_init();



_delay_ms(50);

DDRD |= (1<<PD5);	/* Make OC1A pin as output */
TCNT1 = 0;		/* Set timer1 count zero */
ICR1 = 2499;		/* Set TOP count for timer1 in ICR1 register */

/* Set Fast PWM, TOP in ICR1, Clear OC1A on compare match, clk/64 */
TCCR1A = (1<<WGM11)|(1<<COM1A1);
TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);

	sei();

	char printbuff[100];
	char printbuff2[100];

	//init hx711
	hx711_init(gain, scale, offset);

///////////////////////////////productivity/////////////////////////////////////
	#if HX711_MODECURRENT == HX711_MODERUNNING
	while(1) {
	
	////////////////Smoke Box//////////////
	if(PIND & (1<<PD0)) //If switch is pressed
	{
		PORTC |= (1<<PC7); //Turns ON LED
		_delay_ms(1000); //3 second delay
		PORTC &= ~(1<<PC7); //Turns OFF LED
	_delay_ms(4000);
	}
	///////////////////////////////////////	

	////////////////////////////////////servo//////////////////////////////////////
	adc_result0 = adc_read(0);      // read adc value at PA0

	
		// condition for servo
		if (adc_result0 < LTHRES)
		{
		OCR1A = 175;	/* Set servo at +90� position */
			//_delay_ms(1500);
		}
		else if(adc_result0 > LTHRES)
		{
			OCR1A = 300;	/* Set servo shaft at 0� position */
			//_delay_ms(1500);
		}
	/////////////////////////////////servo/////////////////////////////////////////	
		//OCR1A = 65;	/* Set servo shaft at -90� position */
	//	_delay_ms(1500);
		//OCR1A = 175;	/* Set servo shaft at 0� position */
		//_delay_ms(1500);
	//	OCR1A = 65;	/* Set servo at +90� position */
	//	_delay_ms(1500);

		
	 lcd_init(LCD_BACKLIGHT_ON);			// Initialize LCD
		lcd_clear();			            // Clear LCD
		lcd_goto_xy(0,0);
		
		//get read
	    //	uint32_t read = hx711_read();
		//ltoa(read, printbuff, 10);
		//lcd_puts(printbuff);
		//uart_puts("Read: "); uart_puts(printbuff); uart_puts("\r\n");

		//get weight
		double weight = (-(hx711_getweight()-50))-1.35;
		dtostrf(weight, 3, 3, printbuff);
		lcd_puts("Weight: "); 
		lcd_puts(printbuff);
		sendStringToNode(printbuff);

		 lcd_puts("kg"); //uart_puts("\r\n");
		 
		 
		 lcd_goto_xy(0,1);
		 double prdct = (weight)*12.5;
		 dtostrf(prdct, 2, 2, printbuff2);
		 lcd_puts("Harvest:");
		 lcd_puts(printbuff2);
		 lcd_puts("%"); //uart_puts("\r\n");

		//uart_puts("\r\n");
////////////////////////////////////////////productivity/////////////////////////////////////////

	_delay_ms(500);
			
	}
	#elif HX711_MODECURRENT == HX711_MODECALIBRATION1OF2
	for(;;) {
		//get calibration offset
		lcd_init(LCD_BACKLIGHT_ON);			// Initialize LCD
		lcd_clear();			            // Clear LCD
		lcd_goto_xy(0,0);
		int32_t calibrationoffset = hx711_calibrate1getoffset();
		ltoa(calibrationoffset, printbuff, 10);
		lcd_puts("offset:"); 
		lcd_puts(printbuff); //uart_puts("\r\n");

		//uart_puts("\r\n");

		_delay_ms(500);
	}
	#elif HX711_MODECURRENT == HX711_MODECALIBRATION2OF2
	for(;;) {
		//calibrate
		lcd_init(LCD_BACKLIGHT_ON);			// Initialize LCD
		lcd_clear();			            // Clear LCD
		lcd_goto_xy(0,0);
		hx711_calibrate2setscale(calibrationweight);

		//get scale
		double scale = hx711_getscale();
		dtostrf(scale, 3, 3, printbuff);
		lcd_puts("scale: "); 
		lcd_puts(printbuff); //uart_puts("\r\n");

		//uart_puts("\r\n");

		_delay_ms(500);
	}
	#endif
}
