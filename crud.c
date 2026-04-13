#include "crud.h"

bool createTable(char* csv, char* bin, char*** matrizes, int* nroLinhas){
    FILE *fbin;
    if (bin == NULL || !(fbin = fopen(bin, "wb"))) {
        printf("Falha no processamento do arquivo.\n");
        return false;
    }
    FILE *fcsv;
    if (csv == NULL || !(fcsv = fopen(csv, "r"))) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin);
        return false;
    }
    //Valores iniciais do cabecalho
    CABECALHO cabecalho = {'0', -1, 0, 0, 0};
    fseek(fcsv, 101, SEEK_SET); //Ignora a primeira linha do CSV.
    //44 é a maior string que pode ser inserida de acordo com a especificação do projeto+\0
    //11 é a representação em string do maior valor de int + \0
    char codLin[11], nomeLin[44], distanciaProx[11], codLinInteg[11], codEstInteg[11];
    

    *nroLinhas = 0;
    int contVariavel = 0;
    REGISTRO registro;
    //Colocando valores iniciais do cabecalho
    atualizarCabecalho(cabecalho, fbin);
    int ch = 0;
    int cont = 0;


    while(ch != EOF){
        //Lê cada letra do documento
        ch = fgetc(fcsv);
        //Se for \r ignora o byte
        if(ch == '\r'){
            continue;
        }

        //contaVariavel norteia qual campo está sendo lido
        switch(contVariavel){
            //Os 7 primeiros campos são terminados por vígula
            //O último pode ser terminado por \n ou EOF
            //Caso o final do campo seja tingido é colocado \0 no fim da strin lida
            case 0:
                matrizes[1][*nroLinhas][cont] = ch!=',' ? ch : '\0';
                break;
            case 1:
                matrizes[0][*nroLinhas][cont] = ch!=',' ? ch : '\0';
                break;
            case 2:
                codLin[cont] = ch!=',' ? ch : '\0';
                break;
            case 3:
                nomeLin[cont] = ch!=',' ? ch : '\0';
                break;
            case 4:
                matrizes[2][*nroLinhas][cont] = ch!=',' ? ch : '\0';
                break;
            case 5:
                distanciaProx[cont] = ch!=',' ? ch : '\0';
                break;
            case 6:
                codLinInteg[cont] = ch!=','? ch : '\0';
                break;
            case 7:
                codEstInteg[cont] = (ch!='\n' && ch != EOF) ? ch : '\0';
                break;
        }
        //Conatdor para passar para próximo byte da string
        cont++;
        if(ch != '\n' && ch != EOF){
            if(ch == ','){
                //Se chegou ao final de um campo que não é o final
                //De uma linha, contVariavel aumenta em 1, mostrando que deveria ler o próximo campo
                contVariavel++;
                cont = 0;
            }
            continue;
        }
        //Se chegar no final da linha o registro é escrito no arquivo
        cont = 0;
        contVariavel = 0;
        //atoi de string vazia retorna 0
        //O registro acabou de ser criado então seu campo removido e proximo são por padrão 0 e -1
        registro.removido = '0';
        registro.proximo = -1;
        /*Em cada campo é feita a verificação se o valor inserido é diferente de nulo
        Com esceção dos campos codEstacao e nomeEstacao que não podem ser nulos.
        Caso algum dos campos seja nulo, seu valor passa a ser -1*/
        registro.codEstacao = converterStringParaInt(matrizes[1][*nroLinhas]);
        registro.codLinha = converterStringParaInt(codLin);
        registro.codProxEstacao = converterStringParaInt(matrizes[2][*nroLinhas]);
        registro.distProxEstacao = converterStringParaInt(distanciaProx);
        registro.codLinhaIntegra = converterStringParaInt(codLinInteg);
        registro.codEstIntegra = converterStringParaInt(codEstInteg);

        registro.tamNomeEstacao = strlen(matrizes[0][*nroLinhas]);
        registro.tamNomeLinha = strlen(nomeLin);
        strcpy(registro.nomeEstacao, matrizes[0][*nroLinhas]);
        strcpy(registro.nomeLinha, nomeLin);

        //Chamad da função para escrita no registro
        escreverRegistro(registro, cabecalho.proxRRN*tamRegistro+17, fbin);
        //Aumenta o número do próximo RRN
        cabecalho.proxRRN++;

        //Caso o nome da estação já exista, o nroEstacoes não sofre alteração
        cabecalho.nroEstacoes++;cabecalho.nroParesEstacao++;
        if(duplicidadeEstacoes(matrizes, nroLinhas, matrizes[0][*nroLinhas])){
            cabecalho.nroEstacoes--;
        }
        //O mesmo para caso o par já exista
        if(duplicidadeParesEstacao(matrizes, nroLinhas, matrizes[2][*nroLinhas], matrizes[1][*nroLinhas])){
            cabecalho.nroParesEstacao--;
        }
        //Vê se o i está na última linha da matriz da matriz, se estiver, aloca mais memória
        if((*nroLinhas)% 200 == 0 && (*nroLinhas) > 0){
            realocacao(&matrizes, nroLinhas);
        }
        //Aumenta o número de linhas usadas pela matriz
        (*nroLinhas)++;
        
    }
    //Atualiza o valor do cabecalho
    cabecalho.status = '1';
    atualizarCabecalho(cabecalho, fbin);
    //Fecha as streams
    fclose (fcsv);
    fclose(fbin);
    return true;
}


