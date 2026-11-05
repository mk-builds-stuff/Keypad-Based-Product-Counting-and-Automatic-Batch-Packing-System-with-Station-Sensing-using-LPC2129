#include<LPC21XX.h>
#include "delay.h"

#define LCD_d 0xFF<<0
#define RS 1<<8
#define E 1<<9

void LCD_COMMAND(unsigned char);
void LCD_DATA(unsigned char);
void LCD_INIT(void);
void LCD_STR(unsigned char *);
void LCD_INTEGER(int n);

void LCD_COMMAND(unsigned char cmd)
{
    IOCLR0=LCD_d;
    IOSET0=cmd;
    IOCLR0=RS;
    IOSET0=E;
    delay_milliseconds(2);
    IOCLR0=E;
}
void LCD_DATA(unsigned char data)
{
    IOCLR0=LCD_d;
    IOSET0=data;
    IOSET0=RS;
    IOSET0=E;
    delay_milliseconds(2);
    IOCLR0=E;
}
void LCD_INIT(void)
{
    IODIR0|=LCD_d | RS | E;
    LCD_COMMAND(0X01);
    LCD_COMMAND(0x02);
    LCD_COMMAND(0x0c);
    LCD_COMMAND(0x38);
}
void LCD_STR(unsigned char *str)
{
    unsigned char pos=0;
    while(*str){
        LCD_DATA(*str++);
        pos++;
        if(pos==16)
            LCD_COMMAND(0xc0);
    }
}
void LCD_INTEGER(int n)
{
    signed char i=0;
    unsigned char arr[5];
    if(n==0)
        LCD_DATA('0');
    else{
        if(n<0){
            LCD_DATA('_');
            n=-n;
        }
        while(n>0){
           arr[i++]=n%10;
           n=n/10;
        }
        for(i=i-1;i>=0;i--)
            LCD_DATA(arr[i]+48);
    }
}
