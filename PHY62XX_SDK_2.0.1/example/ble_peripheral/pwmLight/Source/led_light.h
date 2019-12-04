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


#ifndef _LED_LIGHT_H
#define _LED_LIGHT_H

#include "types.h"

//add by aizj
#define us5 1
#define us10 (us5*2)
#define us20 (us5*18)//(us5*18)

//key and pio ctrl x4 for ble+stc mcu 
#define KEY_1     15
#define PIO1      23
#define PIO2      24
#define PIO3      25
#define PIO4      18

//ble board
//#define LEDOUT    18
#define RGBOUT    20
#define GPIO_GREEN    P31//P23 by aizj md
#define GPIO_YELLOW   P32//P31
#define GPIO_RED      P33

#define rnb1 16
#define MODE_NUM  11 
enum enCOLORTag{
   CMD_COLOR1=40,
   CMD_COLOR2,
	 CMD_COLOR3,
	 CMD_COLOR4,
	 CMD_COLOR5,
	 CMD_COLOR6,
	 CMD_MODE1=60,
	 CMD_MODE2,
	 CMD_MODE3,
	 CMD_MODE4,
	 CMD_MODE5,
	 CMD_MODE6,	
   CMD_MODEOFF,	
};
extern unsigned char mode;
extern uint8_t s_rgb[3];
void ctrl_led(int level);
void init_rgb(void);
void ctrl_rgb(unsigned  char a0,unsigned  char a1,unsigned  char r,unsigned char g,unsigned  char b);
void light_timeout_handle(void);
int light_ctrl(uint8_t ch, uint8_t value);
int light_init(void);
void read_rem( void );
void md0();
void md1();
void md2();
void md22();
void md3();
void md4();
void md5();
void md6();
void md7();
void md8();
void play1();
void read_rem( void );
void RGB_led_loop(void);

#endif

