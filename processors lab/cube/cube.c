#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <arch/nxp/lpc23xx.h>                   /* LPC23xx definitions                */

#define CRYSTALFREQ 12000000
#define PREINT_RTC	365//int(12000000/32768)-1  /* Prescaler value, integer portion,PCLK = 12Mhz */
#define PREFRAC_RTC	6912//12000000-((PREINT_RTC+1)*32768)  /* Prescaler value, fraction portion,PCLK = 12Mhz */

#define camada1 (1<<11) 
#define camada2 (1<<12)
#define camada3 (1<<13)
#define led1    (1<<10) 
#define led2    (1<<9) 
#define led3    (1<<8) 
#define led4    (1<<31) 
#define led5    (1<<30) 
#define led6    (1<<29) 
#define led7    (1<<28)
#define led8    (1<<25)
#define led9    (1<<24)

/* Macros para habilitar/desabilitar IRQ/FIQ no CPSR */
#define habilitaFIQ() asm volatile(\
	"mrs r3, cpsr\n" \
	"bic r3, r3, # 0x40\n" \
	"msr cpsr, r3\n")

#define desabilitaFIQ() asm volatile(\
	"mrs r3, cpsr\n" \
	"orr r3, r3, # 0x40\n" \
	"msr cpsr, r3\n")

#define habilitaIRQ() asm volatile(\
	"mrs r3, cpsr\n" \
	"bic r3, r3, # 0x80\n" \
	"msr cpsr, r3\n")

#define desabilitaIRQ() asm volatile(\
	"mrs r3, cpsr\n" \
	"orr r3, r3, # 0x80\n" \
	"msr cpsr, r3\n")

/* Declaracoes especiais para rotinas de atendimento a interrupcoes*/
void SWI_Routine(void) __attribute__ ((interrupt("SWI")));
void IRQ_Routine(void) __attribute__ ((interrupt("IRQ")));
void FIQ_Routine(void) __attribute__ ((interrupt("FIQ")));

void T0_IRQHandler(void) __attribute__ ((interrupt("IRQ")));

/* cpuclock e' uma variavel global usada para armazenar a freq. efetiva da cpu */
int cpuclock;

/* Configura o clock com o PLL. */
int PLL_Init(int pllmulti, int plldiv, int cpudiv)
{
int k;
cpudiv &= -2;	/* cpudiv deve ser par */
if(PLLSTAT & (1 << 25)){	/* Se PLL nao estava configurado */
	PLLCON = 1;	/* Habilita PLL, desconectado */
	PLLFEED=0xAA;PLLFEED=0x55;
	}
PLLCON=0;	/* Desconecta e desabilita o PLL */
PLLFEED=0xAA;PLLFEED=0x55;
/* Depois deste comando deve-se usar FIO1PIN para acessar a porta P1 */
SCS = 0x21;	/* Liga o oscilador externo (12MHz) e o GPIO */
while( !(SCS & 0x40) );	/* Espera estabilizar o oscilador */
CLKSRCSEL = 0x1;	/* Usa o 12MHz como oscilador principal */
PLLCFG = ((plldiv-1)<< 16) + (pllmulti/2 - 1);
PLLFEED=0xAA;PLLFEED=0x55;
CCLKCFG=0;	/* Divisor do clock da CPU = 1 */
PLLCON=1;	/* Habilita o PLL	*/
PLLFEED=0xAA;PLLFEED=0x55;
k=80000;	/* No maximo 80000 voltas esperando o PLL lock */
while((!(PLLSTAT & (1<<26))) && k) k--;	/* espera PLL Lock */
/* Se conseguiu PLL Lock, conecta CPU clock ao PLL */
CCLKCFG=cpudiv-1;	/* Divisor por cpudiv na CPU */
if(k)	{
	cpuclock= pllmulti*CRYSTALFREQ/cpudiv/plldiv;
	PLLCON=3;	/* Conecta o PLL ao clock da CPU */
	}
else	cpuclock=CRYSTALFREQ/cpudiv;
PLLFEED=0xAA;PLLFEED=0x55;
return cpuclock;
}



void T0_IRQHandler(void) 
{
  static int cont=0,x=0,y=1;   
  
  //função de interrupção do timer


  T0IR        = 1;                      /* Clear interrupt flag               */
  VICVectAddr = 0;                      /* Acknowledge Interrupt              */
}


void init_timer(void)
{
//   	T0TCR = 0;		/* Desabilita T0 */
//	T0PR = 12000000/1000 - 1; /* Prescaler para incrementar T0TC a cada milisegundo */
//	T0TCR = 2;	/* Inicializa T0 */
//	T0TCR = 1;	/* Habilita T0	*/		
//	
//	T0MCR=0x03;	//interrupt e reseta quando MRO igual.
//	T0IR = 1;  //LIMPA GLAG DE INTERRUPCAO
//	T0MR0 = 1; // interrupcao a cada 1ms

	T0MR0         = 48000000/1000-1;                       /* 1msec = 12000-1 at 12.0 MHz */
  	T0MCR         = 3;                           /* Interrupt and Reset on MR0  */
  	T0TCR         = 1;                           /* Timer0 Enable               */
  	VICVectAddr4  = (unsigned long)T0_IRQHandler;/* Set Interrupt Vector        */
  	VICVectCntl4  = 15;                          /* use it for Timer0 Interrupt */
  	VICIntEnable  = (1  << 4);                   /* Enable Timer0 Interrupt     */
}

