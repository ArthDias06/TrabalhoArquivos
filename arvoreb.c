#include"arvoreb.h"
#include"matriz.h"
// Esse arquivo contém as funções internas das operações do CRUD aplicadas na árvore B

//Criação de nova raiz quando necessário
void criaRaiz(FILE* fbin, int promover, int promoverChave, int promoverRegistro, ARVOREB_CABECALHO *cabecalho){
    PAGINA pagina = inicializaPagina();
    int rrn = cabecalho->noRaiz;
    int filho, tipoNo;
    //Escreve os valores promovidos para raiz na nova página
    pagina.chave[0] = promoverChave;
    pagina.registro[0] = promoverRegistro;
    //O nó esquerdo é a antiga raiz
    pagina.filho[0] = cabecalho->noRaiz;
    pagina.filho[1] = promover;
    //Verifica se a página deve ser tratada como uma raiz ou folha(só no primeiro nó criado)
    if(pagina.filho[0] == -1){
        pagina.tipoNo = -1;//Nó folha
    } else {
        pagina.tipoNo = 0;//Nó raiz
    }
    pagina.nroChaves++;
    //A raiz é alocada no próximo espeaço livre da árvore
    cabecalho->noRaiz = cabecalho->proxRRN;
    //Muda o tipo nó da antiga raiz, caso ela exista
    if(rrn != -1){
        //Vai para a posição filho[0] da antiga raiz
        fseek(fbin, 54+53*rrn, SEEK_SET);
        fread(&filho, sizeof(int), 1, fbin);
        //Volta para a posição do tipo nó
        fseek(fbin, -36, SEEK_CUR);
        //Se o filho não existir(-1) a antiga raiz é folha, caso contrário é intermediária
        if(filho != -1){
            tipoNo = 1;
        }
        else tipoNo = -1;
        //Atualiza o tipo no do antigo nó raiz no arquivo
        fwrite(&tipoNo, sizeof(int), 1, fbin);
    }
    //Atualiza o cabeçalho no arquivo
    escreverNO(fbin, cabecalho->proxRRN, pagina);
    cabecalho->proxRRN++;
    cabecalho->nroNos++;
}

//Função para procurar uma dada chave em uma paǵina e preencher uma página
PAGINA localizaNo(FILE *fbin, int RRN, int chave, int *posicao, bool *achou) {
    //Pega os valores da paǵina
    PAGINA pagina = lerNO(fbin, RRN);
    int i = 0;
    //Valor de i aumenta até encontrar a posição que a chave deveria ocupar
    //se estive rno registro
    while(i<pagina.nroChaves && chave>pagina.chave[i]) {
        ++i;
    }
    *posicao = i;
    //Analisa se a chave existe na página
    if(i<pagina.nroChaves && chave == pagina.chave[i]) {
        *achou = true;
    } else {
        *achou = false;
    }
    return pagina;
}

int buscaChave(FILE *fbin, int chave, int RRN) {
    //Se o RRN for -1, não foi encontrado
    if(RRN == -1) {
        return -1;
    }
    int posicao;
    bool achou;
    //Procura uma chave em uma dada paǵina
    PAGINA pagina = localizaNo(fbin, RRN, chave, &posicao, &achou);
    //Caso tenha sucesso na busca, retorna a posição, caso contrário,
    //executa recursivamente a função
    if(achou) {
        return pagina.registro[posicao];
    }
    return buscaChave(fbin, chave, pagina.filho[posicao]);
}

//Função para inserir na página
PAGINA ins_in_page(int chave, int registro, int filho, PAGINA pagina){
    int i;
    //Shift das chaves já existentes até vagar a posição correta
    //Da nova chave
    for(i = pagina.nroChaves-1; i>=0 && chave < pagina.chave[i]; i--){
        pagina.chave[i+1] = pagina.chave[i];
        pagina.registro[i+1] = pagina.registro[i];
        pagina.filho[i+2] = pagina.filho[i+1];
    }
    //Alocação dos novos valores na página
    pagina.nroChaves++;
    pagina.chave[i+1] = chave;
    pagina.registro[i+1] = registro;
    pagina.filho[i+2] = filho;
    //Retorno da página atualizada
    return pagina;
}

void escreverNO(FILE *fArvore, int rrn, PAGINA pagina) {
    //Vai até um dado nó
    fseek(fArvore, 17+53*rrn, SEEK_SET);
    //Escreve cada campo na nova página
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
    //Inicializa uma nova página com seus valores-base
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
    //Retorna a nova paǵina
    return pagina;
}

