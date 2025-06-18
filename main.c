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
#include<limits.h>
#include<pthread.h>
#include"leitura_arquivo.h"

#define TAM_BUFFER_ENTRADA 1024
#define LIMITE_ALOCACAO 500
#define MAX_THREADS 16

typedef struct populacao
{
    int tamanho;
    int **cromossomo;
    float *avaliacao;
}populacao;

// typedef struct packVizinhoMaisProximo {
//     int* vetor;
//     int custo;
// }packVizinhoMaisProximo;

typedef struct packVizinhoMaisProximo {
    int **vetor;
    float *custo;
    int inicio, fim;
}packVizinhoMaisProximo;

typedef struct packCruzamento {
    int inicio, fim, *paisSelecionados;
    populacao *pop, *filhosGerados;
}packCruzamento;

typedef struct packCalculaCustoRota {
    populacao* pop;
    int inicio, fim;
}packCalculaCustoRota;

typedef struct packDoisOpt {
    populacao* pop;
    int k;
}packDoisOpt;

int numeroDePaisSelecionadosParaCruzamento;
float chanceMutacao;

FILE* arquivoTimestamp;
FILE* arquivoTempoConstrucao;
coordenada* listaDeVertices;
int dimensao;
int limiteExecucaoHoras = 6;
int algoritmoCruzamento;
int numeroThreads;
clock_t inicioMelhoramento;
clock_t inicioExecucao;
float alpha;

void *vizinhoMaisProximo(void* ptr);
void* calculaCustoRota(void* ptr);
float calculaDistancia(coordenada* c1, coordenada* c2);
void copiarRota(int* fonte, int* destino);

int randMelhorado()
{
    unsigned int alto = (unsigned int)rand();
    unsigned int baixo = (unsigned int)rand();

    unsigned int resultado = ((alto << 15) | (baixo & 0x7FFF)) & 0x7FFFFFFF;
    
    return (int)(resultado % ((unsigned int)INT_MAX + 1));
}

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

    int intervalo = tamanho / numeroThreads;
    int resto = tamanho - (intervalo * numeroThreads);

    // float custo;
    // loop que inicializa cada cromossomo

    pthread_t threadPool[numeroThreads];
    packVizinhoMaisProximo packPool[numeroThreads];

    for (int i = 0; i < tamanho; i++)
    {
        // printf("\nCriando packPool (%d de %d)", i, tamanho);
        nova_populacao->cromossomo[i] = malloc(sizeof(int) * (dimensao + 1));
        nova_populacao->avaliacao[i] = 0.0;
        // packPool[i].vetor = nova_populacao->cromossomo[i];
        // packPool[i].custo = 0;
    }

    int ultimoIndice = 0;
    // distribui a criação dos cromossomos entre as threads em intervalos
    for (int i = 0; i < numeroThreads; i++)
    {
        packPool[i].vetor = nova_populacao->cromossomo;
        packPool[i].custo = nova_populacao->avaliacao;

        packPool[i].inicio = ultimoIndice;
        packPool[i].fim = ultimoIndice + intervalo - 1;
        if (resto) // verifica se ha resto a ser dividido entre as threads (evita que a ultima thread fique sobrecarregada com mais trabalho)
        {
            packPool[i].fim++;
            resto--;
        }

        printf("\nThread %d: Gerando cromossomos do índice %d até %d", i, packPool[i].inicio, packPool[i].fim);
        ultimoIndice = packPool[i].fim + 1;
    }

    for (int i = 0; i < numeroThreads; i++)
    {
        // printf("\nIniciando threads (%d de %d)", i, tamanho);
        pthread_create(&(threadPool[i]), NULL, vizinhoMaisProximo, (void*)&(packPool[i]));
    }

    for (int i = 0; i < numeroThreads; i++)
    {
        // printf("\nJoining threads (%d de %d)", i, tamanho);
        pthread_join(threadPool[i], NULL);

        // nova_populacao->avaliacao[i] = packPool[i].custo;
    }

    // for (int i = 0; i < tamanho; i++)
    // {
    //     // nova_populacao->avaliacao[i] = 0.0;
    //     printf("\nGerando cromossomo %d de %d", i+1, tamanho);
    //     nova_populacao->cromossomo[i] = malloc(sizeof(int) * (dimensao + 1));
    //     vizinhoMaisProximo(nova_populacao->cromossomo[i], &custo);
    //     nova_populacao->avaliacao[i] = custo;
    // }
    
    return nova_populacao;
}

void ordenaPopulacao(populacao *pop) {
    for (int i = 0; i < pop->tamanho - 1; i++) {
        for (int j = 0; j < pop->tamanho - i - 1; j++) {
            if (pop->avaliacao[j] > pop->avaliacao[j + 1]) {
                float tempAvaliacao = pop->avaliacao[j];
                pop->avaliacao[j] = pop->avaliacao[j + 1];
                pop->avaliacao[j + 1] = tempAvaliacao;

                int *tempCromossomo = pop->cromossomo[j];
                pop->cromossomo[j] = pop->cromossomo[j + 1];
                pop->cromossomo[j + 1] = tempCromossomo;
            }
        }
    }
}

