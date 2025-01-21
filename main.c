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

int numeroDePaisSelecionadosParaCruzamento;
float chanceMutacao;

FILE* arquivoTimestamp;
coordenada* listaDeVertices;
int dimensao;
int algoritmoCruzamento;
clock_t inicioMelhoramento;

void vizinhoMaisProximo(int* rotaFinal, float* custo);
float calculaCustoRota(int* rota);
float calculaDistancia(coordenada* c1, coordenada* c2);

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

    float custo;
    // loop que inicializa cada cromossomo
    for (int i = 0; i < tamanho; i++)
    {
        // nova_populacao->avaliacao[i] = 0.0;
        printf("\nGerando cromossomo %d de %d", i+1, tamanho);
        nova_populacao->cromossomo[i] = malloc(sizeof(int) * (dimensao + 1));
        vizinhoMaisProximo(nova_populacao->cromossomo[i], &custo);
        nova_populacao->avaliacao[i] = custo;
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

int* zx (int* genitor1, int* genitor2)
{
    int bin_genitor1[dimensao+1];
    int bin_genitor2[dimensao+1];
    int* filho = malloc(sizeof(int) * (dimensao+1));

    for (int i = 0; i <= dimensao; i++)
    {
        bin_genitor1[i] = 0;
        bin_genitor2[i] = 0;
        filho[i] = -1;
    }

    // sorteia a cidade
    int ind_cidade_inicial = rand() % dimensao;
    coordenada cidade_inicial = listaDeVertices[ind_cidade_inicial];

    gerar_conjunto_pertencente_regiao(0.3, genitor1, bin_genitor1, cidade_inicial);
    gerar_conjunto_pertencente_regiao(0.3, genitor2, bin_genitor2, cidade_inicial);

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

void vizinhoMaisProximo(int* rota, float* custo)
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

        rota[indiceRota] = atual;
        indiceRota++;
    }

    rota[indiceRota] = inicial;
    distanciaTotal += calculaDistancia(&listaDeVertices[atual], &listaDeVertices[inicial]);
    *custo = distanciaTotal;
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

void doisOpt(populacao* pop, int k)
{
    /*
        Realiza a heurística de melhoramento 2-opt aplicado ao first improvement. Apenas um passo.

        Baseado em: https://en.wikipedia.org/wiki/2-opt

        Parâmetros:
            pop: A população atual
            i: O indivíduo o qual se quer aplicar o 2-opt
    */
    int n = dimensao;
    // int* rotaFinal = pop->cromossomo[k];

    for (int i = 0; i < n - 1; i++)
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
                pop->avaliacao[i] += delta;
                return;
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

    int pontoInicio = rand() % (tamanho - 2);
    int pontoFim = pontoInicio + 1 + rand() % (tamanho - pontoInicio - 1);

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
        int v1 = (int) rand() % dimensao;
        int v2 = (int) rand() % dimensao;

        if (v2 < v1)
        {
            int temp = v1;
            v1 = v2;
            v2 = temp;
        }

        float delta;
        int numElementos = dimensao + 1;

        if (v2 == v1 + 1)
        {
            delta = calculaDistancia(&listaDeVertices[pop->cromossomo[i][(v1-1+numElementos)%numElementos]], &listaDeVertices[pop->cromossomo[i][v2]]) +
            calculaDistancia(&listaDeVertices[pop->cromossomo[i][v1]], &listaDeVertices[pop->cromossomo[i][(v2+1)%numElementos]]);
        }
        else if (v2 == v1-1)
        {
            delta = calculaDistancia(&listaDeVertices[pop->cromossomo[i][(v2-1+numElementos)%numElementos]], &listaDeVertices[pop->cromossomo[i][v1]]) +
            calculaDistancia(&listaDeVertices[pop->cromossomo[i][v2]], &listaDeVertices[pop->cromossomo[i][(v1+1)%numElementos]]);
        }
        else
        {
            delta = calculaDistancia(&listaDeVertices[pop->cromossomo[i][(v1-1+numElementos)%numElementos]], &listaDeVertices[pop->cromossomo[i][v2]]) +
            calculaDistancia(&listaDeVertices[pop->cromossomo[i][v2]], &listaDeVertices[pop->cromossomo[i][v1+1]]) +
            calculaDistancia(&listaDeVertices[pop->cromossomo[i][(v2-1+numElementos)%numElementos]], &listaDeVertices[pop->cromossomo[i][v1]]) +
            calculaDistancia(&listaDeVertices[pop->cromossomo[i][v1]], &listaDeVertices[pop->cromossomo[i][(v2+1)%numElementos]]);
        }
        troca(pop->cromossomo[i], v1, v2);
        pop->avaliacao[i] += delta;
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

        paisSelecionados[i] = escolhido;
        selecionados[escolhido] = True;
    }

    // for (int i = 0; i < numeroDePaisSelecionadosParaCruzamento; i++)
    // {
    //     printf("\npais[%d] = %d", i, paisSelecionados[i]);
    // }
}

