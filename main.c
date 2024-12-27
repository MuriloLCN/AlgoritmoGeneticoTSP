/*
    Avaliação 2: Algoritmos heurísticos para o PCV
    Murilo Luis Calvo Neves - RA 129037
    Leandro Eugênio Farias Berton - RA 129268
*/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<time.h>
#include"leitura_arquivo.h"

#define TAM_BUFFER_ENTRADA 1024

FILE* arquivoTimestamp;
coordenada* listaDeVertices;
int dimensao;
clock_t inicioMelhoramento;

void printTimestamp(float custo)
{
    fprintf(arquivoTimestamp, "%f %f \n", ((float) (clock() - inicioMelhoramento)) / CLOCKS_PER_SEC, custo);
}

float calculaCustoRota(int* rota);

float calculaDistancia(coordenada* c1, coordenada* c2)
{
    float dx = c1->x - c2->x;
    float dy = c1->y - c2->y;
    return sqrt(dx*dx + dy*dy);
}

void vizinhoMaisProximo(int* rotaFinal)
{
    int atual = 0;
    int inicial = atual;

    int indiceRota = 0;

    booleano visitados[dimensao];

    for (int i = 0; i < dimensao; i++)
    {
        visitados[i] = False;
    }

    visitados[atual] = True;
    rotaFinal[indiceRota] = atual;
    indiceRota++;

    float distanciaTotal = 0.0;

    for (int passo = 1; passo < dimensao; passo++)
    {
        float menorDistancia = INFINITY;
        int prox = -1;

        for (int j = 0; j < dimensao; j++)
        {
            if (visitados[j] == False)
            {
                float distancia = calculaDistancia(&listaDeVertices[atual], &listaDeVertices[j]);
                if (distancia < menorDistancia)
                {
                    menorDistancia = distancia;
                    prox = j;
                }
            }
        }

        if (prox == -1)
        {
            printf("\nErro: Nenhum vértice restante para visitar.");
            return;
        }

        visitados[prox] = 1;
        distanciaTotal += menorDistancia;
        atual = prox;

        rotaFinal[indiceRota] = atual;
        indiceRota++;
    }

    // Retorna ao vértice inicial
    rotaFinal[indiceRota] = inicial;
    distanciaTotal += calculaDistancia(&listaDeVertices[atual], &listaDeVertices[inicial]);
    printf("\nDistancia total percorrida: %2f\n", distanciaTotal);
}

void cicloInicialDet(int* v1, int*v2, int* v3)
{
    /*
        Encontra dois vértices mais distantes e, em seguida, encontra um terceiro vértice
        cuja a soma das distâncias entre os dois vértices iniciais é máxima.

    */

    float distanciaMax = -INFINITY, distanciaTemp;

    for (int i = 0; i < dimensao; i++)
    {
        for (int j = 0; j < dimensao; j++)
        {
            distanciaTemp = calculaDistancia(&listaDeVertices[i], &listaDeVertices[j]);
            if (distanciaTemp > distanciaMax)
            {
                distanciaMax = distanciaTemp;
                *v1 = i;
                *v2 = j;
            }   
        }
    }

    distanciaMax = -INFINITY;
    for (int i = 0; i < dimensao; i++)
    {
        distanciaTemp = calculaDistancia(&listaDeVertices[*v1], &listaDeVertices[i]) + calculaDistancia(&listaDeVertices[*v2], &listaDeVertices[i]);
        if (distanciaTemp > distanciaMax)
        {
            distanciaMax = distanciaTemp;
            *v3 = i;
        }   
    }

    printf("Ciclo inicial: %d -> %d -> %d -> %d", *v1, *v2, *v3, *v1);
}

