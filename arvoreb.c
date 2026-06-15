#include"arvoreb.h"
#include"matriz.h"

void criaRaiz(FILE* fbin, int promover, int promoverChave, int promoverRegistro, ARVOREB_CABECALHO *cabecalho){
    PAGINA pagina = inicializaPagina();
    int rrn = cabecalho->noRaiz;
    int filho, tipoNo;
    pagina.chave[0] = promoverChave;
    pagina.registro[0] = promoverRegistro;
    pagina.filho[0] = cabecalho->noRaiz;
    pagina.filho[1] = promover;
    if(pagina.filho[0] == -1){
        pagina.tipoNo = -1;//Nó folha = nó raiz
    } else {
        pagina.tipoNo = 0;//Nó raiz
    }
    pagina.nroChaves++;
    cabecalho->noRaiz = cabecalho->proxRRN;
    //Só é válido se já há uma raiz
    if(rrn != -1){
        fseek(fbin, 54+53*rrn, SEEK_SET);
        fread(&filho, sizeof(int), 1, fbin);
        fseek(fbin, -36, SEEK_CUR);
        if(filho != -1){
            tipoNo = 1;
        }
        else tipoNo = -1;
        //Atualiza o tipo no do antigo nó raiz no arquivo
        fwrite(&tipoNo, sizeof(int), 1, fbin);
    }
    escreverNO(fbin, cabecalho->proxRRN, pagina);
    cabecalho->proxRRN++;
    cabecalho->nroNos++;
}


void createIndex(char* arq, char* arvore_arq){
    FILE *fbin;
    if (arq == NULL || !(fbin = fopen(arq, "rb"))) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    FILE *fbin_arvore;
    //É aberto como write+ por conta de que o ponteiro do arquivo pode ser usado para leitura no insert
    if (arvore_arq == NULL || !(fbin_arvore = fopen(arvore_arq, "w+b"))) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin);
        return;
    }
    ARVOREB_CABECALHO cabecalho = {'0', -1, -1, 0, 0};
    escreverCabecalhoArvore(fbin_arvore, cabecalho);
    fseek(fbin, 0, SEEK_SET);
    char status, removido;
    fread(&status, sizeof(char), 1, fbin);
    if(status == '0'){
        printf("Falha no processamento do arquivo!");
        fclose(fbin);
        fclose(fbin_arvore);
        return;
    }
    int rrn=-1;
    fseek(fbin, 16, SEEK_CUR);
    while(fread(&removido, sizeof(char), 1, fbin) == 1){
        //Armazena a posição so registro
        rrn++;
        if(removido == '1') {
            fseek(fbin,79,SEEK_CUR);
            continue;
        }
        //Ignora o campo próximo
        fseek(fbin, 4, SEEK_CUR);
        int chave, promover, promoverChave, promoverRegistro;
        bool flag = false;
        //Chave tem o valor de codEstacao do registro
        fread(&chave, sizeof(int), 1, fbin);
        //Insere na árvore B
        if(insert(fbin_arvore, 17 + rrn*80, cabecalho.noRaiz, chave, &promover, &promoverChave, &promoverRegistro, &cabecalho, &flag)){
            criaRaiz(fbin_arvore, promover, promoverChave, promoverRegistro, &cabecalho);
        }
        //Passa para o próximo registro
        fseek(fbin, 71, SEEK_CUR);
    }
    cabecalho.status = '1';
    escreverCabecalhoArvore(fbin_arvore, cabecalho);
    fclose(fbin_arvore);
    fclose(fbin);
}


int busca(FILE *fbin, int chave, int RRN){
    if(RRN == -1){
        printf("Registro não encontrado!");
        return -1;
    }
    if(fbin == NULL){
        printf("Falha no processamento do arquivo!");
        return -1;
    }
    int pr = buscaChave(fbin, chave, RRN);
    if(pr == -1) {
        printf("Registro não encontrado!");
    }
    return pr;
}

