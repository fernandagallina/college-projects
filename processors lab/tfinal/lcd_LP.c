#include <msp430x261x.h>                 /* LPC23xx definitions                */
#include "lcd_LP.h"

#include <string.h>
   
void lcd_comando(int comando)
{
	P3OUT &= ~rs;
	P4OUT = comando;
	
	P3OUT |= en;
	espera(1);
	P3OUT &= ~en;
	espera(1);
}   

void lcd_dado(int dado)
{
	P3OUT |= rs;
	
	P4OUT = dado;
	
	P3OUT |= en;
	espera(1);
	P3OUT &= ~en;
	espera(1);
}

void lcd_init(void)
{
	lcd_comando(0x38);
	lcd_comando(0x0e);
	lcd_comando(0x06);
	lcd_comando(0x01);
}

void lcd_goto(int linha, int coluna)
{
	if(linha==1)
		lcd_comando(0x80+linha-1);
	else
		lcd_comando(0xc0+coluna-1);
}

void lcd_clr(void)
{
	lcd_comando(0x01);
}

void lcd_string(char vector[])
{
	int i;
	for(i=0; i<strlen(vector); i++)
	{
		if(i==16)
			lcd_goto(2,1);
		lcd_dado(vector[i]);
	}
}

void espera(volatile long x)
{
	volatile long i;
	
	for(i = 0; i < 400*1; i++)
	{
	}
}

void lcd_correrString(char vector[])
{
	int i,falta,j;
	if(strlen(vector) > 32)
	{
		falta = strlen(vector)-32;
		for(j=0; j<falta; j++)
		{
			lcd_goto(1,1);
			for(i=j; i<(strlen(vector)+j-falta); i++)
			{
				if(i==16+j)
					lcd_goto(2,1);
				lcd_dado(vector[i]);
			}
			espera(350000);
		}
	}
	else
	{
		for(i=0; i<strlen(vector); i++)
		{
			if(i==16)
				lcd_goto(2,1);
			lcd_dado(vector[i]);
		}
	}
}
