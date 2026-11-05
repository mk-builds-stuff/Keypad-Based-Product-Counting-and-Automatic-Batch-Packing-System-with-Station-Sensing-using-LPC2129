#include<LPC21XX.H>

void delay_seconds(unsigned int);
void delay_milliseconds(unsigned int);

void delay_seconds(unsigned int seconds)
{
	T0PR=15000000-1;
	T0TCR=0x01;
	while(T0TC<seconds);
	T0TCR|=1<<1;
	T0TCR=0x00;
}
void delay_milliseconds(unsigned int ms)
{
	T0PR=15000-1;
	T0TCR=0x01;
	while(T0TC<ms);
	T0TCR|=1<<1;
	T0TCR=0x00;
}
