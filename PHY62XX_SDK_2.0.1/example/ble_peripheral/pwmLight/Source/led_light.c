#include "led_light.h"
#include "pwm.h"
#include "OSAL.h"
#include "gpio.h"
#include "app_wrist.h"
#include "error.h"
#include "common.h"
#include "log.h"
                                
unsigned char r2=0,g2=0,b2=0;
unsigned char  i,k;
unsigned char j,p,r,g,b,r1,g1,b1,mode=0,br,mode_old=0,res=0;
unsigned char   cr[]={63,0x00,0x00,63,63,0x00,63};
unsigned char   cg[]={0x00,63,0x00,63,0x00,63,63};
unsigned char   cb[]={0x00,0x00,63,0x00,63,63,63};
static uint16_t s_light[3];
uint8_t s_rgb[3];

#if RGB_LED_LOOP
uint16_t cnt = 0;
//static uint8_t s_light_en[3];
void RGB_led_loop(void)
{
    if (mode==0) { md0();}
    if (mode==1) { md1();}
    if (mode==2) { md3();}
    if (mode==3) {r1=0x00;g1=32;b1=0x00;md0(); md5();}
    if (mode==3) {r1=32;g1=32;b1=0x00;md0(); md5();}
    if (mode==3) {r1=16;g1=16;b1=16;md0(); md4();}
    if (mode==3) {r1=32;g1=0x00;b1=16;md0(); md5();}
    if (mode==4) { md6();}
    if (mode==5) { md2();}
    if (mode==6) {r1=32;g1=0;b1=0;md0(); md7();}
    if (mode==6) {r1=0;g1=32;b1=0;md0(); md7();}
    if (mode==6) {r1=32;g1=32;b1=0;md0(); md7();}
    if (mode==6) {r1=32;g1=0;b1=16;md0(); md7();}
    if (mode==8) { md8();}
    if (mode==9) { md22();}
    if (mode==10) { play1();}


    if (mode==7) { 
        if (mode==7) { md1();}
        if (mode==7) { md3();}
        if (mode==7) {r1=0x00;g1=32;b1=0x00;md0(); md5();}
        if (mode==7) {r1=32;g1=32;b1=0x00;md0(); md5();}
        if (mode==7) {r1=16;g1=16;b1=16;md0(); md4();}
        if (mode==7) {r1=32;g1=0x00;b1=16;md0(); md5();}
        if (mode==7) { md6();}
        if (mode==7) { md2();}
        if (mode==7) {r1=32;g1=0;b1=0;md0(); md7();}
        if (mode==7) {r1=0;g1=32;b1=0;md0(); md7();}
        if (mode==7) {r1=32;g1=32;b1=0;md0(); md7();}
        if (mode==7) {r1=32;g1=0;b1=16;md0(); md7();}       
    }
    if(mode == MODE_NUM){
         ctrl_rgb(0,0,s_rgb[0],s_rgb[1],s_rgb[2]); 
    }
	if((cnt++ % 50) == 0){
	    LOG("mode:%d,r:%d, g:%d, b:%d\n", mode,s_rgb[0],s_rgb[1],s_rgb[2]);
	}
}
void md0()//
{
    for(k=0;k<64;k++)   
    {    
        if (r2>r1) r2--; else {if (r2<r1) r2++;}
        if (g2>g1) g2--; else {if (g2<g1) g2++;}
        if (b2>b1) b2--; else {if (b2<b1) b2++;}
        ctrl_rgb(0,0,r2,g2,b2); if (res==1) goto m_end;
        if ((r1==r2)&&(g1==g2)&&(b1==b2)) break;
    }
    r2=r1;g2=g1;b2=b1;
m_end:
    res=0;

}