PAGINA localizaNo(FILE *fbin, int RRN, int chave, int *posicao, bool *achou) {
    PAGINA pagina = lerNO(fbin, RRN);
    int i = 0;
    while(i<pagina.nroChaves && chave>pagina.chave[i]) {
        ++i;
    }
    *posicao = i;
    if(i<pagina.nroChaves && chave == pagina.chave[i]) {
        *achou = true;
    } else {
        *achou = false;
    }
    return pagina;
}

int buscaChave(FILE *fbin, int chave, int RRN) {
    if(RRN == -1) {
        return -1;
    }
    int posicao;
    bool achou;
    PAGINA pagina = localizaNo(fbin, RRN, chave, &posicao, &achou);
    if(achou) {
        return pagina.registro[posicao];
    }
    return buscaChave(fbin, chave, pagina.filho[posicao]);
}


PAGINA ins_in_page(int chave, int registro, int filho, PAGINA pagina){
    int i;
    for(i = pagina.nroChaves-1; i>=0 && chave < pagina.chave[i]; i--){
        pagina.chave[i+1] = pagina.chave[i];
        pagina.registro[i+1] = pagina.registro[i];
        pagina.filho[i+2] = pagina.filho[i+1];
    }
    pagina.nroChaves++;
    pagina.chave[i+1] = chave;
    pagina.registro[i+1] = registro;
    pagina.filho[i+2] = filho;
    return pagina;
}

void escreverNO(FILE *fArvore, int rrn, PAGINA pagina) {
    fseek(fArvore, 17+53*rrn, SEEK_SET);
    fwrite(&pagina.removido, sizeof(char), 1, fArvore);
    fwrite(&pagina.proximo, sizeof(int), 1, fArvore);
    fwrite(&pagina.tipoNo, sizeof(int), 1, fArvore);
    fwrite(&pagina.nroChaves, sizeof(int), 1, fArvore);
    for(int i = 0; i < maxchaves; i++){
        fwrite(&pagina.chave[i], sizeof(int), 1, fArvore);
        fwrite(&pagina.registro[i], sizeof(int), 1, fArvore);
    }
    for(int i = 0; i < ordem; i++){
        fwrite(&pagina.filho[i], sizeof(int), 1, fArvore);
    }
    fflush(fArvore);
}

PAGINA inicializaPagina(){
    PAGINA pagina;
    for(int j = 0; j<maxchaves; j++){
        pagina.chave[j] = -1;
        pagina.registro[j] = -1;
        pagina.filho[j] = -1;
    }
    pagina.filho[maxchaves] = -1;
    pagina.proximo = -1;
    pagina.tipoNo = -1;
    pagina.nroChaves = 0;
    pagina.removido = '0';
    return pagina;
}

