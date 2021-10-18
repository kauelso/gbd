#include "btree.h"

#include <sys/types.h>
#include <sys/stat.h>

bool fileExists(const char *filename) { struct stat statBuf; if (stat(filename,&statBuf) < 0) return false; return S_ISREG(statBuf.st_mode); }

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

void btree::insereChave(int chave, int offsetRegistro) {
    // cabecalho contem o numero da pagina raiz
    leCabecalho();
    //if só tiver a pagina raiz
    if(cabecalhoArvore.paginaRaiz == 1){
        pagina *pg = lePagina(cabecalhoArvore.paginaRaiz);
        //pagina não cheia
        if(pg.numeroElementos < ORDEM){
            int i = pg.numeroElementos-1;
            if(chave > pg.chaves[i]){ // é maior que o ultimo valor da pagina? então insere no final
                pg.chaves[i+1] = chave;
            }
            else{   //se não move todos para frente até colocar na posição correta
                while(pg.chaves[i] > chave){
                    pg.chaves[i] = pg.chaves[i-1];
                    i--;
                }
                pg.chaves[i+1] = chave;
            }
        }
        // caso a pagina esteja cheia
        else{
            //o elemento do meio
            int elemento1fica = pg.chaves[(ORDEM-1)/2];
            //o ultimo elemento
            int elemento2fica = pg.chaves[ORDEM-1];

            // criação das 2 novas paginas
            cabecalhoArvore.numeroPaginas++;
            pagina *novaPagina1(cabecalhoArvore.numeroPaginas);  // saber o que deve colocar no numero da pagina
            cabecalhoArvore.numeroPaginas++;
            pagina *novaPagina2(cabecalhoArvore.numeroPaginas);

            //insere começo das chaves até a metade em uma nova pagina 1
            for(int i=0; i <= (ORDEM-1)/2;i++){
                novaPagina1.chaves[i] = pg.chaves[i];
                novaPagina1.numeroElementos++;
            }
            //insete metade das chaves até a ultima em uma nova pagina 2
            for(int i=(ORDEM+1)/2; i < ORDEM-1;i++){
                novaPagina2.chaves[i-elemento1fica] = pg.chaves[i];
                novaPagina2.numeroElementos++;
            }

            //pagina raiz ficaria assim
            pg.chaves[0] = elemento1fica;
            //pg.ponteiros[0] = novaPagina1;
            pg.chaves[1] = elemento2fica;
            //pg.ponteiros[1] = novaPagina2;
            pg.numeroElementos = 2;

            // altura da arvore almentada
            cabecalhoArvore.alturaArvore++;
        }
    }
    // caso não tiver somente a pagina raiz
    else{
        altura = 1;
        pagina *pg = lePagina(cabecalhoArvore.paginaRaiz);
        pagina *pganterior = lePagina(cabecalhoArvore.paginaRaiz);
        // vai até a pagina folha
        while(altura != cabecalhoArvore.alturaArvore){
            int proximapag;
            if(chave>pg.chaves[numeroElementos-1]){ //caso chave seja a maior inserção já vai trocando o ultimo valor até chegar na pagina folha
                proximapag = pg.chaves[numeroElementos-1];
                pg.chaves[numeroElementos-1] = chave;
            }
            else{
                for(int i=0;i<pg.numeroElementos;i++){
                    if(chave<pg.chaves[i]){
                        proximapag = pg.chaves[i];
                        i = pg.numeroElementos; // BREAK;
                    }
                }
            }
            *pganterior = *pg;
            pagina *pg = lePagina(proximapag);
            altura++;
        }

        // pagina não cheia
        if(pg.numeroElementos < ORDEM){
            int i = pg.numeroElementos-1;
            if(chave > pg.chaves[i]){ // é maior que o ultimo valor da pagina? então insere no final
                pg.chaves[i+1] = chave;
            }
            else{   //se não move todos para frente até colocar na posição correta
                while(pg.chaves[i] > chave){
                    pg.chaves[i] = pg.chaves[i-1];
                    i--;
                }
                pg.chaves[i+1] = chave;
            }
        }
        // pagina folha cheia
        else{
            //o elemento do meio
            int elementomeio = pg.chaves[(ORDEM-1)/2];

            // criação de 1 nova pagina
            cabecalhoArvore.numeroPaginas++;
            pagina *novaPagina1(cabecalhoArvore.numeroPaginas);

            //metade das chaves na pagina nova
            for(int i=0;i<=(ORDEM-1)/2;i++){
                novaPagina1.chaves[i] = pg.chaves[i];
                novaPagina1.numeroElementos++;
            }

            //removendo todas chaves que foram transferidas
            for(int i=0;i<=(ORDEM-1)/2;i++){
                for(int j=0;j<=pg.numeroElementos;j++){
                    pg.chaves[j] = pg.chaves[j+1];
                }
            }
            pg.numeroElementos = (ORDEM-1)/2;



            //chave ser inserida na pagina nova ou na que já existe
            // inserção na nova
            if(chave < novaPagina1.chave[novaPagina1.numeroElementos-1]){
                *pg = *novaPagina1;     //trato novapagina1 como pg para facilitar o codigo
                // igual inserir em folha vazia
                int i = pg.numeroElementos-1;
                if(chave > pg.chaves[i]){ // é maior que o ultimo valor da pagina? então insere no final
                    pg.chaves[i+1] = chave;
                }
                else{   //se não move todos para frente até colocar na posição correta
                    while(pg.chaves[i] > chave){
                        pg.chaves[i] = pg.chaves[i-1];
                        i--;
                    }
                    pg.chaves[i+1] = chave;
                }
                pg.numeroElementos+1;
            }
            //inserção na ja existente
            else{
                // igual inserir em folha vazia
                int i = pg.numeroElementos-1;
                if(chave > pg.chaves[i]){ // é maior que o ultimo valor da pagina? então insere no final
                    pg.chaves[i+1] = chave;
                }
                else{   //se não move todos para frente até colocar na posição correta
                    while(pg.chaves[i] > chave){
                        pg.chaves[i] = pg.chaves[i-1];
                        i--;
                    }
                    pg.chaves[i+1] = chave;
                }
                pg.numeroElementos+1;
            }

            //atualização da pagina superior  (colocar o valor da pagina nova)
            int valor = novaPagina1.chaves[novaPagina1.numeroElementos-1];
            // igual inserir em folha vazia
            int i = pganterior.numeroElementos-1;
            if(valor > pganterior.chaves[i]){ // é maior que o ultimo valor da pagina? então insere no final
                pganterior.chaves[i+1] = valor;
            }
            else{   //se não move todos para frente até colocar na posição correta
                while(pganterior.chaves[i] > valor){
                    pganterior.chaves[i] = pg.chaves[i-1];
                    i--;
                }
                pg.chaves[i+1] = valor;
            }
            pganterior.numeroElementos+1;

        }






    }
    // se (cabecalhoArvore.paginaRaiz == 1) entao raiz eh a unica pagina. Ler raiz, inserir e salvar. Senao...
            // Exemplo:
            // pagina *pg = lePagina(cabecalhoArvore.paginaRaiz);
            // adicionar <chave,offsetRegistro> na pagina pg
            // salvar pagina: salvaPagina(pg->numeroPagina, pg);

    // senao...

    // ler pagina raiz: pagina *pg = lePagina(cabecalhoArvore.paginaRaiz);

    // se inserir, atualizar cabecalho
    cabecalhoArvore.numeroElementos++;
    salvaCabecalho();
}

void btree::removeChave(int chave) {

    // se remover, atualizar cabecalho
    if (true) {
        cabecalhoArvore.numeroElementos--;
        salvaCabecalho();
    }
}

int btree::buscaChave(int chave) {
    // caso não encontrar chave, retornar -1
    return -1;
}