void md1()//
{
    unsigned char k,j,i,t;
    for (k=0;k<7;k++)
    {
        r=cr[k];g=cg[k];b=cb[k];    
        i=1; 
        while(i<64)
        {   
            ctrl_rgb(i,0,r,g,b); if (res==1) goto m_end;
            i+=1;
        }//i
        ctrl_rgb(i-1,0,r,g,b); if (res==1) goto m_end;

    }
m_end:
    res=0;
    
}
void md2()//shine
{
    
    unsigned char k,j,i,t,r2,g2,b2;
    for (k=0;k<2;k++)
    {
        i=1; 
        while(i<64)
        {   
            j=0;
            r1=cr[i%6]>>2;g1=cg[i%6]>>2;b1=cb[i%6]>>2;
            r=r1;g=g1;b=b1;r2=63;g2=63;b2=63;
            while(j<64)
            {   
                j+=1;
                if (r<63) r++;if (g<63) g++;if (b<63) b++;
                ctrl_rgb(i,0,r,g,b); if (res==1) goto m_end;
                if (r2>r1) r2--;if (g2>g1) g2--;if (b2>b1) b2--;
                if (i==1) t=63; else t=i-1;
                ctrl_rgb(t,0,r2,g2,b2); if (res==1) goto m_end;
//j
            }
            i+10;// by aizj md i+=1;
        }//i
    } //k
////////////////////////////////

m_end:
    res=0;

}
void md22()//shine
{
    
    unsigned char k,j,i,t,r2,g2,b2,r3,g3,b3;
//unsigned char cr1[6],cg1[6],cb1[6];
//for (t=0;t<6;t++)
    {
//  cr1[t]=cr[t];cg1[t]=cg[t];cb1[t]=cb[t];
    }   

    for (k=0;k<6;k++)
    {
        i=0; 
        while(i<6)
        {   
            j=0;
            r1=cr[(i+2+k)%6];g1=cg[(i+2+k)%6];b1=cb[(i+2+k)%6];
            r=cr[(i+1+k)%6];g=cg[(i+1+k)%6];b=cb[(i+1+k)%6];
            r3=cr[(i+0)%6]>>0;g3=cg[(i+0)%6]>>0;b3=cb[(i+0)%6]>>0;
            r2=r;g2=g;b2=b;
            while(j<64)
            {   
                j+=1;
                if (r<r1) r++;if (g<g1) g++;if (b<b1) b++;
                if (r>r1) r--;if (g>g1) g--;if (b>b1) b--;
//ctrl_rgb((1*i+1)%63+1,0,r,g,b,130); if (res==1) goto m_end;
                ctrl_rgb(0,(i+1)%6+1,r,g,b); if (res==1) goto m_end;
                
                if (r2>r3) r2--;if (g2>g3) g2--;if (b2>b3) b2--;
                if (r2<r3) r2++;if (g2<g3) g2++;if (b2<b3) b2++;
//ctrl_rgb((1*i)%63+1,0,r2,g2,b2,130); if (res==1) goto m_end;
//ctrl_rgb(0,(i%6)+1,r2,g2,b2,330); if (res==1) goto m_end;
//j
            }
            i+=10;// by aizj md i+=1;
        }//i
    } //k
////////////////////////////////

m_end:
    res=0;

}

void md3()
{
    unsigned char k;
//r=255;g=10;b=255;
//ctrl_rgb(0,0,0,255,0,0,1); if (res==1) goto m_end;
    
    
    
    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,k,0,0);if (res==1) goto m_end;  
    }
    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,63,k,0);if (res==1) goto m_end; 
    }
    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,63-k,63,0);if (res==1) goto m_end;  
    }
    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,0,63,k);if (res==1) goto m_end; 
    }
    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,0,63-k,63);if (res==1) goto m_end;  
    }

    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,k,0,63);if (res==1) goto m_end; 
    }
    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,63,k,63);if (res==1) goto m_end;    
    }
    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,63,63,63-k);if (res==1) goto m_end; 
    }
    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,63-k,63,0);if (res==1) goto m_end;  
    }
    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,0,63-k,0);if (res==1) goto m_end;   
    }
    
//////////////////////////////////   
    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,k,0,0);if (res==1) goto m_end;  
    }

    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,63-k,0x0,0);if (res==1) goto m_end; 
    }
/////////////////////
    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,0,k,0);if (res==1) goto m_end;  
    }

    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,0,63-k,0x0);if (res==1) goto m_end; 
    }
///////////////////////////////
    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,0,0,k);if (res==1) goto m_end;  
    }

    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,0,0x0,63-k);if (res==1) goto m_end; 
    }

///////////////////////////////
    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,k,k,k);if (res==1) goto m_end;  
    }

    for(k=0;k<64;k++)  
    {
        ctrl_rgb(0,0,63-k,63-k,63-k);if (res==1) goto m_end; 
    }



m_end:
    res=0;
}
void md4()///shine
{
    unsigned char j;
    i=1;
    j=0;
    while (j<2)
    {
        i=1;
        while (i<64)
        {
            ctrl_rgb(i,0,0xff,0xff,0x0);if (res==1) goto m_end;
            ctrl_rgb(i,0,r1,g1,b1);if (res==1) goto m_end;
            i++;
        }//i
        j++;
    }
m_end:
    res=0;
}
void md5()///blink
{
    unsigned char j;
    i=1;
    j=0;
    while (j<2)
    {
        i=1;
        while (i<64)
        {
            ctrl_rgb(i,0,0xff,0xff,0xff);if (res==1) goto m_end;
            ctrl_rgb(i,0,r1,g1,b1);if (res==1) goto m_end;
            i++;
        }//i
        j++;
    }
m_end:
    res=0;

}