void insercaoMaisDistante(int* rotaFinal, int v1, int v2, int v3)
{
    // considerando que, devido ao ciclo inicial, já temos 3 vértices na solução
    int indiceRota = 3;

    booleano visitados[dimensao];
    for (int i = 0; i < dimensao; i++)
    {
        visitados[i] = False;
    }

    // int vertice1, vertice2, vertice3;
    // cicloInicialNDet(&vertice1, &vertice2, &vertice3);

    visitados[v1] = True;
    visitados[v2] = True;
    visitados[v3] = True;
    
    rotaFinal[0] = v1;
    rotaFinal[1] = v2;
    rotaFinal[2] = v3;
    rotaFinal[3] = v1;

    int proxInsercao;
    float ultimaDistanciaMaxima=-INFINITY, distanciaTemp;

    // para cada vertice Vk externo, calcular a menor distância entre o vértice Vk e o ciclo
    // distanciaMinima é um vetor para guardar a distância mínima entre o ciclo e um vértice externo Vk
    float* distanciaMinima = malloc(sizeof(float) * dimensao); 
    for (int i = 0; i < dimensao; i++)
    {
        distanciaMinima[i] = INFINITY;
    }

    for (int Vk = 0; Vk < dimensao; Vk++)
    {
        // se o vértice não pertence ao ciclo
        if (visitados[Vk] == False)
        {
            // calcula a menor distância entre o vértice Vk e o ciclo
            for (int i = 0; i < indiceRota; i++)
            {
                distanciaTemp = calculaDistancia(&listaDeVertices[Vk], &listaDeVertices[rotaFinal[i]]);
                if (distanciaTemp < distanciaMinima[Vk])
                {
                    distanciaMinima[Vk] = distanciaTemp;
                }
            }
        }
    }

    // para cada vertice Vk externo, calcular a distância entre os vértices do ciclo
    // enquanto existir vertice para inserir ...
    while (indiceRota < dimensao)
    {
        ultimaDistanciaMaxima=-INFINITY;
        for (int Vk = 0; Vk < dimensao; Vk++)
        {
            // se o vértice ainda não pertence ao ciclo
            if (visitados[Vk] == False)
            {
                if (distanciaMinima[Vk] > ultimaDistanciaMaxima)
                {
                    ultimaDistanciaMaxima = distanciaMinima[Vk];
                    proxInsercao = Vk;
                }
            }
        }

        float custoAtual, custoMinimo=INFINITY;
        int indiceCustoMinimo;

        // inserir vertice Vk escolhido no local que causa o menor impacto no custo (distância)
        // segue-se o critérido de achar a posição em que {d(Vi, Vk) + d(Vk, Vi+1) - d(Vi, Vi+1)} é mínimo
        for (int i = 0; i < indiceRota; i++)
        {
            custoAtual = calculaDistancia(&listaDeVertices[rotaFinal[i]], &listaDeVertices[proxInsercao])
                         + calculaDistancia(&listaDeVertices[proxInsercao], &listaDeVertices[rotaFinal[i+1]])
                         - calculaDistancia(&listaDeVertices[rotaFinal[i]], &listaDeVertices[rotaFinal[i+1]]);
            // TODO alterar < para <= e ver se melhora a solução
            if (custoAtual < custoMinimo)
            {
                custoMinimo = custoAtual;
                indiceCustoMinimo = i + 1; // o indice de custo mínimo é a posição no vetor de rota onde será inserido Vk
            }
        }

        // mais um vértice entra para rota
        indiceRota++;
        // deslocando cada posição para direita, do fim do vetor até a posição onde será inserido Vk
        for (int indiceCiclo = indiceRota; indiceCiclo > indiceCustoMinimo; indiceCiclo--)
        {
            rotaFinal[indiceCiclo] = rotaFinal[indiceCiclo-1];
        }
        rotaFinal[indiceCustoMinimo] = proxInsercao;
        visitados[proxInsercao] = True;
        // printf("%d/%d\n", indiceRota, dimensao);

        // atualizando o vetor de distâncias mínimas
        for (int Vk = 0; Vk < dimensao; Vk++)
        {
            // se o vértice não pertence ao ciclo
            if (visitados[Vk] == False)
            {
                float novaDistancia = calculaDistancia(&listaDeVertices[Vk], &listaDeVertices[proxInsercao]);
                if (novaDistancia < distanciaMinima[Vk])
                {
                    distanciaMinima[Vk] = novaDistancia;
                }
            }

        }
    }
}