PAGINA split(FILE *fbin, int chave, int filho,int registro,  PAGINA *pagina, int *promover_chave, PAGINA novaPagina, int *promover, int *promoverRegistro, ARVOREB_CABECALHO *cabecalho){
    int chavesTrabalho[ordem];
    int filhosTrabalho[ordem+1];
    int registroTrabalho[ordem];
    int i;
    for(i = 0; i < maxchaves; i++){
        chavesTrabalho[i] = pagina->chave[i];
        filhosTrabalho[i] = pagina->filho[i];
        registroTrabalho[i] = pagina->registro[i];
    }
    filhosTrabalho[i] = pagina->filho[i];
    for(i = maxchaves; i > 0 && chave < chavesTrabalho[i-1]; i--){
        chavesTrabalho[i] = chavesTrabalho[i-1];
        filhosTrabalho[i+1] = filhosTrabalho[i];
        registroTrabalho[i] = registroTrabalho[i-1];
    }
    chavesTrabalho[i] = chave;
    filhosTrabalho[i+1] = filho;
    registroTrabalho[i] = registro;
    if(cabecalho->topo == -1){
        *promover = cabecalho->proxRRN++;
    }else{
        int prox;
        *promover = cabecalho->topo;
        fseek(fbin, 18+cabecalho->topo*53, SEEK_SET);
        fread(&prox, sizeof(int), 1, fbin);
        cabecalho->topo = prox;
    }
    novaPagina = inicializaPagina();
    novaPagina.tipoNo = (pagina->tipoNo == -1) ? -1 : 1;
    for(i = 0; i < minchaves; i++){
        pagina->chave[i] = chavesTrabalho[i];
        pagina->filho[i] = filhosTrabalho[i];
        pagina->registro[i] = registroTrabalho[i];
        novaPagina.chave[i] = chavesTrabalho[i + 2 + minchaves];
        novaPagina.filho[i] = filhosTrabalho[i + 2 + minchaves];
        novaPagina.registro[i] = registroTrabalho[i + 2 + minchaves];
        pagina->chave[i + minchaves + 1]  = -1;
        pagina->filho[i + minchaves + 2]  = -1;
        pagina->registro[i + minchaves + 1] = -1;
    }
    pagina->chave[i] = chavesTrabalho[i];
    pagina->filho[i] = filhosTrabalho[i];
    pagina->registro[i] = registroTrabalho[i];
    pagina->filho[minchaves + 1] = filhosTrabalho[minchaves + 1];
    novaPagina.filho[minchaves] = filhosTrabalho[i + minchaves + 2];
    pagina->nroChaves = maxchaves - minchaves;
    novaPagina.nroChaves = minchaves;
    *promover_chave = chavesTrabalho[minchaves + 1];
    *promoverRegistro = registroTrabalho[minchaves + 1];
    return novaPagina;
}

bool insert(FILE* fbin, int registro, int RRN, int chave, int *promover, int *promover_chave, int *promoverRegistro, ARVOREB_CABECALHO *cabecalho, bool *flag){
    if(fbin == NULL){
        printf("Erro no processamento do arquivo!");
        return false;
    }

    PAGINA pagina, novaPagina;
    bool encontrado, promovido;
    int posicao, filho, chave_abaixo, registroAbaixo;
    if(RRN == -1){
        *promover_chave = chave;
        *promoverRegistro = registro;
        *promover = -1;
        return true;
    }

    pagina = localizaNo(fbin, RRN, chave, &posicao, &encontrado);

    if(encontrado){
        *flag = true;
        return false;
    }

    promovido = insert(fbin, registro, pagina.filho[posicao], chave, &filho, &chave_abaixo, &registroAbaixo, cabecalho, flag);
    if(!promovido){
        return false;
    }
    if(pagina.nroChaves < maxchaves){
        pagina = ins_in_page(chave_abaixo, registroAbaixo, filho, pagina);
        escreverNO(fbin, RRN, pagina);
        return false;
    }
    else{
        novaPagina = split(fbin, chave_abaixo, filho, registroAbaixo, &pagina, promover_chave, novaPagina, promover, promoverRegistro, cabecalho);
        cabecalho->nroNos++;
        escreverNO(fbin, RRN, pagina);
        escreverNO(fbin, *promover, novaPagina);
        return true;
    }
}


PAGINA lerNO(FILE *fArvore, int rrn) {
    PAGINA pagina;
    fseek(fArvore, 17+53*rrn, SEEK_SET);
    fread(&pagina.removido, sizeof(char), 1, fArvore);
    fread(&pagina.proximo, sizeof(int), 1, fArvore);
    fread(&pagina.tipoNo, sizeof(int), 1, fArvore);
    fread(&pagina.nroChaves, sizeof(int), 1, fArvore);
    for(int i = 0; i<maxchaves; ++i) {
        fread(&pagina.chave[i], sizeof(int), 1, fArvore);
        fread(&pagina.registro[i], sizeof(int), 1, fArvore);
    }
    for(int i = 0; i<ordem; ++i) {
        fread(&pagina.filho[i], sizeof(int), 1, fArvore);
    }
    return pagina;
}
 