void md6()///blink_multi
{
    unsigned char t;
    for(k=0;k<3;k++)
    {
        j=0;
        while (j<2)
        {
            i=1;
            while (i<64)
            {
                r1=cr[i%6];g1=cg[i%6];b1=cb[i%6];
                ctrl_rgb(i,0,0xff,0xff,0xff);if (res==1) goto m_end;
                
                ctrl_rgb(i,0,r1>>2,g1>>2,b1>>2);if (res==1) goto m_end;
                i++;
            }//i
            j++;
        } //j
    }

m_end:
    res=0;
}
/*
void md66()///blink_multi
 {
     unsigned char t;
     for(k=0;k<3;k++)
     {
 j=0;
 while (j<2)
 {
  i=1;
while (i<64)
{
    r1=cr[i%6];g1=cg[i%6];b1=cb[i%6];
ctrl_rgb(i,0,0xff,0xff,0xff);if (res==1) goto m_end;
     
ctrl_rgb(i,0,r1>>2,g1>>2,b1>>2);if (res==1) goto m_end;
i++;
}//i
j++;
} //j
 }

 m_end:
  res=0;
 }
*/



void md7()//shine
{
    unsigned char k,j,i,t,r2,g2,b2;
    for (k=0;k<2;k++)
    {
        i=0; 
        while(i<31)
        {   
            j=0;
            r=r1;g=g1;b=b1;r2=63;g2=63;b2=63;
            while(j<64)
            {   
                j+=1;
                if (r<63) r++;if (g<63) g++;if (b<63) b++;
                ctrl_rgb((2*i+0)%63+1,0,r,g,b); if (res==1) goto m_end;
                ctrl_rgb((2*i+1)%63+1,0,r,g,b); if (res==1) goto m_end;
                if (r2>r1) r2--;if (g2>g1) g2--;if (b2>b1) b2--;
//if (i==1) t=63; else t=i-1;
                ctrl_rgb((2*i-1)%63+1,0,r2,g2,b2); if (res==1) goto m_end;
                ctrl_rgb((2*i-2)%63+1,0,r2,g2,b2); if (res==1) goto m_end;
//j
            }
            i+=1;
        }//i
    } //k
////////////////////////////////

m_end:
    res=0;
    ///////////////////////////////////////////
}


void read_rem( void )
{
// unsigned char i;
    mode++;
    if (mode > MODE_NUM) mode=0;
    if (mode!=mode_old)    {res=1;mode_old=mode;}
}


void md8()///shine
{
    unsigned char j;
    unsigned char m[14];
    char tmp,i;
    
    j=1;
    m[0]=10;m[1]=20;m[2]=30;m[3]=40;m[4]=50;m[5]=60;
    m[6]=50;m[7]=40;m[8]=30;m[9]=20;m[10]=10;m[11]=20;m[12]=30;m[13]=40;
    for(k=0;k<6;k++)
    {
        for(j=1;j<7;j++)
        {
            for(i=0;i<10;i++)
            {
                if (m[j-1+k+1]== m[j-1+k])  tmp=0;
                ctrl_rgb(0,j,m[j-1+k]+0-0,0x0,0x0);if (res==1) goto m_end;
                
                
                if (m[j-1+k+1]> m[j-1+k])   //tmp=i;
//if (m[j-1+k+1]== m[j-1+k])    tmp=0;
//if (m[j-1+k+1]< m[j-1+k]) tmp=-i;

                    ctrl_rgb(0,j,m[j-1+k]+i-0,0x0,0x0);if (res==1) goto m_end;
                if (m[j-1+k+1]< m[j-1+k])   //tmp=-i;
                    ctrl_rgb(0,j,m[j-1+k]-i-0,0x0,0x0);if (res==1) goto m_end;
            }//i
        }
    }

m_end:
    res=0;
}



void play1()
{
    
    unsigned char a,r,g,b;//,t,tt;
    int k,t,tt;

    //while(1)
    {
        for (k=0;k<96;k++)
        {
            a=1;
            while(a<6)
            {
                tt=((k+a)/rnb1)%3;
                t=(k+a)%rnb1;
                if (tt==0) {r=255-(t*16);g=(t*16);b=0;}
                if (tt==1) {g=255-(t*16);b=(t*16);r=0;}
                if (tt==2) {b=255-(t*16);r=(t*16);g=0;}
                ctrl_rgb(0,a+0,r>>2,g>>2,b>>2);if (res==1) goto m_end;
                a++;
            } //while
        }//for
    }
m_end:
    res=0;
}  
#endif  //end RGB_LED_LOOP