void troca(int* rota, int i, int j)
{
    int buffer = rota[i];
    rota[i] = rota[j];
    rota[j] = buffer;    
}

void pairSwap(int* rota) 
{
    float deltaSemTroca, deltaComTroca, delta;
    int numElementos = dimensao + 1;
    float custoAtual = calculaCustoRota(rota);

    for (int i = 0; i < dimensao; i++)
        {
        for (int j = 0; j <= dimensao; j++)
        {
            if (j == i+1)
            {
                deltaSemTroca = calculaDistancia(&listaDeVertices[rota[(i-1+numElementos)%numElementos]], &listaDeVertices[rota[i]]) +
                    calculaDistancia(&listaDeVertices[rota[j]], &listaDeVertices[rota[(j+1)%numElementos]]);
                deltaComTroca = deltaComTroca = calculaDistancia(&listaDeVertices[rota[(i-1+numElementos)%numElementos]], &listaDeVertices[rota[j]]) +
                    calculaDistancia(&listaDeVertices[rota[i]], &listaDeVertices[rota[(j+1)%numElementos]]);
            }
            else if (j == i-1)
            {
                deltaSemTroca = calculaDistancia(&listaDeVertices[rota[(j-1+numElementos)%numElementos]], &listaDeVertices[rota[j]]) +
                    calculaDistancia(&listaDeVertices[rota[i]], &listaDeVertices[rota[(i+1)%numElementos]]);
                deltaComTroca = deltaComTroca = calculaDistancia(&listaDeVertices[rota[(j-1+numElementos)%numElementos]], &listaDeVertices[rota[i]]) +
                    calculaDistancia(&listaDeVertices[rota[j]], &listaDeVertices[rota[(i+1)%numElementos]]);
            }

            else
            {
                deltaSemTroca = calculaDistancia(&listaDeVertices[rota[(i-1+numElementos)%numElementos]], &listaDeVertices[rota[i]]) +
                    calculaDistancia(&listaDeVertices[rota[i]], &listaDeVertices[rota[i+1]]) +
                    calculaDistancia(&listaDeVertices[rota[(j-1+numElementos)%numElementos]], &listaDeVertices[rota[j]]) +
                    calculaDistancia(&listaDeVertices[rota[j]], &listaDeVertices[rota[(j+1)%numElementos]]);
                
                deltaComTroca = calculaDistancia(&listaDeVertices[rota[(i-1+numElementos)%numElementos]], &listaDeVertices[rota[j]]) +
                    calculaDistancia(&listaDeVertices[rota[j]], &listaDeVertices[rota[i+1]]) +
                    calculaDistancia(&listaDeVertices[rota[(j-1+numElementos)%numElementos]], &listaDeVertices[rota[i]]) +
                    calculaDistancia(&listaDeVertices[rota[i]], &listaDeVertices[rota[(j+1)%numElementos]]);
            }

            delta = deltaComTroca - deltaSemTroca;
            
            if (delta < 0) // se houve uma diminuição do custo
            {
                // custoAtual += delta;
                // printTimestamp(custoAtual);
                troca(rota, i, j);
            }
        }
    }
}

void exportaResultados(int* rotaFinal, float custoTotal, char* nomeArquivo, float tempoGasto)
{
    FILE* arquivoDeSaida;

    arquivoDeSaida = fopen("resultados.txt", "a");

    fprintf(arquivoDeSaida, "Instancia: ");
    fprintf(arquivoDeSaida, nomeArquivo);
    fprintf(arquivoDeSaida, "\nTempo computando (em segundos): ");
    fprintf(arquivoDeSaida, "%f", tempoGasto);
    fprintf(arquivoDeSaida, "\nCusto total: ");
    fprintf(arquivoDeSaida, "%f", custoTotal);
    fprintf(arquivoDeSaida, "\nVertices: ");

    for(int i = 0; i < dimensao + 1; i++)
    {
        fprintf(arquivoDeSaida, "%d -> ", rotaFinal[i]);
    }

    fclose(arquivoDeSaida);
}

