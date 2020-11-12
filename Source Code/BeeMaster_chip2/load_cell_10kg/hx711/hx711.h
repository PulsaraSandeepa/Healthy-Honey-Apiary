/*
hx711 lib 0x01

copyright (c) Davide Gironi, 2013

Released under GPLv3.
Please refer to LICENSE file for licensing information.

Reference:
  + This library is the port of HX711
    https://github.com/bogde/HX711
  + Other reference is
    https://github.com/getsiddd/HX711
*/

#include <avr/io.h>



#ifndef HX711_H_
#define HX711_H_


//set ports and pins
#define HX711_DTPORT PORTB
#define HX711_DTDDR DDRB
#define HX711_DTPIN PINB
#define HX711_DTPINNUM PB0
#define HX711_SCKPORT PORTB
#define HX711_SCKDDR DDRB
#define HX711_SCKPINNUM PB1

//defines gain
#define HX711_GAINCHANNELA128 128
#define HX711_GAINCHANNELA64 64
#define HX711_GAINCHANNELB32 32
#define HX711_GAINDEFAULT HX711_GAINCHANNELA128

//defines scale
#define HX711_SCALEMIN 1
#define HX711_SCALEDEFAULT 10000

//defines offset
#define HX711_OFFSETDEFAULT 8000000

//set how many time to read
#define HX711_READTIMES 10

//set if use average for read
#define HX711_USEAVERAGEONREAD 0

//functions
extern uint32_t hx711_read();
extern uint32_t hx711_readaverage(uint8_t times);
extern double hx711_readwithtare();
extern double hx711_getweight();
extern void hx711_setgain(uint16_t gain);
extern uint16_t hx711_getgain();
extern void hx711_setscale(double scale);
extern double hx711_getscale();
extern void hx711_setoffset(int32_t offset);
extern int32_t hx711_getoffset();
extern void hx711_taretozero();
extern void hx711_powerdown();
extern void hx711_powerup();
extern int32_t hx711_calibrate1getoffset();
extern void hx711_calibrate2setscale(double weight);
extern void hx711_init(uint8_t gain, double scale, int32_t offset);

#endif