//Função de split do insert
PAGINA split(FILE *fbin, int chave, int filho,int registro,  PAGINA *pagina, int *promover_chave, PAGINA novaPagina, int *promover, int *promoverRegistro, ARVOREB_CABECALHO *cabecalho){
    //Vetores para guardar as chaves de uma mesma página e os novos valores a serem inseridos
    int chavesTrabalho[ordem];
    int filhosTrabalho[ordem+1];
    int registroTrabalho[ordem];
    int i;
    //Preeche os vetores de trabalho
    for(i = 0; i < maxchaves; i++){
        chavesTrabalho[i] = pagina->chave[i];
        filhosTrabalho[i] = pagina->filho[i];
        registroTrabalho[i] = pagina->registro[i];
    }
    filhosTrabalho[i] = pagina->filho[i];
    //Aloca os novos valores de forma ordenada
    for(i = maxchaves; i > 0 && chave < chavesTrabalho[i-1]; i--){
        chavesTrabalho[i] = chavesTrabalho[i-1];
        filhosTrabalho[i+1] = filhosTrabalho[i];
        registroTrabalho[i] = registroTrabalho[i-1];
    }
    chavesTrabalho[i] = chave;
    filhosTrabalho[i+1] = filho;
    registroTrabalho[i] = registro;
    //Verifica se há algum registro removido
    if(cabecalho->topo == -1){
        *promover = cabecalho->proxRRN++;
    }else{
        //Se há removidos, processo de desempilhar e uso do
        //espaço do nó removido antigamente
        int prox;
        *promover = cabecalho->topo;
        fseek(fbin, 18+cabecalho->topo*53, SEEK_SET);
        fread(&prox, sizeof(int), 1, fbin);
        cabecalho->topo = prox;
    }
    //Inicializa uma nova página
    novaPagina = inicializaPagina();
    //A nova página vai ter o tipo do nó da página já existente
    //Ocupam o mesmo nível da árvore
    novaPagina.tipoNo = (pagina->tipoNo == -1) ? -1 : 1;
    for(i = 0; i < minchaves; i++){
        //Aloca as primeiras chaves na página antiga
        pagina->chave[i] = chavesTrabalho[i];
        pagina->filho[i] = filhosTrabalho[i];
        pagina->registro[i] = registroTrabalho[i];
        //Alocação das últimas chaves na nova página
        novaPagina.chave[i] = chavesTrabalho[i + 2 + minchaves];
        novaPagina.filho[i] = filhosTrabalho[i + 2 + minchaves];
        novaPagina.registro[i] = registroTrabalho[i + 2 + minchaves];
        //Os últimos valores da antiga chave são apagados
        pagina->chave[i + minchaves + 1]  = -1;
        pagina->filho[i + minchaves + 2]  = -1;
        pagina->registro[i + minchaves + 1] = -1;
    }
    //Escreve os valores na poosição restante da antiga chave
    //pois ela tem uma chave a mais
    pagina->chave[i] = chavesTrabalho[i];
    pagina->filho[i] = filhosTrabalho[i];
    pagina->registro[i] = registroTrabalho[i];
    pagina->filho[minchaves + 1] = filhosTrabalho[minchaves + 1];
    novaPagina.filho[minchaves] = filhosTrabalho[i + minchaves + 2];
    //Define o número de chaves de cada página
    pagina->nroChaves = maxchaves - minchaves;
    novaPagina.nroChaves = minchaves;
    //Define os valores de promoção de chave e de registro
    *promover_chave = chavesTrabalho[minchaves + 1];
    *promoverRegistro = registroTrabalho[minchaves + 1];
    return novaPagina;
}