void insertInto(char* arquivoBin, int nroInsert, char*** matrizes, int* nroLinhas){
    FILE *fbin;
    if(arquivoBin == NULL || !(fbin = fopen(arquivoBin, "r+b"))){
        printf("Erro no Insert!");
        return;
    }
    //Strings dos campos usados, os nomeEstacoa e nomeLinha tem 2 byes a mais por conat das aspas do input
    char codEstacao[11], nomeEstacao[46], codLinha[11], nomeLinha[46], codProxEstacao[11], distProxEstacao[11], codLinhaIntegra[11], codEstIntegra[11];
    REGISTRO registro;
    //Leitura do cabecalho do arquivo
    CABECALHO cabecalho = lerCabecalho(fbin);
    int proxInsercao;

    //Caso a matriz não tenha sido ainda preenchida com os valores do arquivo
    if(*nroLinhas <= 0){
        populaMatriz(matrizes, nroLinhas, fbin, cabecalho.proxRRN);
    }
    //O loop ocorre de acordo com quantas inserções o usuário pretende fazer
    for(int i = 0; i < nroInsert; i++){
        //O proxInsercao marca onde será inserido o próximo registro
        //podendo ser no final do arquivo ou seguir a pilha de remoções
        //Caso haja registros deletados.
        proxInsercao = cabecalho.topo != -1 ? cabecalho.topo : cabecalho.proxRRN;
        ScanQuoteString(codEstacao);
        ScanQuoteString(nomeEstacao);
        ScanQuoteString(codLinha);
        ScanQuoteString(nomeLinha);
        ScanQuoteString(codProxEstacao);
        ScanQuoteString(distProxEstacao);
        ScanQuoteString(codLinhaIntegra);
        ScanQuoteString(codEstIntegra);
        //Verificação se a entrada do susário é válida
        if(!strcmp(codEstacao, "") || !strcmp(nomeEstacao, "")){
            printf("Os 2 primeiros campos não podem ser nulos!\n");
            continue;
        }
        //Preenchimento da variável registro com os valores lidos
        registro.codEstacao = atoi(codEstacao);
        strcpy(registro.nomeEstacao, nomeEstacao);
        strcpy(registro.nomeLinha, nomeLinha);
        registro.codLinha = converterStringParaInt(codLinha);
        registro.codProxEstacao = converterStringParaInt(codProxEstacao);
        registro.distProxEstacao = converterStringParaInt(distProxEstacao);
        registro.codLinhaIntegra = converterStringParaInt(codLinhaIntegra);
        registro.codEstIntegra = converterStringParaInt(codEstIntegra);
        registro.tamNomeEstacao = strlen(nomeEstacao);
        registro.tamNomeLinha = strlen(nomeLinha);
        registro.removido = '0';
        registro.proximo = -1;

        //Se será adicionado no final aumenta o número de proxRRN
        if(cabecalho.proxRRN == proxInsercao){
            cabecalho.proxRRN++;
        }
        //Caso contrário desemilha o vaalor da pilha
        else{
            fseek(fbin, cabecalho.topo*tamRegistro+18,SEEK_SET);
            //Lê o campo próximo do registro removido e substitui no topo do cabecalho
            fread(&cabecalho.topo, sizeof(int), 1, fbin);
        }

        //Escrita do registro no arquivo
        escreverRegistro(registro, proxInsercao*tamRegistro+17,fbin);

        //Caso a matriz tenha chegado no limite é alocad mais memória
        if((*nroLinhas)% 200 == 0 && (*nroLinhas) > 0){
            realocacao(&matrizes, nroLinhas);
        }

        //Verificação se o par e o nome da estação já existem
        cabecalho.nroEstacoes++;cabecalho.nroParesEstacao++;
        //Ver se precisa aumentar nroEstacoes e nroParesEstacao
        if(duplicidadeEstacoes(matrizes, nroLinhas, nomeEstacao)){
            cabecalho.nroEstacoes--;
        }
        if(duplicidadeParesEstacao(matrizes, nroLinhas, codProxEstacao, codEstacao)){
            cabecalho.nroParesEstacao--;
        }

        //O nome, codigo e código da próxima estação são escritos na matriz
        strcpy(matrizes[0][*nroLinhas], nomeEstacao);
        strcpy(matrizes[1][*nroLinhas], codEstacao);
        strcpy(matrizes[2][*nroLinhas], codProxEstacao);
        //O número de linhas aumenta
        (*nroLinhas)++;
    }
    //Atualização do cabeçalho do arquivo
    cabecalho.status = '1';
    atualizarCabecalho(cabecalho, fbin);
    //Fechamento da stream
    fclose(fbin);
}