bool isFolha(PAGINA pagina) {
    if(pagina.filho[0] == -1) {
        return true;
    }
    return false;
}
 
void liberarPagina(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int rrn) {
    char marca = '1';
    int prox = cabecalho->topo;
    fseek(fArvore, 17+53*rrn, SEEK_SET);
    fwrite(&marca, sizeof(char), 1, fArvore);
    fwrite(&prox, sizeof(int), 1, fArvore);
    cabecalho->topo = rrn;
    --cabecalho->nroNos;
}
 
void buscarSucessor(FILE *fArvore, int rrn, int *chave, int *p) {
    PAGINA pagina = lerNO(fArvore, rrn);
    while(!isFolha(pagina)) {
        rrn = pagina.filho[0];
        pagina = lerNO(fArvore, rrn);
    }
    *chave = pagina.chave[0];
    *p = pagina.registro[0];
}
 
void removerChaveFolha(PAGINA *pagina, int posicao) {
    for(int i = posicao; i<pagina->nroChaves-1; ++i) {
        pagina->chave[i] = pagina->chave[i+1];
        pagina->registro[i] = pagina->registro[i+1];
    }
    --pagina->nroChaves;
    pagina->chave[pagina->nroChaves] = -1;
    pagina->registro[pagina->nroChaves] = -1;
}
 
void redistribuir(FILE *fArvore, int rrnPai, int separacao) {
    PAGINA pai = lerNO(fArvore, rrnPai);
    int rrnEsquerda = pai.filho[separacao], rrnDireita = pai.filho[separacao+1];
    PAGINA esquerda = lerNO(fArvore, rrnEsquerda), direita = lerNO(fArvore, rrnDireita);
 
    int totalChaves = 0, totalFilhos = 0, filhos[ordem*2+1], posicoes[ordem*2], chaves[ordem*2];
    for(int i = 0; i<esquerda.nroChaves; ++i) {
        chaves[totalChaves] = esquerda.chave[i];
        posicoes[totalChaves++] = esquerda.registro[i];
        filhos[totalFilhos++] = esquerda.filho[i];
    }
    filhos[totalFilhos++] = esquerda.filho[esquerda.nroChaves];
    chaves[totalChaves] = pai.chave[separacao];
    posicoes[totalChaves++] = pai.registro[separacao];
    for(int i = 0; i<direita.nroChaves; ++i) {
        chaves[totalChaves] = direita.chave[i];
        posicoes[totalChaves] = direita.registro[i];
        ++totalChaves;
    }
    for(int i = 0; i<=direita.nroChaves; ++i) {
        filhos[totalFilhos] = direita.filho[i];
        ++totalFilhos;
    }
 
    int quantEsquerda = totalChaves/2;
    int tipo = esquerda.tipoNo, tipo2 = direita.tipoNo;
    esquerda = inicializaPagina();
    direita = inicializaPagina();
    esquerda.tipoNo = tipo;
    direita.tipoNo = tipo2;
 
    for(int i = 0; i<quantEsquerda; ++i) {
        esquerda.chave[i] = chaves[i];
        esquerda.registro[i] = posicoes[i];
        esquerda.filho[i] = filhos[i];
    }
    esquerda.filho[quantEsquerda] = filhos[quantEsquerda];
    esquerda.nroChaves = quantEsquerda;
 
    pai.chave[separacao] = chaves[quantEsquerda];
    pai.registro[separacao] = posicoes[quantEsquerda];
 
    for(int i = quantEsquerda+1, j=0; i<totalChaves; ++i) {
        direita.chave[j] = chaves[i];
        direita.registro[j] = posicoes[i];
        ++j;
    }
    direita.nroChaves = totalChaves-quantEsquerda-1;
    for(int i = quantEsquerda+1, j=0; i<totalFilhos; ++i) {
        direita.filho[j] = filhos[i];
        ++j;
    }
 
    escreverNO(fArvore,rrnEsquerda,esquerda);
    escreverNO(fArvore,rrnDireita,direita);
    escreverNO(fArvore,rrnPai,pai);
}
 
