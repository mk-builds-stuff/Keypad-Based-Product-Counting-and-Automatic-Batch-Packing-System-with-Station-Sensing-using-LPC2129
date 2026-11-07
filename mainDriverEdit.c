#include<LPC21XX.h>
#include "keypad.h"

#define rLED 1<<17    
#define gLED 1<<18
#define buzzer 1<<19

#define IN1 1<<16
#define IN2 1<<17

#define IR_SENSOR1    1<<16     //EINT0
#define IR_SENSOR2    1<<14     //EINT1

#define EINT0_fn 1<<0
#define EINT1_fn 1<<29

//global vars
volatile unsigned int products=0;
volatile unsigned char ir1_flag=0,ir2_flag=0;
volatile unsigned char faultFlag=0;
//volatile unsigned int ir1Count=0,ir2Count=0;

//ISR
void IR1_ISR(void)__irq //External interrupt 0
{


		if(ir1_flag==0 && ir2_flag==0){
				ir1_flag=1;
				T1TCR=0x03;//Resetting timer 1
				T1TCR=0x01;//Starting the timer 1
		}
    EXTINT=0x01;
    VICVectAddr=0;
}

void TIMER1_ISR(void)__irq
{
    T1IR=0x01;//Clearing Timer interrupt flag
    T1TCR=0x00;//Disabled the Timer 1
	
    //Stop the motor
    IOCLR1=IN1 | IN2;    
    
    faultFlag=1;
    VICVectAddr=0;
}
void IR2_ISR(void)__irq //External interrupt 1
{
    //T1TCR|=1<<1;//Reset Timer Counters
    //T1TCR=0x00;//Disabled the Timer 1
    if(ir1_flag == 1 && ir2_flag == 0){
			T1TCR = 0x00;//Disabled the Timer 1
			T1IR = 0x01;//Clearing the timer interrupt flag
			ir2_flag = 1;
    }else if(ir1_flag == 0){
        faultFlag=1;
        //Stop the motor
        IOCLR1=IN1 | IN2;
    }
    EXTINT=0x02;
    VICVectAddr=0;
}

int main()
{
    unsigned char batchno=0,input=0;
    unsigned int count=0;

    IODIR0|=rLED | gLED | buzzer;
	
		IOSET0 = rLED | gLED;  // Both LEDs OFF
		IOCLR0 = buzzer;       // Buzzer OFF

    IODIR1|= IN1|IN2;//For Motor inputs
    IOCLR1= IN1|IN2;//Making it Low
    LCD_INIT();
 
    PINSEL0|=EINT1_fn;
    PINSEL1|=EINT0_fn;
    
    VICIntSelect=0x00000000;
    
    VICVectCntl0=(1<<5)|14;      //EINT0
    VICVectAddr0=(int)IR1_ISR;
    
    VICVectCntl1=(1<<5)|5;
    VICVectAddr1=(int)TIMER1_ISR;
    
    VICVectCntl2=(1<<5)|15;      //EINT1
    VICVectAddr2=(int)IR2_ISR;
    
    EXTMODE=0x03;
    EXTPOLAR=0x00;//Falling edge sensitivity
    
    T1PR=0;
    T1MCR=0x07;//Interrupt + Reset on match + Stop the timer
    T1MR0=75000000-1; 

    while(1){
        LCD_COMMAND(0x01);
				EXTINT=0x03;
        T1IR=0x01;
        T1TCR=0x03;//Resetting timer 1
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
        delay_milliseconds(500);
        
        LCD_COMMAND(0x01);
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
        delay_milliseconds(500);
        
        LCD_COMMAND(0x01);//Clearing the display
        LCD_COMMAND(0x80);
        LCD_STR("Press Start!");
        do{
            input=keyscan();
        }while(input!='S');
        delay_milliseconds(500);

        EXTINT=0x03;//To clear prev interrupt flags
        T1IR=0x01;
        VICIntEnable= 1<<5 | 1<<14 | 1<<15;
        //Motor Start
        IOSET1=IN1;
        IOCLR1=IN2;
        
        LCD_COMMAND(0x01);        
        LCD_COMMAND(0x80);
        LCD_STR("Batch ");
        LCD_INTEGER(batchno);
        LCD_STR("Started");
        IOCLR0= rLED;
                
        while(count!=products){
                        if(faultFlag==1){
                            VICIntEnClr = 1<<5 | 1<<14 | 1<<15;//Disable the interrupts
                                                            //To prevent getting interrupts during fault
                            
														//IOCLR1 = IN1 | IN2;   // Motor OFF
														IOSET0 = rLED;        // Red LED OFF
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
                            LCD_COMMAND(0x01);
                            LCD_COMMAND(0x80);
                            LCD_STR("Curr Count:");
                            LCD_COMMAND(0xc0);
                            LCD_INTEGER(count);
                        }
        }
        //Stopping the motor
        IOCLR1= IN1|IN2;
        IOSET0=rLED;
             VICIntEnClr = 1<<5 | 1<<14 | 1<<15;
                if(count==products){
                    IOCLR0=gLED;
                    LCD_COMMAND(0x01);
                    LCD_COMMAND(0x80);
                    LCD_INTEGER(products);
                    LCD_STR("Products");
                    LCD_COMMAND(0xc0);
                    LCD_STR("Batch Complete!");
                    IOSET0=buzzer;
                    delay_milliseconds(250);
                    IOCLR0=buzzer;
                    delay_seconds(30);
                }
                count=0;
    }
}

