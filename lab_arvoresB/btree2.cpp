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
        cabecalhoArvore->numeroElementos = 0;
        cabecalhoArvore->paginaRaiz = -1;
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
    //if s� tiver a pagina raiz
    if(cabecalhoArvore->paginaRaiz == 1){
        pagina *pg = lePagina(cabecalhoArvore.paginaRaiz);
        //pagina n�o cheia
        if(pg.numeroElementos < ORDEM){
            int i = pg->numeroElementos-1;
            if(chave > pg->chaves[i]){ // � maior que o ultimo valor da pagina? ent�o insere no final
                pg->chaves[i+1] = chave;
                pg->ponteiros[i+1] = offsetRegistro;
            }
            else{   //se n�o move todos para frente at� colocar na posi��o correta
                while(pg->chaves[i] > chave){
                    pg->chaves[i+1] = pg->chaves[i];
                    pg->ponteiros[i+1] = pg->ponteiros[i];
                    i--;
                }
                pg.chaves[i+1] = chave;
                pg->ponteiros[i+1] = offsetRegistro;
            }
        }
        // caso a pagina esteja cheia
        else{
            // criacao das 2 novas paginas
            //--------------------------------- raiz deixara de ser nó folha
            int indice1;
            pagina *novaPagina1 = novaPagina(&indice1);
            //--------------------------------- no folha novo
            int indice2;
            pagina *novaPagina2 = novaPagina(&indice2);
            //--------------------------------- no folha novo

            //insere comeco das chaves ate a metade em uma nova pagina 1
            for(int i=0; i <= (ORDEM-1)/2;i++){
                novaPagina1->chaves[i] = pg->chaves[i];
                novaPagina1->ponteiros[i] = pg->ponteiros[i];
                novaPagina1->numeroElementos = novaPagina1->numeroElementos + 1;
            }
            //insete metade das chaves ate a ultima em uma nova pagina 2
            for(int i=(ORDEM+1)/2; i < ORDEM-1;i++){
                novaPagina2->chaves[i-(ORDEM+1)/2] = pg->chaves[i];
                novaPagina2->ponteiros[i-(ORDEM+1)/2] = pg->ponteiros[i];
                novaPagina2->numeroElementos = novaPagina2->numeroElementos + 1;
            }

            //insercao em uma das novas pagina
            if(chave < novaPagina1->chave[novaPagina1->numeroElementos-1]){
                // igual inserir em folha vazia
                int i = novaPagina1->numeroElementos-1;
                if(chave > novaPagina1->chaves[i]){ // maior que o ultimo valor da pagina? ent�o insere no final
                    novaPagina1->chaves[i+1] = chave;
                    novaPagina1->ponteiros[i+1] = offsetRegistro;
                }
                else{   //se nao move todos para frente ate colocar na posicao correta
                    while(novaPagina1->chaves[i] > chave){
                        novaPagina1->chaves[i+1] = novaPagina1->chaves[i];
                        novaPagina1->ponteiros[i+1] = novaPagina1->ponteiros[i];
                        i--;
                    }
                    novaPagina1->chaves[i+1] = chave;
                    novaPagina1->ponteiros[i+1] = offsetRegistro;
                }
                novaPagina1->numeroElementos = novaPagina1->numeroElementos + 1;
            }
            else{
                // igual inserir em folha vazia
                int i = novaPagina2->numeroElementos-1;
                if(chave > novaPagina2->chaves[i]){ // maior que o ultimo valor da pagina? ent�o insere no final
                    novaPagina2->chaves[i+1] = chave;
                    novaPagina2->ponteiros[i+1] = offsetRegistro;
                }
                else{   //se nao move todos para frente ate colocar na posicao correta
                    while(novaPagina2->chaves[i] > chave){
                        novaPagina2->chaves[i+1] = novaPagina2->chaves[i];
                        novaPagina2->ponteiros[i+1] = novaPagina2->ponteiros[i];
                        i--;
                    }
                    novaPagina2->chaves[i+1] = chave;
                    novaPagina2->ponteiros[i+1] = offsetRegistro;
                }
                novaPagina2->numeroElementos = novaPagina2->numeroElementos + 1;
            }

            //pagina raiz ficaria assim
            pg->chaves[0] = novaPagina1->chaves[novaPagina1->numeroElementos-1];
            pg->ponteiros[0] = indice1;

            pg->chaves[1] = novaPagina2->chaves[novaPagina1->numeroElementos-1];
            pg->ponteiros[1] = indice2;
           
            pg->numeroElementos = 2;

            // altura da arvore almentada
            cabecalhoArvore->alturaArvore = cabecalhoArvore->alturaArvore + 1;
        }
    }
    // caso n�o tiver somente a pagina raiz
    else{
        altura = 1;
        pagina *pg = lePagina(cabecalhoArvore->paginaRaiz);
        pagina *pganterior = lePagina(cabecalhoArvore->paginaRaiz);
        // vai ate a pagina folha
        while(altura != cabecalhoArvore->alturaArvore){
            int proximapag;
            if(chave>pg->chaves[numeroElementos-1]){ //caso chave seja a maior insercao ja vai trocando o ultimo valor ate chegar na pagina folha
                proximapag = pg->ponteiros[numeroElementos-1];
                pg->chaves[numeroElementos-1] = chave;
            }
            else{
                for(int i=0;i < pg->numeroElementos;i++){
                    if(chave<pg->chaves[i]){
                        proximapag = pg->ponteiros[i];
                        i = ORDEM; // BREAK;
                    }
                }
            }
            *pganterior = *pg;
            pagina *pg = lePagina(proximapag); //--------- é assim que funciona le pagina?
            altura++;
        }

        // pagina nao cheia
        if(pg->numeroElementos < ORDEM){
            int i = pg->numeroElementos-1;
            if(chave > pg->chaves[i]){ // � maior que o ultimo valor da pagina? ent�o insere no final
                pg->chaves[i+1] = chave;
                pg->ponteiros[i+1] = offsetRegistro;
            }
            else{   //se n�o move todos para frente at� colocar na posi��o correta
                while(pg->chaves[i] > chave){
                    pg->chaves[i+1] = pg->chaves[i];
                    pg->ponteiros[i+1] = pg->chave[i];
                    i--;
                }
                pg->chaves[i+1] = chave;
                pg->ponteiros[i+1] = offsetRegistro;
            }
        }
        // pagina folha cheia
        else{
            //o elemento do meio
            int elementomeio = pg->chaves[(ORDEM-1)/2];

            // criacao de 1 nova pagina
            int indice3;
            pagina *novaPagina1 = novaPagina(&indice3);
            //------------ criacao de uma nova folha

            //metade das chaves na pagina nova
            for(int i=0;i<=(ORDEM-1)/2;i++){
                novaPagina1->chaves[i] = pg->chaves[i];
                novaPagina1->ponteiros[i] = pg->ponteiros[i];
                novaPagina1->numeroElementos = novaPagina1->numeroElementos + 1;
            }

            //removendo todas chaves que foram transferidas
            for(int i=0;i<=(ORDEM-1)/2;i++){
                for(int j=0;j<=pg->numeroElementos;j++){
                    pg->chaves[j] = pg->chaves[j+1];
                    pg->ponteiros[j] = pg->ponteiros[j+1];
                }
            }
            pg.numeroElementos = (ORDEM-1)/2;



            //chave ser inserida na pagina nova ou na que ja existe
            // insercao na nova
            if(chave < novaPagina1->chave[novaPagina1->numeroElementos-1]){
                // igual inserir em folha vazia
                int i = novaPagina1->numeroElementos-1;
                if(chave > novaPagina1->chaves[i]){ // e maior que o ultimo valor da pagina? ent�o insere no final
                    novaPagina1->chaves[i+1] = chave;
                    novaPagina1->ponteiros[i+1] = offsetRegistro;
                }
                else{   //se nao move todos para frente ate colocar na posicao correta
                    while(novaPagina1->chaves[i] > chave){
                        novaPagina1->chaves[i+1] = novaPagina1->chaves[i];
                        novaPagina1->ponteiros[i+1] = novaPagina1->ponteiros[i];
                        i--;
                    }
                    novaPagina1->chaves[i+1] = chave;
                    novaPagina1->ponteiros[i+1] = offsetRegistro;
                }
                novaPagina1->numeroElementos = novaPagina1->numeroElementos + 1;
            }
            //insercao na ja existente
            else{
                // igual inserir em folha vazia
                int i = pg->numeroElementos-1;
                if(chave > pg->chaves[i]){ // � maior que o ultimo valor da pagina? ent�o insere no final
                    pg->chaves[i+1] = chave;
                    pg->chaves[i+1] = offsetRegistro;
                }
                else{   //se n�o move todos para frente at� colocar na posi��o correta
                    while(pg->chaves[i] > chave){
                        pg->chaves[i+1] = pg->chaves[i];
                        pg->ponteiros[i+1] = pg->ponteiros[i];
                        i--;
                    }
                    pg->chaves[i+1] = chave;
                    pg->chaves[i+1] = offsetRegistro;
                }
                pg->numeroElementos = pg->numeroElementos+1;
            }

            chave = novaPagina1->chaves[novaPagina1->numeroElementos-1];
            insereRecursao(pganterior->numeroPagina,chave,indice3);
        }
    }

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
    // caso n�o encontrar chave, retornar -1
    return -1;
}