void concatenar(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int rrnPai, int separacao) {
    PAGINA pai = lerNO(fArvore, rrnPai);
    int rrnEsquerda = pai.filho[separacao], rrnDireita = pai.filho[separacao+1];
    PAGINA esquerda = lerNO(fArvore, rrnEsquerda), direita = lerNO(fArvore, rrnDireita);
 
    int posicaoAtual = esquerda.nroChaves;
    
    esquerda.chave[posicaoAtual] = pai.chave[separacao];
    esquerda.registro[posicaoAtual] = pai.registro[separacao];
    esquerda.filho[posicaoAtual+1] = direita.filho[0];
    ++esquerda.nroChaves;
    ++posicaoAtual;
    
    for(int i = 0; i<direita.nroChaves; ++i) {
        esquerda.chave[posicaoAtual] = direita.chave[i];
        esquerda.registro[posicaoAtual] = direita.registro[i];
        esquerda.filho[posicaoAtual+1] = direita.filho[i+1];
        ++esquerda.nroChaves;
        ++posicaoAtual;
    }
 
    escreverNO(fArvore, rrnEsquerda, esquerda);
    liberarPagina(fArvore, cabecalho, rrnDireita);
 
    for(int i = separacao; i<pai.nroChaves-1; ++i) {
        pai.chave[i] = pai.chave[i+1];
        pai.registro[i] = pai.registro[i+1];
        pai.filho[i+1] = pai.filho[i+2];
    }
    --pai.nroChaves;
    pai.chave[pai.nroChaves] = -1;
    pai.registro[pai.nroChaves] = -1;
    pai.filho[pai.nroChaves+1] = -1;
    escreverNO(fArvore, rrnPai, pai);
}
 
void trataUnderflow(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int rrnPai, int posFilho) {
    PAGINA pai = lerNO(fArvore, rrnPai);
 
    if(posFilho<pai.nroChaves) {
        PAGINA direita = lerNO(fArvore, pai.filho[posFilho+1]);
        if(direita.nroChaves>minchaves) {
            redistribuir(fArvore, rrnPai, posFilho);
            return;
        }
    }
    if(posFilho>0) {
        PAGINA esquerda = lerNO(fArvore, pai.filho[posFilho-1]);
        if(esquerda.nroChaves>minchaves) {
            redistribuir(fArvore, rrnPai, posFilho-1);
            return;
        }
    }
    if(posFilho>0) {
        concatenar(fArvore, cabecalho, rrnPai, posFilho-1);
        return;
    }
    
    concatenar(fArvore, cabecalho, rrnPai, posFilho);
}
 
 
bool removerChaveArvoreInterno(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int rrn, int chave) {
    int posicao;
    bool achou;
    PAGINA pagina = localizaNo(fArvore, rrn, chave, &posicao, &achou);
 
    if(isFolha(pagina)) {
        if(!achou) {
            return false;
        }
        removerChaveFolha(&pagina, posicao);
        escreverNO(fArvore, rrn, pagina);
        if(pagina.nroChaves<minchaves) {
            return true;
        }
        return false;
    }
 
    if(achou) {
        int chave2,posicao2;
        buscarSucessor(fArvore, pagina.filho[posicao+1], &chave2, &posicao2);
        pagina.chave[posicao] = chave2;
        pagina.registro[posicao] = posicao2;
        escreverNO(fArvore, rrn, pagina);
        if(removerChaveArvoreInterno(fArvore, cabecalho, pagina.filho[posicao+1], chave2)) {
            trataUnderflow(fArvore, cabecalho, rrn, posicao+1);
        }
    } else {
        if(removerChaveArvoreInterno(fArvore, cabecalho, pagina.filho[posicao], chave)) {
            trataUnderflow(fArvore, cabecalho, rrn, posicao);
        }
    }
 
    pagina = lerNO(fArvore, rrn);
    if(pagina.nroChaves<minchaves) {
        return true;
    }
    return false;
}
 
