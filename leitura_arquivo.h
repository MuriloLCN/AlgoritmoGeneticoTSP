#ifndef LEITURA_ARQUIVO
#define LEITURA_ARQUIVO

#include<stdio.h>

typedef struct {
    float x;
    float y;
} coordenada;

typedef enum {False, True} booleano;

void lerArquivo(FILE* arquivoEntrada, coordenada** listaDeVertices, int* dimensao);

booleano comecaCom(char* palavra, char* prefixo);

booleano terminaCom(char* palavra, char* sufixo);

#endif