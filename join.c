#include "join.h"

void joinNestedLoop(char* arqEntrada1, char*arqEntrada2){
    FILE *fbin1;
    if (arqEntrada1 == NULL || !(fbin1 = fopen(arqEntrada1, "rb"))) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    char status;
    fread(&status, sizeof(char), 1, fbin1);
    if(status == '0'){
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin1);
        return;
    }
    FILE *fbin2;
    if (arqEntrada2 == NULL || !(fbin2 = fopen(arqEntrada2, "rb"))) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin1);
        return;
    }
    fread(&status, sizeof(char), 1, fbin2);
    if(status == '0'){
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin2);
        fclose(fbin1);
        return;
    }

    char removido;
    int cont=1;
    bool flag = false;
    REGISTRO registro1, registro2;
    fseek(fbin1, tamHeader-1, SEEK_CUR);
    fseek(fbin2, tamHeader-1, SEEK_CUR);
    while(fread(&removido, 1, 1, fbin1) == 1) {
        // Pulamos registros logicamente removidos
        if(removido == '1') {
            fseek(fbin1, tamRegistro - 1, SEEK_CUR);
            continue;
        }
        lerRegistro(fbin1, &registro1);
        fseek(fbin1, tamHeader+cont*tamRegistro, SEEK_SET);
        cont++;
        fseek(fbin2, tamHeader, SEEK_SET);
        int cont2=1;
        while(fread(&removido, 1, 1, fbin2) == 1){
            if(removido == '1') {
                fseek(fbin2, tamRegistro - 1, SEEK_CUR);
                continue;
            }
            lerRegistro(fbin2, &registro2);
            fseek(fbin2, tamHeader+cont2*tamRegistro, SEEK_SET);
            cont2++;
            if(registro2.codEstacao == registro1.codProxEstacao){
                flag = true;
                printf("%d %s %s %d %s\n", registro1.codEstacao, registro1.nomeEstacao, registro1.nomeLinha, registro1.codProxEstacao, registro2.nomeEstacao);
            }
        }
    }
    if(!flag){
        printf("Registro inexistente.\n");
    }
    fclose(fbin1);
    fclose(fbin2);
}

void joinUsandoIndice(char *arqEntrada1, char *arqEntrada2, char *arqArvore){
    FILE *fbin1;
    if (arqEntrada1 == NULL || !(fbin1 = fopen(arqEntrada1, "rb"))) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    char status;
    fread(&status, sizeof(char), 1, fbin1);
    if(status == '0'){
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin1);
        return;
    }
    FILE *fbin2;
    if (arqEntrada2 == NULL || !(fbin2 = fopen(arqEntrada2, "rb"))) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    fread(&status, sizeof(char), 1, fbin2);
    if(status == '0'){
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin2);
        fclose(fbin1);
        return;
    }
    FILE *farvore;
    if (arqArvore == NULL || !(farvore = fopen(arqArvore, "rb"))) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    fread(&status, sizeof(char), 1, farvore);
    if(status == '0'){
        printf("Falha no processamento do arquivo.\n");
        fclose(farvore);
        fclose(fbin1);
        fclose(fbin2);
        return;
    }

    char removido;
    int noRaiz, cont=1;;
    bool flag = false;
    REGISTRO registro1, registro2;
    fseek(fbin1, tamHeader-1, SEEK_CUR);
    fseek(fbin2, tamHeader-1, SEEK_CUR);
    fread(&noRaiz, sizeof(int), 1, farvore);
    while(fread(&removido, 1, 1, fbin1) == 1) {
        if(removido == '1'){
            fseek(fbin1, tamRegistro-1, SEEK_CUR);
            continue;
        }
        lerRegistro(fbin1, &registro1);
        fseek(fbin1, tamHeader+cont*tamRegistro, SEEK_SET);
        cont++;
        fseek(farvore, 17, SEEK_SET);
        int offset = buscarChave(farvore, registro1.codProxEstacao, noRaiz);
        if(offset == -1){
            continue;
        }
        fseek(fbin2, offset, SEEK_SET);
        fread(&removido, sizeof(char), 1, fbin2);
        //Verificação a mais, caso registros sejam retirados após a criação do índice, pode estar desatualizado
        if(removido == '1'){
            continue;
        }
        lerRegistro(fbin2, &registro2);
        if(registro1.codProxEstacao == registro2.codEstacao){
            flag = true;
            printf("%d %s %s %d %s\n", registro1.codEstacao, registro1.nomeEstacao, registro1.nomeLinha, registro1.codProxEstacao, registro2.nomeEstacao);
        }
    }
    if(!flag){
        printf("Registro inexistente.\n");
    }
    fclose(fbin1);
    fclose(fbin2);
    fclose(farvore);
}

/*bool orderBy(char* arquivoBin, char* campo, char* arquivoOrd){
    FILE *fbin1;
    if (arquivoBin == NULL || !(fbin1 = fopen(arquivoBin, "rb"))) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    char status;
    fread(&status, sizeof(int), 1, fbin1);
    if(status == '0'){
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin1);
        return;
    }
    FILE *fbin2;
    if (arquivoOrd == NULL || !(fbin2 = fopen(arquivoOrd, "wb"))) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin1);
        return;
    }


}*/