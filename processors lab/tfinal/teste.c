#define __msp430_headers_dma_h

#include <msp430x261x.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "mprintf.h"
#include "lcd_LP.h"

#define rele1 (1<<0)
#define rele2 (1<<1)

#define led 1
   
#define SDA (1<<1) //P3.1 - PLACA 2011
#define SCL (1<<2) //P3.2 - PLACA 2011
#define SDA0 P3OUT &= ~SDA
#define SDA1 P3OUT |= SDA
#define SCL0 P3OUT &= ~SCL
#define SCL1 P3OUT |= SCL    
   
int temperaturaMantida;   
int horas, minutos, segundos;	
int horas2, minutos2, segundos2;

void menu()
{
	string_serial("1. CONFIGURA HORA PLACA\n\r");
	string_serial("2. CONFIGURA HORARIO INICIAL/FINAL\n\r");
	string_serial("3. CONFIGURA MOSTRA HORA\n\r");
	string_serial("4. TEMPERATURA DESEJADA\n\r");
	string_serial("5. MONITORAMENTO");
}  
   
void configura_portas(void)
{
	WDTCTL = WDTPW + WDTHOLD;
	DCOCTL = CALDCO_16MHZ;
	BCSCTL1 = CALBC1_16MHZ & 0x0f;
 	BCSCTL2 = 0x08;
	BCSCTL3 = 0x8c;
	 
    P3DIR = led;
    P3DIR = rs + en + SCL + SDA;
	P4DIR = dados;
	P5DIR = rele1 + rele2;
	
}

void start_i2c(void) //I2C START
{
	SDA1;
	espera(500);//500 microsegundos
	SCL1;
	espera(500);
	SDA0;
	espera(500);
	SCL0;
	espera(500);
}

void stop_i2c(void) //I2C STOP
{
	SDA0;
	espera(500);//500 microsegundos
	SCL0;
	espera(500);
	SCL1;
	espera(500);
	SDA1;
	espera(500);
}

void envia_1_i2c(void) //Envia 1 pelo I2C
{
	SDA1;
	espera(500);
	SCL1;
	espera(500);
	SCL0;
	espera(500);
}

void envia_0_i2c(void) //Envia 0 pelo I2C
{
	SDA0;
	espera(500);
	SCL1;
	espera(500);
	SCL0;
	espera(500);
}

int ack_i2c(void)
{
	int x;
	P3DIR &= ~SDA; //FAZ SDA COMO ENTRADA
	SCL1;
	espera(500);
	if((P3IN & SDA) == 0)
		x = 0;
	else 
		x = 1; //LE O PINO
	SCL0;
	espera(500); // LE 1 BIT
	P3DIR |= SDA; //FAZ SDA COMO SAIDA
	return x; //se 0 ok, se 1 erro
}

void envia_byte(int dado)
{
	
	if((dado & 0x80)==0)
		envia_0_i2c();
	else
		envia_1_i2c();
	
	if((dado & 0x40)==0)
		envia_0_i2c();
	else
		envia_1_i2c();
		
	if((dado & 0x20)==0)
		envia_0_i2c();
	else
		envia_1_i2c();
		
	if((dado & 0x10)==0)
		envia_0_i2c();
	else
		envia_1_i2c();	
	
	if((dado & 0x08)==0)
		envia_0_i2c();
	else
		envia_1_i2c();
	
	if((dado & 0x04)==0)
		envia_0_i2c();
	else
		envia_1_i2c();
	
	if((dado & 0x02)==0)
		envia_0_i2c();
	else
		envia_1_i2c();
		
	if((dado & 0x01)==0)
		envia_0_i2c();
	else
		envia_1_i2c();	
}

int le_byte(void)
{
	int i = 0;
	int x = 0;
	P3DIR &= ~SDA;
	for(i=0; i<8; i++)
	{
		SCL1;
		espera(500);
		x=x<<1;
		if((P3IN & SDA) == 0)
			x|=0;
		else
			x|=1;
		SCL0;
		espera(500);		
	}
	P3DIR |= SDA;
	return x;
}

void configura_rtc(int addr, int dado)
{
	int x = 0;
	int dezena, unidade;
	
	start_i2c();
	envia_1_i2c();
	envia_1_i2c();
	envia_0_i2c();
	envia_1_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_0_i2c(); //wr
	x = ack_i2c();
	envia_byte(addr);
	x = ack_i2c();
	dezena = dado/10;
	unidade = dado%10;
	dado = (dezena<<4)|(unidade);
	envia_byte(dado);
	x = ack_i2c();
	stop_i2c();	
}

void configura_sensor(int addr, int dado)
{
	int x = 0;
	
	start_i2c();
	envia_1_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_1_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_0_i2c(); //wr
	x = ack_i2c();
	envia_byte(addr);
	x = ack_i2c();
	envia_byte(dado);
	x = ack_i2c();
	stop_i2c();	
}

