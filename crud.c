#include "crud.h"

// Esse arquivo foi feito mais para servir para funções específicas do CRUD que são utilizadas por mais de uma estrutura
// Na implementação atual, apenas duas função se encaixam, mas a utilidade desse .c pode aumentar conforme o projeto escala

// Percorremos o arquivo de dados sequencialmente e removemos todos os registros de acordo com o WHERE
void removerSequencial(FILE *fbin, FILE *fArvore, CABECALHO *cabDados, ARVOREB_CABECALHO *cabArvore, int quantAnds, char *condicoes[][2], char ***matrizes, int *nroLinhas) {
    int pos = 0;
    char removido;
    fseek(fbin, tamHeader, SEEK_SET);
    while(fread(&removido, sizeof(char), 1, fbin) == 1) {
        if(removido != '1') {
            REGISTRO registro;
            registro.removido = '0';
            if(lerRegistroVerifica(fbin, &registro, quantAnds, condicoes)) {
                char marca = '1';
                // Marcamos o registro como removido e empilhamos o RRN no cabeçalho de acordo com o especificado
                fseek(fbin, tamHeader + pos*tamRegistro, SEEK_SET);
                fwrite(&marca, sizeof(char), 1, fbin);
                fwrite(&cabDados->topo, sizeof(int), 1, fbin);
                cabDados->topo = pos;
                atualizarRemocaoMatriz(cabDados, matrizes, nroLinhas, registro);
                // Se tiver fArvore, também removemos a chave correspondente da árvore-B
                if(fArvore != NULL) {
                    removerChaveArvore(fArvore, cabArvore, registro.codEstacao);
                }
            }
        }
        ++pos;
        fseek(fbin, tamHeader + pos*tamRegistro, SEEK_SET);
    }
}

// Percorremos o arquivo de dados sequencialmente e imprimimos os registros de acordo com o WHERE
bool buscaSequencial(FILE *fbin, char *condicoes[][2], int quantAnds, bool pararNoPrimeiro) {
    bool encontrou = false;
    int cont = 0;
    fseek(fbin, tamHeader, SEEK_SET);
    char removido;
    while(fread(&removido, 1, 1, fbin) == 1) {
        // Pulamos registros logicamente removidos
        if(removido == '1') {
            ++cont;
            fseek(fbin, tamRegistro - 1, SEEK_CUR);
            continue;
        }
        REGISTRO registro;
        registro.removido = '0';
        bool ok = lerRegistroVerifica(fbin, &registro, quantAnds, condicoes);
        if(ok) {
            encontrou = true;
            imprimeRegistro(registro);
            if(pararNoPrimeiro) {
                break;
            }
        }
        finalizarBusca(fbin, &cont);
    }
    return encontrou;
}