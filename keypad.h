#include<LPC21XX.H>
#include "lcdHeader.h"

#define c0 (IOPIN1 & (1<<23))
#define c1 (IOPIN1 & (1<<24))
#define c2 (IOPIN1 & (1<<25))
#define c3 (IOPIN1 & (1<<26))

#define r0 1<<27
#define r1 1<<28
#define r2 1<<29
#define r3 1<<30

//Modding Keypad values for batch quantity
unsigned int key_lut[4][4]={{1,2,3,4},{5,6,7,8},{9,0,255,255},{255,255,255,'S'}};

unsigned int keyscan(void);
unsigned int twodigit_keyscan(void);

unsigned int keyscan(void){
    unsigned char row_val,col_val;
    IODIR1|=r0|r1|r2|r3;
    while(1){
        IOCLR1=r0|r1|r2|r3;
        IOSET1=c0|c1|c2|c3;
    
        while((c0 && c1 && c2 && c3)==1);
        
        IOCLR1=r0;
        IOSET1=r1|r2|r3;
    
        if((c0 && c1 && c2 && c3) ==0){
            row_val=0;
            break;
        }
    
        IOCLR1=r1;
        IOSET1=r0|r2|r3;
    
        if((c0 && c1 && c2 && c3) ==0){
            row_val=1;
            break;
        }
    
        IOCLR1=r2;
        IOSET1=r0|r1|r3;
    
        if((c0 && c1 && c2 && c3) ==0){
            row_val=2;
            break;
        }
    
        IOCLR1=r3;
        IOSET1=r0|r1|r2;
    
        if((c0 && c1 && c2 && c3) ==0){
            row_val=3;
            break;
        }
    
    }
    
    if(c0==0)
        col_val=0;
    else if(c1==0)
        col_val=1;
    else if(c2==0)
        col_val=2;
    else
        col_val=3;
    
    delay_milliseconds(150);
    while((c0 && c1 && c2 && c3)==0);
    return key_lut[row_val][col_val];
}
unsigned int twodigit_keyscan(void)
{
	unsigned char dig1=0,dig2=0,confirm=0;
	unsigned int res=0;
	
	dig1=keyscan();
	dig2=keyscan();
	
	if(dig2=='S'){
		if(dig1>0 && dig1<=9)
			res=dig1;
		return res;
	}
	confirm=keyscan();
	if(confirm=='S'){
		if((dig1>0 && dig1<=9)&&(dig2<=9)){
			res=(dig1*10)+dig2;
			return res;
		}
	}
	return 0;
}
