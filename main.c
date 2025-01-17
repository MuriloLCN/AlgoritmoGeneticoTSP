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
#define LIMITE_ALOCACAO 500

typedef struct populacao
{
    int tamanho;
    int **cromossomo;
    float *avaliacao;
}populacao;

FILE* arquivoTimestamp;
coordenada* listaDeVertices;
int dimensao;
clock_t inicioMelhoramento;

void vizinhoMaisProximo(int* rotaFinal);
float calculaCustoRota(int* rota);

populacao* gerarPopulacaoInicial(int tamanho)
{
    if (tamanho > LIMITE_ALOCACAO)
    {
        printf("Limite de alocação atingido!");
        return NULL;
    }
    
    populacao* nova_populacao = malloc(sizeof(populacao));
    nova_populacao->tamanho = tamanho;
    nova_populacao->avaliacao = malloc(sizeof(float) * tamanho);
    nova_populacao->cromossomo = malloc(sizeof(int*) * tamanho);

    // loop que inicializa cada cromossomo
    for (int i = 0; i < tamanho; i++)
    {
        nova_populacao->avaliacao[i] = 0.0;
        nova_populacao->cromossomo[i] = malloc(sizeof(int) * (dimensao + 1));
        vizinhoMaisProximo(nova_populacao->cromossomo[i]);
    }
    
    return nova_populacao;
}

void avaliarCromossomos (populacao* populacao_atual)
{
    for (int i = 0; i < populacao_atual->tamanho; i++)
    {
        populacao_atual->avaliacao[i] = calculaCustoRota(populacao_atual->cromossomo[i]);
    }
}

booleano pertence_regiao(float lx, float ly, coordenada coordenadaInicial, coordenada atual)
{
    if ((atual.x <= (coordenadaInicial.x + lx)) && (atual.x >= (coordenadaInicial.x - lx)) && (atual.y <= (coordenadaInicial.y + ly)) && (atual.y >= (coordenadaInicial.y - ly)))
        return True;
    else 
        return False;
}

void calcula_variacao_coordenadas (float* dx, float* dy)
{
    float max_x = -INFINITY, min_x = INFINITY, max_y = -INFINITY, min_y = INFINITY;

    for (int i = 0; i < dimensao; i++)
    {
        if ((&listaDeVertices[i])->x > max_x)
            max_x = (&listaDeVertices[i])->x;
    }

    for (int i = 0; i < dimensao; i++)
    {
        if ((&listaDeVertices[i])->y > max_y)
            max_y = (&listaDeVertices[i])->y;
    }

    for (int i = 0; i < dimensao; i++)
    {
        if ((&listaDeVertices[i])->x < min_x)
            min_x = (&listaDeVertices[i])->x;
    }

    for (int i = 0; i < dimensao; i++)
    {
        if ((&listaDeVertices[i])->x < min_y)
            min_y = (&listaDeVertices[i])->y;
    }

    *dx = max_x - min_x;
    *dy = max_y - min_y;
}

void gerar_conjunto_pertencente_regiao (float alpha, int* rotaFinal, int* vet)
{
    // calcula Xw e Yw
    float x_largura, y_altura;
    calcula_variacao_coordenadas(&x_largura, &y_altura);
    // calula Lx e Ly
    float lx = x_largura * alpha;
    float ly = y_altura * alpha;

    // sorteia a cidade
    int ind_cidade_inicial = rand() % dimensao;
    coordenada cidade_inicial = listaDeVertices[ind_cidade_inicial];

    // aloca dinamicamente vet
    vet = malloc(sizeof(int) * (dimensao+1));
    for (int i = 0; i <= dimensao; i++)
    {
        vet[i] = 0;
    }

    // pra cada cidade da rota final, veficiar se pertence a regiao e atualizar o vet
    for (int i = 0; i <= dimensao; i++)
    {
        if (pertence_regiao(lx, ly, cidade_inicial, listaDeVertices[rotaFinal[i]]))
        {
            vet[rotaFinal[i]] = 1;
        }
    }
}