int le_rtc(int addr)
{
	int x = 0, dado, dezena, unidade;
	start_i2c();
	envia_1_i2c();
	envia_1_i2c();
	envia_0_i2c();
	envia_1_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_0_i2c(); //wr
	x = ack_i2c();
	envia_byte(addr);
	x = ack_i2c();
	start_i2c();
	envia_1_i2c();
	envia_1_i2c();
	envia_0_i2c();
	envia_1_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_1_i2c(); //rd
	x = ack_i2c();
	dado = le_byte();
	dezena = (dado>>4) & 0x0f;
	unidade = dado & 0x0f;
	dezena = dezena * 10;
	dado = dezena + unidade;
	x = ack_i2c();
	envia_1_i2c();//nack
	stop_i2c();	

	return dado;
}

int le_sensor(void)
{
	int x = 0, dado;

	start_i2c();
	envia_1_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_1_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_0_i2c();
	envia_1_i2c(); //rd
	x = ack_i2c();
	dado = le_byte();
	envia_1_i2c();//nack
	stop_i2c();	

	return dado;
}

void init_serial(void)
{
	P3DIR |= 0x10; //configura pino de TX como saida
	P3DIR &= ~0x20; //configura pino de RX como entrada
	P3SEL = 0x30; //seleciona a funcionalidade uart dos pinos P3.4 e P3.5,5
	UCA0CTL1 = UCSSEL_SMCLK | UCSWRST;//=0x81, configura SMCLK e serial parada
	UCA0CTL0 = 0X0; //8bits, sem paridade, 1 stop bit
	UCA0BR0 = 104; //ver tabela com UCOS16=1 para clock de 16MHz e baund 9600
	UCA0BR1 = 0; //ver tabela com UCOS16=1 para clock de 16MHz e baund 9600
	UCA0MCTL = UCBRF_3 | UCBRS_0 | UCOS16; // ver tabela UCBRF=3,UCBRS=0,UCOS16=1
	UCA0CTL1 &= ~UCSWRST; // Inicializa a maquina de estados da serial
}

void envia_serial(int dado)
{
	while(!(IFG2 & 2)); //enquanto n~ao terminou de enviar esperaa
	UCA0TXBUF = dado; //envia o dado
	IFG2 &= (~2); //limpa flag de transmiss~ao
}

void string_serial(char text[])
{
	int i;
	for(i = 0; i < strlen(text); i++)
	{
		envia_serial(text[i]);
	}
}

int le_serial(void)
{
	while(!(IFG2 & 1)); //enquanto n~ao recebe espera
	IFG2 &= (~1);       //limpa flag de recepção
	return UCA0RXBUF;   //retorna o dado recebido
}

void configuraHoraPlaca()
{	
    string_serial("\n\rHorario Placa: ");
    horas = (le_serial()-0x30)*10 + (le_serial()-0x30);
    
	configura_rtc(0x02, horas);
    
    envia_serial(le_rtc(0x02)/10 + 0x30);
	envia_serial(le_rtc(0x02)%10 + 0x30);
    
    string_serial("\n\rMinutos: ");
    minutos = (le_serial()-0x30)*10 + (le_serial()-0x30);
    envia_serial(minutos/10 + 0x30);
	envia_serial(minutos%10 + 0x30);
	
	configura_rtc(0x01, minutos);				
	
	string_serial("\n\rSegundos: ");	    
    segundos = (le_serial()-0x30)*10 + (le_serial()-0x30);
	envia_serial(segundos/10 + 0x30);
	envia_serial(segundos%10 + 0x30);
	
	configura_rtc(0x00, segundos);
    
    string_serial("\n\rHorario Placa: ");	    
    envia_serial(horas/10 + 0x30);
	envia_serial(horas%10 + 0x30);
    string_serial(":");	 
    envia_serial(minutos/10 + 0x30);
	envia_serial(minutos%10 + 0x30);
    string_serial(":");	 
    envia_serial(segundos/10 + 0x30);
	envia_serial(segundos%10 + 0x30);	
	
	string_serial("\n\r");	
	
	menu();
}

