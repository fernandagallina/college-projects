#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include "Processo.h"

using namespace std;

int Preempcao(ifstream &arquivo, int numeroProcessos, Processo p[], int tC, int exec)
{
    int preemp = 0;

    for(int i = 0; i < numeroProcessos; i++)
    {
        if(tC >= p[i].getTempoChegada())
        {
            if(exec != i)
                {
                    if((p[exec].getPrioridade() > p[i].getPrioridade()) && (p[i].getBool() == true))
                    {
                        preemp = 1; /// Passa-se por todos processos e ve-se se algum chegou com prioridade melhor
                                    /// se SIM, informa-se PREEMPÇÃO.
                    }
                }
        }
    }

    return preemp;
}

void SJF(ifstream &arquivo, int numeroProcessos, Processo p[])
{
    /// tC = tempo de contagem, diferente de TC que é tempo de troca.
    /// tE = tempo de execução recebe um valor alto para ser substituido na condição, menorTE = menor tempo de execução
    int tC = 0, tE = 100, menorTE = -1, auxiliarContagem = 0, auxiliarTraco, numProcessosNrodados;
    float somatorioTempoEspera = 0, tempoEspera, somatorioTurnaround = 0, turnaround, somatorioTempoResposta = 0, tempoResposta;
    float ta, te, tr;
    bool aux = true;

    numProcessosNrodados = numeroProcessos;

    while(aux == true)
    {
        menorTE = -1;
        auxiliarTraco = 0;
        for(int i = 0; i < numeroProcessos; i++)
        {
             if(tC < p[i].getTempoChegada()) /// se não há processos, imprime "-"
           {
               auxiliarTraco++;
               if(auxiliarTraco == numProcessosNrodados)
               {
                   cout<<"-";
                   tC++;
               }
           }

            if((tC >= p[i].getTempoChegada()) && (p[i].getBool() == true))
            {
                if(p[i].getTempoExecucao() < tE)
                {
                    tE = p[i].getTempoExecucao(); /// se tExecucao do processo 'i' for menor que do tExecucao atual, troca-se
                    menorTE = i;
                }
                if(p[i].getTempoExecucao() == tE)
                {
                    if(p[i].getPrioridade() < p[menorTE].getPrioridade())
                    {
                        tE = p[i].getTempoExecucao(); /// se forem iguais os tempos então opta-se por prioridade
                        menorTE = i;
                    }
                }
            }
        }
        if(menorTE != -1) /// verifica se há processo a ser executado
        {
            for(int x = 0; x < p[menorTE].getTempoExecucao(); x++)
            {
                printf("%i", menorTE + 1); /// 'executa' todo o processo
                if(x == 0)
                {/// se é a primeira vez que se executa então diminui-se tempo de contagem atual com tempo de chegada do processo
                    tempoResposta = tC - p[menorTE].getTempoChegada();
                    somatorioTempoResposta = somatorioTempoResposta + tempoResposta;
                }
                tC ++; /// logo após cada 'execução' incrementa 1 em tempo de contagem
            }
            p[menorTE].setBool(false);
            numProcessosNrodados = numProcessosNrodados - 1;
            turnaround = tC - p[menorTE].getTempoChegada();
            tempoEspera = turnaround - p[menorTE].getTempoExecucao();
            somatorioTurnaround = somatorioTurnaround + turnaround;
            somatorioTempoEspera = somatorioTempoEspera + tempoEspera;
            tE = 1000;
            tC = tC + 2; /// TEMPO DE TROCA LEVA 2 UNIDADES DE TEMPO.
            cout << "TC" ;
        }

        for(int j = 0; j < numeroProcessos; j++)
        {
            if(p[j].getBool() == false) /// sempre se checa se todos processo acabaram, para poder sair do while
            {
                auxiliarContagem = auxiliarContagem + 1;
            }

            if(auxiliarContagem == numeroProcessos)
                aux = false;
        }

        auxiliarContagem = 0;
      }

      //somatorioTempoResposta = somatorioTurnaround - somatorioTempoEspera ;
      tr = somatorioTempoResposta/numeroProcessos;
      ta = somatorioTurnaround/numeroProcessos;
      te = somatorioTempoEspera/numeroProcessos;
      cout << "\nTURNAROUND: " << ta;
      cout << "\nTEMPO ESPERA: " << te;
      cout << "\nTEMPO RESPOSTA: " << tr;
}

