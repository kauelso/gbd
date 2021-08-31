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
    }
    // destrutor
    ~listaInvertida() { 
    }
    // adiciona palavra na estrutura
    void adiciona(char *palavra, int offset) {
        int tamanhoPalavra = strlen(palavra) + 1;
        string buffer = "";
        for(int i = 0; i<tamanhoPalavra; i++){
            buffer = buffer + palavra[i];
        }

        if(mapIndiceSecundario[buffer] != 0){
            int ultimoOffset;
            ultimoOffset = mapIndiceSecundario[buffer];
            mapIndiceSecundario[buffer] = offset;
            mapListaInvertida[offset] = ultimoOffset;
        }
        else{
            mapIndiceSecundario[buffer] = offset;
            mapListaInvertida[offset] = -1;
        }
    }
    // realiza busca, retornando vetor de offsets que referenciam a palavra
    int * busca(char *palavra, int *quantidade) {
        int tamanhoPalavra = strlen(palavra) + 1;
        string buffer = "";
        for(int i = 0; i<tamanhoPalavra; i++){
            buffer = buffer + palavra[i];
        }

        int offsets[1] = {1};
        *quantidade = 1;
        list <int> lista;

        cout << mapIndiceSecundario[buffer];

        return offsets;
    }
private:
    map<int,int> mapListaInvertida;
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

