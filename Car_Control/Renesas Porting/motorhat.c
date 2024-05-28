/*
 * motorhat.c
 *
 *  Created on: 2024. 5. 20.
 *      Author: SSAFY
 */


#include "hal_data.h"
#include "motorhat.h"
void iic_callback(i2c_master_callback_args_t *p_args)
{
    g_iic_callback_event = p_args->event;
}

uint8_t read_byte_data(uint8_t reg){

    uint8_t buf;
    R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &reg, 1, true);
    R_BSP_SoftwareDelay(50,BSP_DELAY_UNITS_MILLISECONDS);
    while(g_iic_callback_event!=I2C_MASTER_EVENT_TX_COMPLETE);
    buf = 0;
    R_IIC_MASTER_Read(&g_i2c_master0_ctrl, &buf, 1, false);
    R_BSP_SoftwareDelay(50,BSP_DELAY_UNITS_MILLISECONDS);
    while(g_iic_callback_event!=I2C_MASTER_EVENT_RX_COMPLETE);
    return buf;

}

void write_byte_data(int reg, int val){
    uint8_t wbuf[2] = {reg, val};
    R_IIC_MASTER_Write(&g_i2c_master0_ctrl, wbuf, 2, false);
    R_BSP_SoftwareDelay(50,BSP_DELAY_UNITS_MILLISECONDS);
    while(g_iic_callback_event!=I2C_MASTER_EVENT_TX_COMPLETE);
}
void write8( int reg, int value ){
    write_byte_data(reg, value);
}
uint8_t readU8(uint8_t reg){
    return read_byte_data(reg);
}
void setPWM(int channel, int on, int off){

  write8(__LED0_ON_L+4*channel, on & 0xFF);
  write8(__LED0_ON_H+4*channel, on >> 8);
  write8(__LED0_OFF_L+4*channel, off & 0xFF);
  write8(__LED0_OFF_H+4*channel, off >> 8);
}
void setAllPWM(int on, int off){
  write8(__ALL_LED_ON_L, on & 0xFF);
  write8(__ALL_LED_ON_H, on >> 8);
  write8(__ALL_LED_OFF_L, off & 0xFF);
  write8(__ALL_LED_OFF_H, off >> 8);
}
void setPWMFreq(int freq){
   float prescaleval = 25000000.0;
   prescaleval /= 4096.0;
   prescaleval /= (float)freq;
   prescaleval -= 1.0;

   int prescale = (int)(prescaleval + 0.5);

   uint8_t oldmode = readU8(__MODE1);
   int newmode = (oldmode & 0x7F) | 0x10;
   write8(__MODE1, newmode);
   write8(__PRESCALE, (int)(prescale));
   write8(__MODE1, oldmode);
   R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
   write8(__MODE1, oldmode | 0x80);
}
void setPin(int pin, int value){
    if ( value==0 ) setPWM(pin, 0, 4096);
    if ( value==1 ) setPWM(pin, 4096, 0);
}
void Forward(){
    setPin(IN2pin, 0);
    setPin(IN1pin, 1);
}
void Backward(){
    setPin(IN1pin, 0);
    setPin(IN2pin, 1);
}
void Release(){
    setPin(IN1pin,0);
    setPin(IN2pin,0);
}

void setSpeed(int speed){
    if (speed < 0) speed = 0;
    if (speed > 255) speed = 255;
    setPWM(PWMpin, 0, speed*16);
}
void PWM(int addr){
    setAllPWM(0, 0);
    write8(__MODE2, __OUTDRV);
    write8(__MODE1, __ALLCALL);
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);

    uint8_t mode1 = readU8(__MODE1);
    mode1 = mode1 & ~__SLEEP;
    write8(__MODE1, mode1);
    R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
}
void init(int addr){
    int freq = 1600; //default
    PWM(addr);
    setPWMFreq(freq);
}

void Left(){
    setPWM(0,0,230); // 값은 motor에 맞게 수정해야 한다.
}
void Right(){
    setPWM(0,0,510); // 값은 motor에 맞게 수정해야 한다.
}
void Mid(){
    setPWM(0,0,400); // mid
}