void espera(unsigned int t)
{
    unsigned int tf;
	tf = T1TC + t;		/* tf = Valor futuro do T0TC */
	while(tf != T1TC);	/* espera ate que x==T0TC */
}  

void init_timer1(void)
{
   	T1TCR = 0;		/* Desabilita T0 */
	T1PR = 48000000/1000 - 1; /* Prescaler para incrementar T0TC a cada milisegundo */
	T1TCR = 2;	/* Inicializa T0 */
	T1TCR = 1;	/* Habilita T0	*/		
} 

//*************************************************************//

void random(void)
{
	int camada, i, led; 
	for(i=0;i<40;i++)
	{
		camada = rand()%3+1;
		led =  rand()%9+1;

		switch(camada)
		{
			case 1: 
				FIO4CLR |= camada1;
				break;
			case 2: 
					FIO4CLR |= camada2;
					break;
			case 3: 
					FIO4CLR |= camada3;
					break;
		}
		
		switch(led)
		{
			case 1:
				FIO4CLR |= led1;
				espera(100);
				break;
			
			case 2:
				FIO4CLR |= led2;
				espera(100);
				break;
				
			case 3:
				FIO4CLR |= led3;
				espera(100);
				break;
			
			case 4:
				FIO4CLR |= led4;
				espera(100);
				break;
				
			case 5:
				FIO4CLR |= led5;
				espera(100);
				break;
				
			case 6:
				FIO4CLR |= led6;
				espera(100);
				break;
				
			case 7: 
				FIO4CLR |= led7;
				espera(100);
				break;
				
			case 8:
				FIO4CLR |= led8;
				espera(100);
				break;
				
			case 9:
				FIO4CLR |= led9;
				espera(100);
				break;
		}
		
		FIO4SET |= camada1 + camada2 + camada3 + led1 + led2 + led3 + led4 + led5 + led6 + led7 + led8 + led9;
	}	
}

void ponta(void)
{
	int estado = 0,i;
	FIO4SET |= camada1 + camada2+ camada3 + led1 + led2 + led3 + led4 + led5 + led6 + led7 + led8 + led9;

	while(estado < 7)
	{
		estado++; 
		switch(estado)
		{
			case 1:
				for(i=0;i<6;i++)
				{
					FIO4CLR |=  camada1 + led1;
					espera(10);
					FIO4SET |=  camada1 + led1;
				}
					espera(1000);
				break;
			
			case 2:
				for(i=0;i<6;i++)
				{
					FIO4CLR |= camada1 + led2 + led4;
					espera(10);
					FIO4SET |= camada1 + led2 + led4;
					FIO4CLR |= camada2 + led1;
					espera(10);
					FIO4SET |= camada2 + led1;			
				}	
					espera(1000);
				break;
			
			case 3:
				for(i=0;i<6;i++)
				{
					FIO4CLR |= camada1 + led3 + led5 + led7;
					espera(10);
					FIO4SET |= camada1 + led3 + led5 + led7;
					FIO4CLR |= camada2 + led2 + led4;
					espera(10);
					FIO4SET |= camada2 + led2 + led4;
					FIO4CLR |= camada3 + led1;
					espera(10);
					FIO4SET |= camada3 + led1;
				}
					espera(1000);
				break;
			
			case 4:
				for(i=0;i<6;i++)
				{
					FIO4CLR |= camada1 + led6 + led8;
					espera(10);
					FIO4SET |= camada1 + led6 + led8;
					FIO4CLR |= camada2 + led3 + led5 + led7;
					espera(10);			
					FIO4SET |= camada2 + led3 + led5 + led7;
					FIO4CLR |= camada3 + led2 + led4;
					espera(10);
					FIO4SET |= camada3 + led2 + led4;
				}
					espera(1000);
				break;
			
			case 5:
				for(i=0;i<6;i++)
				{
					FIO4CLR |= camada1 + led9;
					espera(10);
					FIO4SET |= camada1 + led9;
					FIO4CLR |= camada2 + led6 + led8;
					espera(10);
					FIO4SET |= camada2 + led6 + led8;
					FIO4CLR |= camada3 + led3 + led5 + led7;
					espera(10);
					FIO4SET |= camada3 + led3 + led5 + led7;
				}
					espera(1000);
				break;
			
			case 6:
				for(i=0;i<6;i++)
				{
					FIO4CLR |= camada2 + led9;
					espera(10);
					FIO4SET |= camada2 + led9;
					FIO4CLR |= camada3 + led6 + led8;
					espera(10);
					FIO4SET |= camada3 + led6 + led8;
				}
					espera(1000);
					break;
				
			case 7:
				for(i=0;i<6;i++)
				{
					FIO4CLR |= camada3 + led9;
					espera(10);
					FIO4SET |= camada3 + led9;
				}
				espera(1000);
				break;
		}
		espera(45);
	}
}

