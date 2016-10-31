#include <stdio.h>      /* para printf() e fprintf() */
#include <sys/socket.h> /* para socket(), bind(), e connect() */
#include <arpa/inet.h>  /* para sockaddr_in e inet_ntoa() */
#include <stdlib.h>     /* para atoi() e exit() */
#include <string.h>     /* para memset() */
#include <unistd.h>     /* para close()/sleep() */
#include <vector>
#include <iostream>
#include <time.h>
#include "memory.h"

#include <sstream>

#define ECHOMAX 255		/* Longest string to echo */
#define MAXPENDING 5    /* Numero maximo de conexoes que podem ficar esperando serem aceitas */
#define RCVBUFSIZE 255   /* Tamanho do buffer de recebimento */

using namespace std;

struct hosts{
	char ip[ECHOMAX];
	int portUDP;
	int portTCP;
	char name[ECHOMAX];
	time_t beat;
};

struct Dados {
	char ip[ECHOMAX];
	int portUDP;
	int portTCP;
	char name[ECHOMAX];
	vector<hosts> H;
};

void * heartbeatControl( void *args )
{
    struct Dados *dado = (struct Dados*) args;

    for(;;)
    {
        sleep(1);

        for( int i = 0; i < dado->H.size(); i++ )
        {
            if(abs(time(0) - dado->H.at(i).beat) > 30 )
            {
            	printf("Apagando host %s\n", dado->H.at(i).name);
            	dado->H.erase(dado->H.begin()+i);
            }    
        }
    }
}

void * recvUDP( void *args )
{
    struct Dados *dado = (struct Dados*) args;

    int sock;       /* Socket */
    struct sockaddr_in echoServAddr;    /* Local address */
    struct sockaddr_in echoClntAddr;    /* Client address */
    unsigned int cliAddrLen;    /* Length of incoming message */
    char echoBuffer[ECHOMAX];   /* Buffer for echo string */
    char echoAux[ECHOMAX];
	char aux[ECHOMAX];
    unsigned short echoServPort;    /* Server port */
    int recvMsgSize;    /* Size of received message */
    char serverName[ECHOMAX];

    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket");
        exit(1);
    }

    echoServPort = dado->portUDP;
    
    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket");
        exit(1);
    }

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof (echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;  /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);   /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);    /* Local port */

    const int trueValue = 1;
   setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &trueValue, sizeof(trueValue));
	#ifdef __APPLE__   // MacOS/X requires an additional call also
   setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &trueValue, sizeof(trueValue));
	#endif

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof (echoServAddr)) < 0) {
        perror("bind");
        exit(1);
    }

    for (;;) {/* Run forever */
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof (echoClntAddr);

        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,
                        (struct sockaddr *) &echoClntAddr,
                        &cliAddrLen)) < 0) {
            perror("recvfrom");
            exit(1);
        }

         // INFO
        char *token = strtok(echoBuffer, " ");
        strcpy(echoAux, token);
        
        int tem = 0;
        if(!strcmp(echoAux, "START"))
        {
        	token = strtok(NULL, " ");
        	strcpy(serverName, token);
            for(int i = 0; i < dado->H.size(); i++)
            {
                if(strcmp(dado->H.at(i).name, serverName) == 0)
                {
                    tem = 1;
                }                   
            }
            
            if ( tem == 0 ) // se não  existe
            {
                struct hosts element;
                strcpy(element.ip, inet_ntoa(echoClntAddr.sin_addr));
                strcpy(element.name, serverName);
                element.beat = time(0);
                dado->H.push_back(element);
            }

			printf("received START from %s \n", serverName);           
            char message[ECHOMAX];
            strcpy(message, "HEARTBEAT ");
            strcat(message, dado->name);
            /* Send received datagram back to the client */
            if (sendto(sock, message, strlen(message)+1, 0,
                   (struct sockaddr *) &echoClntAddr,
                   sizeof (echoClntAddr)) != strlen(message)+1) {
                perror("sendto");
                exit(1);
            }

        }
        else if(!strcmp(echoAux, "HEARTBEAT"))
        {
        	tem = 0;
        	for(int i = 0; i < dado->H.size(); i++)
            {
                if(strcmp(dado->H.at(i).name, serverName) == 0)
                {
                    tem = 1;
                }                   
            }
            
            if ( tem == 0 ) // se não  existe
            {
                struct hosts element;
                strcpy(element.ip, inet_ntoa(echoClntAddr.sin_addr));
                strcpy(element.name, serverName);
                element.beat = time(0);
                dado->H.push_back(element);
            }

        	token = strtok(NULL, " ");
        	strcpy(serverName, token);
        
            for( int i = 0; i < dado->H.size(); i++ )
            {
                if( strcmp(dado->H.at(i).name, serverName) == 0 )
                {
                    dado->H.at(i).beat = time(0);
					strcpy(dado->H.at(i).name, serverName);
					printf("received HEARTBEAT from %s \n", dado->H.at(i).name);           
                }
            }
        }
    }

    close(sock);
    return 0;
}