void RoundRobin(ifstream &arquivo, int numeroProcessos, Processo p[], int fatiaTempo)
{
   int tC = 0, auxiliarContagem = 0, auxiliarTempo = 0, auxiliarTraco, numProcessosNrodados, estavaExec = -1;
   float somatorioTempoEspera = 0, tempoEspera, somatorioTurnaround = 0, turnaround, somatorioTempoResposta = 0, tempoResposta;
   float ta, te, tr;
   bool aux = true;
   numProcessosNrodados = numeroProcessos;
   while(aux == true)
   {
      auxiliarTraco = 0;

      for(int i = 0; i < numeroProcessos; i++)
       {
             if(tC < p[i].getTempoChegada()) /// se não há processos, imprime "-"
           {
               auxiliarTraco++;
               if(auxiliarTraco == numProcessosNrodados)
               {
                   cout<<"-";
                   tC++;
               }
           }

            if((tC >= p[i].getTempoChegada()) && p[i].getBool() == true)
            { /// executa processos por ordem de chegada, e sequencialmente
                     if((estavaExec != i)&&(estavaExec != -1))
                    {
                        tC = tC + 2;
                        cout<<"TC";
                    }
                    estavaExec = i;
                    for(int j = 0; j < fatiaTempo; j++)
                    { /// é executado apenas até certa fatia de tempo
                            if(p[i].getTempoExecucao() > 0)
                            {
                                if(p[i].getPrimeiraVez() == 1)
                                {/// se é a primeira vez que se executa então diminui-se tempo de contagem atual com tempo de chegada do processo
                                    p[i].setPrimeiraVez(0);
                                    tempoResposta = tC - p[i].getTempoChegada();
                                    somatorioTempoResposta = somatorioTempoResposta + tempoResposta;
                                    //cout << "\ntC: "<<tC<<"\ttempoChegada: "<< p[i].getTempoChegada()<<"\n";
                                }
                                cout << i+1;
                                /// a cada tempo passado decrementa-se 1 e tC soma-se 1...
                                auxiliarTempo = p[i].getTempoExecucao() - 1;
                                p[i].setTempoExecucao(auxiliarTempo);
                                tC ++;
                            }

                            if(p[i].getTempoExecucao() == 0 && p[i].getBool() == true)
                            { /// se processo já acabou, recebe informação para não executar mais
                                p[i].setBool(false);
                                numProcessosNrodados = numProcessosNrodados - 1;
                                turnaround = tC - p[i].getTempoChegada() - 1;
                                somatorioTurnaround = somatorioTurnaround + turnaround;
                                tempoEspera = turnaround - p[i].getTempoExecucaoInicio();
                                somatorioTempoEspera = somatorioTempoEspera + tempoEspera;
                               // cout << "\nturn\t"<<turnaround <<"\ntempoExecucao\t"<< p[i].getTempoExecucaoInicio()<<endl;
                            }
                    }

                //tC = tC + 2;
                //cout << "TC";

            }

            for(int j = 0; j < numeroProcessos; j++)
            {
                if(p[j].getBool() == false)
                {
                    auxiliarContagem = auxiliarContagem + 1;
                }

                if(auxiliarContagem == numeroProcessos)
                    aux = false;
            }
            auxiliarContagem = 0;
      }
   }
      tr = somatorioTempoResposta/numeroProcessos;
      ta = somatorioTurnaround/numeroProcessos;
      te = somatorioTempoEspera/numeroProcessos;
      cout << "\nTURNAROUND: " << ta;
      cout << "\nTEMPO ESPERA: " << te;
      cout << "\nTEMPO RESPOSTA: " << tr;
}

