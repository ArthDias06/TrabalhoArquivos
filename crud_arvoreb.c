#include "crud.h"
// Esse arquivo contém as funções de operações do CRUD aplicadas na árvore B

bool createIndex(char* arq, char* arvore_arq){
    FILE *fbin;
    if (arq == NULL || !(fbin = fopen(arq, "rb"))) {
        printf("Falha no processamento do arquivo.\n");
        return true;
    }
    FILE *fbin_arvore;
    //É aberto como write+ por conta de que o ponteiro do arquivo pode ser usado para leitura no decorrer do insert
    if (arvore_arq == NULL || !(fbin_arvore = fopen(arvore_arq, "w+b"))) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin);
        return true;
    }
    //Inicializa o cabeçalho com seus valores-base
    ARVOREB_CABECALHO cabecalho = {'0', -1, -1, 0, 0};
    //Escreve o cabeçalho no arquivo
    escreverCabecalhoArvore(fbin_arvore, cabecalho);
    fseek(fbin, 0, SEEK_SET);
    char status, removido;
    //Verifica a consistência do arquivo de dados
    fread(&status, sizeof(char), 1, fbin);
    //Se for inconsistente, retorna erro
    if(status == '0'){
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin);
        fclose(fbin_arvore);
        return true;
    }
    int rrn=-1;
    //Ignora o cabeçalho
    fseek(fbin, 16, SEEK_CUR);
    while(fread(&removido, sizeof(char), 1, fbin) == 1){
        //Armazena a posição do registro
        rrn++;
        //Se for removido, ignora o registro
        if(removido == '1') {
            fseek(fbin,79,SEEK_CUR);
            continue;
        }
        //Pula o campo próximo
        fseek(fbin, 4, SEEK_CUR);
        int chave, promover, promoverChave, promoverRegistro;
        bool flag = false;
        //Chave tem o valor de codEstacao do registro
        fread(&chave, sizeof(int), 1, fbin);
        //Insere na árvore B
        if(insert(fbin_arvore, 17 + rrn*80, cabecalho.noRaiz, chave, &promover, &promoverChave, &promoverRegistro, &cabecalho, &flag)){
            //Se insert retorna verdadeiro, precisa criar uma nova raiz
            criaRaiz(fbin_arvore, promover, promoverChave, promoverRegistro, &cabecalho);
        }
        //Passa para o próximo registro
        fseek(fbin, 71, SEEK_CUR);
    }
    //atualiza o valor do cabeçalho
    cabecalho.status = '1';
    escreverCabecalhoArvore(fbin_arvore, cabecalho);
    fclose(fbin_arvore);
    fclose(fbin);
    return false;
}


bool deleteFromWhereArvore(char *arquivoBin, char *arquivoArvore, int n, char ***matrizes, int *nroLinhas) {
    // Processamento padrão de arquivos
    FILE *fbin;
    if(arquivoBin == NULL || !(fbin = fopen(arquivoBin, "r+b"))) {
        printf("Falha no processamento do arquivo.\n");
        return 1;
    }
    FILE *fArvore;
    if(arquivoArvore == NULL || !(fArvore = fopen(arquivoArvore, "r+b"))) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin);
        return 1;
    }
    CABECALHO cabDados;
    ARVOREB_CABECALHO cabArvore;
    if(!lerCabecalho(fbin, &cabDados) || !lerCabecalhoArvore(fArvore, &cabArvore)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin);
        fclose(fArvore);
        return 1;
    }

    // Garantimos que a matriz esteja atualizada de acordo
    if(*nroLinhas <= 0) {
        populaMatriz(matrizes, nroLinhas, fbin, cabDados.proxRRN);
    }

    // Fazemos os OR's do DELETE
    for(int i = 0; i<n; ++i) {
        int quantAnds;
        scanf("%d", &quantAnds);
        char *condicoes[quantAnds][2];
        lerCondicoesBusca(quantAnds, condicoes);

        // Verificamos se o codEstacao está entre as condições de busca
        bool temChave = false;
        int chaveBusca = -1;
        for(int j = 0; j<quantAnds; ++j) {
            if((int)(intptr_t)condicoes[j][0] == 2) {
                temChave = true;
                chaveBusca = atoi(condicoes[j][1]);
            }
        }

        // Se a busca tiver o codEstacao, recuperamos com o índice árvore-B
        // Se for busca sem o campo chave, apenas percorremos o arquivo de dados de maneira sequencial
        if(temChave) {
            int offset = buscarChave(fArvore, chaveBusca, cabArvore.noRaiz);
            if(offset != -1) {
                int rrn = (offset - tamHeader) / tamRegistro;
                fseek(fbin, offset, SEEK_SET);
                char removido;
                fread(&removido, sizeof(char), 1, fbin);
                if(removido != '1') {
                    REGISTRO registro;
                    registro.removido = '0';
                    bool ok = lerRegistroVerifica(fbin, &registro, quantAnds, condicoes);
                    if(ok) {
                        char marca = '1';
                        fseek(fbin, offset, SEEK_SET);
                        fwrite(&marca, sizeof(char), 1, fbin);
                        fwrite(&cabDados.topo, sizeof(int), 1, fbin);
                        cabDados.topo = rrn;
                        atualizarRemocaoMatriz(&cabDados, matrizes, nroLinhas, registro);
                        removerChaveArvore(fArvore, &cabArvore, registro.codEstacao);
                    }
                }
            }
        }
        else {
            removerSequencial(fbin, fArvore, &cabDados, &cabArvore, quantAnds, condicoes, matrizes, nroLinhas);
        }
        liberarCondicoes(quantAnds, condicoes);
    }

    cabDados.status = '1';
    atualizarCabecalho(cabDados, fbin);
    cabArvore.status = '1';
    escreverCabecalhoArvore(fArvore, cabArvore);
    fclose(fbin);
    fclose(fArvore);
    return 0;
}

