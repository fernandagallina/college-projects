#include <stdio.h>      /* para printf() e fprintf() */
#include <sys/socket.h> /* para socket(), connect(), send(), e recv() */
#include <arpa/inet.h>  /* para sockaddr_in and inet_addr() */
#include <stdlib.h>     /* para atoi() e exit() */
#include <string.h>     /* para memset() */
#include <unistd.h>     /* para close() */
#include <iostream>

using namespace std;

#define RCVBUFSIZE 255   /* Tamanho do buffer de recebimento */

int main(int argc, char *argv[])
{
    int sock;                        /* Descritor de socket */
    struct sockaddr_in echoServAddr; /* Struct para especificar o endereco do servidor */
    unsigned short echoServPort;     /* Numero da porta do servidor */
    char *servIP;                    /* Endereco IP do servidor */
    char echoString[RCVBUFSIZE];                /* String para ser enviada ao servidor */
    char echoBuffer[RCVBUFSIZE];     /* Buffer para receber a string vinda como resposta do servidor */
    unsigned int echoStringLen;      /* Tamanho da string */
    int bytesRcvd, totalBytesRcvd;   /* Numero de bytes recebidos pela funcao recv() 
                                        e numero total de bytes recebidos */

    if (argc < 3)    /* Testa se o numero de argumentos esta correto */
    {
       fprintf(stderr, "Usage: %s <Server IP> <Echo Port>\n",
               argv[0]);
       exit(1);
    }

    servIP = argv[1];             /* Primeiro argumetno: endereco IP do servidor */
    echoServPort = atoi(argv[2]); /* Segundo argumento: porta do servidor */
    //echoString = argv[3];         /* Terceiro argumetno: string com a mensagem a ser enviada ao servidor */

    /* Cria um socket TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket");
        exit(1);
    }

    /* Constroi struct de endereco do servidor */
    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Preenche struct com zeros */
    echoServAddr.sin_family      = AF_INET;             /* Famila de enderecos de internet (IP) */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Endereco IP do servidor */
    echoServAddr.sin_port        = htons(echoServPort); /* Porta do servidor */

    /* Estabelece uma conexao com o servidor */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
        perror("connect");
        exit(1);
    }

    cout << "Comando desejado:" << endl;
    fgets(echoString, RCVBUFSIZE-1, stdin);
    echoStringLen = strlen(echoString);          /* Determina o comprimento da string de entrada */
    echoString[--echoStringLen] = '\0';

    /* Envia uma string para o servidor */
    if (send(sock, echoString, echoStringLen+1, 0) != echoStringLen+1) {
        perror("send");
        exit(1);
    }

    /* Recebe a mesma string de volta do servidor */
    totalBytesRcvd = 0;
//    for (;;)
//    {
        /* Recebe dados do servidor ate o tamanho do buffer (menos 1 para deixar espaco para
           o '\0' terminador de string */
        if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0) {
            perror("recv");
            exit(1);
        }
        totalBytesRcvd += bytesRcvd;   /* Atualiza o total de bytes recebidos */
        echoBuffer[bytesRcvd] = '\0';  /* Adiciona o terminador de string */
        printf("%s", echoBuffer);            /* Imprime o conteudo do buffer recebido */
//    }

    printf("\n");    /* Imprime o final de linha */

    close(sock);
    exit(0);
}