void RoundRobinPreemptivo(ifstream &arquivo, int numeroProcessos, Processo p[], int fatiaTempo)
{
   int tC = 0, auxiliarTraco, numProcessosNrodados, melhorPrioridade = 100, exec = -1, estavaExec = -1, pegouExec = 0, auxiliarContagem = 0, VerificaPreempcao;
   float somatorioTempoEspera = 0, tempoEspera, somatorioTurnaround = 0, turnaround, somatorioTempoResposta = 0, tempoResposta;
   float ta, te, tr;
   bool aux = true;
   int f=0;
   numProcessosNrodados = numeroProcessos;
   while((aux == true)&&f<200)
   {
       f++;
       auxiliarTraco = 0;
       melhorPrioridade = 1000;
       auxiliarContagem = 0;
       for(int i = 0; i < numeroProcessos; i++)
       {
           if(tC < p[i].getTempoChegada()) /// se não há processos, imprime "-"
           {
               auxiliarTraco++;
               if(auxiliarTraco == numProcessosNrodados)
               {
                   cout<<"-";
                   tC++;
               }
           }

           if(tC >= p[i].getTempoChegada() && p[i].getBool() != false) /// verifica para cada tC qual processo tem melhor prioridade
           {
               if(melhorPrioridade > p[i].getPrioridade())

               {
                   melhorPrioridade = p[i].getPrioridade();
                   exec = i; /// exec é o programa que será executado na "rodada"
               }

               if(melhorPrioridade == p[i].getPrioridade())
               {
                   if(estavaExec == exec)
                    {
                        melhorPrioridade = p[i].getPrioridade(); /// se programa ja estava executando
                        exec = i;
                    }                     /// e aparece outro com mesma prioridade: permitir que proximo rode.
               }
           }
       }

       if(exec != -1) /// se há o programa a ser executado
       {
                    if((estavaExec != exec)&&(estavaExec != -1))
                    {
                        tC = tC + 2;
                        cout<<"TC";
                    }

                   for(int j = 0; j < fatiaTempo; j++)
                   {
                       if(p[exec].getTempoExecucao() > 0)
                       {
                           if(p[exec].getPrimeiraVez() == 1)
                                {/// se é a primeira vez que se executa então diminui-se tempo de contagem atual com tempo de chegada do processo
                                    p[exec].setPrimeiraVez(0);
                                    tempoResposta = tC - p[exec].getTempoChegada();
                                    somatorioTempoResposta = somatorioTempoResposta + tempoResposta;
                                    //cout << "\ntC: "<<tC<<"\ttempoChegada: "<< p[exec].getTempoChegada()<<"\n";
                                }
                           estavaExec = exec ; /// esta variavel ajudara no caso de ter processos de mesma prioridade, e deverão alterar entre si
                           cout << exec + 1;
                           p[exec].setTempoExecucao(p[exec].getTempoExecucao() - 1);
                           tC++;
                       }

                       if(p[exec].getTempoExecucao() == 0 && p[exec].getBool() == true)
                       {
                           p[exec].setBool(false);
                            numProcessosNrodados = numProcessosNrodados - 1;
                           //cout << "\ntC: " << tC <<"\ttempoChegada: "<<p[exec].getTempoChegada()<<endl;
                           turnaround = tC - p[exec].getTempoChegada() - 1;
                           tempoEspera = turnaround - p[exec].getTempoExecucaoInicio();
                           somatorioTurnaround = somatorioTurnaround + turnaround;
                           somatorioTempoEspera = somatorioTempoEspera + tempoEspera;
                       }

                       VerificaPreempcao = Preempcao(arquivo, numeroProcessos, p, tC, exec);
                       if(VerificaPreempcao == 1)
                       {
                           j = fatiaTempo; /// se há preempção, sai do for que executa, e vai volta para o for que verifica processos
                                           /// o que chegou ganhara a vez
                       }
                   }

                //cout <<"TC";
               // tC = tC + 2;
       }

       for(int fe = 0; fe < numeroProcessos; fe++)
        {
            if(p[fe].getBool() == false)
            {
                auxiliarContagem = auxiliarContagem + 1;
            }

            if(auxiliarContagem == numeroProcessos)
                aux = false;
        }
   }

      tr = somatorioTempoResposta/numeroProcessos;
      ta = somatorioTurnaround/numeroProcessos;
      te = somatorioTempoEspera/numeroProcessos;
      cout << "\nTURNAROUND: " << ta;
      cout << "\nTEMPO ESPERA: " << te;
      cout << "\nTEMPO RESPOSTA: " << tr;
}

int main() /// NO PRINCIPAL LE-SE O ARQUIVO, E CHAMA-SE AS FUNCOES REQUERIDAS
{
    ifstream arquivo;
    int escalonamento;
    int numeroProcessos, fatiaTempo;
    int tempoChegada, tempoExecucao, prioridade;

    arquivo.open("arquivo.txt", ios::in);

    if(arquivo.is_open())
    {
        arquivo >> numeroProcessos;
        arquivo >> fatiaTempo;
        Processo process;
        Processo p[numeroProcessos];
        for(int i=0; i<numeroProcessos; i++)
         {
           // cout <<"Processo"<<i+1<<endl;
            arquivo >> tempoChegada;
            process.setTempoChegada(tempoChegada);
            arquivo >> tempoExecucao;
            process.setTempoExecucao(tempoExecucao);
            process.setTempoExecucaoInicio(tempoExecucao);
            arquivo >> prioridade;
            process.setPrioridade(prioridade);
            process.setBool(true);
            process.setPrimeiraVez(1);
            p[i] = process;
         }

        cout<<"Qual escalonamento usar ? \n 1: SJF; \n 2: Round Robin; \n 3: Round Robin Preemptivo; \n";
        cin >> escalonamento;
        if(escalonamento == 1) SJF(arquivo, numeroProcessos, p);
        if (escalonamento == 2) RoundRobin(arquivo, numeroProcessos, p, fatiaTempo);
        if (escalonamento == 3) RoundRobinPreemptivo(arquivo, numeroProcessos, p, fatiaTempo);
        arquivo.close();
    }
    else cout << "Não foi possível abrir o arquivo.";

}