void removerChaveArvore(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int chave) {
    if(cabecalho->noRaiz == -1) {
        return;
    }
 
    removerChaveArvoreInterno(fArvore, cabecalho, cabecalho->noRaiz, chave);
 
    PAGINA raiz = lerNO(fArvore, cabecalho->noRaiz);
    if(raiz.nroChaves == 0) {
        if(!isFolha(raiz)) {
            int antigaRaiz = cabecalho->noRaiz;
            cabecalho->noRaiz = raiz.filho[0];
            liberarPagina(fArvore, cabecalho, antigaRaiz);
            PAGINA novaRaiz = lerNO(fArvore, cabecalho->noRaiz);
            if(isFolha(novaRaiz)) {
                novaRaiz.tipoNo = -1;
            } else {
                novaRaiz.tipoNo = 0;
            }
            escreverNO(fArvore, cabecalho->noRaiz, novaRaiz);
        } else {
            liberarPagina(fArvore, cabecalho, cabecalho->noRaiz);
            cabecalho->noRaiz = -1;
        }
    }
}
 
bool lerCabecalhoDados(FILE *fbin, CABECALHO *cab) {
    fseek(fbin, 0, SEEK_SET);
    fread(&cab->status, sizeof(char), 1, fbin);
    if(cab->status == '0') {
        return false;
    }
    cab->status = '0';
    fseek(fbin, 0, SEEK_SET);
    fwrite(&cab->status, sizeof(char), 1, fbin);
    fseek(fbin, 1, SEEK_SET);
    fread(&cab->topo, sizeof(int), 1, fbin);
    fread(&cab->proxRRN, sizeof(int), 1, fbin);
    fread(&cab->nroEstacoes, sizeof(int), 1, fbin);
    fread(&cab->nroParesEstacao, sizeof(int), 1, fbin);
    return true;
}
 
void escreverCabecalhoDados(FILE *fbin, CABECALHO cab) {
    fseek(fbin, 0, SEEK_SET);
    fwrite(&cab.status, sizeof(char), 1, fbin);
    fwrite(&cab.topo, sizeof(int), 1, fbin);
    fwrite(&cab.proxRRN, sizeof(int), 1, fbin);
    fwrite(&cab.nroEstacoes, sizeof(int), 1, fbin);
    fwrite(&cab.nroParesEstacao, sizeof(int), 1, fbin);
}
 
bool lerCabecalhoArvore(FILE *fArvore, ARVOREB_CABECALHO *cab) {
    fseek(fArvore, 0, SEEK_SET);
    fread(&cab->status, sizeof(char), 1, fArvore);
    if(cab->status == '0') {
        return false;
    }
    cab->status = '0';
    fseek(fArvore, 0, SEEK_SET);
    fwrite(&cab->status, sizeof(char), 1, fArvore);
    fseek(fArvore, 1, SEEK_SET);
    fread(&cab->noRaiz, sizeof(int), 1, fArvore);
    fread(&cab->topo, sizeof(int), 1, fArvore);
    fread(&cab->proxRRN, sizeof(int), 1, fArvore);
    fread(&cab->nroNos, sizeof(int), 1, fArvore);
    return true;
}
 
void escreverCabecalhoArvore(FILE *fArvore, ARVOREB_CABECALHO cab) {
    fseek(fArvore, 0, SEEK_SET);
    fwrite(&cab.status, sizeof(char), 1, fArvore);
    fwrite(&cab.noRaiz, sizeof(int), 1, fArvore);
    fwrite(&cab.topo, sizeof(int), 1, fArvore);
    fwrite(&cab.proxRRN, sizeof(int), 1, fArvore);
    fwrite(&cab.nroNos, sizeof(int), 1, fArvore);
}

