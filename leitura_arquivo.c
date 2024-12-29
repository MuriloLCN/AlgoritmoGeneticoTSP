/*
    Avaliação 3: Algoritmo genético para o PCV
    Murilo Luis Calvo Neves - RA 129037
    Leandro Eugênio Farias Berton - RA 129268
*/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<time.h>

#define TAM_BUFFER_ENTRADA 1024

typedef enum {False, True} booleano;

typedef struct {
    float x;
    float y;
} coordenada;

void pegaCoordenadasStr(char* str, float* x, float* y, int* linha)
{
    /*
        Quebra uma linha de definição de coordadas em suas partes básicas, por exemplo:

        "574 1232 5454"    (formato do .tsp, 'linha x y')

        É quebrado em:

        linha = 574
        x = 1232
        y = 5454
    */

    int idx_x = 0;
    int idx_y = 0;
    int idx_l = 0;

    char str_x[TAM_BUFFER_ENTRADA];
    char str_y[TAM_BUFFER_ENTRADA];
    char str_l[TAM_BUFFER_ENTRADA];

    int idx_leitura = 0;

    while (str[idx_leitura] == ' ')
    {
        idx_leitura++;
    }

    // Lendo 'linha' até o primeiro espaço
    while (str[idx_leitura] != ' ')
    {
        str_l[idx_l] = str[idx_leitura];
        idx_leitura++;
        idx_l++;
    }

    //idx_leitura++;
    str_l[idx_l] = '\0';

    while (str[idx_leitura] == ' ')
    {
        idx_leitura++;
    }

    // Lendo 'x' até o próximo espaço
    while (str[idx_leitura] != ' ')
    {
        str_x[idx_x] = str[idx_leitura];
        idx_leitura++;
        idx_x++;
    }

    // idx_leitura++;
    str_x[idx_x] = '\0';

    while (str[idx_leitura] == ' ')
    {
        idx_leitura++;
    }

    // Lendo 'y' até o fim da linha
    while (str[idx_leitura] != '\0' && str[idx_leitura] != '\n')
    {
        str_y[idx_y] = str[idx_leitura];
        idx_leitura++;
        idx_y++;
    }

    str_y[idx_y] = '\0';

    *linha = atoi(str_l);
    *x = atof(str_x);
    *y = atof(str_y);

}

booleano comecaCom(char* palavra, char* prefixo)
{
    /*
        Verifica se uma palavra começa com algum prefixo específico
        Parâmetros:
            char* palavra: A string a qual se quer ver o prefixo
            char* prefixo: O prefixo a ser comparado com o início da palavra
        Retorno:
            True: Caso a palavra tenha o prefixo
            False: Caso a palavra não tenha o prefixo
    */
    int tamanho_palavra = strlen(palavra);
    int tamanho_prefixo = strlen(prefixo);

    if (tamanho_prefixo > tamanho_palavra)
    {
        return False;
    }

    int i = 0;
    while (i < tamanho_prefixo)
    {
        if (palavra[i] != prefixo[i])
        {
            return False;
        }
        i++;
    }

    return True;
}

booleano terminaCom(char* palavra, char* sufixo)
{
    /*
        Verifica se uma palavra termina com algum sufixo específico
        Parâmetros:
            char* palavra: A string a qual se quer ver o prefixo
            char* sufixo: O sufixo a ser comparado com o fim da palavra
        Retorno:
            True: Caso a palavra tenha o sufixo
            False: Caso a palavra não tenha o sufixo
    */
    int tamanho_palavra = strlen(palavra);
    int tamanho_sufixo = strlen(sufixo);

    if (tamanho_sufixo > tamanho_palavra)
    {
        return False;
    }

    int i = tamanho_palavra - 1;
    int j = tamanho_sufixo - 1;
    while (j >= 0)
    {
        if (palavra[i] != sufixo[j])
        {
            return False;
        }
        i--;
        j--;
    }
    return True;
}

void lerArquivo(FILE* arquivoEntrada, coordenada** listaDeVertices, int* dimensao)
{
    char buffer[TAM_BUFFER_ENTRADA];

    // Nome
    fgets(buffer, TAM_BUFFER_ENTRADA, arquivoEntrada);

    if (comecaCom(buffer, "NAME") == False)
    {
        printf("\nFormato de arquivo não corresponde ao esperado de um .tsp: Não inicia com \"NAME: \"");
        return;
    }

    while (comecaCom(buffer, "NODE_COORD_SECTION") == False)
    {
        fgets(buffer, TAM_BUFFER_ENTRADA, arquivoEntrada);

        if (comecaCom(buffer, "DIMENSION") == True)
        {
            *dimensao = atoi(buffer + 11);
        }
    }

    *listaDeVertices = malloc(*dimensao * sizeof(coordenada));

    printf("\nDimensao do problema: %d\n", *dimensao);

    for (int i = 1; i <= (*dimensao); i++)
    {
        fgets(buffer, TAM_BUFFER_ENTRADA, arquivoEntrada);
        // printf("%s", buffer);
        float x;
        float y;
        int linha;
        pegaCoordenadasStr(buffer, &x, &y, &linha);

        (*listaDeVertices)[linha - 1].x = x;
        (*listaDeVertices)[linha - 1].y = y;
        // printf("\nLinha: %d X: %d Y: %d", linha, x, y);
    }

}
