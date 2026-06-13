#include"arvoreb.h"

#define ordem 4
#define maxchaves ordem-1
#define minchaves ordem/2-1

typedef struct cabecalho{
    char status;
    int noRaiz;
    int topo;
    int proxRRN;
    int nroNos;
}ARVOREB_CABECALHO;

typedef struct pagina{
    int chave[maxchaves]; //codEstacao
    int filho[ordem];
    int registro[maxchaves]; //Referência à posição do registro no arquivo de dados
    char removido;
    int proximo;
    int tipoNo;
    int nroChaves;
}PAGINA;


void criaRaiz(FILE* fbin, int promover, int promoverChave, ARVOREB_CABECALHO *cabecalho){
    PAGINA pagina = inicializaPagina();
    int rrn = cabecalho->noRaiz;
    int filho, tipoNo;
    pagina.tipoNo = 0;//Nó raiz
    pagina.chave[0] = promoverChave;
    pagina.filho[0] = cabecalho->noRaiz;
    pagina.filho[1] = promover;
    pagina.nroChaves++;
    cabecalho->noRaiz = cabecalho->proxRRN;
    fseek(fbin, 54+53*rrn, SEEK_SET);
    fread(&filho, sizeof(int), 1, fbin);
    fseek(fbin, -36, SEEK_CUR);
    if(filho != -1){
        tipoNo = 1;
    }
    else tipoNo = -1;
    //Atualiza o tipo no do antigo nó raiz no arquivo
    fwrite(&tipoNo, sizeof(int), 1, fbin);
    escreverNO(cabecalho->proxRRN, pagina);
    cabecalho->proxRRN++;
}


void createIndex(char* arq, char* arvore_arq){
    FILE *fbin;
    if (bin == NULL || !(fbin = fopen(bin, "rb"))) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    FILE *fbin_arvore;
    if (csv == NULL || !(fbin_arvore = fopen(arvore_arq, "wb"))) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fbin);
        return;
    }
    ARVOREB_CABECALHO cabecalho = {'1', -1, -1, 0, 0};
    fseek(fbin, 0, SEEK_SET);
    char status, removido;
    fread(&status, sizeof(char), 1, fbin);
    if(status == '0'){
        printf("Erro no processamento do arquivo!");
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
        int chave, promover, promoverChave;
        //Chave tem o valor de codEstacao do registro
        fread(&chave, sizeof(int), 1, fbin);
        //Insere na árvore B
        if(insert(fbin_arvore, rrn, cabecalho.noRaiz, chave, &promover, &promoverChave)){
            criaRaiz(fbin_arvore, promover, promoverChave, &cabecalho);
        }
        //Passa para o próximo registro
        fseek(fbin, 71, SEEK_CUR);
    }
    fclose(fbin_arvore);
    fclose(fbin);
    BinarioNaTela(arvore_arq);
}


