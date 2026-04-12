#ifndef REGISTRO_H
#define REGISTRO_H
    
    //Criação das structs de registro de cabecalho
    typedef struct cabecalho{
        char status;
        int topo;
        int proxRRN;
        int nroEstacoes;
        int nroParesEstacao;
    }CABECALHO;

    typedef struct registro{
        char removido;
        int proximo;
        int codEstacao;
        int codLinha;
        int codProxEstacao;
        int distProxEstacao;
        int codLinhaIntegra;
        int codEstIntegra;
        int tamNomeEstacao;
        char nomeEstacao[44];
        int tamNomeLinha;
        char nomeLinha[44];
    }REGISTRO;

    void atualizaCabecalho(CABECALHO, FILE*);
    CABECALHO leituraCabecalho(FILE*);
    void escritaRegistro(REGISTRO, int, FILE*);

#endif