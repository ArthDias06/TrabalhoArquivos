#include "join.h"

//Função de junção por brute force
void joinNestedLoop(char* arqEntrada1, char*arqEntrada2){
    //Abertura dos arquivos e verificação das consistências
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
    //Guarda se houve pelo menos um registro existente
    bool flag = false;
    REGISTRO registro1, registro2;
    //Pula o cabeçalho de ambos arquivos
    fseek(fbin1, tamHeader-1, SEEK_CUR);
    while(fread(&removido, 1, 1, fbin1) == 1) {
        // Pulamos registros logicamente removidos
        if(removido == '1') {
            fseek(fbin1, tamRegistro - 1, SEEK_CUR);
            continue;
        }
        lerRegistro(fbin1, &registro1);
        //Vai para o próximo registro
        fseek(fbin1, tamHeader+cont*tamRegistro, SEEK_SET);
        cont++;
        //Posiciona o fbin2 no início dos registros do segundo arquivo
        fseek(fbin2, tamHeader, SEEK_SET);
        //Inicializa o segundo contador
        int cont2=1;
        while(fread(&removido, 1, 1, fbin2) == 1){
            if(removido == '1') {
                fseek(fbin2, tamRegistro - 1, SEEK_CUR);
                continue;
            }
            lerRegistro(fbin2, &registro2);
            fseek(fbin2, tamHeader+cont2*tamRegistro, SEEK_SET);
            cont2++;
            //Caso o codProxEstacao do registro1 seja igual ao codEstcao do registro2
            if(registro2.codEstacao == registro1.codProxEstacao){
                //Flag vira true, pois existe um registro
                flag = true;
                //As informações são mostradas
                printf("%d %s %s %d %s\n", registro1.codEstacao, registro1.nomeEstacao, registro1.nomeLinha, registro1.codProxEstacao, registro2.nomeEstacao);
            }
        }
    }
    //Se nenhum registro exixstir, retorna para o usuário
    if(!flag){
        printf("Registro inexistente.\n");
    }
    //Fecha as streams
    fclose(fbin1);
    fclose(fbin2);
}

//Junção de loop único
void joinUsandoIndice(char *arqEntrada1, char *arqEntrada2, char *arqArvore){
    //Abertura das streams e verificação das consistências
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
    FILE *farvore;
    if (arqArvore == NULL || !(farvore = fopen(arqArvore, "rb"))) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin1);
        fclose(fbin2);
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
    //Leitura do nó raiz da árvore B
    fread(&noRaiz, sizeof(int), 1, farvore);
    while(fread(&removido, 1, 1, fbin1) == 1) {
        if(removido == '1'){
            fseek(fbin1, tamRegistro-1, SEEK_CUR);
            continue;
        }
        lerRegistro(fbin1, &registro1);
        fseek(fbin1, tamHeader+cont*tamRegistro, SEEK_SET);
        cont++;
        //Posiciona a stream da árvore no início dos registros
        fseek(farvore, 17, SEEK_SET);
        //Verifica a existência da chave
        int offset = buscarChave(farvore, registro1.codProxEstacao, noRaiz);
        //offset == -1, não há a chave
        if(offset == -1){
            continue;
        }
        //Vai até o registro com a chave
        fseek(fbin2, offset, SEEK_SET);
        fread(&removido, sizeof(char), 1, fbin2);
        //Verificação se o registro existe
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

int campoOrd;

// Comparador para ordenar os registros de forma crescente pelo campo de ordenação
int comparaOrdenacao(const void *a, const void *b){
    const REGISTRO *r1 = a;
    const REGISTRO *r2 = b;
    //Selecionamos o valor do campo de ordenação de cada registro
    int v1,v2;
    if(campoOrd == 2) {
        v1 = r1->codEstacao;
    } else {
        v1 = r1->codProxEstacao;
    }
    if(campoOrd == 2) {
        v2 = r2->codEstacao;
    } else {
        v2 = r2->codProxEstacao;
    }

    if(v1 == -1 && v2 == -1) {
        return 0;
    }
    if(v1 == -1) {
        return 1;
    }
    if(v2 == -1) {
        return -1;
    }

    return v1-v2;
}


bool orderBy(char* arquivoBin, char* campo, char* arquivoOrd){
    // Processamento padrão de arquivos
    FILE *fbin1;
    if (arquivoBin == NULL || !(fbin1 = fopen(arquivoBin, "rb"))) {
        printf("Falha no processamento do arquivo.\n");
        return false;
    }

    CABECALHO cabecalho;
    fseek(fbin1, 0, SEEK_SET);
    fread(&cabecalho.status, sizeof(char), 1, fbin1);
    if(cabecalho.status == '0'){
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin1);
        return false;
    }
    fread(&cabecalho.topo, sizeof(int), 1, fbin1);
    fread(&cabecalho.proxRRN, sizeof(int), 1, fbin1);
    fread(&cabecalho.nroEstacoes, sizeof(int), 1, fbin1);
    fread(&cabecalho.nroParesEstacao, sizeof(int), 1, fbin1);

    // Definimos qual campo será usado na ordenação (considerando somente codEstacao e codProxEstacao)
    campoOrd = strcmp(campo, "codEstacao") == 0 ? 2 : 4;

    // Alocamos espaço para todos os registros
    REGISTRO *registros = NULL;
    if(cabecalho.proxRRN > 0){
        registros = malloc(cabecalho.proxRRN * sizeof(REGISTRO));
        if(registros == NULL){
            printf("Falha no processamento do arquivo.\n");
            fclose(fbin1);
            return false;
        }
    }

    // Lemos todos os registros não removidos do arquivo e colocamos no espaço alocado
    int total = 0;
    char removido;
    for(int rrn = 0; rrn<cabecalho.proxRRN; ++rrn){
        fseek(fbin1, tamHeader + rrn*tamRegistro, SEEK_SET);
        fread(&removido, sizeof(char), 1, fbin1);

        if(removido == '1'){
            continue;
        }

        lerRegistro(fbin1, &registros[total]);
        registros[total].removido = '0';
        ++total;
    }

    // Ordenamos os registros usando qsort (como especificado para usar algum da biblioteca do C)
    qsort(registros, total, sizeof(REGISTRO), comparaOrdenacao);

    //Só agora abrimos o segundo arquivo, útil para o caso em que arquivo1 == arquivo2
    FILE *fbin2;
    if (arquivoOrd == NULL || !(fbin2 = fopen(arquivoOrd, "wb"))) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin1);
        return false;
    }

    // Escrevemos o cabeçalho do arquivo ordenado, sem a pilha de removidos e com o novo proxRRN
    cabecalho.status = '0';
    cabecalho.topo = -1;
    cabecalho.proxRRN = total;
    atualizarCabecalho(cabecalho, fbin2);

    // Escrevemos o resultado no arquivo de saída e liberamos a memórias
    for(int i = 0; i < total; ++i){
        escreverRegistro(registros[i], fbin2);
    }
    cabecalho.status = '1';
    atualizarCabecalho(cabecalho, fbin2);

    free(registros);
    fclose(fbin1);
    fclose(fbin2);
    return true;
}