void configuraHorarioInicialFinal()
{	
	int dezena, unidade;
	
    string_serial("\n\rHorario Inicial: ");
    horas = (le_serial()-0x30)*10 + (le_serial()-0x30);
    dezena = horas/10;
    unidade = horas%10;
    envia_serial(dezena+0x30);
    envia_serial(unidade+0x30);
        
    string_serial("\n\rMinutos: ");
    minutos = (le_serial()-0x30)*10 + (le_serial()-0x30);
    dezena = minutos/10;
    unidade = minutos%10;
    envia_serial(dezena+0x30);
    envia_serial(unidade+0x30);
    
	string_serial("\n\rSegundos: ");	    
    segundos = (le_serial()-0x30)*10 + (le_serial()-0x30);
	dezena = segundos/10;
    unidade = segundos%10;
    envia_serial(dezena+0x30);
    envia_serial(unidade+0x30);
	
    string_serial("\n\rHorario Inicial: ");	    
    dezena = horas/10;
    unidade = horas%10;
    envia_serial(dezena+0x30);
    envia_serial(unidade+0x30);
    string_serial(":");	 
    dezena = minutos/10;
    unidade = minutos%10;
    envia_serial(dezena+0x30);
    envia_serial(unidade+0x30);
    string_serial(":");	
    dezena = segundos/10;
    unidade = segundos%10;
    envia_serial(dezena+0x30);
    envia_serial(unidade+0x30);	
	string_serial("\n\r");	
	
	string_serial("\n\r");	
	
	string_serial("\n\rHorario Final: ");
    horas2 = (le_serial()-0x30)*10 + (le_serial()-0x30);
    dezena = horas2/10;
    unidade = horas2%10;
    envia_serial(dezena+0x30);
    envia_serial(unidade+0x30);
    
    string_serial("\n\rMinutos: ");
    minutos2 = (le_serial()-0x30)*10 + (le_serial()-0x30);
    dezena = minutos2/10;
    unidade = minutos2%10;
    envia_serial(dezena+0x30);
    envia_serial(unidade+0x30);
	
	string_serial("\n\rSegundos: ");	    
    segundos2 = (le_serial()-0x30)*10 + (le_serial()-0x30);
	dezena = segundos2/10;
    unidade = segundos2%10;
    envia_serial(dezena+0x30);
    envia_serial(unidade+0x30);
	
    string_serial("\n\rHorario Final: ");	    
    dezena = horas2/10;
    unidade = horas2%10;
    envia_serial(dezena+0x30);
    envia_serial(unidade+0x30);
    string_serial(":");	 
    dezena = minutos2/10;
    unidade = minutos2%10;
    envia_serial(dezena+0x30);
    envia_serial(unidade+0x30);
    string_serial(":");	
    dezena = segundos2/10;
    unidade = segundos2%10;
    envia_serial(dezena+0x30);
    envia_serial(unidade+0x30);	
	string_serial("\n\r");	
	
	menu();
}

void ExibeHoraData(void)
{

	char buffer[30];
	char buffer2[30];

	msprintf(buffer, "Data: %d/%d/%d", le_rtc(0x04), le_rtc(0x05), le_rtc(0x06));
	string_serial(buffer);
	
	msprintf(buffer2, "Hora: %d:%d:%d", le_rtc(0x02), le_rtc(0x01), le_rtc(0x00));
	string_serial(buffer2);
	
	while(1)
	{

		msprintf(buffer, "Data: %d/%d/%d", le_rtc(0x04), le_rtc(0x05), le_rtc(0x06));
		lcd_goto(1,1);
		lcd_string(buffer);
	
		msprintf(buffer2, "Hora: %d:%d:%d", le_rtc(0x02), le_rtc(0x01), le_rtc(0x00));
		lcd_goto(2,1);
		lcd_string(buffer2);

	}



}

void temperaturaSetUp(void)
{
	string_serial("\n\rTemperatura a ser mantida: ");
	temperaturaMantida = (le_serial()-0x30)*10 + (le_serial()-0x30);
		
	string_serial("\n\r");	
	
	menu();
}

int vetor[20];

void ligaLampada(void) // RELÉ 1
{
	//desliga cooler
	P5OUT = 0x01; 
	
}

void ligaCooler(void) // RELÉ 2
{
	//desliga lampada
	P5OUT = 0x02;
}

void modoMonitoramento(void)
{
	int flag = 0, temperaturaSensor;
	string_serial("\n\rMONITORAMENTO\n\r");
	while(flag == 0)
	{
		if((le_rtc(0x02)>=horas) && (le_rtc(0x01)>=minutos) && (le_rtc(0x00)>=segundos))
		{
			temperaturaSensor = le_sensor();
			msprintf(vetor, "\n\rTemperatura = %d", temperaturaSensor);
			string_serial(vetor);
				
			if((temperaturaSensor - 1) < temperaturaMantida)
			{
				ligaLampada();
			}
						
			else if((temperaturaSensor + 1) > temperaturaMantida)
			{
				ligaCooler();
			}
			
			else P5OUT = 0x00;
		}		
		if((le_rtc(0x02)>=horas2) && (le_rtc(0x01)>=minutos2) && (le_rtc(0x00)>=segundos2))
		{
			flag = 1;
		}
	}
	
	menu();
}

int main(void)
{
	int i, x;
	int dezena, unidade;
	init_serial();
	configura_portas();
	lcd_init();
	lcd_goto(1,1);
	
	configura_sensor(0,0);
	
	menu();
	
	while(1)
	{
		x = le_serial()-0x30;
		
		switch(x)
		{
			 case 1:
				configuraHoraPlaca();
				break;
			
			case 2: 
			    configuraHorarioInicialFinal();			    
				break;
			 
			case 3:
			    ExibeHoraData();
				break;
			
			case 4: 
				temperaturaSetUp();
				break;
				
			case 5: 
				modoMonitoramento();
				break;
			
		}
		if(x == '1')
		{
			dezena = le_serial();
			unidade = le_serial();
		}	
	}
	
	
	
	
	return 0;
}