float calculaCustoRota(int* rota)
{
    float custoTotal = 0;

    for(int i = 0; i < dimensao; i++)
    {
        custoTotal += calculaDistancia(&listaDeVertices[rota[i]], &listaDeVertices[rota[i+1]]);
    }

    return custoTotal;
}

void trocar_pontas(int* rotaFinal, int i, int j)
{
    // Fonte: https://en.wikipedia.org/wiki/2-opt
    i += 1;
    while (i < j)
    {
        int temp = rotaFinal[i];
        rotaFinal[i] = rotaFinal[j];
        rotaFinal[j] = temp;
        i++;
        j--;
    }
}

void doisOpt(int* rotaFinal)
{
    /*
        Realiza a heurística de melhoramento 2-opt aplicado ao first improvement

        Baseado em: https://en.wikipedia.org/wiki/2-opt

        Parâmetros:
            *rotaFinal: um vetor com DIMESAO+1 elementos que define a ordem dos vértices a serem visitados
    */
    
    int n = dimensao;
    booleano encontrouMelhoria = True;

    clock_t atual = clock();

    float tempoLimiteSegundos = 30;

    float tamanhoAtual = calculaCustoRota(rotaFinal);

    while (encontrouMelhoria == True)
    {
        encontrouMelhoria = False;
        for (int i = 0; i < n - 1; i++)
        {
            for (int j = i + 2; j < n; j++)
            {
                if (((double) (clock() - atual)) / CLOCKS_PER_SEC > tempoLimiteSegundos)
                {
                    break;
                }
                float d1 = -calculaDistancia(&(listaDeVertices[rotaFinal[i]]), &(listaDeVertices[rotaFinal[(i+1)]]));
                float d2 = -calculaDistancia(&(listaDeVertices[rotaFinal[j]]), &(listaDeVertices[rotaFinal[(j+1) % n]]));
                float d3 = calculaDistancia(&(listaDeVertices[rotaFinal[i]]), &(listaDeVertices[rotaFinal[j]]));
                float d4 = calculaDistancia(&(listaDeVertices[rotaFinal[(i+1)]]), &(listaDeVertices[rotaFinal[(j+1) % n]]));

                float delta = d1 + d2 + d3 + d4;

                if (delta < 0)
                {
                    atual = clock();
                    trocar_pontas(rotaFinal, i, j);
                    tamanhoAtual += delta;
                    encontrouMelhoria = True;
                    // printf("\nEncontrou melhoria! Custo atual: %f", tamanhoAtual);
                    // printTimestamp(tamanhoAtual);
                }
            }
        }
    }
}

