#ifndef PROCESSO_H
#define PROCESSO_H


class Processo
{
    int tempoChegada, tempoExecucao, prioridade, primeiraVez, tempoExecucaoInicio;
    bool booleano;
    public:
        Processo();
        virtual ~Processo();
        void setPrioridade(int p);
        void setTempoChegada(int tC);
        void setTempoExecucao(int tE);
        void setBool(bool b);
        void setPrimeiraVez(int pv);
        void setTempoExecucaoInicio(int tE);
        bool getBool();
        int getPrioridade();
        int getTempoChegada();
        int getTempoExecucao();
        int getTempoExecucaoInicio();
        int getPrimeiraVez();

};

#endif // PROCESSO_H