void selectFromWhereArvore(char *arquivoBin, char *arquivoArvore, int quantBuscas) {
    // Processamento padrão dos arquivos
    FILE *fbin;
    if(arquivoBin == NULL || !(fbin = fopen(arquivoBin, "rb"))){
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    FILE *fArvore;
    if(arquivoArvore == NULL || !(fArvore = fopen(arquivoArvore, "rb"))){
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin);
        return;
    }

    // Conferimos a consistência do arquivo
    char status;
    fseek(fbin, 0, SEEK_SET);
    fread(&status, sizeof(char), 1, fbin);
    if(status == '0'){
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin);
        fclose(fArvore);
        return;
    }
    // Conferimos a consistência do índice e lemos o RRN do raiz
    int noRaiz;
    fseek(fArvore, 0, SEEK_SET);
    fread(&status, sizeof(char), 1, fArvore);
    if(status == '0'){
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin);
        fclose(fArvore);
        return;
    }
    fread(&noRaiz, sizeof(int), 1, fArvore);

    // Fazemos os "OR's" do SELECT (isso é, cada uma das buscas)
    for(int i = 0; i<quantBuscas; ++i) {
        int quantAnds = 0;
        scanf("%d", &quantAnds);
        char *condicoes[quantAnds][2];
        lerCondicoesBusca(quantAnds, condicoes);

        // Verificamos se o codEstacao está entre as condições de busca
        bool temChave = false;
        int chaveBusca = -1;
        for(int j = 0; j<quantAnds; ++j) {
            if((int)(intptr_t)condicoes[j][0] == 2) {
                temChave = true;
                chaveBusca = strcmp(condicoes[j][1], "NULO")==0 ? -1 : atoi(condicoes[j][1]);
            }
        }

        bool encontrou = 0;
        // Se a busca tiver o codEstacao, recuperamos com o índice árvore-B
        // Se for busca sem o campo chave, apenas percorremos o arquivo de dados de maneira sequencial, como no selectFromWhere
        if(temChave) {
            int offset = buscarChave(fArvore, chaveBusca, noRaiz);
            if(offset != -1) {
                fseek(fbin, offset, SEEK_SET);
                char removido;
                fread(&removido, sizeof(char), 1, fbin);
                // Pulamos registros logicamente removidos
                if(removido != '1') {
                    REGISTRO registro;
                    registro.removido = '0';
                    bool ok = lerRegistroVerifica(fbin, &registro, quantAnds, condicoes);
                    if(ok) {
                        encontrou = 1;
                        imprimeRegistro(registro);
                    }
                }
            }
        }
        else {
            encontrou = buscaSequencial(fbin, condicoes, quantAnds, false);
        }

        if(!encontrou) {
            printf("Registro inexistente.\n");
        }
        liberarCondicoes(quantAnds, condicoes);
        printf("\n");
    }
    fclose(fbin);
    fclose(fArvore);
}