void * sendUDP( void *args )
{
	struct Dados *dado = (struct Dados*) args;

	int sock;		/* Socket descriptor */
	struct sockaddr_in echoServAddr;	/* Echo server address */
	struct sockaddr_in fromAddr;	/* Source address of echo */
	unsigned short echoServPort;	/* Echo server port */
	unsigned int fromSize;	/* In-out of address size for recvfrom() */
	char *servIP;		/* IP address of server */
	char *echoInitialString = (char *) malloc(100);
	char *echoString = (char *) malloc(100);	/* String to send to echo server */
	char echoBuffer[ECHOMAX + 1];	/* Buffer for receiving echoed string */
	int echoInitialStringLen;
	int echoStringLen;	/* Length of string to echo */
	int respStringLen;	/* Length of received response */
    int broadcast = 1;

	servIP = dado->ip;
	echoServPort = dado->portUDP;

	strcpy(echoInitialString, "START ");
	strcat(echoInitialString, dado->name);

	

	printf("%s\n", echoInitialString );

	if ((echoInitialStringLen = strlen(echoInitialString)) > ECHOMAX) {	/* Check input length */
		printf("Echo word too long");
		exit(1);
	}

	/* Create a datagram/UDP socket */
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("socket");
		exit(1);
	}

	if((setsockopt(sock,SOL_SOCKET,SO_BROADCAST,
				&broadcast,sizeof broadcast)) == -1)
	{
		perror("setsockopt");
		exit(1);
	}

	/* Construct the server address structure */
	memset(&echoServAddr, 0, sizeof (echoServAddr));	/* Zero out structure */
	echoServAddr.sin_family = AF_INET;	/* Internet addr family */
	echoServAddr.sin_addr.s_addr = inet_addr(servIP);	/* Server IP address */
	echoServAddr.sin_port = htons(echoServPort);	/* Server port */

	/* Send the string to the server */
	if (sendto(sock, echoInitialString, echoInitialStringLen+1, 0, (struct sockaddr *)
		   &echoServAddr, sizeof (echoServAddr)) != echoInitialStringLen+1) {
		perror("sendto");
		exit(1);
	}

	for(;;)
	{

        strcpy(echoString, "HEARTBEAT ");
        strcat(echoString, dado->name);

        if ((echoStringLen = strlen(echoString)) > ECHOMAX) 
        {   /* Check input length */
            printf("Echo word too long");
            exit(1);
        }

		sleep(10);
		printf("%s\n", echoString );

		/* Send the string to the server */
		if (sendto(sock, echoString, echoStringLen+1, 0, (struct sockaddr *)
			   &echoServAddr, sizeof (echoServAddr)) != echoStringLen+1) {
			perror("sendto");
			exit(1);
		}
	}

	close(sock);
	return 0;
}