void update(char *arquivoBin, int nroUpdate, char ***matrizes, int* nroLinhas){
    FILE *fbin;
    if(arquivoBin == NULL || !(fbin = fopen(arquivoBin, "r+b"))){
        printf("Erro no Update!");
        return;
    }

    //Leitura do cabecalho do arquivo
    CABECALHO cabecalho = lerCabecalho(fbin);

    //Se a mtriz não tiver sido preenchida ainda
    if(*nroLinhas <= 0){
        populaMatriz(matrizes, nroLinhas, fbin, cabecalho.proxRRN);
    }

    //Passa pelo loop de acordo com quantas vezes o usuário quer atualizar registros
    for(int i = 0; i<nroUpdate; ++i) {
        int cont = 0;
        //Variável para armazenar o número de condições dadas no where
        int quantAnds = 1;
        scanf("%d", &quantAnds);
        //Matriz com o nome do campo e valor do campo em cada condição lida
        char *condicoes[quantAnds][2];
        lerCondicoesBusca(quantAnds,condicoes);
    
        char removido;
        //Variável para armazenar o número de valores a serem atualizados
        int quantAndsUpdate;
        scanf("%d", &quantAndsUpdate);
        //Criação de matrizes com os nomes dos campos e valores dos campos com os
        //valores que serão colocados na atualização
        char nomeCamposUpdate[quantAndsUpdate][23];
        char valoresCamposUpdate[quantAndsUpdate][46];
        //Leitura dos campos e de seus valores
        for(int j = 0; j < quantAndsUpdate; j++){
            scanf(" %s", nomeCamposUpdate[j]);
            ScanQuoteString(valoresCamposUpdate[j]);
        }
        //Ignora o cabeçalho do arquivo
        fseek(fbin,17,SEEK_SET);
        //Lê até o final do arquivo
        while(fread(&removido, 1, 1, fbin) == 1){
            //Ignora os removidos
            if(removido == '1') {
                ++cont;
                fseek(fbin,79,SEEK_CUR);
                continue;
            }
            // Definimos o registro para lê-lo e verificar se atende aos requisitos do WHERE
            REGISTRO registro;
            registro.removido = '0';
            //Função para a leitura de um registro
            bool ok = lerRegistroVerifica(fbin, &registro, quantAnds, condicoes);
            
            //Caso o registro atenda ao que foi pedido é atualizado
            if(ok){
                for(int j = 0; j<quantAndsUpdate; ++j) {
                    //Tranforma o valor lido no valor Campos Update em int
                    int convertido = converterStringParaInt(valoresCamposUpdate[j]);

                    //Caso o campo alterado seja o nome da estação é necessaŕio verificar se o nome altera o npumero de estações
                    if(strcmp(nomeCamposUpdate[j], "nomeEstacao")==0) {
                        int cont2 = 0;
                        /*Aqui há casos a serem vistos como:
                        1-Se o nome retirado não for de uma estação única então não há problema
                        Mas caso contrário o número de estações deve diminuir.
                        2-Considerando  que o número diminuiu, o próximo valor inserido
                        deve ser pode ser único, o que aumenta o valor do número de estações
                        ou não, o que deixa ele como está.
                        Por isso há uma maior verificação desses casos.*/
                        for(int k = 0; k<*nroLinhas; k++){
                            if(!strcmp(matrizes[0][k], registro.nomeEstacao)){
                                ++cont2;
                                if(cont2>1) {
                                    break;
                                }
                            }
                            if(k+1 == *nroLinhas) {
                                cabecalho.nroEstacoes--;
                            }
                        }
                        //Aqui é verificado a repetição do nome da estação
                        ++cabecalho.nroEstacoes;
                        if(duplicidadeEstacoes(matrizes, nroLinhas, registro.nomeEstacao)){
                            --cabecalho.nroEstacoes;
                        }
                        strcpy(matrizes[0][cont], registro.nomeEstacao);
                    /*Cada campo é verificado de acordo com a inserção
                    e seus valores são adiocionados no registro de acordo com as especificações.
                    A função sprintf tranforma um valor inteiro para string nesse caso.*/
                    } else if(strcmp(nomeCamposUpdate[j], "nomeLinha")==0) {
                        if (convertido==-1) {
                            registro.tamNomeLinha = 0;
                            strcpy(registro.nomeLinha, "");
                        } else {
                            registro.tamNomeLinha = strlen(valoresCamposUpdate[j]);
                            strcpy(registro.nomeLinha, valoresCamposUpdate[j]);
                        }
                    } else if(strcmp(nomeCamposUpdate[j], "codEstacao")==0) {
                        char codEstacaoAnt[11], codProxAnt[11];
                        if(registro.codEstacao == -1) {
                            strcpy(codEstacaoAnt, "");
                        }
                        else {
                            sprintf(codEstacaoAnt, "%d", registro.codEstacao);
                        }

                        if(registro.codProxEstacao == -1) {
                            strcpy(codProxAnt, "");
                        }
                        else {
                            sprintf(codProxAnt, "%d", registro.codProxEstacao);
                        }
                        //Age da mesma forma que no caso do campo nomeEstacao
                        int contador = 0;
                        for(int k = 0; k < *nroLinhas; k++){
                            if((!strcmp(matrizes[1][k], codEstacaoAnt) && !strcmp(matrizes[2][k], codProxAnt)) ||  (!strcmp(matrizes[2][k], codEstacaoAnt) && !strcmp(matrizes[1][k], codProxAnt))) {
                                contador++;
                                if(contador>1) {
                                    break;
                                }
                            }
                            if(k+1 == *nroLinhas) {
                                cabecalho.nroParesEstacao--;
                            }
                        }
                        registro.codEstacao = atoi(valoresCamposUpdate[j]);

                        cabecalho.nroParesEstacao++;
                        char codProxAtual[11];
                        if(registro.codProxEstacao == -1) strcpy(codProxAtual, "");
                        else sprintf(codProxAtual, "%d", registro.codProxEstacao);

                        if(duplicidadeParesEstacao(matrizes, nroLinhas, codProxAtual, valoresCamposUpdate[j])) {
                            cabecalho.nroParesEstacao--;
                        }
                        strcpy(matrizes[1][cont], valoresCamposUpdate[j]);

                    } else if(strcmp(nomeCamposUpdate[j], "codLinha")==0) {
                        registro.codLinha = convertido;
                    } else if(strcmp(nomeCamposUpdate[j], "codProxEstacao")==0) {

                        char codEstacaoAnt[11], codProxAnt[11];
                        if(registro.codEstacao == -1) {
                            strcpy(codEstacaoAnt, "");
                        }
                        else { 
                            sprintf(codEstacaoAnt, "%d", registro.codEstacao);
                        }
                        
                        if(registro.codProxEstacao == -1) {
                            strcpy(codProxAnt, "");
                        }
                        else {
                            sprintf(codProxAnt, "%d", registro.codProxEstacao);
                        }

                        int contador = 0;
                        for(int k = 0; k < *nroLinhas; k++){
                            if((!strcmp(matrizes[1][k], codEstacaoAnt) && !strcmp(matrizes[2][k], codProxAnt)) || (!strcmp(matrizes[2][k], codEstacaoAnt) && !strcmp(matrizes[1][k], codProxAnt))) {
                                contador++;
                                if(contador>1) {
                                    break;
                                }
                            }
                            if(k+1 == *nroLinhas) { 
                                cabecalho.nroParesEstacao--;
                            }
                        }

                        registro.codProxEstacao = convertido;
                        
                        cabecalho.nroParesEstacao++;
                        char codEstAtual[11];
                        if(registro.codEstacao == -1) strcpy(codEstAtual, "");
                        else sprintf(codEstAtual, "%d", registro.codEstacao);

                        if(duplicidadeParesEstacao(matrizes, nroLinhas, codEstAtual, valoresCamposUpdate[j])) {
                            cabecalho.nroParesEstacao--;
                        }
                        strcpy(matrizes[2][cont], valoresCamposUpdate[j]);
                    } else if(strcmp(nomeCamposUpdate[j], "distProxEstacao")==0) {
                        registro.distProxEstacao = convertido;
                    } else if(strcmp(nomeCamposUpdate[j], "codLinhaIntegra")==0) {
                        registro.codLinhaIntegra = convertido;
                    } else if(strcmp(nomeCamposUpdate[j], "codEstIntegra")==0) {
                        registro.codEstIntegra = convertido;
                    }
                }
                //O registro é escrito norquivo
                escreverRegistro(registro, 17 + (cont * 80), fbin);

            }

            finalizarBusca(fbin,&cont);
        }
        //Libera a matriz de condições criada
        liberarCondicoes(quantAnds,condicoes);
    }
    cabecalho.status = '1';
    //Atualiza os valores no cabeçalho
    atualizarCabecalho(cabecalho, fbin);
    //Fechamento d astream
    fclose(fbin);
}

