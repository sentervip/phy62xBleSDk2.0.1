/**************************************************************************************************
 
  Phyplus Microelectronics Limited confidential and proprietary. 
  All rights reserved.

  IMPORTANT: All rights of this software belong to Phyplus Microelectronics 
  Limited ("Phyplus"). Your use of this Software is limited to those 
  specific rights granted under  the terms of the business contract, the 
  confidential agreement, the non-disclosure agreement and any other forms 
  of agreements as a customer or a partner of Phyplus. You may not use this 
  Software unless you agree to abide by the terms of these agreements. 
  You acknowledge that the Software may not be modified, copied, 
  distributed or disclosed unless embedded on a Phyplus Bluetooth Low Energy 
  (BLE) integrated circuit, either as a product or is integrated into your 
  products.  Other than for the aforementioned purposes, you may not use, 
  reproduce, copy, prepare derivative works of, modify, distribute, perform, 
  display or sell this Software and/or its documentation for any purposes.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  PHYPLUS OR ITS SUBSIDIARIES BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
  
**************************************************************************************************/


#include "led_light.h"
#include "pwm.h"
#include "OSAL.h"
#include "gpio.h"
#include "app_wrist.h"
#include "error.h"
#include "common.h"
#include "log.h"

#define GPIO_GREEN    P31//P23
#define GPIO_YELLOW   P23//P31
#define GPIO_RED      P32



static uint16_t s_light[3];
uint8_t s_rgb[3];
//static uint8_t s_light_en[3];
void delay_us(unsigned int t)
{   
    if(t == 1){
        for(unsigned int i=0; i<10;i++)
            for(unsigned int j=0; j<50;j++);
        //WaitUs(t);
    }else{
        WaitUs(t);
    }
}
void ctrl_led(int level)
{
    
    hal_gpio_fast_write(LEDOUT, level);
}

void ctrl_rgb(unsigned  char a0,unsigned  char a1,unsigned  char r,unsigned char g,unsigned  char b)
{
    unsigned char j,tmp,i;
    unsigned int ii;

   // r=0xff;
    
    LOG("a0:%d, a1:%d,r:%d, g:%d, b:%d\n", a0, a1,r,g,b);
    //hal_gpio_pin_init(RGBOUT, OEN);
    hal_gpio_fast_write(RGBOUT, 1);
    tmp=0x1;
    for(j=0;j<6;j++)
    {
        hal_gpio_fast_write(RGBOUT, 0);//P1=P1&0xF7;
        delay_us(us5);
        if ((a0&tmp)==0) 
        {
            hal_gpio_fast_write(RGBOUT, 1);//P1=(P1|0x8);
        }else{
            hal_gpio_fast_write(RGBOUT, 0);//P1=P1&0xF7;
        }
        delay_us(us5);
        tmp=tmp<<1;
        hal_gpio_fast_write(RGBOUT, 1);//P1=(P1|0x8); 
        delay_us(us10);
    }

    tmp=0x1;
    for(j=0;j<4;j++)
    {
        hal_gpio_fast_write(RGBOUT, 0);//P1=P1&0xF7;
        delay_us(us5);
        if ((a1&tmp)==0) 
        {
            hal_gpio_fast_write(RGBOUT, 1);//P1=(P1|0x8);
        }else{
            hal_gpio_fast_write(RGBOUT, 0);//P1=P1&0xF7;
        }
        delay_us(us5);
        tmp=tmp<<1;
        hal_gpio_fast_write(RGBOUT, 1);//P1=(P1|0x8); 
        delay_us(us10);
    }
   
    tmp=0x1;
    for(j=0;j<6;j++)
    {
        hal_gpio_fast_write(RGBOUT, 0);//P1=P1&0xF7;
        delay_us(us5);
        if ((r&tmp)==0) 
        {
            hal_gpio_fast_write(RGBOUT, 1);//P1=(P1|0x8);
        }else{
            hal_gpio_fast_write(RGBOUT, 0);//P1=P1&0xF7;
        }
        delay_us(us5);
        tmp=tmp<<1;
        hal_gpio_fast_write(RGBOUT, 1);//P1=(P1|0x8); 
        delay_us(us10);
    }
    tmp=0x1;
    for(j=0;j<6;j++)
    {
        hal_gpio_fast_write(RGBOUT, 0);//P1=P1&0xF7;
        delay_us(us5);
        if ((g&tmp)==0) 
        {
            hal_gpio_fast_write(RGBOUT, 1);//P1=(P1|0x8);
        }else{
            hal_gpio_fast_write(RGBOUT, 0);//P1=P1&0xF7;
        }
        delay_us(us5);
        tmp=tmp<<1;
        hal_gpio_fast_write(RGBOUT, 1);//P1=(P1|0x8); 
        delay_us(us10);
    }

    tmp=0x1;
    for(j=0;j<6;j++)
    {
        hal_gpio_fast_write(RGBOUT, 0);//P1=P1&0xF7;
        delay_us(us5);
        if ((b&tmp)==0) 
        {
            hal_gpio_fast_write(RGBOUT, 1);//P1=(P1|0x8);
        }else{
            hal_gpio_fast_write(RGBOUT, 0);//P1=P1&0xF7;
        }
        delay_us(us5);
        tmp=tmp<<1;
        hal_gpio_fast_write(RGBOUT, 1);//P1=(P1|0x8); 
        delay_us(us10);
    }

    
//delay_us(us5);
    hal_gpio_fast_write(RGBOUT, 0);//P1=P1&0xF7; 
    delay_us(us20);
    hal_gpio_fast_write(RGBOUT, 1);//P1=(P1|0x8);  
    delay_us(us20);   

}