void HandleTCPClient(int clntSocket, Dados* dado)
{
    char echoBuffer[ECHOMAX];        /* Buffer para a string de eco */

    int recvMsgSize;                    /* Tamanho da mensagem recebida */

    char echoAux[ECHOMAX];
    char newName[ECHOMAX];
    char message[ECHOMAX];
    char type[ECHOMAX];
    char value[ECHOMAX];
    char ads[ECHOMAX];
	char aux[ECHOMAX];

    int shut = 0;

    unsigned int address = 0;
    unsigned char byte = 0;
    unsigned int word = 0;

    /* Recebe uma mensagem do cliente */
    if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0) {
        perror("recv");
        exit(1);
    }

    /* Envia a string recebida de volta ao cliente e recebe outra ate o final da transmissao */

        char *token = strtok(echoBuffer, " ");
        if(strcmp(token, "NAME") == 0)
        {
            token = strtok(NULL, " ");
            if(token != NULL) // se há palavra após NAME :
            {
                strcpy(dado->name, token);
            }
            strcpy(message, "200 OK\n");
            strcat(message, dado->name);
            strcat(message, "\0");
        }
        else if(strcmp(token, "READ") == 0)
        {
            token = strtok(NULL, " ");
			if(token == NULL)
			{
				strcpy(message, "400 Not found\0");
			}
            else
            {
                strcpy(type, token);
                token = strtok(NULL, " ");
    			if(token == NULL)
    			{
    				strcpy(message, "400 Not found\0");
    			}
                else
                {
                    strcpy(ads, token);
                    address = atoi(ads);
                    if(strcmp(type, "byte") == 0)
                    {   
                        if (memory_read_byte(address, &byte) < 0)
                        {
                            printf("Erro ao ler byte da memoria.\n");
                            strcpy(message, "404 Invalid address\0");
                        }
                        else
                        {
                            strcpy(message, "200 OK\n");
                            strcat(message, type);
                            strcat(message, " ");
        					sprintf(aux, "%d", byte);
                            strcat(message, aux);
                        }
                    }
                    else if(strcmp(type, "word") == 0)
                    {
                        if (memory_read_word(address, &word) < 0)
                        {
                            printf("Erro ao ler byte da memoria.\n");
                            strcpy(message, "404 Invalid address\0");
                        }
                        else
                        {
                            strcpy(message, "200 OK\n");
                            strcat(message, type);
                            strcat(message, " ");
        					sprintf(aux, "%d", word);
                            strcat(message, aux);
                        }
                    }
                    else
                    {
                        strcpy(message, "400 Not found\0"); 
                    }
                }
            }

        }
        else if(strcmp(token, "WRITE") == 0)
        {
            token = strtok(NULL, " ");
			if(token == NULL)
			{
				strcpy(message, "400 Not found\0");
				
			}
            else
            {
                strcpy(type, token);
                
                token = strtok(NULL, " ");
    			if(token == NULL)
    			{
    				strcpy(message, "400 Not found\0");
    			}
                else
                {
                    strcpy(ads, token);
                    address = atoi(ads);

                    token = strtok(NULL, " ");
                    if(token == NULL)
                    {
                        strcpy(message, "400 Not found\0");
                    }
                    else
                    {
                        strcpy(value, token);

                        if(strcmp(type, "byte") == 0)
                        {
                            byte = (unsigned char) atoi(value);
                            if (memory_write_byte(address, &byte) < 0)
                            {
                              printf("Erro ao escrever byte na memoria.\n");
                              strcpy(message, "404 Invalid address\0");
                            }
                            else
                            {
                                strcpy(message, "200 OK\n");
                                strcat(message, type);
                                strcat(message, " ");
                                strcat(message, value);                    
                            }
                        }
                        else if(strcmp(type, "word") == 0)
                        {
                            word = (unsigned int) atol(value);
                            if (memory_write_word(address, &word) < 0)
                            {
                                printf("Erro ao escrever byte na memoria.\n");
                                strcpy(message, "404 Invalid address\0");
                            }
                            else
                            {
                                strcpy(message, "200 OK\n");
                                strcat(message, type);
                                strcat(message, " ");
                                strcat(message, value);
                            }
                        }
            			else
            		    {
            		        strcpy(message, "400 Not found\0");
            		    }
                    }
                }
            }
        } 
        else if(strcmp(token, "SHUTDOWN") == 0)
        {
            shut = 1;
            strcpy(message, "200 OK\0");
        }
        else if(strcmp(token, "HOSTS") == 0)
        {
            strcpy(message, "200 OK\n");
			int size = dado->H.size();
			sprintf(aux, "%d", size);
            strcat(message, aux);
            for(int i = 0; i < dado->H.size(); i++)
            {
                strcat(message, "\n");
                strcat(message, dado->H.at(i).ip);
                strcat(message, " ");
                strcat(message, dado->H.at(i).name);
            }
        }
        else
        {
            strcpy(message, "400 Not found\0");
        }

        /* Envia a mensagem de volta ao cliente */
        if (send(clntSocket, message, strlen(message)+1, 0) != strlen(message)+1) {
            perror("send");
            exit(1);
        }

        if(shut == 1)
        {
            exit(1);
        }

    

    close(clntSocket);    /* Fecha o socket desse cliente */
}