void laterais(void)
{
	// liga todo cubo
	FIO4CLR |= camada1 + camada2+ camada3 + led1 + led2 + led3 + led4 + led5 + led6 + led7 + led8 + led9;

	espera(1000);
	
	// desliga laterais
	FIO4SET |= camada1 + camada2+ camada3 + led1 + led2 + led3 + led4 + led5 + led6 + led7 + led8 + led9;

	int i;
	for(i=0;i<6;i++)
	{
		FIO4CLR |= camada1 + camada2 + camada3 + led1 + led2 + led3;
		espera(1000);
		FIO4SET |= led1 + led2 + led3;
		FIO4CLR |= led4 + led5 + led6;
		espera(1000);
		FIO4SET |= led4 + led5 + led6;
		FIO4CLR |= led7 + led8 + led9;
		espera(1000);
		FIO4SET |= led7 + led8 + led9;
	}
}

void espiral()
{
	int estado = 0;
	while(estado<27)
	{
		if(estado == 27)
			estado = 0;
		estado++;
		
		FIO4SET |= camada1 + camada2+ camada3 + led1 + led2 + led3 + led4 + led5 + led6 + led7 + led8 + led9;
		
		switch(estado)
		{
			case 1 :
				FIO4CLR |= camada1 + led1;
				break;
			case 2 :
				FIO4CLR |= camada1 + led2;
				break;
			
			case 3 :
				FIO4CLR |= camada1 + led3;
				break;
			
			case 4 :
				FIO4CLR |= camada1 + led6;
				break;
			
			 case 5 :
				FIO4CLR |= camada1 + led9;
				break;
			
			case 6 :
				FIO4CLR |= camada1 + led8;
				break;
			
			case 7 :
				FIO4CLR |= camada1 + led7;
				break;
			
			case 8 :
				FIO4CLR |= camada1 + led4;
				break;
			
			case 9 :
				FIO4CLR |= camada1 + led5;
				break;
			
			case 10:
				FIO4CLR |= camada2 + led5;
				break;
			
			case 11:
				FIO4CLR |= camada2 + led6;
				break;
			
			case 12:
				FIO4CLR |= camada2 + led9;
				break;
			
			case 13:
				FIO4CLR |= camada2 + led8;
				break;
			
			case 14:
				FIO4CLR |= camada2 + led7;
				break;
			
			case 15:
				FIO4CLR |= camada2 + led4;
				break;
			
			case 16:
				FIO4CLR |= camada2 + led1;
				break;
			
			case 17:
				FIO4CLR |= camada2 + led2;
				break;
			
			case 18:
				FIO4CLR |= camada2 + led3;
				break;
			
			case 19:
				FIO4CLR |= camada3 + led3;
				break;
			
			case 20:
				FIO4CLR |= camada3 + led6;
				break;
			
			case 21:
				FIO4CLR |= camada3 + led9;
				break;
			
			case 22:
				FIO4CLR |= camada3 + led8;
				break;
			
			case 23:
				FIO4CLR |= camada3 + led7;
				break;
			
			case 24:
				FIO4CLR |= camada3 + led4;
				break;
			
			case 25:
				FIO4CLR |= camada3 + led1;
				break;
			
			case 26:
				FIO4CLR |= camada3 + led2;
				break;
			
			case 27: 
				FIO4CLR |= camada3 + led5;
				break;
		}
		espera(300);
	}
}

void canvas()
{
	
}

int main (void) 
{
    char buffer[30];
	int i;
	int x=0;
	
	FIO4DIR = camada1 + camada2 + camada3 + led1 + led2 + led3 + led4 + led5 + led6 + led7 + led8 + led9;
	
	int pllm=12, plld=1, cpud=3; //cpu para 48Mhz
	int cpuclk;
	
	cpuclk=PLL_Init(pllm,plld, cpud);	/* Configura o sistema de clock */
	
	init_timer();
	init_timer1();
	habilitaIRQ();
	
	FIO4SET |= camada1 + camada2+ camada3 + led1 + led2 + led3 + led4 + led5 + led6 + led7 + led8 + led9;
	
	while (1) 
  	{        
		switch(botao)
		{
			case 1: 
				espiral();
				break;
			case 2:
				laterais();
				break;
			case 3:
				ponta();
				break;
			case 4:
				random();
				break;		
		}
	}
	return 0;
}

/* Estas rotinas sao chamados pelo crt.s
Devem existir, mas ainda nao estao sendo usadas */
void UNDEF_Routine(){}
void SWI_Routine(){}
void FIQ_Routine(){}