//Função do insert
bool insert(FILE* fbin, int registro, int RRN, int chave, int *promover, int *promover_chave, int *promoverRegistro, ARVOREB_CABECALHO *cabecalho, bool *flag){
    //Verificação da integridade do ponteiro
    if(fbin == NULL){
        printf("Erro no processamento do arquivo!");
        return false;
    }

    PAGINA pagina, novaPagina;
    bool encontrado, promovido;
    int posicao, filho, chave_abaixo, registroAbaixo;
    //Se foi tentado acessar um filho inexistente, preenche as variáveis de promoção
    //com os valores-base e retorna para a função chamadora
    if(RRN == -1){
        *promover_chave = chave;
        *promoverRegistro = registro;
        *promover = -1;
        //retorna ture, pois ppode precisar de promoção
        return true;
    }
    //Vê se uma dada chave está em uma dada página
    pagina = localizaNo(fbin, RRN, chave, &posicao, &encontrado);

    //Caso tenha sido encontrada, retorna pois não podemos ter duas chaves idênticas
    if(encontrado){
        *flag = true;
        return false;
    }
    //Caso não tenha sido encontrada mantém a busca pelo nó em que a chave será inserida
    promovido = insert(fbin, registro, pagina.filho[posicao], chave, &filho, &chave_abaixo, &registroAbaixo, cabecalho, flag);
    //Se ao final não houver necessidade promoção, retorna false
    if(!promovido){
        return false;
    }
    //Se uma página ainda tiver espaço, insere nela a chave
    if(pagina.nroChaves < maxchaves){
        pagina = ins_in_page(chave_abaixo, registroAbaixo, filho, pagina);
        escreverNO(fbin, RRN, pagina);
        return false;
    }
    //Caso contrário, realiza o split
    else{
        novaPagina = split(fbin, chave_abaixo, filho, registroAbaixo, &pagina, promover_chave, novaPagina, promover, promoverRegistro, cabecalho);
        //Nesse caso o número de nós é alterado
        cabecalho->nroNos++;
        //Atualiza os nós da nova e antiga páginas
        escreverNO(fbin, RRN, pagina);
        escreverNO(fbin, *promover, novaPagina);
        //Retorna true, pois pode precisar de promoção
        return true;
    }
}


