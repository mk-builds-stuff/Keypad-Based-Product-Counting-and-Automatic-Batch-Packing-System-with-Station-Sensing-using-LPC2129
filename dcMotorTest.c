#include<LPC21XX.H>

#define IN1 1<<18
#define IN2 1<<19

int main()
{
	IODIR1=IN1|IN2;
	while(1)
	{
	    //Motor Start
        IOSET1=IN1;
        IOCLR1=IN2;
	}
}