void salvaResultados(float custoTotal, float tempoGasto)
{
    FILE* arquivoDeSaida;

    arquivoDeSaida = fopen("resultados-IMD.txt", "a");

    // fprintf(arquivoDeSaida, "++++++++++++++++++  %s  ++++++++++++++++++", nomeInstancia);

    fprintf(arquivoDeSaida, "\nTempo computando (em segundos): ");
    fprintf(arquivoDeSaida, "%f", tempoGasto);
    fprintf(arquivoDeSaida, "\nCusto total: ");
    fprintf(arquivoDeSaida, "%f\n", custoTotal);

    fclose(arquivoDeSaida);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 3) {
        printf("\nArgumentos incorretos, uso correto:\n[programa] arquivo_de_entrada.tsp [n:0-5]");
        printf("\n\nArgumentos:\n -- [programa]: O executavel compilado\n -- arquivo_de_entrada.tsp: O arquivo contendo a instancia a ser executada");
        printf("\n -- n: Um inteiro de 0 a 5 indicando qual heuristica sera utilizada:");
        printf("\n -- >> 0: Vizinho mais proximo");
        printf("\n -- >> 1: Insercao do vizinho mais distante + escolha do ciclo inicial (opcao 0, 1 ou 2)");
        printf("\n -- >> 2: Vizinho mais proximo + 2-opt");
        printf("\n -- >> 3: Vizinho mais proximo + Pair Swap");
        printf("\n -- >> 4: Insercao do vizinho mais distante + 2-opt");
        printf("\n -- >> 5: Insercao do vizinho mais distante + Pair Swap");
        return 1;
    }

    int algoritmoEscolhido = atoi(argv[2]);

    if (algoritmoEscolhido < 0 || algoritmoEscolhido > 5) {
        printf("\nErro: n deve ser um valor entre 0 e 5");
        return 1;
    }

    FILE *arquivoEntrada = fopen(argv[1], "r");
    FILE *arquivoTimestamp = fopen("timestamp.txt", "w+");

    if (arquivoEntrada == NULL) {
        printf("\nErro ao abrir arquivo de entrada");
        return 1;
    }

    if (!terminaCom(argv[1], ".tsp")) {
        printf("\nArquivo de entrada deve ser do tipo .tsp");
        return 1;
    }

    lerArquivo(arquivoEntrada, &listaDeVertices, &dimensao);

    int *rotaFinal = malloc(sizeof(int) * (dimensao + 1));

    clock_t start, end;
    start = clock();

    if (algoritmoEscolhido == 0) {
        printf("\nAlgoritmo VIZINHO MAIS PROXIMO escolhido");
        vizinhoMaisProximo(rotaFinal);
    } else if (algoritmoEscolhido == 1) {
        printf("\nAlgoritmo INSERCAO VIZINHO DO MAIS DISTANTE escolhido");
        if (argc < 4) {
            printf("\nErro: escolha de ciclo inicial nao fornecida");
            return 1;
        }
        int cicloInicial = atoi(argv[3]);
        if (cicloInicial == 0) {
            int v1, v2, v3;
            cicloInicialDet(&v1, &v2, &v3);
            insercaoMaisDistante(rotaFinal, v1, v2, v3);
        } else if (cicloInicial == 1) {
            insercaoMaisDistante(rotaFinal, 0, 1, 2);
        } else if (cicloInicial == 2) {
            insercaoMaisDistante(rotaFinal, dimensao - 1, dimensao - 2, dimensao - 3);
        } else {
            printf("\nErro! Escolha de ciclo inicial <<%d>> invalida!", cicloInicial);
            return 1;
        }
    } else if (algoritmoEscolhido == 2) {
        printf("\nAlgoritmo VIZINHO MAIS PROXIMO com 2-OPT escolhido");
        vizinhoMaisProximo(rotaFinal);
        doisOpt(rotaFinal);
    } else if (algoritmoEscolhido == 3) {
        printf("\nAlgoritmo VIZINHO MAIS PROXIMO com PAIR SWAP escolhido");
        vizinhoMaisProximo(rotaFinal);
        pairSwap(rotaFinal);
    } else if (algoritmoEscolhido == 4) {
        printf("\nAlgoritmo INSERCAO VIZINHO DO MAIS DISTANTE com 2-OPT escolhido");
        insercaoMaisDistante(rotaFinal, 0, 1, 2);
        doisOpt(rotaFinal);
    } else if (algoritmoEscolhido == 5) {
        printf("\nAlgoritmo INSERCAO VIZINHO DO MAIS DISTANTE com PAIR SWAP escolhido");
        insercaoMaisDistante(rotaFinal, 0, 1, 2);
        pairSwap(rotaFinal);
    } else {
        printf("\nValor nao reconhecido, n deve ser um inteiro entre 0 e 5");
        return 1;
    }

    end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    float custoTotal = calculaCustoRota(rotaFinal);

    printf("\nFinalizado! Tempo gasto: %lf", cpu_time_used);
    printf("\nRota calculada: %f\n", custoTotal);

    exportaResultados(rotaFinal, custoTotal, argv[1], cpu_time_used);

    free(listaDeVertices);
    free(rotaFinal);
    fclose(arquivoEntrada);
    fclose(arquivoTimestamp);

    return 0;
}
