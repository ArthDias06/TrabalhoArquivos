//Arthur de Castro Dias - 16855302
//Gabriel Carraro Salzedas - 16827905

#include "crud.h"

int main(){
    int operacao, nroLinhas = 0, n;
    char arquivoBin[101];
    char arquivoCSV[101];
    char arquivoArvore[101];
    char*** matrizes = criaMatriz();
    bool erro;
    while(scanf("%d", &operacao) == 1){
        switch(operacao){
            // Caso 1: Comando CREATE TABLE
            case 1:
                scanf("%100s %100s", arquivoCSV, arquivoBin);
                if(createTable(arquivoCSV, arquivoBin, matrizes, &nroLinhas))
                    BinarioNaTela(arquivoBin);
                break;
            // Caso 2: Comando SELECT com cláusula FROM e sem cláusula WHERE
            case 2:
                scanf("%100s",arquivoBin);
                selectFromWhere(arquivoBin,1,false);
                break;
            // Caso 3: Comando SELECT com cláusulas FROM e WHERE
            case 3:
                int n0;
                scanf("%100s %d",arquivoBin,&n0);
                selectFromWhere(arquivoBin,n0,true);
                break;
            // Caso 4: Comando DELETE com cláusulas FROM e WHERE
            case 4:
                int n1;
                scanf("%100s %d",arquivoBin,&n1);
                erro = deleteFromWhere(arquivoBin,n1,matrizes,&nroLinhas);
                if(!erro) {
                    BinarioNaTela(arquivoBin);
                }
                break;
            // Caso 5: Comando INSERT INTO
            case 5:
                scanf("%100s %d", arquivoBin, &n);
                erro = insertInto(arquivoBin, n, matrizes, &nroLinhas, false, NULL);
                if(!erro) {
                    BinarioNaTela(arquivoBin);
                }
                break;
            // Caso 6: Comando UPDATE com cláusula WHERE
            case 6:
                scanf("%100s %d", arquivoBin, &n);
                update(arquivoBin, n, matrizes, &nroLinhas);
                BinarioNaTela(arquivoBin);
                break;
            case 7:
                scanf("%100s %100s", arquivoBin, arquivoArvore);
                createIndex(arquivoBin, arquivoArvore);
                BinarioNaTela(arquivoArvore);
                break;
            case 8:
                scanf("%100s %100s %d", arquivoBin, arquivoArvore, &n);
                selectFromWhereArvore(arquivoBin, arquivoArvore, n);
                break;
            case 9:
                scanf("%100s %100s %d", arquivoBin, arquivoArvore, &n);
                erro = insertInto(arquivoBin, n, matrizes, &nroLinhas, true, arquivoArvore);
                if(!erro) {
                    BinarioNaTela(arquivoBin);
                    BinarioNaTela(arquivoArvore);
                }
                break;
            case 10:
                scanf("%100s %100s %d", arquivoBin, arquivoArvore, &n);
                deleteFromWhereArvore(arquivoBin, arquivoArvore, n, matrizes, &nroLinhas);
                BinarioNaTela(arquivoBin);
                BinarioNaTela(arquivoArvore);
                break;
        }
    }
    //Cálculo de quanta memória foi criada para armazenar a matriz
    deletaMatriz(&matrizes, nroLinhas);
}
