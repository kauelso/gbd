/*
 * File:   main.cpp
 * Integrantes:
 * Kauê Lucas Silvério Oliveira - 11821BCC007
 * Guilherme de Castro Berti - 11821BCC023
 * Yan Stivaletti e Souza - 11821BCC002
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <list>

using namespace std;

// remove pontuacao de uma palavra
void removePontuacao (char *palavra) {
    int length = strlen(palavra);
    if (
        (palavra[length-1] == '.') || (palavra[length-1] == ',') || (palavra[length-1] == ';') ||
        (palavra[length-1] == ':') || (palavra[length-1] == '?') || (palavra[length-1] == '!')
       )
        palavra[length-1] = '\0';
}

// imprime linha do arquivo com base no offset da palavra
void imprimeLinha(int offset) {
    FILE *f = fopen("biblia.txt","rt");
    int pos = -1;
    char linha[2048];
    while (pos < offset) {
        fgets(linha,2047,f);
        pos = ftell(f);
    }
    printf("%s",linha);
    fclose(f);
}

// classe que implementa a lista invertida
class listaInvertida {
public:
    // construtor
    listaInvertida() {
        arquivoListaInvertida.open("listaInvertida.txt", fstream::in | fstream::out | fstream::app);
        if (!arquivoListaInvertida.is_open()){
        printf("\n\n Nao consegui abrir arquivo listaInvertida.txt. Sinto muito.\n\n\n\n");
        }
    }
    // destrutor
    ~listaInvertida() {
        arquivoListaInvertida.close();
    }
    // adiciona palavra na estrutura
    void adiciona(char *palavra, int offset) {
        int tamanhoPalavra = strlen(palavra) + 1; //Tamanho da palavra + o \0
        string buffer = ""; //Buffer para transformar a palavra de um array de char para uma string
        for(int i = 0; i<tamanhoPalavra; i++){
            buffer = buffer + palavra[i];
        }

        if(mapIndiceSecundario[buffer] != 0){ //Caso a palavra ja exista
            int offsetAnterior = mapIndiceSecundario[buffer]; //Posicao atual no indice secundario
            int offsetListaInvertida = arquivoListaInvertida.tellg(); //Pega a posicao atual do arquivo, ou seja, a posicao onde a palavra sera escrita
            mapIndiceSecundario[buffer] = offsetListaInvertida; //Coloca na no indice secundario o novo endereco
            arquivoListaInvertida << offset << "|" << offsetAnterior << "|" << endl; //Escreve no arquivo de lista invertida
        }
        else{ //Caso a palavra nao exista no indice ainda
            int offsetListaInvertida = arquivoListaInvertida.tellg(); //Pega a posicao atual do arquivo, ou seja, a posicao onde a palavra sera escrita
            mapIndiceSecundario[buffer] = offsetListaInvertida; //Grava essa posicao no indice secundario
            arquivoListaInvertida << offset << "|" << -1 << "|" << endl; //Escreve no arquivo da lista invertida
        }
    }
    // realiza busca, retornando vetor de offsets que referenciam a palavra
    int * busca(char *palavra, int *quantidade) {
        int posicaoAtual = arquivoListaInvertida.tellg();
        int tamanhoPalavra = strlen(palavra) + 1;
        string buffer = "";
        for(int i = 0; i<tamanhoPalavra; i++){
            buffer = buffer + palavra[i];
        }

        int ultimoRegistro =  mapIndiceSecundario[buffer];

        int *offsets;
        *quantidade = 0;

        if(ultimoRegistro == 0){
            return offsets;
        }
        list <int> listaOffsets;

        int proximoOffset = ultimoRegistro; //Offset do ultimo registro da lista invertida
        while(proximoOffset != -1){
            arquivoListaInvertida.seekg(proximoOffset); //Vai para a posicao do ultimo registro no arquivo
            string linha;
            string delimitador = "|";
            getline(arquivoListaInvertida,linha); //Pega a linha do registro
            string offsetDaPalavraString = linha.substr(0, linha.find(delimitador)); // Pega o primeiro valor que sera o offset no arquivo da biblia
            linha.erase(0, linha.find(delimitador) + delimitador.length()); //Exclui a primeira parte da linha
            string proximoOffsetString = linha.substr(0, linha.find(delimitador)); //Pega a parte restante que sera o proximo registro

            int offsetDaPalavra = stoi(offsetDaPalavraString); //Transforma o offset da palavra na bilbia em int (quando lido ele e uma string)
            proximoOffset = stoi(proximoOffsetString);//Transforma o proximo offset em int (quando lido ele e uma string)
            listaOffsets.insert(listaOffsets.end(),offsetDaPalavra); //Insere o offset da palavra na lista de offsets
            *quantidade = *quantidade + 1; //Aumenta em 1 a quantidade de offsets na lista
        }

        offsets = (int*)malloc(sizeof(int)*listaOffsets.size()); //Aloca o espaco no array de offsets
        copy(listaOffsets.begin(),listaOffsets.end(),offsets); //Copia os dados da lista de offsets para o array de offsets
        arquivoListaInvertida.seekg(posicaoAtual); //Devolve o ponteiro do arquivo para a posicao que estava antes da busca
        return offsets; //Restorna o array de offsets
    }
private:
    fstream arquivoListaInvertida;
    map<string,int> mapIndiceSecundario;
};

// programa principal
int main(int argc, char** argv) {
    // abrir arquivo
    ifstream in("biblia.txt");
    if (!in.is_open()){
        printf("\n\n Nao consegui abrir arquivo biblia.txt. Sinto muito.\n\n\n\n");
    }
    else{
        // vamos ler o arquivo e criar a lista invertida com as palavras do arquivo
        char *palavra = new char[100];
        int offset, contadorDePalavras = 0;
        listaInvertida lista;
        // ler palavras
        while (!in.eof()) {
            // ler palavra
            in >> palavra;
            // pegar offset
            offset = in.tellg();
            // remover pontuacao
            removePontuacao(palavra);
            // desconsiderar palavras que sao marcadores do arquivo
            if (!((palavra[0] == '#') || (palavra[0] == '[') || ((palavra[0] >= '0') && (palavra[0] <= '9')))) {
                //printf("%d %s\n", offset,palavra); fflush(stdout); // debug :-)
                lista.adiciona(palavra, offset);
                contadorDePalavras++;
                if (contadorDePalavras % 1000 == 0) { printf(".");  fflush(stdout); }
            }
        }
        in.close();

        // agora que ja construimos o indice, podemos realizar buscas
        do {
            printf("\nDigite a palavra desejada ou \"SAIR\" para sair: ");
            scanf("%s",palavra);
            if (strcmp(palavra,"SAIR") != 0) {
                int quantidade;
                int *offsets = lista.busca(palavra,&quantidade);
                if (quantidade > 0) {
                    for (int i = 0; i < quantidade; i++)
                        imprimeLinha(offsets[i]);
                }
                else
                    printf("nao encontrou %s\n",palavra);
            }
        } while (strcmp(palavra,"SAIR") != 0);

        printf("\n\nAte mais!\n\n");
    }

    return (EXIT_SUCCESS);
}

