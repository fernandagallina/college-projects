#include "Processo.h"

Processo::Processo()
{
    //ctor
}

Processo::~Processo()
{
    //dtor
}

    int Processo::getPrimeiraVez()
    {
        return primeiraVez;
    }

    void Processo::setPrimeiraVez(int pv)
    {
        primeiraVez = pv;
    }

    void Processo::setBool(bool b)
    {
        booleano = b;
    }

    void Processo::setTempoChegada(int tC)
    {
        tempoChegada = tC;
    }

    void Processo::setTempoExecucao(int tE)
    {
        tempoExecucao = tE;
    }

     void Processo::setTempoExecucaoInicio(int tE)
    {
        tempoExecucaoInicio = tE;
    }

    void Processo::setPrioridade(int p)
    {
        prioridade = p;
    }

    bool Processo::getBool()
    {
        return booleano;
    }

    int Processo::getTempoChegada()
    {
        return tempoChegada;
    }

    int Processo::getTempoExecucao()
    {
        return tempoExecucao;
    }

    int Processo::getTempoExecucaoInicio()
    {
        return tempoExecucaoInicio;
    }

    int Processo::getPrioridade()
    {
        return prioridade;
    }
