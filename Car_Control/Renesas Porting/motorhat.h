/*
 * motorhat.h
 *
 *  Created on: 2024. 5. 14.
 *      Author: SSAFY
 */

#ifndef MOTORHAT_H_
#define MOTORHAT_H_
#include "hal_data.h"
uint8_t __MODE1              = 0x00;
uint8_t __MODE2              = 0x01;
uint8_t __SUBADR1            = 0x02;
uint8_t __SUBADR2            = 0x03;
uint8_t __SUBADR3            = 0x04;
uint8_t __PRESCALE           = 0xFE;
uint8_t __LED0_ON_L          = 0x06;
uint8_t __LED0_ON_H          = 0x07;
uint8_t __LED0_OFF_L         = 0x08;
uint8_t __LED0_OFF_H         = 0x09;
uint8_t __ALL_LED_ON_L       = 0xFA;
uint8_t __ALL_LED_ON_H       = 0xFB;
uint8_t __ALL_LED_OFF_L      = 0xFC;
uint8_t __ALL_LED_OFF_H      = 0xFD;

uint8_t __RESTART            = 0x80;
uint8_t __SLEEP              = 0x10;
uint8_t __ALLCALL            = 0x01;
uint8_t __INVRT              = 0x10;
uint8_t __OUTDRV             = 0x04;

int PWMpin = 13;
int IN1pin = 12;
int IN2pin = 11;

volatile i2c_master_event_t g_iic_callback_event;
void iic_callback(i2c_master_callback_args_t *p_args);
uint8_t read_byte_data(uint8_t reg);
void write_byte_data(int reg, int val);
void write8( int reg, int value );
uint8_t readU8(uint8_t reg);
void setPWM(int channel, int on, int off);
void setAllPWM(int on, int off);
void setPWMFreq(int freq);
void setPin(int pin, int value);
void Forward();
void Backward();
void Release();
void setSpeed(int speed);
void PWM(int addr);
void init(int addr);
void Left();
void Right();
void Mid();

#endif /* MOTORHAT_H_ */