void printTimestamp(float custo)
{
    fprintf(arquivoTimestamp, "%f %f \n", ((float) (clock() - inicioMelhoramento)) / CLOCKS_PER_SEC, custo);
}

float calculaDistancia(coordenada* c1, coordenada* c2)
{
    float dx = c1->x - c2->x;
    float dy = c1->y - c2->y;
    return sqrt(dx*dx + dy*dy);
}

void vizinhoMaisProximo(int* rota)
{
    int atual = (int)rand() % dimensao;
    int inicial = atual;

    int indiceRota = 0;

    booleano visitados[dimensao];

    for (int i = 0; i < dimensao; i++)
    {
        visitados[i] = False;
    }

    visitados[atual] = True;
    rota[indiceRota] = atual;
    indiceRota++;

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
        atual = prox;

        rota[indiceRota] = atual;
        indiceRota++;
    }

    rota[indiceRota] = inicial;
}

void troca(int* rota, int i, int j)
{
    int buffer = rota[i];
    rota[i] = rota[j];
    rota[j] = buffer;    
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

void doisOpt(populacao* pop, int i)
{
    /*
        Realiza a heurística de melhoramento 2-opt aplicado ao first improvement. Apenas um passo.

        Baseado em: https://en.wikipedia.org/wiki/2-opt

        Parâmetros:
            *rotaFinal: um vetor com DIMESAO+1 elementos que define a ordem dos vértices a serem visitados
    */
    int* rotaFinal = pop->cromossomo[i];
    
    int n = dimensao;
    
    // TODO: Trabalhar com o i-ésimo elemento da população para evitar re-calcular o custo a cada vez

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = i + 2; j < n; j++)
        {
            float d1 = -calculaDistancia(&(listaDeVertices[rotaFinal[i]]), &(listaDeVertices[rotaFinal[(i+1)]]));
            float d2 = -calculaDistancia(&(listaDeVertices[rotaFinal[j]]), &(listaDeVertices[rotaFinal[(j+1) % n]]));
            float d3 = calculaDistancia(&(listaDeVertices[rotaFinal[i]]), &(listaDeVertices[rotaFinal[j]]));
            float d4 = calculaDistancia(&(listaDeVertices[rotaFinal[(i+1)]]), &(listaDeVertices[rotaFinal[(j+1) % n]]));

            float delta = d1 + d2 + d3 + d4;

            if (delta < 0)
            {
                trocar_pontas(rotaFinal, i, j);
                pop->avaliacao[i] += delta;
            }
        }
    }
    
}

booleano presente(int* ciclo, int tamanho, int vertice) {
    for (int i = 0; i < tamanho - 1; i++) {  // Ignorar última posição (ciclo fechado)
        if (ciclo[i] == vertice) {
            return True;
        }
    }
    return False;
}