void cruzarCromossomos(populacao* pop, int* paisSelecionados, populacao* filhosGerados)
{
    /*
        Cruza os pais selecionados da população para gerar numeroDePaisSelecionadosParaCruzamento/2 filhos.
    */
    
    for (int i = 0; i < numeroDePaisSelecionadosParaCruzamento / 2; i++)
    {
        int indicePai1 = paisSelecionados[2 * i];
        int indicePai2 = paisSelecionados[(2 * i) + 1];


        if (algoritmoCruzamento == 0)
        {
            filhosGerados->cromossomo[i] = zx(pop->cromossomo[indicePai1], pop->cromossomo[indicePai2]);
        }
        else
        {   
            exx_crossover(pop->cromossomo[indicePai1], pop->cromossomo[indicePai2], filhosGerados->cromossomo[i]);
        }

        // printf("\nFilho gerado %d: ", i);
        // for (int j = 0; j <= dimensao; j++)
        // {
        //     printf("%d ", filhosGerados->cromossomo[i][j]);
        // }
    }
}

void printarPopulacao(populacao* pop)
{
    printf("\nTamanho da populacao: %d", pop->tamanho);
    for (int i = 0; i < pop->tamanho; i++)
    {
        printf("\nCromossomo %d, avaliacao: %f, ordem: ", i, pop->avaliacao[i]);
        // for (int j = 0; j <= dimensao; j++)
        // {
        //     printf("%d ", pop->cromossomo[i][j]);
        // }
    }
}