void avaliarCromossomos (populacao* populacao_atual)
{
    pthread_t threadPool[numeroThreads];
    packCalculaCustoRota packPool[numeroThreads];

    int intervalo = populacao_atual->tamanho / numeroThreads;
    int resto = populacao_atual->tamanho - (intervalo * numeroThreads);

    int ultimoElemento = -1;

    for (int i = 0; i < numeroThreads; i++)
    {
        packPool[i].pop = populacao_atual;

        packPool[i].inicio = ultimoElemento + 1;
        packPool[i].fim = packPool[i].inicio + intervalo;

        if (resto)
        {
            packPool[i].fim += 1;
            resto -= 1;
        }
    }

    for (int i = 0; i < numeroThreads; i++)
    {
        pthread_create(&(threadPool[i]), NULL, calculaCustoRota, (void*)&(packPool[i]));
    }

    for (int i = 0; i < numeroThreads; i++)
    {
        pthread_join(threadPool[i], NULL);
    }

    // for (int i = 0; i < numeroThreads; i++)
    // {
    //     populacao_atual->avaliacao[i] = packPool[i].custo;
    // }
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
        if ((&listaDeVertices[i])->y < min_y)
            min_y = (&listaDeVertices[i])->y;
    }

    *dx = max_x - min_x;
    *dy = max_y - min_y;
}

void gerar_conjunto_pertencente_regiao (float alpha, int* rotaFinal, int* vet, coordenada cidade_inicial)
{
    // calcula Xw e Yw
    float x_largura, y_altura;
    calcula_variacao_coordenadas(&x_largura, &y_altura);
    // calula Lx e Ly
    float lx = x_largura * alpha;
    float ly = y_altura * alpha;
    
    // pra cada cidade da rota final, veficiar se pertence a regiao e atualizar o vet
    int num_cidades = 0;
    for (int i = 0; i < dimensao; i++)
    {
        if (pertence_regiao(lx, ly, cidade_inicial, listaDeVertices[rotaFinal[i]]))
        {
            vet[i] = 1;
            num_cidades++;
        }
    }
}

float menor_distancia(int* bin_genitor, int indice_cidade, int* indice_genitor, int bin)
{
    int indice_cidade_mais_proxima, i = 0, n_bin, elementos_percorridos = 0;
    float menor_distancia = INFINITY, temp_distancia;

    // n_bin eh negacao de bin
    if (bin == 0) {n_bin = 1;}
    else {n_bin = 0;}  

    // encontra o primeiro conjunto de bin's do INICIO
    while ((bin_genitor[i % dimensao] == bin) && (elementos_percorridos <= dimensao)) 
    {
        elementos_percorridos++;
        i++;
    }
    // tem que começar em uma posição com valor 0, caso contrário complica para detectar o componente conexo
    while ((bin_genitor[i % dimensao] == n_bin) && (elementos_percorridos <= dimensao)) 
    {
        elementos_percorridos++;
        i++;
    }
    
    // evita loop infinito
    if (elementos_percorridos > dimensao)
    {
        *indice_genitor = -1;
        return INFINITY;
    }

    else 
        elementos_percorridos = 0;

    while (elementos_percorridos < dimensao)
    {
        // achou um componente conexo
        if (bin_genitor[i % dimensao] == bin) 
        {
            temp_distancia = calculaDistancia(&listaDeVertices[i % dimensao], &listaDeVertices[indice_cidade % dimensao]);
            if (temp_distancia < menor_distancia)
            {
                menor_distancia = temp_distancia;
                indice_cidade_mais_proxima = i % dimensao;
            }

            while ((bin_genitor[i % dimensao] == bin) && (elementos_percorridos < dimensao))
            {
                i++;
                elementos_percorridos++;
            } 
        }

        // se não for o que estou procurando, pula
        i++;
        elementos_percorridos++;
    }
    
    *indice_genitor = indice_cidade_mais_proxima;

    return menor_distancia;
}