// Função que representa o SELECT FROM, com ou sem cláusula WHERE
void selectFromWhere(char *arquivoBin, int quantBuscas, bool temWhere) {
    // Processamento padrão de arquivo
    FILE *fbin;
    if(arquivoBin == NULL || !(fbin = fopen(arquivoBin, "rb"))){
        printf("Erro no processamento do arquivo.\n");
        return;
    }
    // Fazemos os "OR's" do SELECT (isso é, cada uma das buscas)
    for(int i = 0; i<quantBuscas; ++i) {
        int cont = 0;
        int quantAnds = 0;
        // Se tiver cláusula WHERE (função 3), podemos ter AND - chamado de "m"/"quantidade de vezes que o par nome e valor do campo pode repetir na busca
        if(temWhere) {
            scanf("%d", &quantAnds);
        }
        char *condicoes[quantAnds][2];  
        if(temWhere) {
            lerCondicoesBusca(quantAnds,condicoes);
        }
        fseek(fbin,17,SEEK_SET);
        
        bool encontrou = 0;
        char removido;
        // Procedimento padrão de leitura dos registros - lemos o primeiro int, que indica se o registro foi logicamente removido
        // Se sim, pulamos
        while(fread(&removido, 1, 1, fbin) == 1){
            if(removido == '1') {
                ++cont;
                fseek(fbin,79,SEEK_CUR);
                continue;
            }

            // Definimos o registro para lê-lo e verificar se atende aos requisitos do WHERE
            REGISTRO registro;
            registro.removido = '0';
            bool ok = lerRegistroVerifica(fbin,&registro,quantAnds,condicoes);

            // Se encontrou ao menos um registro condizente com os parâmetros de busca, imprimimos as informações do registro
            if(ok) {
                encontrou = 1;
                // Antes de imprimir, verificamos se os campos são nulos, exceto pelo código e nome da estação, que são, garantidamente, não nulos
                // para fazer a impressão apropriada
                printf("%d ", registro.codEstacao);
                printf("%s ", registro.nomeEstacao);
                if(registro.codLinha == -1) {
                    printf("NULO "); 
                } else {
                    printf("%d ", registro.codLinha);
                }
                if(registro.tamNomeLinha == 0) {
                    printf("NULO "); 
                } else {
                    printf("%s ", registro.nomeLinha);
                }
                if(registro.codProxEstacao == -1) {
                    printf("NULO "); 
                } else {
                    printf("%d ", registro.codProxEstacao);
                }
                if(registro.distProxEstacao == -1) {
                    printf("NULO "); 
                } else {
                    printf("%d ", registro.distProxEstacao);
                }
                if(registro.codLinhaIntegra == -1) {
                    printf("NULO ");
                 } else {
                    printf("%d ", registro.codLinhaIntegra);
                 }
                if(registro.codEstIntegra == -1) {
                    printf("NULO\n"); 
                }else {
                    printf("%d\n", registro.codEstIntegra);
                }
            }
            
            finalizarBusca(fbin,&cont);
        }
        // Se nenhum registro foi encontrado, fazemos a impressão conforme pedido
        if(!encontrou) {
            printf("Registro inexistente.\n");
        }
        if(temWhere) {
            liberarCondicoes(quantAnds,condicoes);
            printf("\n");
        }
    }
    fclose(fbin);
}