//Junção por ordenação-intercalação
void joinIntercalacao(char* arquivoBin1, char* arquivoBin2){
    //Ordena os arquivos primeiro
    if(!orderBy(arquivoBin1, "codProxEstacao", arquivoBin1) || !orderBy(arquivoBin2, "codEstacao", arquivoBin2)){
        //Sem necessidade de mensagem de erro aqui, pois seria printado na função orderBy
        return;
    }

    FILE *fbin1;
    if (!(fbin1 = fopen(arquivoBin1, "rb"))) {
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
    if (!(fbin2 = fopen(arquivoBin2, "rb"))) {
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

    int contProxEstacao=0, contEstacao=0;
    int proxRRN1, proxRRN2;
    REGISTRO registro1, registro2;
    bool flag = false;

    //Leitura de proxRRN1 e proxRRN2
    fseek(fbin1, sizeof(int), SEEK_CUR);
    fread(&proxRRN1, sizeof(int), 1, fbin1);
    fseek(fbin2, sizeof(int), SEEK_CUR);
    fread(&proxRRN2, sizeof(int), 1, fbin2);
    //Percorre ambos arquivos até que um deles chegue ao fim
    while(contProxEstacao < proxRRN1 && contEstacao < proxRRN2){
        fseek(fbin1, tamHeader+1+contProxEstacao*tamRegistro, SEEK_SET);
        fseek(fbin2, tamHeader+1+contEstacao*tamRegistro, SEEK_SET);
        lerRegistro(fbin1, &registro1);
        lerRegistro(fbin2, &registro2);
        if(registro1.codProxEstacao == registro2.codEstacao){
            flag = true;
            printf("%d %s %s %d %s\n", registro1.codEstacao, registro1.nomeEstacao, registro1.nomeLinha, registro1.codProxEstacao, registro2.nomeEstacao);
            //Se os dois foram iguais, apenas o codProxEstacao aumenta, pois como ele
            //não é uma chave única, ela pode se repetir, então poderiam haver dois
            //codEstacao's diferentes com o mesmo codProxEstacao, isso evita qualquer perda de informação.
            contProxEstacao++;
        }
        //Aumenta o contador daquele que representa o campo de menor valor
        else if(registro1.codProxEstacao > registro2.codEstacao){
            contEstacao++;
        }else{
            contProxEstacao++;
        }
    }
    if(!flag){
        printf("Registro inexistente.\n");
    }
    fclose(fbin1);
    fclose(fbin2);
}