static void light_start_timer(void)
{
  osal_start_timerEx(AppWrist_TaskID, TIMER_LIGHT_EVT, 30*1000);
  
}
static void light_stop_timer(void)
{
  osal_stop_timerEx(AppWrist_TaskID, TIMER_LIGHT_EVT);
}


static void light_reflash(void)
{
  if(s_light[0] + s_light[1] + s_light[2]){
    
    hal_pwm_close_channel(PWM_CH0);
    hal_pwm_destroy(PWM_CH0);
    hal_pwm_close_channel(PWM_CH1);
    hal_pwm_destroy(PWM_CH1);
    hal_pwm_close_channel(PWM_CH1);
    hal_pwm_destroy(PWM_CH1);
    hal_pwm_stop();
    hal_gpio_pin_init(GPIO_GREEN, IE);
    hal_gpio_pin_init(GPIO_RED, IE);
    hal_gpio_pin_init(GPIO_YELLOW, IE);
    hal_gpio_pull_set(GPIO_GREEN, WEAK_PULL_UP);
    hal_gpio_pull_set(GPIO_RED, WEAK_PULL_UP);
    hal_gpio_pull_set(GPIO_YELLOW, WEAK_PULL_UP);
    
    if(s_light[0]){
      hal_pwm_init(PWM_CH0, PWM_CLK_DIV_64, PWM_CNT_UP, PWM_POLARITY_RISING);
      hal_pwm_set_count_val(PWM_CH0, s_light[0], 100);
      hal_pwm_open_channel(PWM_CH0, GPIO_GREEN);
    }
    
    if(s_light[1]){
      hal_pwm_init(PWM_CH1, PWM_CLK_DIV_64, PWM_CNT_UP, PWM_POLARITY_RISING);
      hal_pwm_set_count_val(PWM_CH1, s_light[1], 100);
      hal_pwm_open_channel(PWM_CH1, GPIO_YELLOW);
    }
    
    if(s_light[2]){
      hal_pwm_init(PWM_CH2, PWM_CLK_DIV_64, PWM_CNT_UP, PWM_POLARITY_RISING);
      hal_pwm_set_count_val(PWM_CH2, s_light[2], 100);
      hal_pwm_open_channel(PWM_CH2, GPIO_RED);
    }
    
    hal_pwm_start();
    light_stop_timer();
    light_start_timer();
  }
  else
  {
    hal_pwm_close_channel(PWM_CH0);
    hal_pwm_destroy(PWM_CH0);
    hal_pwm_close_channel(PWM_CH1);
    hal_pwm_destroy(PWM_CH1);
    hal_pwm_close_channel(PWM_CH1);
    hal_pwm_destroy(PWM_CH1);
    hal_pwm_stop();
    hal_gpio_pin_init(GPIO_GREEN, IE);
    hal_gpio_pin_init(GPIO_RED, IE);
    hal_gpio_pin_init(GPIO_YELLOW, IE);
    hal_gpio_pull_set(GPIO_GREEN, WEAK_PULL_UP);
    hal_gpio_pull_set(GPIO_RED, WEAK_PULL_UP);
    hal_gpio_pull_set(GPIO_YELLOW, WEAK_PULL_UP);
    light_stop_timer();
  }
}

void light_timeout_handle(void)
{
  s_light[0] = 0;
  s_light[1] = 0;
  s_light[2] = 0;
  hal_pwm_close_channel(PWM_CH0);
  hal_pwm_destroy(PWM_CH0);
  hal_pwm_close_channel(PWM_CH1);
  hal_pwm_destroy(PWM_CH1);
  hal_pwm_close_channel(PWM_CH1);
  hal_pwm_destroy(PWM_CH1);
  hal_pwm_stop();
  hal_gpio_pin_init(GPIO_GREEN, IE);
  hal_gpio_pin_init(GPIO_RED, IE);
  hal_gpio_pin_init(GPIO_YELLOW, IE);
  hal_gpio_pull_set(GPIO_GREEN, WEAK_PULL_UP);
  hal_gpio_pull_set(GPIO_RED, WEAK_PULL_UP);
  hal_gpio_pull_set(GPIO_YELLOW, WEAK_PULL_UP);
  
}
int light_ctrl(uint8_t ch, uint8_t value)
{
  if(ch >2 || (value > 100))
    return PPlus_ERR_INVALID_PARAM;

  s_light[ch] = (uint16_t)value;

  light_reflash();
    return PPlus_SUCCESS;
}


int light_init(void)
{
  s_light[0] = 0;
  s_light[1] = 0;
  s_light[2] = 0;
  hal_gpio_pin_init(GPIO_GREEN, IE);
  hal_gpio_pin_init(GPIO_RED, IE);
  hal_gpio_pin_init(GPIO_YELLOW, IE);
  hal_gpio_pull_set(GPIO_GREEN, WEAK_PULL_UP);
  hal_gpio_pull_set(GPIO_RED, WEAK_PULL_UP);
  hal_gpio_pull_set(GPIO_YELLOW, WEAK_PULL_UP);
  //hal_gpio_pull_set(GPIO_GREEN, STRONG_PULL_UP);
  //hal_gpio_pull_set(GPIO_RED, STRONG_PULL_UP);
  //hal_gpio_pull_set(GPIO_YELLOW, STRONG_PULL_UP);
  
  osal_start_reload_timer( AppWrist_TaskID, TIMER_RGBLED_REFLASH_EVT , 50);
  return PPlus_SUCCESS;
}