int* zx (int* genitor1, int* genitor2, int* filho)
{
    int bin_genitor1[dimensao+1];
    int bin_genitor2[dimensao+1];

    for (int i = 0; i <= dimensao; i++)
    {
        bin_genitor1[i] = 0;
        bin_genitor2[i] = 0;
        filho[i] = -1;
    }

    // sorteia a cidade
    int ind_cidade_inicial = randMelhorado() % dimensao;
    coordenada cidade_inicial = listaDeVertices[ind_cidade_inicial];

    gerar_conjunto_pertencente_regiao(alpha, genitor1, bin_genitor1, cidade_inicial);
    gerar_conjunto_pertencente_regiao(alpha, genitor2, bin_genitor2, cidade_inicial);

    float distancia_genitor1, distancia_genitor2;
    int indice_filho = 0, indice_genitor1 = 0, indice_genitor2 = 0, cidades_inseridas = 0;
    
    // encontra o primeiro conjunto de 0's do INICIO
    while (bin_genitor2[indice_genitor2 % dimensao] == 0) {indice_genitor2++;}
    while (bin_genitor2[indice_genitor2 % dimensao] == 1) {indice_genitor2++;}
    while (bin_genitor2[indice_genitor2 % dimensao] == 0)
    {
        filho[indice_filho % dimensao] = genitor2[indice_genitor2 % dimensao];
        bin_genitor2[indice_genitor2 % dimensao] = 1;
        indice_filho++;
        indice_genitor2++;
        cidades_inseridas++;
    }
    
    while (indice_filho < dimensao)
    {
        // se tivermos uma desconexão, procura nos dois genitores qual componente conexo conectar
        distancia_genitor1 = menor_distancia(bin_genitor1, filho[(indice_filho-1)], &indice_genitor1, 1);
        distancia_genitor2 = menor_distancia(bin_genitor2, filho[(indice_filho-1)], &indice_genitor2, 0);

        if ((distancia_genitor1 == INFINITY) && (distancia_genitor2 == INFINITY))
        {
            // printf("deu pau\n");
            break;
        }

        // próximo componente a ser conectado fica no genior 1
        if (distancia_genitor1 <= distancia_genitor2)
        {
            while((bin_genitor1[(indice_genitor1 + dimensao) % dimensao]) == 1 && (indice_filho < dimensao))
            {
                filho[indice_filho] = genitor1[(indice_genitor1 + dimensao) % dimensao];
                bin_genitor1[(indice_genitor1 + dimensao) % dimensao] = 0; // marcando que já foi selecionado e desconsiderando para as próximas procuras
                indice_filho++;
                indice_genitor1++;
                cidades_inseridas ++;
            }
        }

        else if (distancia_genitor1 > distancia_genitor2)
        {
            while((bin_genitor2[(indice_genitor2 + dimensao) % dimensao] == 0) && (indice_filho < dimensao))
            {
                filho[indice_filho] = genitor2[(indice_genitor2 + dimensao) % dimensao];
                bin_genitor2[(indice_genitor2 + dimensao) % dimensao] = 1; // marcando que já foi selecionado e desconsiderando para as próximas procuras
                indice_filho++;
                indice_genitor2++;
                cidades_inseridas++;
            }
        }       
    }
    filho[dimensao] = filho[0];

    return filho;       
}

void ha_repetidos(int vetor[]) {
    for (int i = 0; i < dimensao; i++) {
        for (int j = i + 1; j < dimensao; j++) {
            if (vetor[i] == vetor[j]) {
                printf("Repetido: %d %d\n", vetor[i], vetor[j]);
            }
        }
    }
}

void printTempoConstrucao(float tempo)
{
    fprintf(arquivoTempoConstrucao, "%f", tempo);
}

void printTimestamp(float custo)
{
    fprintf(arquivoTimestamp, "%f %f \n", ((float) (clock() - inicioMelhoramento)) / CLOCKS_PER_SEC, custo);
}

void printTimestampIteracao(float custo, int iter, float custoMedio, float custoPiorPop, float custoMelhorPop)
{
    fprintf(arquivoTimestamp, "%d %f %f %f %f %f\n", iter, ((float) (clock() - inicioMelhoramento)) / CLOCKS_PER_SEC, custo, custoMedio, custoPiorPop, custoMelhorPop);
}

float calculaDistancia(coordenada* c1, coordenada* c2)
{
    float dx = c1->x - c2->x;
    float dy = c1->y - c2->y;
    return sqrt(dx*dx + dy*dy);
}

void *vizinhoMaisProximo(void* ptr)
{
    packVizinhoMaisProximo* pack;
    pack = (packVizinhoMaisProximo*)ptr;
    int** rota = pack->vetor;
    float* custo = pack->custo;
    // int atual = (int)randMelhorado() % dimensao;
    // int inicial = atual;
    int atual, inicial, indiceCromossomoAtual, indiceRota;

    // int indiceRota = 0;

    booleano visitados[dimensao];

    for (indiceCromossomoAtual = pack->inicio; indiceCromossomoAtual <= pack->fim; indiceCromossomoAtual++)
    {
        atual = (int)randMelhorado() % dimensao;
        inicial = atual;
        indiceRota = 0;

        for (int i = 0; i < dimensao; i++)
        {
            visitados[i] = False;
        }

        visitados[atual] = True;
        rota[indiceCromossomoAtual][indiceRota] = atual;
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
                exit(1);
            }

            visitados[prox] = 1;
            distanciaTotal += menorDistancia;
            atual = prox;

            rota[indiceCromossomoAtual][indiceRota] = atual;
            indiceRota++;
        }

        rota[indiceCromossomoAtual][indiceRota] = inicial;
        distanciaTotal += calculaDistancia(&listaDeVertices[atual], &listaDeVertices[inicial]);
        pack->custo[indiceCromossomoAtual] = distanciaTotal;
    }
        // printf("\nDistancia total percorrida: %2f\n", distanciaTotal);

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