int busca(FILE *fbin, int chave, int RRN){
    if(RRN == -1){
        printf("Registro não encontrado!");
        return -1;
    }
    if(fbin == NULL){
        printf("Erro no processamento do arquivo!");
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


PAGINA ins_in_page(int chave, int filho, PAGINA pagina){
    for(int i = pagina.nroChaves-1; chave < pagina.chave[i-1] && i>0; i--){
        pagina.chave[i] = pagina.chave[i-1];
        pagina.filho[i+1] = pagina.filho[i];
        pagina.nroChaves++;
        pagina.chave[i] = chave;
        pagina.filho[i+1] = filho;
    }
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
}

PAGINA inicializaPagina(PAGINA pagina){
    for(int j = 0; j<maxchaves; j++){
        pagina.chave[j] = -1;
        pagina.registro[j] = -1;
        pagina.filho[j] = -1;
    }
    pagina.filho[maxchaves] = -1;
    pagina.proximo = -1;
    pagina.tipoNo = -1;
    pagina.removido = '0';
    return pagina;
}

PAGINA split(int chave, int filho, PAGINA pagina, int *promover_chave, PAGINA novaPagina, int *promover, ARVOREB_CABECALHO cabecalho){
    int chavesTrabalho[ordem];
    int filhosTrabalho[ordem+1];
    
    for(int i = 0; i < maxchaves; i++){
        chavesTrabalho[i] = pagina.chave[i];
        filhosTrabalho[i] = pagina.filho[i];
    }
    filhosTrabalho[i] = pagina.filho[i];
    for(int i = maxchaves; chave < chavesTrabalho[i-1] && i > 0; i--){
        chavesTrabalho[i] = chavesTrabalho[i-1];
        filhosTrabalho[i+1] = filhosTrabalho[i];
    }
    chavesTrabalho[i] = chave;
    filhosTrabalho[i+1] = filho;
    *promover = cabecalho.proxRRN;
    novaPagina = inicializaPagina(novaPagina);
    for(int i = 0; i < maxchaves-minchaves; i++){
        pagina.chave[i] = chavesTrabalho[i];
        pagina.filho[i] = filhosTrabalho[i];
        if(i < minchaves){
            novaPagina.chave[i] = chavesTrabalho[i + 1 + majorCount];
            novaPagina.filho[i] = filhosTrabalho[i + 1 + majorCount];
        }
        pagina.chave[i+minchaves] = -1;
        pagina.filho[i+1+minchaves] = -1;
    }
    pagina.filho[minchaves] = filhosTrabalho[minchaves];
    novaPagina.filho[minchaves] = filhosTrabalho[maxchaves+1];
    novaPagina.nroChaves = minchaves;
    pagina.nroChaves = maxchaves-minchaves;
    *promover_chave = chavesTrabalho[maxchaves-minchaves];
    return novaPagina;
}

bool insert(FILE* fbin, int registro, int RRN, int chave, int *promover, int *promover_chave, ARVOREB_CABECALHO *cabecalho){
    if(fbin == NULL){
        printf("Erro no processamento do arquivo!");
        return false;
    }
    if(cabecalho->status == '0'){
        printf("Erro no processamento do arquivo!");
        return false;
    }

    PAGINA pagina, novaPagina;
    bool encontrado, promovido;
    int posicao, filho, chave_abaixo;
    if(RRN == -1){
        *promover_chave = chave;
        *promover = -1;
        return true;
    }

    pagina = localizaNo(fbin, RRN, chave, &posicao, &encontrado);

    if(encontrado){
        printf("Chave já cadastrada!");
        return false;
    }

    promovido = insert(fbin, registro, pagina.filho[posicao], chave, &filho, &chave_abaixo, cabecalho);
    if(!promovido){
        return false;
    }
    if(pagina.nroChaves < maxchaves){
        pagina = ins_in_page(chave_abaixo, filho, pagina);
        escreverNO(fbin, RRN, pagina);
        return false;
    }
    else{
        novaPagina = split(chave_abaixo, filho, pagina, promover_chave, novaPagina, promover, *cabecalho);
        escreverNO(fbin, RRN, pagina);
        escreverNO(fbin, *promover, novaPagina);
        cabecalho->proxRRN++;
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
 
void liberaPagina(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int rrn) {
    char marca = '1';
    int prox = cabecalho->topo;
    fseek(fArvore, 17+53*rrn, SEEK_SET);
    fwrite(&marca, sizeof(char), 1, fArvore);
    fwrite(&prox, sizeof(int), 1, fArvore);
    cabecalho->topo = rrn;
    --cabecalho->nroNos;
}
 
void buscaSucessor(FILE *fArvore, int rrn, int *chave, int *p) {
    PAGINA pagina = lerNO(fArvore, rrn);
    while(!isFolha(pagina)) {
        rrn = pagina.filho[0];
        pagina = lerNO(fArvore, rrn);
    }
    *chave = pagina.chave[0];
    *p = pagina.registro[0];
}
 
void removeChaveFolha(PAGINA *pagina, int posicao) {
    for(int i = posicao; i<pagina->nroChaves-1; ++i) {
        pagina->chave[i] = pagina->chave[i+1];
        pagina->registro[i] = pagina->registro[i+1];
    }
    --pagina->nroChaves;
    pagina->chave[pagina->nroChaves] = -1;
    pagina->registro[pagina->nroChaves] = -1;
}
 
void redistribui(FILE *fArvore, int rrnPai, int separacao) {
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
 
void concatena(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int rrnPai, int separacao) {
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
    liberaPagina(fArvore, cabecalho, rrnDireita);
 
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
            redistribui(fArvore, rrnPai, posFilho);
            return;
        }
    }
    if(posFilho>0) {
        PAGINA esquerda = lerNO(fArvore, pai.filho[posFilho-1]);
        if(esquerda.nroChaves>minchaves) {
            redistribui(fArvore, rrnPai, posFilho-1);
            return;
        }
    }
    if(posFilho>0) {
        concatena(fArvore, cabecalho, rrnPai, posFilho-1);
        return;
    }
    
    concatena(fArvore, cabecalho, rrnPai, posFilho);
}
 
 
bool removeChaveArvoreInterno(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int rrn, int chave) {
    int posicao;
    bool achou;
    PAGINA pagina = localizaNo(fArvore, rrn, chave, &posicao, &achou);
 
    if(isFolha(pagina)) {
        if(!achou) {
            return false;
        }
        removeChaveFolha(&pagina, posicao);
        escreverNO(fArvore, rrn, pagina);
        if(pagina.nroChaves<minchaves) {
            return true;
        }
        return false;
    }
 
    if(achou) {
        int chave2,posicao2;
        buscaSucessor(fArvore, pagina.filho[posicao+1], &chave2, &posicao2);
        pagina.chave[posicao] = chave2;
        pagina.registro[posicao] = posicao2;
        escreverNO(fArvore, rrn, pagina);
        if(removeChaveArvoreInterno(fArvore, cabecalho, pagina.filho[posicao+1], chave2)) {
            trataUnderflow(fArvore, cabecalho, rrn, posicao+1);
        }
    } else {
        if(removeChaveArvoreInterno(fArvore, cabecalho, pagina.filho[posicao], chave)) {
            trataUnderflow(fArvore, cabecalho, rrn, posicao);
        }
    }
 
    pagina = lerNO(fArvore, rrn);
    if(pagina.nroChaves<minchaves) {
        return true;
    }
    return false;
}
 
void removeChaveArvore(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int chave) {
    if(cabecalho->noRaiz == -1) {
        return;
    }
 
    removeChaveArvoreInterno(fArvore, cabecalho, cabecalho->noRaiz, chave);
 
    PAGINA raiz = lerNO(fArvore, cabecalho->noRaiz);
    if(raiz.nroChaves == 0) {
        if(!isFolha(raiz)) {
            int antigaRaiz = cabecalho->noRaiz;
            cabecalho->noRaiz = raiz.filho[0];
            liberaPagina(fArvore, cabecalho, antigaRaiz);
            PAGINA novaRaiz = lerNO(fArvore, cabecalho->noRaiz);
            if(isFolha(novaRaiz)) {
                novaRaiz.tipoNo = -1;
            } else {
                novaRaiz.tipoNo = 0;
            }
            escreverNO(fArvore, cabecalho->noRaiz, novaRaiz);
        } else {
            liberaPagina(fArvore, cabecalho, cabecalho->noRaiz);
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
 
void executaRemocoes(FILE *fbin, FILE *fArvore, CABECALHO *cabDados, ARVOREB_CABECALHO *cabArvore, int n) {
    for(int i = 0; i<n; ++i) {
        int quantAnds;
        scanf("%d", &quantAnds);
        char *condicoes[quantAnds][2];
        lerCondicoesBusca(quantAnds, condicoes);
        bool temChave = false;
        int chaveBusca = -1;
        for(int j = 0; j<quantAnds; ++j) {
            if((int)(intptr_t)condicoes[j][0] == 2) {
                temChave = true;
                chaveBusca = atoi(condicoes[j][1]);
            }
        }
        if(temChave) {
            int posicao = buscaChave(fArvore, chaveBusca, cabArvore->noRaiz);
            if(posicao != -1) {
                fseek(fbin, tamHeader + posicao*tamRegistro, SEEK_SET);
                char removido;
                fread(&removido, sizeof(char), 1, fbin);
                if(removido != '1') {
                    REGISTRO registro;
                    registro.removido = '0';
                    bool ok = lerRegistroVerifica(fbin, &registro,quantAnds, condicoes);
                    if(ok) {
                        char marca = '1';
                        fseek(fbin, tamHeader + posicao * tamRegistro, SEEK_SET);
                        fwrite(&marca, sizeof(char), 1, fbin);
                        fwrite(&cabDados->topo, sizeof(int), 1, fbin);
                        cabDados->topo = posicao;
                        --cabDados->nroEstacoes;
                        if(registro.codProxEstacao != -1) {
                            --cabDados->nroParesEstacao;
                        }
                        removeChaveArvore(fArvore, cabArvore, registro.codEstacao);
                    }
                }
            }
        } else {
            int posicao = 0;
            fseek(fbin, tamHeader, SEEK_SET);
            char removido;
            while(fread(&removido, sizeof(char), 1, fbin) == 1) {
                if(removido == '1') {
                    fseek(fbin, tamRegistro - 1, SEEK_CUR);
                    ++posicao;
                    continue;
                }
                REGISTRO registro;
                registro.removido = '0';
                bool ok = lerRegistroVerifica(fbin, &registro, quantAnds, condicoes);
                if(ok) {
                    char marca = '1';
                    fseek(fbin, tamHeader + posicao*tamRegistro, SEEK_SET);
                    fwrite(&marca, sizeof(char), 1, fbin);
                    fwrite(&cabDados->topo, sizeof(int), 1, fbin);
                    cabDados->topo = posicao;
                    --cabDados->nroEstacoes;
                    if(registro.codProxEstacao != -1) {
                        --cabDados->nroParesEstacao;
                    }
                    removeChaveArvore(fArvore, cabArvore, registro.codEstacao);
                    fseek(fbin, tamHeader + (posicao+1) * tamRegistro, SEEK_SET);
                }
                ++posicao;
            }
        }
        liberarCondicoes(quantAnds, condicoes);
    }
}

void delete(char *arquivoBin, char *arquivoArvore, int n) {
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
    executaRemocoes(fbin, fArvore, &cabDados, &cabArvore, n);
    cabDados.status = '1';
    escreverCabecalhoDados(fbin, cabDados);
    cabArvore.status = '1';
    escreverCabecalhoArvore(fArvore, cabArvore);
    fclose(fbin);
    fclose(fArvore);
    BinarioNaTela(arquivoBin);
    BinarioNaTela(arquivoArvore);
}