void executarRemocoes(FILE *fbin, FILE *fArvore, CABECALHO *cabDados, ARVOREB_CABECALHO *cabArvore, int n, char ***matrizes, int *nroLinhas) {
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
            int offset = buscaChave(fArvore, chaveBusca, cabArvore->noRaiz);
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
                        fwrite(&cabDados->topo, sizeof(int), 1, fbin);
                        cabDados->topo = rrn;
                        atualizarRemocaoMatriz(cabDados, matrizes, nroLinhas, registro);
                        removerChaveArvore(fArvore, cabArvore, registro.codEstacao);
                    }
                }
            }
        }
        else {
            removerSequencial(fbin, fArvore, cabDados, cabArvore, quantAnds, condicoes, matrizes, nroLinhas);
        }
        liberarCondicoes(quantAnds, condicoes);
    }
}

void deleteFromWhereArvore(char *arquivoBin, char *arquivoArvore, int n, char ***matrizes, int *nroLinhas) {
    // Processamento padrão de arquivos
    FILE *fbin;
    if(arquivoBin == NULL || !(fbin = fopen(arquivoBin, "r+b"))) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    FILE *fArvore;
    if(arquivoArvore == NULL || !(fArvore = fopen(arquivoArvore, "r+b"))) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin);
        return;
    }
    CABECALHO cabDados;
    ARVOREB_CABECALHO cabArvore;
    if(!lerCabecalhoDados(fbin, &cabDados) || !lerCabecalhoArvore(fArvore, &cabArvore)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin);
        fclose(fArvore);
        return;
    }

    // Garantimos que a matriz esteja atualizada de acordo
    if(*nroLinhas <= 0) {
        populaMatriz(matrizes, nroLinhas, fbin, cabDados.proxRRN);
    }

    executarRemocoes(fbin, fArvore, &cabDados, &cabArvore, n, matrizes, nroLinhas);
    cabDados.status = '1';
    escreverCabecalhoDados(fbin, cabDados);
    cabArvore.status = '1';
    escreverCabecalhoArvore(fArvore, cabArvore);
    fclose(fbin);
    fclose(fArvore);
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
            int offset = buscaChave(fArvore, chaveBusca, noRaiz);
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


 void atualizarRemocaoMatriz(CABECALHO *cabDados, char ***matrizes, int *nroLinhas, REGISTRO registro) {
    char codEstacaoAnt[11], codProxAnt[11];
    if(registro.codEstacao == -1) {
        strcpy(codEstacaoAnt, "");
    } else {
        sprintf(codEstacaoAnt, "%d", registro.codEstacao);
    }
    if(registro.codProxEstacao == -1) {
        strcpy(codProxAnt, "");
    } else {
        sprintf(codProxAnt, "%d", registro.codProxEstacao);
    }
    for(int k = 0; k<*nroLinhas; ++k) {
        if(!strcmp(matrizes[0][k], registro.nomeEstacao) && !strcmp(matrizes[1][k], codEstacaoAnt) && !strcmp(matrizes[2][k], codProxAnt)) {
            strcpy(matrizes[0][k], "");
            strcpy(matrizes[1][k], "");
            strcpy(matrizes[2][k], "");
            break;
        }
    }
    if(!duplicidadeEstacoes(matrizes, nroLinhas, registro.nomeEstacao)) {
        --cabDados->nroEstacoes;
    }
    if(!duplicidadeParesEstacao(matrizes, nroLinhas, codProxAnt, codEstacaoAnt)) {
        --cabDados->nroParesEstacao;
    }
}

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
                fseek(fbin, tamHeader + pos*tamRegistro, SEEK_SET);
                fwrite(&marca, sizeof(char), 1, fbin);
                fwrite(&cabDados->topo, sizeof(int), 1, fbin);
                cabDados->topo = pos;
                atualizarRemocaoMatriz(cabDados, matrizes, nroLinhas, registro);
                if(fArvore != NULL) {
                    removerChaveArvore(fArvore, cabArvore, registro.codEstacao);
                }
            }
        }
        ++pos;
        fseek(fbin, tamHeader + pos*tamRegistro, SEEK_SET);
    }
}