PAGINA lerNO(FILE *fArvore, int rrn) {
    PAGINA pagina;
    //Leitura de cada campo de uma dada página
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
 
//Verificação se um dado nó é folha
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
 
// Função que redistribui chaves entre dois nós irmãos
void redistribuir(FILE *fArvore, int rrnPai, int separacao) {
    PAGINA pai = lerNO(fArvore, rrnPai);
    int rrnEsquerda = pai.filho[separacao], rrnDireita = pai.filho[separacao+1];
    PAGINA esquerda = lerNO(fArvore, rrnEsquerda), direita = lerNO(fArvore, rrnDireita);

    // Colocamos chaves, posições e filhos dos 2 nós e do separador em vetores
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
 
    // A ideia é a chave do meio subir para o pai como um novo separador; as demais são divididas igualmente
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
 
// Função que concatena dois nós irmãos (usado quando nenhum dos irmãos tem chaves pra redistribuir)
void concatenar(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int rrnPai, int separacao) {
    PAGINA pai = lerNO(fArvore, rrnPai);
    int rrnEsquerda = pai.filho[separacao], rrnDireita = pai.filho[separacao+1];
    PAGINA esquerda = lerNO(fArvore, rrnEsquerda), direita = lerNO(fArvore, rrnDireita);

    int posicaoAtual = esquerda.nroChaves;

    // Descemos o separador do pai para o fim do nó esquerdo
    esquerda.chave[posicaoAtual] = pai.chave[separacao];
    esquerda.registro[posicaoAtual] = pai.registro[separacao];
    esquerda.filho[posicaoAtual+1] = direita.filho[0];
    ++esquerda.nroChaves;
    ++posicaoAtual;

    // Copiamos as chaves do nó direito pro esquerdo
    for(int i = 0; i<direita.nroChaves; ++i) {
        esquerda.chave[posicaoAtual] = direita.chave[i];
        esquerda.registro[posicaoAtual] = direita.registro[i];
        esquerda.filho[posicaoAtual+1] = direita.filho[i+1];
        ++esquerda.nroChaves;
        ++posicaoAtual;
    }

    escreverNO(fArvore, rrnEsquerda, esquerda);
    // Como o nó direito foi pro esquerdo, liberamos o direito
    liberarPagina(fArvore, cabecalho, rrnDireita);

    // Removemos o separador do pai e deslocamos as chaves e filhos que estão à direita dele
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
 
// Tratamos o underflow de um nó filho após uma remoção (nroChaves < minchaves)
void trataUnderflow(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int rrnPai, int posFilho) {
    PAGINA pai = lerNO(fArvore, rrnPai);

    // Verificamos se o irmão direito tem chaves para "emprestar"
    if(posFilho<pai.nroChaves) {
        PAGINA direita = lerNO(fArvore, pai.filho[posFilho+1]);
        if(direita.nroChaves>minchaves) {
            redistribuir(fArvore, rrnPai, posFilho);
            return;
        }
    }
    // Se o direito não tinha, verificamos se o esquerdo tem chaves para "emprestar"
    if(posFilho>0) {
        PAGINA esquerda = lerNO(fArvore, pai.filho[posFilho-1]);
        if(esquerda.nroChaves>minchaves) {
            redistribuir(fArvore, rrnPai, posFilho-1);
            return;
        }
    }
    // Se nenhum deles tinha, concatenamos com o irmão esquerdo se ele existir; do contrário, concatenamos com o direito
    if(posFilho>0) {
        concatenar(fArvore, cabecalho, rrnPai, posFilho-1);
        return;
    }
    concatenar(fArvore, cabecalho, rrnPai, posFilho);
}
 
 
// Removemos chave na árvore B a partir do nó de RRN dado de maneira recursiva
bool removerChaveArvoreInterno(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int rrn, int chave) {
    int posicao;
    bool achou;
    PAGINA pagina = localizaNo(fArvore, rrn, chave, &posicao, &achou);

    // Já removemos se a chave estiver no nó folha
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
        // Se encontramos a chave num nó interno, substituímos, em ordem, pelo sucessor
        int chave2,posicao2;
        buscarSucessor(fArvore, pagina.filho[posicao+1], &chave2, &posicao2);
        pagina.chave[posicao] = chave2;
        pagina.registro[posicao] = posicao2;
        escreverNO(fArvore, rrn, pagina);
        // Removemos o sucessor da subtárvore direita e tratamos underflow caso assim seja necessário
        if(removerChaveArvoreInterno(fArvore, cabecalho, pagina.filho[posicao+1], chave2)) {
            trataUnderflow(fArvore, cabecalho, rrn, posicao+1);
        }
    } else {
        // Se a chave não está no nó, descemos para o filho correto e tratamos underflow caso necessário
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
 
// Função chamada pelo deleteFromWhere que cuida da remoção das chaves
void removerChaveArvore(FILE *fArvore, ARVOREB_CABECALHO *cabecalho, int chave) {
    if(cabecalho->noRaiz == -1) {
        return;
    }

    removerChaveArvoreInterno(fArvore, cabecalho, cabecalho->noRaiz, chave);

    // Transformamos o filho de uma raiz sem chaves após remoção na nova raiz ou liberamos se a árvore ficou vazia
    PAGINA raiz = lerNO(fArvore, cabecalho->noRaiz);
    if(raiz.nroChaves == 0) {
        if(!isFolha(raiz)) {
            int antigaRaiz = cabecalho->noRaiz;
            cabecalho->noRaiz = raiz.filho[0];
            liberarPagina(fArvore, cabecalho, antigaRaiz);
            PAGINA novaRaiz = lerNO(fArvore, cabecalho->noRaiz);
            // Atualizamos o tipoNo da nova raiz de acordo com a estrutura
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
bool lerCabecalhoArvore(FILE *fArvore, ARVOREB_CABECALHO *cabecalho) {
    fseek(fArvore, 0, SEEK_SET);
    fread(&cabecalho->status, sizeof(char), 1, fArvore);
    if(cabecalho->status == '0') {
        return false;
    }
    cabecalho->status = '0';
    fseek(fArvore, 0, SEEK_SET);
    fwrite(&cabecalho->status, sizeof(char), 1, fArvore);
    fseek(fArvore, 1, SEEK_SET);
    fread(&cabecalho->noRaiz, sizeof(int), 1, fArvore);
    fread(&cabecalho->topo, sizeof(int), 1, fArvore);
    fread(&cabecalho->proxRRN, sizeof(int), 1, fArvore);
    fread(&cabecalho->nroNos, sizeof(int), 1, fArvore);
    return true;
}
 
void escreverCabecalhoArvore(FILE *fArvore, ARVOREB_CABECALHO cabecalho) {
    fseek(fArvore, 0, SEEK_SET);
    fwrite(&cabecalho.status, sizeof(char), 1, fArvore);
    fwrite(&cabecalho.noRaiz, sizeof(int), 1, fArvore);
    fwrite(&cabecalho.topo, sizeof(int), 1, fArvore);
    fwrite(&cabecalho.proxRRN, sizeof(int), 1, fArvore);
    fwrite(&cabecalho.nroNos, sizeof(int), 1, fArvore);
}
