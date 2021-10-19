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
    if(pg->ponteiros[0] == -1){
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
    else{
        pagina *pg = lePagina(cabecalhoArvore.paginaRaiz);
        if(pg->numeroElementos < ORDEM )
            insereRecursao(chave,cabecalhoArvore.paginaRaiz);
        else{
            //o elemento do meio
            int elemento1fica = pg->chaves[(ORDEM-1)/2];
            //o ultimo elemento
            int elemento2fica = pg->chaves[ORDEM-1];

            // criacao das 2 novas paginas
            int indice1;
            pagina *novaPagina1 = novaPagina(&indice1);
            int indice2;
            pagina *novaPagina2 = novaPagina(&indice2);

            //insere comeco das chaves ate a metade em uma nova pagina 1
            for(int i=0; i <= (ORDEM-1)/2;i++){
                novaPagina1->chaves[i] = pg->chaves[i];
                novaPagina1->numeroElementos = novaPagina1->numeroElementos + 1;
            }
            //insere metade das chaves ate a ultima em uma nova pagina 2
            for(int i=(ORDEM+1)/2; i < ORDEM-1;i++){
                novaPagina2->chaves[i-elemento1fica] = pg->chaves[i];
                novaPagina2->numeroElementos = novaPagina2->numeroElementos + 1;
            }

            //pagina raiz ficaria assim
            pg->chaves[0] = elemento1fica;
            pg->ponteiros[0] = indice1;
            //pg.ponteiros[0] = novaPagina1;
            pg->chaves[1] = elemento2fica;
            pg->ponteiros[1] = indice2;
            //pg.ponteiros[1] = novaPagina2;
            pg->numeroElementos = 2;

            // altura da arvore aumentada
            cabecalhoArvore.alturaArvore++;
        }
        
    }
    //ja existem paginas na arvore
    
    
    // caso a pagina esteja cheia
    // se (cabecalhoArvore.paginaRaiz == 1) entao raiz eh a unica pagina. Ler raiz, inserir e salvar. Senao...
            // Exemplo:
            // pagina *pg = lePagina(cabecalhoArvore.paginaRaiz);
            // adicionar <chave,offsetRegistro> na pagina pg
            // salvar pagina: salvaPagina(pg->numeroPagina, pg);

    // senao...

    // ler pagina raiz: pagina *pg = lePagina(cabecalhoArvore.paginaRaiz);

    // se inserir, atualizar cabecalho
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