void atualizarPopulacao(populacao* populacaoAtual, populacao* novosIndividuos)
{
    // Considerando que as duas populações estão ordenadas pela aptidão, utiliza a técnica do Stead Stated em lote

    populacao* buffer_populacao = malloc(sizeof(populacao));
    buffer_populacao->tamanho = populacaoAtual->tamanho;
    buffer_populacao->avaliacao = malloc(sizeof(float) * populacaoAtual->tamanho);
    buffer_populacao->cromossomo = malloc(sizeof(int*) * populacaoAtual->tamanho);

    int indice_pop_atual = 0;
    int indice_pop_nova = 0;
    for (int i = 0; i < populacaoAtual->tamanho; i++)
    {
        if ((indice_pop_atual < populacaoAtual->tamanho) && (indice_pop_nova < novosIndividuos->tamanho))
        {
            if (populacaoAtual->avaliacao[indice_pop_atual] < novosIndividuos->avaliacao[indice_pop_nova])
            {
                buffer_populacao->avaliacao[i] = populacaoAtual->avaliacao[indice_pop_atual];
                buffer_populacao->cromossomo[i] = populacaoAtual->cromossomo[indice_pop_atual];
                indice_pop_atual++;
            }

            else
            {
                buffer_populacao->avaliacao[i] = novosIndividuos->avaliacao[indice_pop_nova];
                buffer_populacao->cromossomo[i] = novosIndividuos->cromossomo[indice_pop_nova];
                indice_pop_nova++;
            }
        }
    
        else if (indice_pop_atual < populacaoAtual->tamanho)
        {
            buffer_populacao->avaliacao[i] = populacaoAtual->avaliacao[indice_pop_atual];
            buffer_populacao->cromossomo[i] = populacaoAtual->cromossomo[indice_pop_atual];
            indice_pop_atual++;
        }

        else if (indice_pop_nova < novosIndividuos->tamanho)
        {
            buffer_populacao->avaliacao[i] = novosIndividuos->avaliacao[indice_pop_nova];
            buffer_populacao->cromossomo[i] = novosIndividuos->cromossomo[indice_pop_nova];
            indice_pop_nova++;
        }
    }

    // Limpando os ponteiros dos cromossomos descartados

    for (int i = indice_pop_atual; i < populacaoAtual->tamanho; i++)
    {
        free(populacaoAtual->cromossomo[i]);
    }

    for (int i = indice_pop_nova; i < novosIndividuos->tamanho; i++)
    {
        free(novosIndividuos->cromossomo[i]);
    }

    // Copiando para a população atual novamente

    for (int i = 0; i < populacaoAtual->tamanho; i++)
    {
        populacaoAtual->cromossomo[i] = buffer_populacao->cromossomo[i];
        populacaoAtual->avaliacao[i] = buffer_populacao->avaliacao[i];
    }
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

    algoritmoCruzamento = atoi(argv[2]);
    int tamanhoPopulacao = atoi(argv[3]);
    chanceMutacao = atof(argv[4]);
    int numeroGeracoesSemMelhoriaParaParar = atoi(argv[5]);

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

    float custoMelhorRotaConhecida = INFINITY;
    int indiceMelhorRotaConhecida = -1;

    // Populacao atual
    populacao* pop = gerarPopulacaoInicial(tamanhoPopulacao);

    printf("\nPopulacao inicial gerada");

    printarPopulacao(pop);

    // População de filhos gerados a cada iteração
    populacao* novosIndividuos = malloc(sizeof(populacao));
    novosIndividuos->tamanho = numeroDePaisSelecionadosParaCruzamento / 2;
    novosIndividuos->avaliacao = malloc(sizeof(float) * novosIndividuos->tamanho);
    novosIndividuos->cromossomo = malloc(sizeof(int*) * novosIndividuos->tamanho);
    
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

    printf("\nMelhor rota conhecida: %f, indice: %d", custoMelhorRotaConhecida, indiceMelhorRotaConhecida);

    printf("\nMemoria alocada!");

    printTimestamp(custoMelhorRotaConhecida);

    printf("\nEntrando no laco");

    while (atingiuCriterioParada == False)
    {
        avaliarCromossomos(pop);

        printf("\nAvaliou cromossomos");

        selecionarCromossomos(pop, paisSelecionados);

        printf("\nSelecionou cromossomos");

        cruzarCromossomos(pop, paisSelecionados, novosIndividuos);

        printf("\nCruzou cromossomos");

        for (int i = 0; i < pop->tamanho; i++)
        {
            printf("\nIteracao com o cromossomo %d", i);

            printf("\nMutando...");

            mutarCromossomo(pop, i);
            
            printf("\nAplicando doisOpt...");

            doisOpt(pop, i);
        }

        printf("\nMutou e fez 2opt com os cromossomos");
        
        atingiuCriterioParada = True;

        // atualizarPopulacao();

        // buscar na populacao a melhor rota

        // se ela for melhor que a melhorRotaConhecida, atualizar ela e o seu indice
        //     redefinir o contadorGeracoesSemMelhoria para o valor inicial

        // se não
        //     subtrair 1 do contadorGeracoesSemMelhoria

        // se o contador for igual a zero, atingiuCriterioDeParada = True
        
        printTimestamp(custoMelhorRotaConhecida);

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

    free(probabilidades);

    fclose(arquivoEntrada);
    fclose(arquivoTimestamp);

    return 0;
}
