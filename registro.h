#ifndef REGISTRO_H
#define tamRegistro 80
#define tamHeader 17
#define REGISTRO_H
    
    // Definição das estruturas no binário que são utilizadas no processamento dos dados (cabeçalho e registro)
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

    void atualizarCabecalho(CABECALHO, FILE*);

    bool lerCabecalho(FILE*, CABECALHO*);

    void escreverRegistro(REGISTRO, FILE*);

    void lerCondicoesBusca(int, char *[][2]);

    // Função para ser chamada ao fim de todas as buscas de um comando para dar free na memória de condicoes
    void liberarCondicoes(int, char *[][2]);

    // Função para ser chamada ao fim de cada busca (cada OR do SELECT) para aumentar o contador e mover o cursor
    void finalizarBusca(FILE *, int *);

    // Função que lê o registro e retorna um booleano que responde à pergunta "o registro atende aos requisitos do WHERE?"
    bool lerRegistroVerifica(FILE *, REGISTRO *, int, char*[][2]);

    int converterStringParaInt(char *);

    void imprimeRegistro(REGISTRO registro);
    bool buscaSequencial(FILE *fbin, char *condicoes[][2], int quantAnds, bool pararNoPrimeiro);

    void lerRegistro(FILE* fbin, REGISTRO* registro);

#endif