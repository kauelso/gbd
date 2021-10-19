#include "btree.h"

#include <sys/types.h>
#include <sys/stat.h>

bool fileExists(const char *filename) { struct stat statBuf; if (stat(filename,&statBuf) < 0) return false; return true; }

btree::btree() {
    char *nomearquivo = "arvoreb.dat";

    // se arquivo ja existir, abrir e carregar cabecalho
    if (fileExists(nomearquivo)) {
        // abre arquivo
        arquivo = fopen(nomearquivo,"r+");
        leCabecalho();
    }
    // senao, criar novo arquivo e salvar o cabecalho
    else {
        // cria arquivo
        arquivo = fopen(nomearquivo,"w+");

        // atualiza cabecalho
        cabecalhoArvore.numeroElementos = 0;
        cabecalhoArvore.paginaRaiz = -1;
        salvaCabecalho();
    }
}

btree::~btree() {
    // fechar arquivo
    fclose(arquivo);
}

int btree::computarTaxaOcupacao() {
    return 0;
}

int btree::insereRecursao(int chave, int indicePg){
    pagina *pg = lePagina(indicePg);
    //pagina nao cheia
        if(pg->ponteiros[0] == -1){ //Caso o primeiro dado dos ponteiros seja -1, o noh eh folha
            if(pg->numeroElementos == ORDEM){
                //Dividir no e inserir o elemento
                //o elemento do meio
                int elementomeio = pg->chaves[(ORDEM-1)/2];

                // cria��o de 1 nova pagina
                int indice3;
                //pagina *novaPagina1 = novaPagina(indice3);

                //metade das chaves na pagina nova
                for(int i=0;i<=(ORDEM-1)/2;i++){
                    novaPagina1->chaves[i] = pg->chaves[i];
                    novaPagina1->numeroElementos++;
                }

                //removendo todas chaves que foram transferidas
                for(int i=0;i<=(ORDEM-1)/2;i++){
                    for(int j=0;j<=pg->numeroElementos;j++){
                        pg->chaves[j] = pg->chaves[j+1];
                    }
                }
                pg->numeroElementos = (ORDEM-1)/2;
                // fim da divisao


                // insercao na nova
                if(chave < novaPagina1->chave[novaPagina1->numeroElementos-1]){
                    *pg = *novaPagina1;     //trato novapagina1 como pg para facilitar o codigo
                    // igual inserir em folha vazia
                    int i = pg->numeroElementos-1;
                    if(chave > pg->chaves[i]){ // e maior que o ultimo valor da pagina? entao insere no final
                        pg->chaves[i+1] = chave;
                    }
                    else{   //se n�o move todos para frente ate colocar na posicao correta
                        while(pg->chaves[i] > chave){
                            pg->chaves[i] = pg->chaves[i-1];
                            i--;
                        }
                        pg->chaves[i+1] = chave;
                    }
                    pg->numeroElementos+1;
                }
                //insercao na ja existente
                else{
                    // igual inserir em folha vazia
                    int i = pg->numeroElementos-1;
                    if(chave > pg->chaves[i]){ // � maior que o ultimo valor da pagina? ent�o insere no final
                        pg->chaves[i+1] = chave;
                    }
                    else{   //se n�o move todos para frente at� colocar na posi��o correta
                        while(pg->chaves[i] > chave){
                            pg->chaves[i] = pg->chaves[i-1];
                            i--;
                        }
                        pg->chaves[i+1] = chave;
                    }
                    pg->numeroElementos+1;
                }

                





            }
            int i = pg->numeroElementos-1;
            if(chave > pg->chaves[i]){ // e maior que o ultimo valor da pagina? entao insere no final
                pg->chaves[i+1] = chave;
            }
            else{ 
                pg->chaves[i+1] = pg->chaves[i];  //se nao move todos para frente ate colocar na posicao correta
                while(pg->chaves[i] > chave){
                    pg->chaves[i] = pg->chaves[i-1];
                    i--;
                }
                pg->chaves[i+1] = chave;
            }

            pg->numeroElementos = pg->numeroElementos + 1;
            salvaPagina(cabecalhoArvore.paginaRaiz, pg);
            salvaCabecalho();
            return 0;
        }else{
            //Recursao caso precise inserir em uma das folhas
            //Faz um loop para ver em qual folha sera inserido, comparando o valor com a chave
            //Na folha que sera inserido, chama essa funcao passando o numero da pagina da folha
            //Retorna 0 caso seja inserido com sucesso
            //Caso o valor tenha que ser inserido na raiz, retorna 1
        }
}

void btree::insereChave(int chave, int offsetRegistro) {
    // cabecalho contem o numero da pagina raiz
    leCabecalho();
    //if s� tiver a pagina raiz
    if(cabecalhoArvore.paginaRaiz == -1){ //Nao existem paginas na arvore
        int indicePagina;
        pagina *raiz = novaPagina(&indicePagina);

        raiz->chaves[raiz->numeroElementos] = chave;

        cabecalhoArvore.numeroElementos++;
        cabecalhoArvore.paginaRaiz = indicePagina;
        salvaPagina(indicePagina,raiz);
        salvaCabecalho();
        return;
    }
    insereRecursao(chave,cabecalhoArvore.paginaRaiz);
}
void btree::removeChave(int chave) {

    // se remover, atualizar cabecalho
    if (true) {
        cabecalhoArvore.numeroElementos--;
        salvaCabecalho();
    }
}

int btree::buscaChave(int chave) {
    // caso n�o encontrar chave, retornar -1
    return -1;
}

