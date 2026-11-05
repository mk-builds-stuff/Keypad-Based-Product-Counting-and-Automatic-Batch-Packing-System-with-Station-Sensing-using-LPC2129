#include<LPC21XX.h>
#include "keypad.h"

#define rLED 1<<17
#define gLED 1<<18
#define buzzer 1<<19

#define IR_SENSOR1    1<<16
#define IR_SENSOR2    1<<14
#define EINT0_fn 1<<0
#define EINT1_fn 1<<29

//global vars
volatile unsigned int products=0;
volatile unsigned char ir1_flag=0,ir2_flag=0;
volatile unsigned char faultFlag=0;

//ISR
void IR2_ISR(void)__irq //External interrupt 1
{
    EXTINT=0x02;
    T1TCR|=1<<1;//Reset Timer Counters
    T1TCR=0x00;//Disabled the Timer 1
    if(ir1_flag)
			ir2_flag=1;
		else{
			faultFlag=1;
			//Stop the motor
			PWMTCR |= 1<<1;
			PWMTCR = 0x00;
		}
    VICVectAddr=0;
}
void TIMER1_ISR(void)__irq
{
    T1IR=0x01;//Clearing Timer interrupt flag
	  T1TCR|=1<<1;//Reset Timer Counters
    T1TCR=0x00;//Disabled the Timer 1
		//Stop the motor
		PWMTCR |= 1<<1;
		PWMTCR = 0x00;
	
		faultFlag=1;
    VICVectAddr=0;
}
void IR1_ISR(void)__irq //External interrupt 0
{

    EXTINT=0x01;
		if(ir1_flag==0){
			ir1_flag=1;
			T1TCR|=1<<1;//Resetting timer 1
			T1TCR=0x01;//Starting the timer 1
		}
    VICVectAddr=0;
}

int main()
{
    unsigned char batchno=0,input=0;
    unsigned int count=0;
    //pwm
    PINSEL1|=0x400; //PWM5 in P0.21
    IODIR0|=rLED | gLED | buzzer;
    LCD_INIT();

    PWMMR0=15000-1;
    PWMMR5=11250-1;//75% Duty cycle
    PWMMCR= 0x02;
    PWMPCR=1<<13;
		PWMLER=1<<5 |1<<0;
    PWMTCR=0x08;//Just Enabling PWM n not starting timer counter for pwm
    
    PINSEL0|=EINT1_fn;
    PINSEL1|=EINT0_fn;
    
    VICIntSelect=0x00000000;
    
    VICVectCntl0=(1<<5)|15;      //EINT1
    VICVectAddr0=(int)IR2_ISR;
    
    VICVectCntl1=(1<<5)|5;
    VICVectAddr1=(int)TIMER1_ISR;
    
    VICVectCntl2=(1<<5)|14;      //EINT0
    VICVectAddr2=(int)IR1_ISR;
    
    EXTMODE=0x03;
    EXTPOLAR=0x00;//Falling edge sensitivity
    
		T1PR=0;
    T1MCR=0x07;//Interrupt + Reset on match + Stop the timer
    T1MR0=(60 *15000000)-1;
    

    while(1){
				EXTINT=0x03;
				T1IR=0x01;
				T1TCR|=1<<1;//Resetting timer 1
				T1TCR=0x00;//Disabling the timer 1
        IOSET0=gLED;
				faultFlag=0;
				ir1_flag=0;
				ir2_flag=0;
			
        LCD_COMMAND(0x80);
				LCD_STR("Batch?");
				do{
				batchno=twodigit_keyscan();
				}while(batchno==0);
				LCD_COMMAND(0x01);
				LCD_COMMAND(0x80);
				LCD_STR("Batch:");
				LCD_COMMAND(0xc0);
				LCD_INTEGER(batchno);
				
				LCD_COMMAND(0x80);
				LCD_STR("Prod Quantity?");
				do{
        products=twodigit_keyscan();
				}while(products==0);
				LCD_COMMAND(0x01);
				LCD_COMMAND(0x80);
				LCD_STR("Prod Quantity:");
        LCD_COMMAND(0xc0);
        LCD_INTEGER(products);
				
				LCD_COMMAND(0x01);//Clearing the display
				LCD_COMMAND(0x80);
				LCD_STR("Press Start!");
				do{
					input=keyscan();
				}while(input!='S');
				
				EXTINT=0x03;
				T1IR=0x01;
				VICIntEnable= 1<<5 | 1<<14 | 1<<15;
        PWMTCR=0x09;//PWM start
				
        LCD_COMMAND(0x80);
        LCD_STR("Batch ");
        LCD_INTEGER(batchno);
				LCD_STR("Started");
        IOCLR0= rLED;
				
        while(count!=products){
						if(faultFlag==1){
							VICIntEnClr = 1<<5 | 1<<14 | 1<<15;//Disable the interrupts
																								//To prevent getting interrupts during fault
							faultFlag=0;
							ir1_flag=0;
							ir2_flag=0;
							LCD_COMMAND(0x01);
							LCD_STR("Belt Fault!");
							
							IOSET0=buzzer;
							delay_seconds(60);
							
							IOCLR0=buzzer;
							break;
						}
						if(ir1_flag && ir2_flag){
                count++;
								ir1_flag=0;
								ir2_flag=0;
            }
        }
				//Stopping the motor
        PWMTCR|=1<<1;//Resetting PWM timer
        PWMTCR=0x00;//Disabling pwm
        IOSET0=rLED;
				VICIntEnClr = 1<<5 | 1<<14 | 1<<15;
				if(count==products){
					IOCLR0=gLED;
					LCD_COMMAND(0x80);
					LCD_INTEGER(products);
					LCD_STR("Products");
					LCD_COMMAND(0xc0);
					LCD_STR("Batch Complete!");
					IOSET0=buzzer;
					delay_milliseconds(250);
					IOCLR0=buzzer;
				}
				count=0;
    }

}
