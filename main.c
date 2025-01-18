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

    // aloca dinamicamente vet
    // vet = malloc(sizeof(int) * (dimensao+1));
    // for (int i = 0; i <= dimensao; i++)
    // {
    //     vet[i] = 0;
    // }

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
    // printf("--> num_cidades: %d\n", num_cidades);
}

float menor_distancia(int* bin_genitor, int indice_cidade, int* indice_genitor, int bin)
{
    int indice_cidade_mais_proxima, i = 0, n_bin, elementos_percorridos = 0;
    float menor_distancia = INFINITY, temp_distancia;

    // n_bin eh negacao de bin
    if (bin == 0) {n_bin = 1;}
    else {n_bin = 0;}  

    // printf("primeiro loop\n");
    // encontra o primeiro conjunto de 0's do INICIO
    while ((bin_genitor[i % dimensao] == bin) && (elementos_percorridos <= dimensao)) 
    {
        elementos_percorridos++;
        i++;
    }
    // printf("segundo loop\n");
    // tem que começar em uma posição com valor 0, caso contrário complica para detectar o componente conexo
    while ((bin_genitor[i % dimensao] == n_bin) && (elementos_percorridos <= dimensao)) 
    {
        elementos_percorridos++;
        i++;
    }
    
    // evita loop infinito
    if (elementos_percorridos > dimensao)
    {
        // printf("pegou\n");
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
                // printf("i: %d\n", i);
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
    // printf("cidade mais proxima: %d\n", indice_cidade_mais_proxima);

    return menor_distancia;
}

int* zx (int* genitor1, int* genitor2)
{
    // int* bin_genitor1 =  malloc(sizeof(int) * (dimensao+1));
    // int* bin_genitor2 =  malloc(sizeof(int) * (dimensao+1));
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

    // printf("\n\n[");
    // for (int i = 0; i < dimensao; i++)
    // {
    //     printf("%d, ", bin_genitor1[i]);
    // }
    // printf("]\n");

    // printf("\n[");
    // for (int i = 0; i < dimensao; i++)
    // {
    //     printf("%d, ", bin_genitor2[i]);
    // }
    // printf("]\n");

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
    
    // printf("preparado!\n");

    while (indice_filho < dimensao)
    {
        // // se for um componente conexo, só copia
        // if (bin_genitor2[indice_genitor2] == 0)
        // {
        //     filho[indice_filho] = genitor2[indice_genitor2];
        //     bin_genitor2[indice_genitor2] = 1;
        //     indice_filho++;
        //     indice_genitor2++;
        // }

        // se tivermos uma desconexão, procura nos dois genitores qual componente conexo conectar
        distancia_genitor1 = menor_distancia(bin_genitor1, filho[(indice_filho-1)], &indice_genitor1, 1);
        // printf("distancia_genitor1!\n");
        distancia_genitor2 = menor_distancia(bin_genitor2, filho[(indice_filho-1)], &indice_genitor2, 0);
        // distancia_genitor2 = 999999999;
        // printf("distancia_genitor2!\n");

        if ((distancia_genitor1 == INFINITY) && (distancia_genitor2 == INFINITY))
        {
            // printf("deu pau\n");
            break;
        }

        // próximo componente a ser conectado fica no genior 1
        if (distancia_genitor1 <= distancia_genitor2)
        {
            // inserir o componente
            // atualizar com 0 o antigo componente do genitor 1
            while((bin_genitor1[(indice_genitor1 + dimensao) % dimensao]) == 1 && (indice_filho < dimensao))
            {
                // printf("G1 - Filho recebe %d\n", genitor1[(indice_genitor1 + dimensao) % dimensao]);
                // printf("indice filho: %d\n indice genitor1: %d\n", indice_filho, ((indice_genitor1 + dimensao) % dimensao));
                filho[indice_filho] = genitor1[(indice_genitor1 + dimensao) % dimensao];
                bin_genitor1[(indice_genitor1 + dimensao) % dimensao] = 0; // marcando que já foi selecionado e desconsiderando para as próximas procuras
                indice_filho++;
                indice_genitor1++;
                cidades_inseridas ++;
                    // printf("\n\n[");
                    // for (int i = 0; i < dimensao; i++)
                    // {
                    //     printf("%d-%d, ", bin_genitor1[i], genitor1[i]);
                    // }
                    // printf("]\n");
                    // printf("\n\n[");
                    // for (int i = 0; i < dimensao; i++)
                    // {
                    //     printf("%d, ", filho[i]);
                    // }
                    // printf("]\n");
            }
        }

        else if (distancia_genitor1 > distancia_genitor2)
        {
            // inserir o componente
            // atualizar com 1 o antigo componente do genitor 2
            while((bin_genitor2[(indice_genitor2 + dimensao) % dimensao] == 0) && (indice_filho < dimensao))
            {
                // printf("G2 - Filho recebe %d\n", genitor2[(indice_genitor2 + dimensao) % dimensao]);
                // printf("indice filho: %d\n indice genitor2: %d\n", indice_filho, ((indice_genitor2 + dimensao) % dimensao));
                filho[indice_filho] = genitor2[(indice_genitor2 + dimensao) % dimensao];
                bin_genitor2[(indice_genitor2 + dimensao) % dimensao] = 1; // marcando que já foi selecionado e desconsiderando para as próximas procuras
                indice_filho++;
                indice_genitor2++;
                cidades_inseridas++;
                    // printf("\n[");
                    // for (int i = 0; i < dimensao; i++)
                    // {
                    //     printf("%d-%d, ", bin_genitor2[i], genitor2[i]);
                    // }
                    // printf("]\n");
                    // printf("\n\n[");
                    // for (int i = 0; i < dimensao; i++)
                    // {
                    //     printf("%d, ", filho[i]);
                    // }
                    // printf("]\n");
            }
        }

    // break;
        // printf("executou o while!\n");
        
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
    fprintf(arquivoDeSaida, "%s", nomeArquivo);
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

booleano pertence_ao_vetor(int* vetor, int no, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        if (vetor[i] == no) {
            return True;
        }
    }
    return False;
}

// EXX (Edge Exchange Crossover)
// void exx_crossover(int* pai1, int* pai2, int* filho1, int* filho2, int tamanho) {
//     int pontoInicio = rand() % (tamanho / 2);  // Ponto de início para o segmento de troca (randomizado)
//     int pontoFim = pontoInicio + (rand() % (tamanho / 2)); // Ponto de fim

//     // Copia o segmento de arestas do primeiro pai para o filho 1
//     for (int i = pontoInicio; i <= pontoFim; i++) {
//         filho1[i] = pai1[i];
//     }

//     // Copia o segmento de arestas do segundo pai para o filho 2
//     for (int i = pontoInicio; i <= pontoFim; i++) {
//         filho2[i] = pai2[i];
//     }

//     // Preencher os filhos com os vértices restantes do pai 2 para filho 1
//     int index1 = 0;
//     for (int i = 0; i < tamanho; i++) {
//         if (!presente(filho1, pai2[i], pontoInicio)) {
//             while (presente(filho1, pai2[i], pontoInicio)) {
//                 i++;
//             }
//             filho1[index1++] = pai2[i];
//         }
//     }

//     // Preencher os filhos com os vértices restantes do pai 1 para filho 2
//     int index2 = 0;
//     for (int i = 0; i < tamanho; i++) {
//         if (!presente(filho2, pai1[i], pontoInicio)) {
//             while (presente(filho2, pai1[i], pontoInicio)) {
//                 i++;
//             }
//             filho2[index2++] = pai1[i];
//         }
//     }

//     // Garantir que o último vértice de cada filho conecte de volta ao primeiro
//     filho1[tamanho - 1] = filho1[0];
//     filho2[tamanho - 1] = filho2[0];
// }

// int main(int argc, char *argv[]) {
//     srand(time(NULL));

//     // Parâmetros:

//     /*
//         Windows:
//             executavel.exe [instancia] [operador_cruzamento] [tamanho_populacao] [chance_mutacao] [criterio_parada]

//         Linux:
//             ./executavel [instancia] [operador_cruzamento] [tamanho_populacao] [chance_mutacao] [criterio_parada]

//         Onde:
//         [instancia]: O caminho da instância a ser executada
//         [operador_cruzamento]: 0 para [operador 1] e 1 para [operador 2]
//         [tamanho_populacao]: Um valor inteiro positivo para o tamanho da população (>= 0)
//         [chance_mutacao]: Um valor entre 0-1 que indica a chance de alguma mutação ocorrer em um indivíduo
//         [criterio_parada]: O número de gerações sem melhoria para parar o algoritmo 
//     */  
    
//     if (argc != 6) {
//         printf("\nArgumentos incorretos, uso correto: \n[programa] arquivo_de_entrada.tsp [operador_cruzamento] [tamanho_populacao] [chance_mutacao] [criterio_parada]");
//         printf("\n\nArgumentos:\n -- [programa]: O executavel compilado\n -- arquivo_de_entrada.tsp: O arquivo contendo a instancia a ser executada");
//         printf("\n -- [operador_cruzamento]: Indica qual heuristica sera utilizada:");
//         printf("\n -- >> 0: Operador 1");
//         printf("\n -- >> 1: Operador 2");
//         printf("\n -- [tamanho_populacao]: Um valor inteiro positivo para o tamanho da populacao (>= 0)");
//         printf("\n -- [chance_mutacao]: Um valor entre 0 e 1 que indica a chance de alguma mutacao ocorrer em um individuo");
//         printf("\n -- [criterio_parada]: O numero de geracoes sem melhoria para parar o algoritmo ");
//         return 1;
//     }

//     int algoritmoCruzamento = atoi(argv[2]);
//     int tamanhoPopulacao = atoi(argv[3]);
//     float chanceMutacao = atof(argv[4]);
//     int numeroGeracoesSemMelhoriaParaParar = atoi(argv[5]);

//     if (algoritmoCruzamento != 0 && algoritmoCruzamento != 1)
//     {
//         printf("\nErro: O operador de cruzamento deve ser 0 ou 1");
//         return 1;
//     }
    
//     FILE *arquivoEntrada = fopen(argv[1], "r");
//     FILE *arquivoTimestamp = fopen("timestamp.txt", "w+");

//     if (arquivoEntrada == NULL) {
//         printf("\nErro ao abrir arquivo de entrada");
//         return 1;
//     }

//     if (!terminaCom(argv[1], ".tsp")) {
//         printf("\nArquivo de entrada deve ser do tipo .tsp");
//         return 1;
//     }

//     lerArquivo(arquivoEntrada, &listaDeVertices, &dimensao);

//     clock_t start, end;
//     start = clock();

//     booleano atingiuCriterioParada = False;

//     int contadorGeracoesSemMelhoria = numeroGeracoesSemMelhoriaParaParar;

//     printf("\nIniciando construcao inicial");

//     int custoMelhorRotaConhecida = INFINITY;
//     int indiceMelhorRotaConhecida = -1;

//     populacao* pop = gerarPopulacaoInicial(tamanhoPopulacao);

//     for (int i = 0; i < pop->tamanho; i++)
//     {
//         if (pop->avaliacao[i] < custoMelhorRotaConhecida)
//         {
//             custoMelhorRotaConhecida = pop->avaliacao[i];
//             indiceMelhorRotaConhecida = i;
//         }
//     }

//     printTimestamp(custoMelhorRotaConhecida);

//     while (atingiuCriterioParada == False)
//     {
//         avaliarCromossomos(pop);
//         // selecionarCromossomos();
//         // cruzarCromossomos();
//         // mutarCromossomos();

//         for (int i = 0; i < pop->tamanho; i++)
//         {
//             doisOpt(pop, i);
//         }
        
//         // atualizarPopulacao();

//         // buscar na populacao a melhor rota

//         // se ela for melhor que a melhorRotaConhecida, atualizar ela e o seu indice
//         //     redefinir o contadorGeracoesSemMelhoria para o valor inicial

//         // se não
//         //     subtrair 1 do contadorGeracoesSemMelhoria

//         // se o contador for igual a zero, atingiuCriterioDeParada = True

//     }

//     end = clock();
//     double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

//     printf("\nFinalizado! Tempo gasto: %lf", cpu_time_used);
//     printf("\nRota calculada: %f\n", custoMelhorRotaConhecida);

//     exportaResultados(pop->cromossomo[indiceMelhorRotaConhecida], custoMelhorRotaConhecida, argv[1], cpu_time_used);

//     free(listaDeVertices);

//     for (int i = 0; i < pop->tamanho; i++)
//     {
//         free(pop->cromossomo[i]);
//     }

//     free(pop->cromossomo);
//     free(pop->avaliacao);

//     fclose(arquivoEntrada);
//     fclose(arquivoTimestamp);

//     return 0;
// }

int main(int argc, char *argv[]) {
    srand(time(NULL));

    FILE *arquivoEntrada = fopen("instancias/pla85900.tsp", "r");

    if (arquivoEntrada == NULL) {
        printf("\nErro ao abrir arquivo de entrada");
        return 1;
    }

    lerArquivo(arquivoEntrada, &listaDeVertices, &dimensao);

    printf("\nIniciando construcao inicial");

    int custoMelhorRotaConhecida = INFINITY;
    int indiceMelhorRotaConhecida = -1;
    booleano atingiuCriterioParada = False;

    populacao* pop = gerarPopulacaoInicial(2);
    avaliarCromossomos(pop);

    for (int i = 0; i < pop->tamanho; i++)
    {
        if (pop->avaliacao[i] < custoMelhorRotaConhecida)
        {
            custoMelhorRotaConhecida = pop->avaliacao[i];
            indiceMelhorRotaConhecida = i;
        }
    }

    printf("\n+++++\n");
    for (int i = 0; i < dimensao; i++)
    {
        printf(" %d ", pop->cromossomo[0][i]);
    }
    printf("\n+++++\n");
    for (int i = 0; i < dimensao; i++)
    {
        printf(" %d ", pop->cromossomo[1][i]);
    }

    int* filho = zx(pop->cromossomo[0], pop->cromossomo[1]);


    printf("\n+++++\n");
    for (int i = 0; i < dimensao; i++)
    {
        printf(" %d ", filho[i]);
    }
    ha_repetidos(filho);
    // // while (atingiuCriterioParada == False)
    // // {
    // //     avaliarCromossomos(pop);
    // //     // selecionarCromossomos();
    // //     // cruzarCromossomos();
    // //     // mutarCromossomos();

    // //     for (int i = 0; i < pop->tamanho; i++)
    // //     {
    // //         doisOpt(pop, i);
    // //     }
        
    // //     // atualizarPopulacao();

    // //     // buscar na populacao a melhor rota

    // //     // se ela for melhor que a melhorRotaConhecida, atualizar ela e o seu indice
    // //     //     redefinir o contadorGeracoesSemMelhoria para o valor inicial

    // //     // se não
    // //     //     subtrair 1 do contadorGeracoesSemMelhoria

    // //     // se o contador for igual a zero, atingiuCriterioDeParada = True

    // // }


    // free(listaDeVertices);

    // for (int i = 0; i < pop->tamanho; i++)
    // {
    //     free(pop->cromossomo[i]);
    // }

    // free(pop->cromossomo);
    // free(pop->avaliacao);

    // fclose(arquivoEntrada);

    return 0;
}