void* calculaCustoRota(void* ptr)
{
    packCalculaCustoRota* pack;
    pack = (packCalculaCustoRota*) ptr;
    populacao* pop = pack->pop;

    for (int j = pack->inicio; j < pack->fim; j++)
    {
        float custoTotal = 0;
        int* rota = pop->cromossomo[j];

        for(int i = 0; i < dimensao; i++)
        {
            custoTotal += calculaDistancia(&listaDeVertices[rota[i]], &listaDeVertices[rota[i+1]]);
        }

        pop->avaliacao[j] = custoTotal;
    }
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

void* doisOpt(void* ptr)
{
    /*
        Realiza a heurística de melhoramento 2-opt aplicado ao first improvement. Apenas um passo.

        Baseado em: https://en.wikipedia.org/wiki/2-opt

        Parâmetros:
            pop: A população atual
            i: O indivíduo o qual se quer aplicar o 2-opt
    */
    
    packDoisOpt* pack;
    pack = (packDoisOpt*) ptr;

    populacao* pop = pack->pop;
    int k = pack->k;
    
    int n = dimensao;
    // int* rotaFinal = pop->cromossomo[k];

    int inicio_i = ((float) rand() / RAND_MAX) * (n - 1);
    int fim_i = inicio_i + 0.1 * dimensao;

    if (fim_i >= n)
    {
        fim_i = n - 1;
        inicio_i = fim_i - 0.1 * dimensao;
    }

    for (int i = inicio_i; i < fim_i; i++)
    {
        for (int j = i + 2; j < n; j++)
        {
            float d1 = -calculaDistancia(&(listaDeVertices[pop->cromossomo[k][i]]), &(listaDeVertices[pop->cromossomo[k][(i+1)]]));
            float d2 = -calculaDistancia(&(listaDeVertices[pop->cromossomo[k][j]]), &(listaDeVertices[pop->cromossomo[k][(j+1) % n]]));
            float d3 = calculaDistancia(&(listaDeVertices[pop->cromossomo[k][i]]), &(listaDeVertices[pop->cromossomo[k][j]]));
            float d4 = calculaDistancia(&(listaDeVertices[pop->cromossomo[k][(i+1)]]), &(listaDeVertices[pop->cromossomo[k][(j+1) % n]]));

            float delta = d1 + d2 + d3 + d4;

            if (delta < 0)
            {
                trocar_pontas(pop->cromossomo[k], i, j);
                // pop->avaliacao[i] += delta;
                // return;
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

void exx_crossover(int* pai1, int* pai2, int* filho) {
    // EXX (Edge Exchange Crossover)
    int tamanho = dimensao + 1;

    for (int i = 0; i < tamanho; i++) {
        filho[i] = -1;
    }

    int pontoInicio = randMelhorado() % dimensao;
    int pontoFim = randMelhorado() % dimensao;

    while (pontoInicio == pontoFim)
    {
        pontoInicio = randMelhorado() % dimensao;
    }

    if (pontoFim < pontoInicio)
    {
        int temp = pontoFim;
        pontoFim = pontoInicio;
        pontoInicio = temp;
    }

    for (int i = pontoInicio; i <= pontoFim; i++) {
        filho[i] = pai1[i];
    }

    int posicao = 0;
    for (int i = 0; i < tamanho - 1; i++) {
        if (!presente(filho, tamanho, pai2[i])) {
            while (filho[posicao] != -1) {
                posicao++;
            }
            filho[posicao] = pai2[i];
        }
    }

    filho[tamanho - 1] = filho[0];
}

void mutarCromossomo(populacao* pop, int i)
{
    /*
        Sorteia uma possível mutação no i-ésimo indivíduo da população
    */

    float num_aleatorio = (float) rand() / RAND_MAX;

    if (num_aleatorio < chanceMutacao)
    {
        float taxaAlteracao = ((float) rand()) / RAND_MAX;
        int numeroDeMutacoes = (int) (dimensao * chanceMutacao * taxaAlteracao) - 1;

        if (numeroDeMutacoes == 0)
        {
            numeroDeMutacoes = 1;
        }

        for (int j = 0; j < numeroDeMutacoes; j++)
        {
            int v1 = (int) randMelhorado() % dimensao - 1;
            int v2 = (int) randMelhorado() % dimensao - 1;

            if (v1 == v2)
            {
                continue;
            }

            if (v1 < 0)
            {
                v1 = 0;
            }

            if (v2 < 0)
            {
                v2 = 0;
            }

            if (v2 < v1)
            {
                int temp = v1;
                v1 = v2;
                v2 = temp;
            }

            troca(pop->cromossomo[i], v1, v2);
        }

    }
}

float* probabilidades;
booleano* selecionados;

void selecionarCromossomos(populacao* pop, int* paisSelecionados) {
    float totalFitness = 0.0;

    for (int i = 0; i < pop->tamanho; i++) {
        probabilidades[i] = 1.0 / pop->avaliacao[i];
        selecionados[i] = False;
        totalFitness += probabilidades[i];
    }

    for (int i = 0; i < pop->tamanho; i++) {
        probabilidades[i] /= totalFitness;
    }

    for (int i = 0; i < numeroDePaisSelecionadosParaCruzamento; i++) {
        float sorteio;
        int escolhido;
        do {
            sorteio = (float)rand() / RAND_MAX;
            float acumulado = 0.0;

            for (int j = 0; j < pop->tamanho; j++) {
                acumulado += probabilidades[j];
                if (sorteio <= acumulado) {
                    escolhido = j;
                    break;
                }
            }
        } while (selecionados[escolhido] == True);

        selecionados[escolhido] = True;
    }
    
    printf("\nfinalzin da selecao\n");

    // for (int i = 0; i < numeroDePaisSelecionadosParaCruzamento; i++)
    // {
    //     printf("\npais[%d] = %d", i, paisSelecionados[i]);
    // }
}

void *wrapperOperadorCruzamento (void* ptr)
{
    packCruzamento *pack;
    pack = (packCruzamento*) ptr;

    for (int i = pack->inicio; i <= pack->fim; i++)
    {
        int indicePai1 = pack->paisSelecionados[2 * i];
        int indicePai2 = pack->paisSelecionados[(2 * i) + 1];

        if (algoritmoCruzamento == 0)
        {
            zx(pack->pop->cromossomo[indicePai1], pack->pop->cromossomo[indicePai2], pack->filhosGerados->cromossomo[i]);
        }
        else
        {   
            exx_crossover(pack->pop->cromossomo[indicePai1], pack->pop->cromossomo[indicePai2], pack->filhosGerados->cromossomo[i]);
        }

    }
}

void cruzarCromossomos(populacao* pop, int* paisSelecionados, populacao* filhosGerados)
{
    /*
        Cruza os pais selecionados da população para gerar numeroDePaisSelecionadosParaCruzamento/2 filhos.
    */
    
    // TODO: paralelizar essa função de cruzamento
    // OBS.: não sei se paralelizar rotinas dentro dos operadores é vantajoso, devido ao overhead de criação de threads. 
    //       Nesse sentido, talvez seja melhor criar uma thread para um determinado intervalo de pais selecionados,
    //       seguindo a mesma ideia de vizinho mais próximo.

    // caso o numero de threads definido pelo usuário seja maior que o número de pais selecionados,
    // reduz o número de threads usadas para o número de pais selecionados
    int numeroDeThreadsUsadas = numeroThreads;
    if ((numeroDePaisSelecionadosParaCruzamento / 2) < numeroThreads)
    {
        numeroDeThreadsUsadas = numeroDePaisSelecionadosParaCruzamento;
    }

    // se vou fazer dois cruzamentos por thread, o numero do intervalo é 1, se forem feitos três cruzamentos por thread, o intervalo é 2, e assim por diante.
    int intervalo = ((numeroDePaisSelecionadosParaCruzamento / 2) / numeroDeThreadsUsadas) - 1;
    int resto = (numeroDePaisSelecionadosParaCruzamento / 2)  % numeroDeThreadsUsadas;

    // printf("\nnumeroDePaisSelecionadosParaCruzamento: %d", numeroDePaisSelecionadosParaCruzamento);
    // printf("\numeroDeThreadsUsadas: %d", numeroDeThreadsUsadas);
    // printf("\nintervalo: %d", intervalo);
    // printf("\nresto: %d", resto);

    pthread_t threadPool[numeroDeThreadsUsadas];
    packCruzamento packPool[numeroDeThreadsUsadas];
    int threadRet[numeroDeThreadsUsadas];

    int ultimoIndice = -1;
    for (int i = 0; i < numeroDeThreadsUsadas; i++)
    {   
        // inicialização básica, apenas para os pacotes apontarem para os mesmos endereços de memória
        packPool[i].pop = pop;
        packPool[i].paisSelecionados = paisSelecionados;
        packPool[i].filhosGerados = filhosGerados;

        packPool[i].inicio = ultimoIndice + 1;
        packPool[i].fim = packPool[i].inicio + intervalo;
        if (resto)
        {
            packPool[i].fim++;
            resto--;
        }

        ultimoIndice = packPool[i].fim;  
    }

    // disparando as threads
    for (int i = 0; i < numeroDeThreadsUsadas; i++)
    {
        threadRet[i] = pthread_create(&(threadPool[i]), NULL, wrapperOperadorCruzamento, (void*) &(packPool[i]));
    }
    
    for (int i = 0; i < numeroDeThreadsUsadas; i++)
    {
        pthread_join(threadPool[i], NULL);
    }

    // for (int i = 0; i < numeroDePaisSelecionadosParaCruzamento / 2; i++)
    // {
    //     int indicePai1 = paisSelecionados[2 * i];
    //     int indicePai2 = paisSelecionados[(2 * i) + 1];


    //     if (algoritmoCruzamento == 0)
    //     {
    //         zx(pop->cromossomo[indicePai1], pop->cromossomo[indicePai2], filhosGerados->cromossomo[i]);
    //     }
    //     else
    //     {   
    //         exx_crossover(pop->cromossomo[indicePai1], pop->cromossomo[indicePai2], filhosGerados->cromossomo[i]);
    //     }

        // printf("\nFilho gerado %d: ", i);
        // for (int j = 0; j <= dimensao; j++)
        // {
        //     printf("%d ", filhosGerados->cromossomo[i][j]);
        // }
    // }
}

void printarPopulacao(populacao* pop)
{
    printf("\nTamanho da populacao: %d", pop->tamanho);
    for (int i = 0; i < pop->tamanho; i++)
    {
        printf("\nCromossomo %d, avaliacao: %f, ordem: ", i, pop->avaliacao[i]);
        for (int j = 0; j <= dimensao; j++)
        {
            printf("%d ", pop->cromossomo[i][j]);
        }
    }
}

void atualizarPopulacao(populacao* populacaoAtual, populacao* novosIndividuos) {
    // Populações já estão ordenadas por aptidão

    int indice_pop_atual = 0;
    int indice_pop_nova = 0;

    for (int i = 0; i < populacaoAtual->tamanho; i++) {
        if (indice_pop_nova < novosIndividuos->tamanho &&
            (indice_pop_atual >= populacaoAtual->tamanho || 
            novosIndividuos->avaliacao[indice_pop_nova] < populacaoAtual->avaliacao[indice_pop_atual])) {
                copiarRota(novosIndividuos->cromossomo[indice_pop_nova], populacaoAtual->cromossomo[i]);
                populacaoAtual->avaliacao[i] = novosIndividuos->avaliacao[indice_pop_nova];

            indice_pop_nova++;
        } else {
            indice_pop_atual++;
        }
    }
}

void copiarRota(int* fonte, int* destino)
{
    /* Copia uma rota da fonte para o destino*/

    for (int i = 0; i <= dimensao; i++)
    {
        destino[i] = fonte[i];
    }
}

void calculaCustoMedioPopulacao(populacao* pop, float* media, float* custoPiorIndividuo, float* custoMelhorIndividuo)
{
    *custoMelhorIndividuo = INFINITY;
    *custoPiorIndividuo = 0;

    float soma = 0;
    for (int i = 0; i < pop->tamanho; i++)
    {
        soma += pop->avaliacao[i];

        if (pop->avaliacao[i] < *custoMelhorIndividuo)
        {
            *custoMelhorIndividuo = pop->avaliacao[i];
        }

        if (pop->avaliacao[i] > *custoPiorIndividuo)
        {
            *custoPiorIndividuo = pop->avaliacao[i];
        }
    }
    *media = (soma / pop->tamanho);
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
        [operador_cruzamento]: 0 para ZX e 1 para EXX
        [tamanho_populacao]: Um valor inteiro positivo para o tamanho da população (>= 0)
        [chance_mutacao]: Um valor entre 0-1 que indica a chance de alguma mutação ocorrer em um indivíduo
        [criterio_parada]: O número de gerações sem melhoria para parar o algoritmo 
    */  
    
    if (argc > 8) {
        printf("\nArgumentos incorretos, uso correto: \n[programa] arquivo_de_entrada.tsp [operador_cruzamento] [tamanho_populacao] [chance_mutacao] [criterio_parada]");
        printf("\n\nArgumentos:\n -- [programa]: O executavel compilado\n -- arquivo_de_entrada.tsp: O arquivo contendo a instancia a ser executada");
        printf("\n -- [operador_cruzamento]: Indica qual heuristica sera utilizada:");
        printf("\n -- >> 0: Operador 1");
        printf("\n -- >> 1: Operador 2");
        printf("\n -- [tamanho_populacao]: Um valor inteiro positivo para o tamanho da populacao (>= 0)");
        printf("\n -- [chance_mutacao]: Um valor entre 0 e 1 que indica a chance de alguma mutacao ocorrer em um individuo");
        printf("\n -- [criterio_parada]: O numero de geracoes sem melhoria para parar o algoritmo ");
        printf("\n -- [threads]: O numero de threads a serem utilizadas (opcional, padrão: 1)");
        return 1;
    }

    algoritmoCruzamento = atoi(argv[2]);
    int tamanhoPopulacao = atoi(argv[3]);
    chanceMutacao = atof(argv[4]);
    int numeroGeracoesSemMelhoriaParaParar = atoi(argv[5]);
    numeroThreads = atoi(argv[6]);

    // Verificacao se o numero de threads esta dentro do intervalo definido como permitido
    if ((numeroThreads <= 0) || (numeroThreads > MAX_THREADS)) {
        printf("\nErro: O número de threads deve ser um inteiro positivo e menor ou igual a %d", MAX_THREADS);
        printf("\nAdotando o valor padrão de 1 thread.\n");
        numeroThreads = 1;
        return 1;
    }

    printf("\nNúmero de threads: %d", numeroThreads);

    if (tamanhoPopulacao <= 1)
    {
        printf("\nO sistema precisa de pelo menos dois indivíduos para funcionar");
        return 1;
    }

    numeroDePaisSelecionadosParaCruzamento = (int) (0.15 * tamanhoPopulacao);

    // Precisamos de pares. Aqui foi utilizado uma monogamia intrageracional.
    // Caso um pai fique sem par, ele não poderia cruzar, então precisamos garantir que um número par deles sejam selecionados.
    if (numeroDePaisSelecionadosParaCruzamento % 2 != 0)
    {
        numeroDePaisSelecionadosParaCruzamento += 1;
    }

    if (numeroDePaisSelecionadosParaCruzamento <= 1)
    {
        numeroDePaisSelecionadosParaCruzamento = 2;
    }

    if (algoritmoCruzamento != 0 && algoritmoCruzamento != 1)
    {
        printf("\nErro: O operador de cruzamento deve ser 0 ou 1");
        return 1;
    }
    
    FILE *arquivoEntrada = fopen(argv[1], "r");
    arquivoTimestamp = fopen("timestamp.txt", "w+");
    arquivoTempoConstrucao = fopen("tempo_construcao.txt", "w+");

    if (arquivoTimestamp == NULL) {
        printf("\nErro ao criar arquivo timestamp.txt\n");
        return 1;
    }

    if (arquivoTempoConstrucao == NULL)
    {
        printf("\nErro ao criar o arquivo de log para o tempo de construcao");
        return 1;
    }

    if (arquivoEntrada == NULL) {
        printf("\nErro ao abrir arquivo de entrada");
        return 1;
    }

    if (!terminaCom(argv[1], ".tsp")) {
        printf("\nArquivo de entrada deve ser do tipo .tsp");
        return 1;
    }

    if (algoritmoCruzamento == 1)
    {
        alpha = atof(argv[6]);
    }

    lerArquivo(arquivoEntrada, &listaDeVertices, &dimensao);

    clock_t start, end;
    inicioExecucao = clock();
    start = clock();

    booleano atingiuCriterioParada = False;

    int contadorGeracoesSemMelhoria = numeroGeracoesSemMelhoriaParaParar;

    printf("\nIniciando construcao inicial");

    float custoMelhorRotaConhecida = INFINITY;
    int indiceMelhorRotaConhecida = -1;
    int* melhorRotaConhecida = malloc(sizeof(int) * (dimensao + 1));

    float custoMedio;
    float custoPiorIndividuo;
    float custoMelhorIndividuo; // difere da melhorRotaConhecida pq esse deve obrigatoriamente ser da população atual

    clock_t inicioGeracaoPopulacaoInicial, fimGeracaoPopulacaoInicial;
    double tempoCorrido;

    inicioGeracaoPopulacaoInicial = clock();
    
    // Populacao atual
    populacao* pop = gerarPopulacaoInicial(tamanhoPopulacao);
    
    fimGeracaoPopulacaoInicial = clock();

    tempoCorrido = (double)(fimGeracaoPopulacaoInicial - inicioGeracaoPopulacaoInicial) / CLOCKS_PER_SEC;

    printf("\nPopulacao inicial gerada, tempo gasto: %fs", tempoCorrido);
    printTempoConstrucao(tempoCorrido);

    // printarPopulacao(pop);

    // População de filhos gerados a cada iteração
    populacao* novosIndividuos = malloc(sizeof(populacao));
    novosIndividuos->tamanho = numeroDePaisSelecionadosParaCruzamento / 2;
    novosIndividuos->avaliacao = malloc(sizeof(float) * novosIndividuos->tamanho);
    novosIndividuos->cromossomo = malloc(sizeof(int*) * novosIndividuos->tamanho);

    printf("\nnovosIndividuos->tamanho: %d\n", novosIndividuos->tamanho);
    
    for (int i = 0; i < novosIndividuos->tamanho; i++)
    {
        novosIndividuos->avaliacao[i] = 0.0;
        novosIndividuos->cromossomo[i] = malloc(sizeof(int) * (dimensao + 1));
    }

    int* paisSelecionados = malloc(sizeof(int) * numeroDePaisSelecionadosParaCruzamento);
    probabilidades = malloc(sizeof(float) * tamanhoPopulacao);
    selecionados = malloc(sizeof(booleano) * pop->tamanho);

    for (int i = 0; i < pop->tamanho; i++)
    {
        if (pop->avaliacao[i] < custoMelhorRotaConhecida)
        {
            // printf("\nAvaliacao de pop[%d]: %f", i, pop->avaliacao[i]);
            custoMelhorRotaConhecida = pop->avaliacao[i];
            indiceMelhorRotaConhecida = i;
        }
    }

    copiarRota(pop->cromossomo[indiceMelhorRotaConhecida], melhorRotaConhecida);

    printf("\nMelhor rota conhecida: %f, indice: %d", custoMelhorRotaConhecida, indiceMelhorRotaConhecida);

    printf("\nMemoria alocada!");

    inicioMelhoramento = clock();
    // printTimestamp(custoMelhorRotaConhecida);

    calculaCustoMedioPopulacao(pop, &custoMedio, &custoPiorIndividuo, &custoMelhorIndividuo);

    printTimestampIteracao(custoMelhorRotaConhecida, 0, custoMedio, custoPiorIndividuo, custoMelhorIndividuo);

    printf("\nEntrando no laco");
    
    int numeroDeGeracoes = 1;

    avaliarCromossomos(pop);

    while (atingiuCriterioParada == False)
    {
        selecionarCromossomos(pop, paisSelecionados);

        cruzarCromossomos(pop, paisSelecionados, novosIndividuos);

        for (int i = 0; i < pop->tamanho; i++)
        {
            mutarCromossomo(pop, i);
        }
        
        pthread_t threadPoolDoisOpt[pop->tamanho];
        packDoisOpt packPoolDoisOpt[pop->tamanho];

        for (int i = 0; i < pop->tamanho; i++)
        {
            packPoolDoisOpt[i].pop = pop;
            packPoolDoisOpt[i].k = i;
        }

        for (int i = 0; i < pop->tamanho; i++)
        {
            pthread_create(&(threadPoolDoisOpt[i]), NULL, doisOpt, (void*)&(packPoolDoisOpt[i]));
        }

        for (int i = 0; i < pop->tamanho; i++)
        {
            pthread_join(threadPoolDoisOpt[i], NULL);
        }

        // printf("\nMutou e fez 2opt com os cromossomos");
        avaliarCromossomos(novosIndividuos);
        
        ordenaPopulacao(pop);
        ordenaPopulacao(novosIndividuos);

        // printf("\nPOP:");
        // for (int i = 0; i < pop->tamanho; i++)
        // {
        //     printf("\nAvaliacao de pop[%d]: %f", i, pop->avaliacao[i]);
        // }

        // printf("\nFILHOS:");
        // for (int i = 0; i < novosIndividuos->tamanho; i++)
        // {
        //     printf("\nAvaliacao de novosIndividuos[%d]: %f", i, novosIndividuos->avaliacao[i]);
        // }

        // printf("\nAtualizando populacao");
        atualizarPopulacao(pop, novosIndividuos);

        // printf("\nPopulacao atualizada");

        avaliarCromossomos(pop);

        // printf("\nAvaliou cromossomos");

        booleano flagMelhorouRota = False;

        for (int i = 0; i < pop->tamanho; i++)
        {
            // printf("\nAvaliacao de pop[%d]: %f", i, pop->avaliacao[i]);
            if (pop->avaliacao[i] < custoMelhorRotaConhecida)
            {
                custoMelhorRotaConhecida = pop->avaliacao[i];
                indiceMelhorRotaConhecida = i;
                flagMelhorouRota = True;
            }
        }

        if (flagMelhorouRota == True)
        {
            copiarRota(pop->cromossomo[indiceMelhorRotaConhecida], melhorRotaConhecida);
            contadorGeracoesSemMelhoria = numeroGeracoesSemMelhoriaParaParar;
            
            // printf("\nMelhor rota conhecida: ");
            // for (int j = 0; j <= dimensao; j++)
            // {
            //     printf("%d ", melhorRotaConhecida[j]);
            // }
            printf("\n[Iteracao %d] melhor custo obtido: %.2f | custo medio: %.2f | contador: %d", numeroDeGeracoes, custoMelhorRotaConhecida, custoMedio, contadorGeracoesSemMelhoria);
        }
        else
        {
            contadorGeracoesSemMelhoria -= 1;
        }

        printTimestampIteracao(custoMelhorRotaConhecida, numeroDeGeracoes, custoMedio, custoPiorIndividuo, custoMelhorIndividuo);
         
        if (contadorGeracoesSemMelhoria == 0)
        {
            atingiuCriterioParada = True;
        }

        if (((double)(clock() - inicioExecucao)/CLOCKS_PER_SEC) / 3600 > limiteExecucaoHoras)
        {
            atingiuCriterioParada = True;
        }

        numeroDeGeracoes += 1;
        // printTimestamp(custoMelhorRotaConhecida);

        calculaCustoMedioPopulacao(pop, &custoMedio, &custoPiorIndividuo, &custoMelhorIndividuo);

        // printf("\n[Iteracao %d] melhor custo obtido: %.2f | custo medio: %.2f | contador: %d", numeroDeGeracoes, custoMelhorRotaConhecida, custoMedio, contadorGeracoesSemMelhoria);
    }

    end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("\nFinalizado! Tempo gasto: %lf", cpu_time_used);
    printf("\nRota calculada: %f\n", custoMelhorRotaConhecida);

    // exportaResultados(pop->cromossomo[indiceMelhorRotaConhecida], custoMelhorRotaConhecida, argv[1], cpu_time_used);

    free(listaDeVertices);

    for (int i = 0; i < pop->tamanho; i++)
    {
        free(pop->cromossomo[i]);
    }

    free(pop->cromossomo);
    free(pop->avaliacao);

    free(probabilidades);

    fclose(arquivoEntrada);
    fclose(arquivoTimestamp);
    fclose(arquivoTempoConstrucao);

    return 0;
}
