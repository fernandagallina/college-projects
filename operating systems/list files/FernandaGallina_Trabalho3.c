/**
    Esse programa acessa o sistema de arquivos, lista o conteúdo de diretórios de maneira recursiva e imprime os atributos
 de acesso, tamanho e nome de todos os arqvuiso existentes em cada diretório. Possui a estrutura "ls [-l] [-R] [nome_dir]".
 Quando não é lido o nome do diretório, usa-se o diretório corrente.

  Autor: Avelino F. Zorzo
  Data: 06.06.2012
  Modificado por Fernanda M. Gallina
  Data: 05.06.2014
**/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>

#define GREEN   "\x1b[32m"
#define YELLOW    "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define WHITE   "\x1b[37m"
#define RED   "\x1b[31m"

void imprimeAtributos(char *argumento)
{
    struct stat buf;

    if (!stat(argumento,&buf)) { /* Se nome de arquivo válido */

           /* imprime atributos de acesso do proprietário do arquivo */
           printf(GREEN "%1s",((buf.st_mode & S_IRUSR) ?  "r" : "-"));
           printf("%1s",((buf.st_mode & S_IWUSR) ?  "w" : "-"));
           printf("%1s",((buf.st_mode & S_IXUSR) ?  "x" : "-"));

           /* imprime attributos de acesso para membros do grupo */
           printf("%1s",((buf.st_mode & S_IRGRP) ?  "r" : "-"));
           printf("%1s",((buf.st_mode & S_IWGRP) ?  "w" : "-"));
           printf("%1s",((buf.st_mode & S_IXGRP) ?  "x" : "-"));

           /* imprime atributos de acesso para outros */
           printf("%1s",((buf.st_mode & S_IROTH) ?  "r" : "-"));
           printf("%1s",((buf.st_mode & S_IWOTH) ?  "w" : "-"));
           printf("%1s",((buf.st_mode & S_IXOTH) ?  "x" : "-"));

           printf(YELLOW"%7d  ", (unsigned int) buf.st_size);


        } else {
           printf("Arquivo não existe!\n");
        }
}

void listaDir(char *argv, int impAtrib, int recurs)
{
    DIR *dp = opendir (argv); /// abre diretório corrente
    struct dirent *ep;
    char nomeCompleto[512];

    printf(MAGENTA "\n%s :\n" WHITE, argv); /// Mostra diretório atual :)

       if (dp != NULL) {

        while (ep = readdir (dp)) { /// Enquanto ainda houver arquivos a serem lidos...

            strcpy(nomeCompleto, argv);
            strcat(nomeCompleto, "/");
            strcat(nomeCompleto, ep->d_name);

            if(impAtrib == 1) /// Se foi solicitado "-l" então impAtrib chega como '1'.
            {
                imprimeAtributos(nomeCompleto); /// Chama função que lista os atributos
                                                /// de determinado arquivo.
            }

             switch (ep->d_type) {
               case DT_UNKNOWN: printf(RED"?"); break;
               case DT_REG    : printf(RED"f"WHITE); break;
               case DT_DIR    : printf(RED"d"WHITE); break;
               default        : printf(RED"o"); break;
             }
             printf(BLUE " %5s\n" WHITE, ep->d_name);
          }
        closedir (dp); /// Fecha diretório atual

    /// Decidi não usar lista, pois quando abro o arquivo, mostrando seus diretórios e fechando em seguida,
    /// posso abrir novamente e chamar a recursividade para cada diretório dentro do diretório atual,
    /// atuando sempre recursivamente.

        dp = opendir(argv); /// Abre novamente o diretório para usar a recursividade
        while(ep = readdir(dp))
         if(strncmp(ep->d_name, ".", 1))
         {
          if((recurs == 1) && (ep->d_type == DT_DIR)) /// Se houver pedido de recursividade
            {                                         /// e arquivo 'atual' é um diretório
                                                      /// concatena-se diretório atual com
             sprintf(nomeCompleto, "%s/%s", argv, ep->d_name); /// diretório a ser "entrado"
             listaDir(nomeCompleto, impAtrib, recurs); /// * Para na haver erro de arquivo
                                                       /// NÃO EXISTE
            }
         }
    closedir(dp); /// Fecha diretório da "recursidade"
     }  else
         printf ("Não foi possível abrir o diretório %s", argv[1]);

}

int  main (int argc, char *argv[])
{
   if (argc < 2) {
                            /// Se não foi passado argumento,
       listaDir("./", 0, 0);/// passa-se o "./" no lugar
                            /// para ser listado então o diretório corrente.
    }

    if(argc == 2)           /// Se há apenas dois parâmetros, sabe-se que o segundo é o nome
        {                   /// ou -l ou -R.
            if(strcmp("-R", argv[1]) == 0)
                listaDir("./", 0, 1);
            else if(strcmp("-l", argv[1]) == 0)
                listaDir("./", 1, 0);
            else listaDir(argv[1], 0, 0);
        }
    else if(argc == 3)      /// Se há três parâmetros pode ter sido pedido ou -l ou -R
    {                       /// com o argumento ou ambos sem o nome do diretório.
        if(((strcmp("-R", argv[1]) == 0)&&(strcmp("-l", argv[2]) == 0))||
           ((strcmp("-R", argv[2]) == 0)&&(strcmp("-l", argv[1]) == 0)))
                listaDir("./", 1, 1);
        else if(strcmp("-l", argv[1]) == 0)
            listaDir(argv[2], 1, 0);
        else if(strcmp("-R", argv[1]) == 0)
            listaDir(argv[2], 0, 1);
    }
    else if(argc == 4)          /// Se há quatro parâmetros, tudo foi passado, então haverá
	    listaDir(argv[3], 1, 1);/// listagem de atributos e recursividade.

       return 0;
}