// Função que representa o DELETE FROM WHERE. Retorna se houve erro (1) ou não (0)
bool deleteFromWhere(char *arquivoBin, int quantRemocoes, char ***matrizes, int *nroLinhas) {
    // Processamento padrão de arquivos
    FILE *fbin;
    if(arquivoBin == NULL || !(fbin = fopen(arquivoBin, "r+b"))){
        printf("Falha no processamento do arquivo.\n");
        return 1;
    }

    CABECALHO cabecalho = lerCabecalho(fbin);

    if(*nroLinhas <= 0){
        populaMatriz(matrizes, nroLinhas, fbin, cabecalho.proxRRN);
    }

    // Fazemos os "OR's" da remoção (ou quantidade de remoções diferentes a serem feitas)
    for(int i = 0; i<quantRemocoes; ++i) {
        int cont = 0;
        int quantAnds = 1;
        // Pedimos quantos "AND's" tem esse DELETE (quantas condições um registro tem que cumprir para ser deletado)
        scanf("%d", &quantAnds);
        
        char *condicoes[quantAnds][2];
        lerCondicoesBusca(quantAnds,condicoes);
        
        fseek(fbin, 17, SEEK_SET);

        char removido;

        // Procedimento padrão de leitura dos registros - lemos o primeiro int, que indica se o registro foi logicamente removido
        // Se sim, pulamos
        while(fread(&removido, 1, 1, fbin) == 1){
            if(removido == '1') {
                fseek(fbin, 79, SEEK_CUR);
                ++cont;
                continue;
            }

            // Definimos o registro para lê-lo e verificar se atende aos requisitos do WHERE
            REGISTRO registro;
            registro.removido = '0';
            bool ok = lerRegistroVerifica(fbin, &registro, quantAnds, condicoes);
            // Se o registro atender aos requisitos do WHERE, fazemos a sua deleção
            if(ok) {
                // Reposicionamos o cursor no início do registro
                fseek(fbin, tamHeader + (cont * tamRegistro), SEEK_SET);
                
                // Marcamos o registro como logicamente removido e atualizamos o cabeçalho conforme a necessidade
                char marcarRemocao = '1';
                fwrite(&marcarRemocao, 1, 1, fbin);
                fwrite(&cabecalho.topo, 4, 1, fbin);
                cabecalho.topo = cont;
                char codEstacaoAnt[11], codProxAnt[11];
                if(registro.codEstacao == -1) {
                    strcpy(codEstacaoAnt, "");
                }
                else {
                    sprintf(codEstacaoAnt, "%d", registro.codEstacao);
                }
                if(registro.codProxEstacao == -1) {
                    strcpy(codProxAnt, ""); 
                } else {
                    sprintf(codProxAnt, "%d", registro.codProxEstacao);
                }
                // Apagamos o registro da matriz
                for(int k = 0; k < *nroLinhas; ++k) {
                    // Verificamos se o registro na matriz corresponde ao que estamos lidando com
                    if(!strcmp(matrizes[0][k], registro.nomeEstacao) && !strcmp(matrizes[1][k], codEstacaoAnt) && !strcmp(matrizes[2][k], codProxAnt)) {
                        // Limpamos as informações do registro na matriz
                        strcpy(matrizes[0][k], "");
                        strcpy(matrizes[1][k], "");
                        strcpy(matrizes[2][k], "");
                        // Se encontramos o registro na matriz, paramos
                        break;
                    }
                }

                // Checamos se as duplicidades ainda existem na matriz e atualizamos o cabeçalho conforme necessidade
                if(!duplicidadeEstacoes(matrizes, nroLinhas, registro.nomeEstacao)) {
                    --cabecalho.nroEstacoes;
                }
                if(!duplicidadeParesEstacao(matrizes, nroLinhas, codProxAnt, codEstacaoAnt)) {
                    --cabecalho.nroParesEstacao;
                }
            }
            
            finalizarBusca(fbin,&cont);
        }
        
        liberarCondicoes(quantAnds,condicoes);
    }
    // Atualizamos o cabeçalho no arquivo após as deleções realizadas
    cabecalho.status = '1';
    atualizarCabecalho(cabecalho, fbin);

    fclose(fbin);
    return 0;
}