//buscaPagina = pagina anterior para colocar o valor que subiu
//chave = o valor que subiu para colocar na pagina anterior
//ponteirobewpag = o valor da pagina que foi criada com o slit
int insereRecursao(int buscaPagina,int chave,int ponteiroNewPag){
    leCabecalho();

    if(buscaPagina == cabecalhoArvore->paginaRaiz){
        pagina *pg = lePagina(cabecalhoArvore.paginaRaiz);
        //pagina nao cheia
        if(pg->numeroElementos < ORDEM){
            int i = pg->numeroElementos-1;
            
           while(pg->chaves[i] > chave){
                pg->chaves[i+1] = pg->chaves[i];
                pg->ponteiros[i+1] = pg->ponteiros[i];
                i--;
            }
            pg->chaves[i+1] = chave;
            pg->numeroElementos = pg->numeroElementos + 1;
            pg->ponteiros[i+1] = ponteiroNewPag;
        }
        // caso a pagina esteja cheia
        else{

            // criacao das 2 novas paginas
            int indice1;
            pagina *novaPagina1 = novaPagina(&indice1);
            //------------- cricao de pagina interna (nao folha)
            int indice2;
            pagina *novaPagina2 = novaPagina(&indice2);
            //------------- cricao de pagina interna (nao folha)


            //insere comeco das chaves ate a metade em uma nova pagina 1
            for(int i=0; i <= (ORDEM-1)/2;i++){
                novaPagina1->chaves[i] = pg->chaves[i];
                novaPagina1->numeroElementos = novaPagina1->numeroElementos + 1;
                novaPagina1->ponteiros[i] = pg->ponteiros[i];
            }
            //insete metade das chaves at� a ultima em uma nova pagina 2
            for(int i=(ORDEM+1)/2; i < ORDEM-1;i++){
                novaPagina2->chaves[i-(ORDEM+1)/2] = pg->chaves[i];
                novaPagina2->numeroElementos = novaPagina2->numeroElementos + 1;
                novaPagina2->ponteiros[i] = pg->ponteiros[i];
            }

            //insercao em uma das novas pagina
            if(chave < novaPagina1->chave[novaPagina1->numeroElementos-1]){
                // igual inserir em folha vazia
                int i = novaPagina1->numeroElementos-1;
                if(chave > novaPagina1->chaves[i]){ // maior que o ultimo valor da pagina? ent�o insere no final
                    novaPagina1->chaves[i+1] = chave;
                    novaPagina1->ponteiros[i+1] = ponteiroNewPag;
                }
                else{   //se nao move todos para frente ate colocar na posicao correta
                    while(novaPagina1->chaves[i] > chave){
                        novaPagina1->chaves[i+1] = novaPagina1->chaves[i];
                        novaPagina1->ponteiros[i+1] = novaPagina1->ponteiros[i];
                        i--;
                    }
                    novaPagina1->chaves[i+1] = chave;
                    novaPagina1->ponteiros[i+1] = ponteiroNewPag;
                }
                novaPagina1->numeroElementos = novaPagina1->numeroElementos + 1;
            }
            else{
                // igual inserir em folha vazia
                int i = novaPagina2->numeroElementos-1;
                if(chave > novaPagina2->chaves[i]){ // maior que o ultimo valor da pagina? ent�o insere no final
                    novaPagina2->chaves[i+1] = chave;
                    novaPagina2->ponteiros[i+1] = ponteiroNewPag;
                }
                else{   //se nao move todos para frente ate colocar na posicao correta
                    while(novaPagina2->chaves[i] > chave){
                        novaPagina2->chaves[i+1] = novaPagina2->chaves[i];
                        novaPagina2->ponteiros[i+1] = novaPagina2->ponteiros[i];
                        i--;
                    }
                    novaPagina2->chaves[i+1] = chave;
                     novaPagina2->ponteiros[i+1] = ponteiroNewPag;
                }
                novaPagina2->numeroElementos = novaPagina2->numeroElementos + 1;
            }
            

            //pagina raiz ficaria assim
            pg->chaves[0] = novaPagina1->chaves[novaPagina1->numeroElementos-1];
            pg->ponteiros[0] = indice1;

            pg->chaves[1] = novaPagina2->chaves[novaPagina1->numeroElementos-1];
            pg->ponteiros[1] = indice2;
           
            pg->numeroElementos = 2;

            // altura da arvore almentada
            cabecalhoArvore->alturaArvore = cabecalhoArvore->alturaArvore + 1;
        }
    }
    else{
        pagina *pg = lePagina(cabecalhoArvore->paginaRaiz);
        pagina *pganterior = lePagina(cabecalhoArvore->paginaRaiz);
        // vai ate a pagina folha
        while(buscaPagina != pg->numeroPagina){
            int proximapag;
            for(int i=0;i < pg->numeroElementos;i++){
                if(chave<pg->chaves[i]){
                    proximapag = pg->ponteiros[i];
                    i = ORDEM; // BREAK;
                }
            }
            *pganterior = *pg;
            pagina *pg = lePagina(proximapag);
        }

        // pagina nao cheia
        if(pg->numeroElementos < ORDEM){
            int i = pg->numeroElementos-1;
            //move todos para frente at� colocar na posi��o correta
            while(pg->chaves[i] > chave){
                pg->chaves[i+1] = pg->chaves[i];
                pg->ponteiros[i+1] = pg->ponteiros[i];
                i--;
            }
            pg->chaves[i+1] = chave;
            pg->ponteiros[i+1] = ponteiroNewPag;
            pg->numeroElementos = pg->numeroElementos + 1;
        }


        else{
            //o elemento do meio
            int elementomeio = pg->chaves[(ORDEM-1)/2];

            // criacao de 1 nova pagina
            int indice3;
            pagina *novaPagina1 = novaPagina(&indice3);
            //------------- cricao de pagina interna (nao folha)

            //metade das chaves na pagina nova
            for(int i=0;i<=(ORDEM-1)/2;i++){
                novaPagina1->chaves[i] = pg->chaves[i];
                novaPagina1->ponteiros[i] = pg->ponteiros[i];
                novaPagina1->numeroElementos = novaPagina1->numeroElementos + 1;
            }

            //removendo todas chaves que foram transferidas
            for(int i=0;i<=(ORDEM-1)/2;i++){
                for(int j=0;j<=pg->numeroElementos;j++){
                    pg->chaves[j] = pg->chaves[j+1];
                    pg->ponteiros[j] = pg->ponteiros[j+1];
                }
            }
            pg.numeroElementos = (ORDEM-1)/2;



            //chave ser inserida na pagina nova ou na que ja existe
            // insercao na nova
            if(chave < novaPagina1->chave[novaPagina1->numeroElementos-1]){
                // igual inserir em folha vazia
                int i = novaPagina1->numeroElementos-1;
                //move todos para frente at� colocar na posi��o correta
                while(novaPagina1->chaves[i] > chave){
                    novaPagina1->chaves[i+1] = novaPagina1->chaves[i];
                    novaPagina1->ponteiros[i+1] = novaPagina1->ponteiros[i];
                    i--;
                }
                novaPagina1->chaves[i+1] = chave;
                novaPagina1->ponteiros[i+1] = ponteiroNewPag;
                    
                novaPagina1->numeroElementos = novaPagina1->numeroElementos + 1;
            }
            //insercao na ja existente
            else{
                // igual inserir em folha vazia
                int i = pg->numeroElementos-1;
                //se nao move todos para frente at� colocar na posi��o correta
                while(pg->chaves[i] > chave){
                    pg->chaves[i+1] = pg->chaves[i];
                    pg->ponteiros[i+1] = pg->ponteiros[i];
                    i--;
                }
                pg->chaves[i+1] = chave;
                pg->ponteiros[i+1] = ponteiroNewPag;
                
                pg->numeroElementos = pg->numeroElementos + 1;
            }

            chave = novaPagina1->chaves[novaPagina1->numeroElementos-1];
            insereRecursao(pganterior->numeroPagina,chave,indice3);
        

    }
}