void * tcp( void *args )
{
	struct Dados *dado = (struct Dados*) args;
	int servSock;                    /* Descritor de socket para o servidor */
    int clntSock;                    /* Descritor de socket para o cliente */
    struct sockaddr_in echoServAddr; /* Struct para o endereco local */
    struct sockaddr_in echoClntAddr; /* Struct para o endereco do cliente */
    unsigned short echoServPort;     /* Numero da porta do servidor */
    unsigned int clntLen;            /* Tamanho da struct de endereco do cliente */

	echoServPort = dado->portTCP;

	/* Cria um socket TCP para receber conexoes de clientes */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket");
        exit(1);
    }

    /* Constroi uma estrutura para o endereco local */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Preenche a struct com zeros */
    echoServAddr.sin_family = AF_INET;                /* Faminha de enderecos Internet (IP) */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Permite receber de qualquer endereco/interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Porta local */

    /* "Amarra" o endereco local (o qual inclui o numero da porta) para esse servidor */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
        perror("bind");
        exit(1);
    }

    /* Faz com que o socket possa "escutar" a rede e receber conexoes de clientes */
    if (listen(servSock, MAXPENDING) < 0) {
        perror("listen");
        exit(1);
    }

    for (;;) /* Executa para sempre */
    {
        /* Configura o tamanho da struct de endereco do cliente */
        clntLen = sizeof(echoClntAddr);

        /* Espera por conexoes de clientes */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, 
                               &clntLen)) < 0) {
            perror("accept");
            exit(1);
        }

        /* Nesse ponto, um cliente ja conectou!
         * O socket clntSock contem a conexao com esse cliente! */

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

        /* Essa funcao e' chamada para tratar a conexao com o cliente, a qual e' definida pelo socket clntSock */
        HandleTCPClient(clntSock, dado);
    }

    close(servSock);
}


int main( int argc, char *argv[] )
{
	pthread_t recv, send, tcpTCP, beats;
	struct Dados dados; 

	// atributos a receber: porta UDP, porta TCP, nome
	if ((argc < 4)) {	/* Test for correct number of arguments */
		fprintf(stderr,
			"Usage: %s <UDP Port> <TCP Port> <Host name>\n",
			argv[0]);
		exit(1);
	}
	dados.portUDP = atoi(argv[1]);
	dados.portTCP = atoi(argv[2]);
	strcpy(dados.name, argv[3]);
	printf("%s\n", dados.name);

	if ( pthread_create( &beats, NULL, heartbeatControl, &dados ) != 0 )
	{
		printf( "Erro ao criar a thread BEATS.\n" );
		exit( -1 );
	}

	if ( pthread_create( &tcpTCP, NULL, tcp, &dados ) != 0 )
	{
		printf( "Erro ao criar a thread TCP.\n" );
		exit( -1 );
	}
    
	if ( pthread_create( &recv, NULL, recvUDP, &dados ) != 0 )
	{
		printf( "Erro ao criar a thread RECV.\n" );
		exit( -1 );
	}
	
	if ( pthread_create( &send, NULL, sendUDP, &dados ) != 0 )
	{
		printf( "Erro ao criar a thread SEND.\n" );
		exit( -1 );
	}

	pthread_exit( NULL );
}
