#include<LPC21XX.H>
#include "delay.h"

#define al_led1 1<<8
#define al_led2 1<<9
#define ir1 1<<14
#define ir2 1<<15

int main()
{
	IODIR0= al_led1 | al_led2;
	
	while(1){
		if(((IOPIN0>>14)&1)==0)
			IOCLR0=al_led1;
		else
			IOSET0=al_led1;

		if(((IOPIN0>>15)&1)==0)
			IOCLR0=al_led2;
		else
			IOSET0=al_led2;
		
	}	
}