void exx_crossover(int* pai1, int* pai2, int* filho1, int* filho2) {
    // EXX (Edge Exchange Crossover)

    int tamanho = dimensao + 1;

    // Inicializa os filhos com -1 (para identificar posições vazias)
    for (int i = 0; i < tamanho; i++) {
        filho1[i] = -1;
        filho2[i] = -1;
    }

    // Seleciona aleatoriamente dois pontos de corte
    int pontoInicio = rand() % (tamanho - 2);
    int pontoFim = pontoInicio + 1 + rand() % (tamanho - pontoInicio - 1);

    // Copia segmento do pai1 para filho1 e do pai2 para filho2
    for (int i = pontoInicio; i <= pontoFim; i++) {
        filho1[i] = pai1[i];
        filho2[i] = pai2[i];
    }

    // Preenche os filhos com os vértices restantes
    int posicao1 = 0, posicao2 = 0;
    for (int i = 0; i < tamanho - 1; i++) {  // Ignora última posição do ciclo

        if (!presente(filho1, tamanho, pai2[i])) {
            while (filho1[posicao1] != -1) {
                posicao1++;
            }
            filho1[posicao1] = pai2[i];
        }

        if (!presente(filho2, tamanho, pai1[i])) {
            while (filho2[posicao2] != -1) {
                posicao2++;
            }
            filho2[posicao2] = pai1[i];
        }
    }

    // Garante que os filhos sejam ciclos fechados
    filho1[tamanho - 1] = filho1[0];
    filho2[tamanho - 1] = filho2[0];
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    // Parâmetros:

    /*
        Windows:
            executavel.exe [instancia] [operador_cruzamento] [tamanho_populacao] [chance_mutacao] [criterio_parada]

        Linux:
            ./executavel [instancia] [operador_cruzamento] [tamanho_populacao] [chance_mutacao] [criterio_parada]

        Onde:
        [instancia]: O caminho da instância a ser executada
        [operador_cruzamento]: 0 para [operador 1] e 1 para [operador 2]
        [tamanho_populacao]: Um valor inteiro positivo para o tamanho da população (>= 0)
        [chance_mutacao]: Um valor entre 0-1 que indica a chance de alguma mutação ocorrer em um indivíduo
        [criterio_parada]: O número de gerações sem melhoria para parar o algoritmo 
    */  
    
    if (argc != 6) {
        printf("\nArgumentos incorretos, uso correto: \n[programa] arquivo_de_entrada.tsp [operador_cruzamento] [tamanho_populacao] [chance_mutacao] [criterio_parada]");
        printf("\n\nArgumentos:\n -- [programa]: O executavel compilado\n -- arquivo_de_entrada.tsp: O arquivo contendo a instancia a ser executada");
        printf("\n -- [operador_cruzamento]: Indica qual heuristica sera utilizada:");
        printf("\n -- >> 0: Operador 1");
        printf("\n -- >> 1: Operador 2");
        printf("\n -- [tamanho_populacao]: Um valor inteiro positivo para o tamanho da populacao (>= 0)");
        printf("\n -- [chance_mutacao]: Um valor entre 0 e 1 que indica a chance de alguma mutacao ocorrer em um individuo");
        printf("\n -- [criterio_parada]: O numero de geracoes sem melhoria para parar o algoritmo ");
        return 1;
    }

    int algoritmoCruzamento = atoi(argv[2]);
    int tamanhoPopulacao = atoi(argv[3]);
    float chanceMutacao = atof(argv[4]);
    int numeroGeracoesSemMelhoriaParaParar = atoi(argv[5]);

    if (algoritmoCruzamento != 0 && algoritmoCruzamento != 1)
    {
        printf("\nErro: O operador de cruzamento deve ser 0 ou 1");
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

    clock_t start, end;
    start = clock();

    booleano atingiuCriterioParada = False;

    int contadorGeracoesSemMelhoria = numeroGeracoesSemMelhoriaParaParar;

    printf("\nIniciando construcao inicial");

    int custoMelhorRotaConhecida = INFINITY;
    int indiceMelhorRotaConhecida = -1;

    populacao* pop = gerarPopulacaoInicial(tamanhoPopulacao);

    for (int i = 0; i < pop->tamanho; i++)
    {
        if (pop->avaliacao[i] < custoMelhorRotaConhecida)
        {
            custoMelhorRotaConhecida = pop->avaliacao[i];
            indiceMelhorRotaConhecida = i;
        }
    }

    printTimestamp(custoMelhorRotaConhecida);

    while (atingiuCriterioParada == False)
    {
        avaliarCromossomos(pop);
        // selecionarCromossomos();
        // cruzarCromossomos();
        // mutarCromossomos();

        for (int i = 0; i < pop->tamanho; i++)
        {
            doisOpt(pop, i);
        }
        
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

    exportaResultados(pop->cromossomo[indiceMelhorRotaConhecida], custoMelhorRotaConhecida, argv[1], cpu_time_used);

    free(listaDeVertices);

    for (int i = 0; i < pop->tamanho; i++)
    {
        free(pop->cromossomo[i]);
    }

    free(pop->cromossomo);
    free(pop->avaliacao);

    fclose(arquivoEntrada);
    fclose(arquivoTimestamp);

    return 0;
}
