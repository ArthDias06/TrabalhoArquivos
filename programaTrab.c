#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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



bool createTable(char* csv, char* bin){
    FILE *fbin;
    if (bin == NULL || !(fbin = fopen(bin, "wb"))) {
        printf("Erro na abertura do arquivo binário!");
        return false;
    }
    FILE *fcsv;
    if (csv == NULL || !(fcsv = fopen(csv, "r"))) {
        printf("Erro na abertura do arquivo csv!");
        fclose(fbin);
        return false;
    }
    CABECALHO cabecalho = {'0', -1, 0, 0, 0};
    fseek(fcsv, 100, SEEK_SET); //Ignora a primeira linha do CSV.
    char codLin[11], nomeLin[44], distanciaProx[11], codLinInteg[11], codEstInteg[11];
    char **nomeEst = malloc(sizeof(char*) * 201);
    char **codEst = malloc(sizeof(char*) * 201);
    char **codProx = malloc(sizeof(char*) * 201);
    for(int i = 0; i < 201; i++){
        nomeEst[i] = malloc(sizeof(char) * 44);
        codEst[i] = malloc(sizeof(char) * 11);
        codProx[i] = malloc(sizeof(char) * 11);
    }
    int i = 0;
    int contVariavel = 0;
    REGISTRO registro;
    //Colocando valores iniciais para ir para a inserção dos inserts
    fwrite("", sizeof(char), 1, fbin);
    int t[4] = {0};
    fwrite(t, sizeof(int), 4, fbin);
    //Lê cada letra do documento
    int ch = 0;
    int cont = 0;
    int contByte = 0; //Conta quantos bytes foram passados para completar com lixo depois


    while(ch != EOF){
        ch = fgetc(fcsv); //Colocado aqui para poder ler até o final
        switch(contVariavel){
            case 0:
                codEst[i][cont] = ch!=',' ? ch : '\0';
                break;
            case 1:
                nomeEst[i][cont] = ch!=',' ? ch : '\0';
                contByte++;
                break;
            case 2:
                codLin[cont] = ch!=',' ? ch : '\0';
                break;
            case 3:
                nomeLin[cont] = ch!=',' ? ch : '\0';
                contByte++;
                break;
            case 4:
                codProx[i][cont] = ch!=',' ? ch : '\0';
                break;
            case 5:
                distanciaProx[cont] = ch!=',' ? ch : '\0';
                break;
            case 6:
                codLinInteg[cont] = ch!=','? ch : '\0';
                break;
            case 7:
                codEstInteg[cont] = (ch!='\n' && ch!=EOF) ? ch : '\0';
                break;
        }
        cont++;
        if(ch != '\n' && ch!=EOF){
            if(ch == ','){
                contVariavel++;
                cont = 0;
            }
            continue;
        }
        contByte -= 2;//Retira os \0 das duas strings
        printf("%s %s %s %s %s %s %s %s\n", codEst[i], nomeEst[i], codLin, nomeLin, codProx[i], distanciaProx, codLinInteg, codEstInteg);
        cont = 0;
        contVariavel = 0;
        
        contByte+=37;
        contByte = 80 - contByte;
        //atoi de string vazia retorna 0
        registro.removido = '0';
        registro.proximo = -1;
        registro.codEstacao = atoi(codEst[i]);
        registro.codLinha = atoi(codLin);
        registro.codProxEstacao = atoi(codProx[i]);
        registro.distProxEstacao = atoi(distanciaProx);
        registro.codLinhaIntegra = atoi(codLinInteg);
        registro.codEstIntegra = atoi(codEstInteg);
        registro.tamNomeEstacao = strlen(nomeEst[i]);
        registro.tamNomeLinha = strlen(nomeLin);
        strcpy(registro.nomeEstacao, nomeEst[i]);
        strcpy(registro.nomeLinha, nomeLin);

        fwrite(&registro.removido, sizeof(char), 1, fbin);
        fwrite(&registro.proximo, sizeof(int), 1, fbin);
        fwrite(&registro.codEstacao, sizeof(int), 1, fbin);
        fwrite(&registro.codLinha, sizeof(int), 1, fbin);
        fwrite(&registro.codProxEstacao, sizeof(int), 1, fbin);
        fwrite(&registro.distProxEstacao, sizeof(int), 1, fbin);
        fwrite(&registro.codLinhaIntegra, sizeof(int), 1, fbin);
        fwrite(&registro.codEstIntegra, sizeof(int), 1, fbin);
        fwrite(&registro.tamNomeEstacao, sizeof(int), 1, fbin);
        fwrite(registro.nomeEstacao, sizeof(char), strlen(registro.nomeEstacao), fbin);
        fwrite(&registro.tamNomeLinha, sizeof(int), 1, fbin);
        fwrite(registro.nomeLinha, sizeof(char), strlen(registro.nomeLinha), fbin);
        cabecalho.proxRRN++;



        cabecalho.nroEstacoes++;cabecalho.nroParesEstacao++;
        for(int j = 0; j < i; j++){
            if(!strcmp(nomeEst[j], nomeEst[i])){
                cabecalho.nroEstacoes--;
                break;
            }
        }
        for(int j = 0; j < i; j++){
            if(!strcmp(codEst[j], codEst[i]) && !strcmp(codProx[j], codProx[i])){
                cabecalho.nroParesEstacao--;
                break;
            }
        }
        //Vê se o i está na última linha da matriz da matriz, se estiver, aloca mais memória
        if((i+1)% 201 == 0){
            nomeEst = realloc(nomeEst, sizeof(char*) * (i+202));
            codEst = realloc(codEst, sizeof(char*) * (i+202));
            codProx = realloc(codProx, sizeof(char*) * (i+202));
            for(int j = i+1; j < i+202; j++){
                nomeEst[j] = malloc(sizeof(char) * 44);
                codEst[j] = malloc(sizeof(char) * 11);
                codProx[j] = malloc(sizeof(char) * 11);
            }
        }
        i++;
        if(contByte > 0){
            char lixo[contByte];
            for(int j = 0; j < contByte; j++){
                lixo[j] = '$';
            }
            fwrite(lixo, sizeof(char), contByte, fbin);
        }
        contByte = 0;
    }
    for(int j = 0; j < i; j++){
        free(nomeEst[j]);
        free(codEst[j]);
        free(codProx[j]);
    }
    free(nomeEst);
    free(codEst);
    free(codProx);

    cabecalho.status = '1';
    fseek(fbin, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, fbin);
    fwrite(&cabecalho.proxRRN, sizeof(int), 1, fbin);
    fwrite(&cabecalho.nroEstacoes, sizeof(int), 1, fbin);
    fwrite(&cabecalho.nroParesEstacao, sizeof(int), 1, fbin);

    fclose (fcsv);
    fclose(fbin);
    return true;
}






void BinarioNaTela(char *arquivo) {
    FILE *fs;
    if (arquivo == NULL || !(fs = fopen(arquivo, "rb"))) {
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura. "
                "Ele existe e você tá passando o nome certo? Você lembrou de "
                "fechar ele com fclose depois de usar?\n");
        return;
    }

    fseek(fs, 0, SEEK_END);
    size_t fl = ftell(fs);

    fseek(fs, 0, SEEK_SET);
    unsigned char *mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    unsigned long cs = 0;
    for (unsigned long i = 0; i < fl; i++) {
        cs += (unsigned long)mb[i];
    }

    printf("%lf\n", (cs / (double)100));

    free(mb);
    fclose(fs);
}









int main(){
    int operacao;
    scanf("%d", &operacao);
    char arquivoBin[101];
    char arquivoCSV[101];
    switch(operacao){
        case 1:
            scanf("%100s %100s", arquivoCSV, arquivoBin);
            if(createTable(arquivoCSV, arquivoBin));
                BinarioNaTela(arquivoBin);
            break;
    }
}