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

    // Parâmetros:

    /*
        Windows:
            executavel.exe [instancia] [construcao_inicial] [tamanho_populacao] [chance_mutacao] [criterio_parada]

        Linux:
            ./executavel [instancia] [construcao_inicial] [tamanho_populacao] [chance_mutacao] [criterio_parada]

        Onde:
        [instancia]: O caminho da instância a ser executada
        [construcao_inicial]: 0 para VMP e 1 para IMVD
        [tamanho_populacao]: Um valor inteiro positivo para o tamanho da população (>= 0)
        [chance_mutacao]: Um valor entre 0-1 que indica a chance de alguma mutação ocorrer em um indivíduo
        [criterio_parada]: O número de gerações sem melhoria para parar o algoritmo 
    */  
    
    if (argc != 6) {
        printf("\nArgumentos incorretos, uso correto: \n[programa] arquivo_de_entrada.tsp [construcao_inicial] [tamanho_populacao] [chance_mutacao] [criterio_parada]");
        printf("\n\nArgumentos:\n -- [programa]: O executavel compilado\n -- arquivo_de_entrada.tsp: O arquivo contendo a instancia a ser executada");
        printf("\n -- [construcao_inicial]: Indica qual heuristica sera utilizada:");
        printf("\n -- >> 0: Vizinho mais proximo");
        printf("\n -- >> 1: Insercao do vizinho mais distante");
        printf("\n -- [tamanho_populacao]: Um valor inteiro positivo para o tamanho da populacao (>= 0)");
        printf("\n -- [chance_mutacao]: Um valor entre 0 e 1 que indica a chance de alguma mutacao ocorrer em um individuo");
        printf("\n -- [criterio_parada]: O numero de geracoes sem melhoria para parar o algoritmo ");
        return 1;
    }

    int algoritmoConstrucaoInicial = atoi(argv[2]);
    int tamanhoPopulacao = atoi(argv[3]);
    float chanceMutacao = atof(argv[4]);
    int numeroGeracoesSemMelhoriaParaParar = atoi(argv[5]);

    if (algoritmoConstrucaoInicial != 0 && algoritmoConstrucaoInicial != 1)
    {
        printf("\nErro: O algoritmo de construcao inicial deve ser 0 ou 1");
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

    // int *rotaFinal = malloc(sizeof(int) * (dimensao + 1));

    clock_t start, end;
    start = clock();

    booleano atingiuCriterioParada = False;

    int contadorGeracoesSemMelhoria = numeroGeracoesSemMelhoriaParaParar;

    printf("\nIniciando construcao inicial");

    int custoMelhorRotaConhecida;
    int indiceMelhorRotaConhecida;

    // Alocando espaço para a população
    int *populacao = malloc(sizeof(int) * tamanhoPopulacao);

    for (int i = 0; i < tamanhoPopulacao; i++)
    {
        populacao[i] = malloc(sizeof(int) * (dimensao + 1));
    }

    if (algoritmoConstrucaoInicial == 0)
    {
        printf("\nVizinho mais proximo escolhido");
        // gerarPopulacaoInicialVMP();
    }
    else 
    {
        printf("\nInsercao do vizinho mais distante escolhida");
        // gerarPopulacaoInicialIVMD();
    }

    // buscar na populacao a melhor rota e atualizar o custoMelhorRotaConhecida e o seu indice
    // salvar o valor no timestamp

    while (atingiuCriterioParada == False)
    {
        // avaliarCromossomos();
        // selecionarCromossomos();
        // cruzarCromossomos();
        // mutarCromossomos();
        // buscaLocalCromossomos();
        // atualizarPopulacao();

        // buscar na populacao a melhor rota

        // se ela for melhor que a melhorRotaConhecida, atualizar ela e o seu indice
        //     redefinir o contadorGeracoesSemMelhoria para o valor inicial

        // se não
        //     subtrair 1 do contadorGeracoesSemMelhoria

        // se o contador for igual a zero, atingiuCriterioDeParada = True

    }

    end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("\nFinalizado! Tempo gasto: %lf", cpu_time_used);
    printf("\nRota calculada: %f\n", custoMelhorRotaConhecida);

    exportaResultados(populacao[indiceMelhorRotaConhecida], custoMelhorRotaConhecida, argv[1], cpu_time_used);

    free(listaDeVertices);

    for (int i = 0; i < tamanhoPopulacao; i++)
    {
        free(populacao[i]);
    }

    free(populacao);
    
    fclose(arquivoEntrada);
    fclose(arquivoTimestamp);

    return 0;
}