void delay_us(unsigned int t)
{   
    if(t == 1){
        for(unsigned int i=0; i<1;i++)
            for(unsigned int j=0; j<33;j++);
        //WaitUs(t);
    }else{
        WaitUs(t);
    }
}
void init_rgb(void)
{
	//hal_gpio_pull_set(RGBOUT, WEAK_PULL_UP);
//	hal_gpio_pin_init(LEDOUT, OEN); 
	hal_gpio_pin_init(PIO1, OEN); 
	hal_gpio_pin_init(PIO2, OEN); 
	hal_gpio_pin_init(PIO3, OEN); 
	hal_gpio_pin_init(PIO4, OEN); 
	hal_gpio_pin_init(RGBOUT, OEN);
}
void SetMode(int mode)
{
   switch(mode){
	case CMD_COLOR1:
			 hal_gpio_fast_write(PIO4, 0);hal_gpio_fast_write(PIO3, 0); 
			 hal_gpio_fast_write(PIO2, 0); hal_gpio_fast_write(PIO1, 1);
			 break;
	 case CMD_COLOR2:
			 hal_gpio_fast_write(PIO4, 0);hal_gpio_fast_write(PIO3, 0); 
			 hal_gpio_fast_write(PIO2, 1); hal_gpio_fast_write(PIO1, 0);
			 break;	 
	 case CMD_COLOR3:
			 hal_gpio_fast_write(PIO4, 0);hal_gpio_fast_write(PIO3, 0); 
			 hal_gpio_fast_write(PIO2, 1); hal_gpio_fast_write(PIO1, 1);
			 break;
	 case CMD_COLOR4:
			 hal_gpio_fast_write(PIO4, 0);hal_gpio_fast_write(PIO3, 0); 
			 hal_gpio_fast_write(PIO2, 1); hal_gpio_fast_write(PIO1, 0);
			 break;	 		
	 case CMD_COLOR5:
			 hal_gpio_fast_write(PIO4, 0);hal_gpio_fast_write(PIO3, 0); 
			 hal_gpio_fast_write(PIO2, 1); hal_gpio_fast_write(PIO1, 1);
			 break;
	 case CMD_COLOR6:
			 hal_gpio_fast_write(PIO4, 0);hal_gpio_fast_write(PIO3, 0); 
			 hal_gpio_fast_write(PIO2, 1); hal_gpio_fast_write(PIO1, 0);
			 break;
	 case CMD_MODE1://9
			 hal_gpio_fast_write(PIO4, 1);hal_gpio_fast_write(PIO3, 0); 
			 hal_gpio_fast_write(PIO2, 0); hal_gpio_fast_write(PIO1, 1);
			 break;				 
	 case CMD_MODE2://10
			 hal_gpio_fast_write(PIO4, 1);hal_gpio_fast_write(PIO3, 0); 
			 hal_gpio_fast_write(PIO2, 1); hal_gpio_fast_write(PIO1, 0);
			 break;				 
	 case CMD_MODE3://11
			 hal_gpio_fast_write(PIO4, 1);hal_gpio_fast_write(PIO3, 0); 
			 hal_gpio_fast_write(PIO2, 1); hal_gpio_fast_write(PIO1, 1);
			 break;				 
	case CMD_MODE4://12
			 hal_gpio_fast_write(PIO4, 1);hal_gpio_fast_write(PIO3, 1); 
			 hal_gpio_fast_write(PIO2, 0); hal_gpio_fast_write(PIO1, 0);
			 break;		 
	case CMD_MODE5://13
			 hal_gpio_fast_write(PIO4, 1);hal_gpio_fast_write(PIO3, 1); 
			 hal_gpio_fast_write(PIO2, 0); hal_gpio_fast_write(PIO1, 1);
			 break;				 
	case CMD_MODE6://14
			 hal_gpio_fast_write(PIO4, 1);hal_gpio_fast_write(PIO3, 1); 
			 hal_gpio_fast_write(PIO2, 1); hal_gpio_fast_write(PIO1, 0);
			 break;	
    case CMD_MODEOFF://15
			 hal_gpio_fast_write(PIO4, 1);hal_gpio_fast_write(PIO3, 1); 
			 hal_gpio_fast_write(PIO2, 1); hal_gpio_fast_write(PIO1, 1);
			 break;			 
	default: break;
	}
}

void ctrl_rgb(unsigned  char a0,unsigned  char a1,unsigned  char r,unsigned char g,unsigned  char b)
{
    unsigned char j,tmp,i;
    unsigned int ii;

   // r=0xff;
    
    //LOG("mode:%d,a0:%d, a1:%d,r:%d, g:%d, b:%d\n", mode,a0, a1,r,g,b);
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

