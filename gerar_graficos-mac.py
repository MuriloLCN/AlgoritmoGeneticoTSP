import os
import matplotlib.pyplot as plt

folder_path = "timestamps-leandro"
output_folder = "graficos-leandro"

os.makedirs(output_folder, exist_ok=True)

def ler_numero_iteracoes(arquivo: str) -> int:
    # Retorna a lista iteracao, tempo
    # lista_tempo = []
    lista_iteracao = []

    with open(os.path.join(folder_path, arquivo), "r+") as arquivo:
        for linha in arquivo:
            partes = linha.split()
            lista_iteracao.append(int(partes[0]))
            # lista_tempo.append(float(partes[1]))

    return lista_iteracao[-1]

def ler_tempo_real(arquivo: str) -> float:
    with open(os.path.join(folder_path, arquivo), "r+") as arquivo:
        for linha in arquivo:
            partes = linha.split()
            if not partes:
                continue
            # The output from the `time` command is typically on stderr,
            # but you might have redirected it. We'll check for "real".
            if partes[0] == "real":
                time_str = partes[1]  # e.g., '0m27.412s'
                
                # Remove the trailing 's' and split by 'm'
                minutes_part, seconds_part = time_str.replace('s', '').split('m')
                
                # Convert minutes to seconds and add the remaining seconds
                total_seconds = float(minutes_part) * 60 + float(seconds_part)
                
                return total_seconds
    # It's good practice to return a default value or raise an error 
    # if the 'real' time is not found in the file.
    return 0.0
                                  

def plotar_instancia_velocidade(instancia: str, algoritmo_constr: int):
    lista_x_tempo_threads = [0] * 8
    lista_y_iteracoes_threads = [0] * 8
    x_tempo_sequencial = 0
    y_iteracoes_sequencial = 0

    if instancia.endswith(".tsp"):
        instancia = instancia.replace(".tsp", "")

    str_inst = f"{instancia}-{algoritmo_constr}"
    for i in range(8):
        lista_y_iteracoes_threads[i] = ler_numero_iteracoes(f'{str_inst}-{i+1}.txt')
        lista_x_tempo_threads[i] = ler_tempo_real(f'tempo-{str_inst}-{i+1}.txt')
    
    x_tempo_sequencial = ler_tempo_real(f'tempo-{str_inst}-sequencial.txt')
    y_iteracoes_sequencial = ler_numero_iteracoes(f'{str_inst}-sequencial.txt')

#    for arq in os.listdir(folder_path):
#        if arq.endswith(".txt"):
#            if arq.startswith(f"{instancia}-{str(algoritmo_constr)}"):
#                if arq.endswith("sequencial.txt"):
#                    lista_y_sequencial_iteracoes, lista_x_sequencial_tempo = ler_dados_arquivo(arq)
#                else:
#                    indice = int(((arq.replace(".txt", "")).split("-"))[2])
#                    print(indice)
#                    print(arq)
#                    listas_y_threads_iteracoes[indice-1], listas_x_threads_tempo[indice-1] = ler_dados_arquivo(arq)

    plt.figure()
    for i in range(8):
        plt.plot([0,lista_x_tempo_threads[i]], [0,lista_y_iteracoes_threads[i]], label=f"{instancia}: {i+1} thread(s)")
    
    plt.plot([0,x_tempo_sequencial], [0,y_iteracoes_sequencial], label=f"{instancia}: sequencial")
    
    plt.title(f"Velocidades da instância {instancia} com algoritmo {algoritmo_constr}")

    plt.xlabel("Tempo (s)")
    plt.ylabel("Número de iterações")
    plt.grid(True)
    plt.legend()

    arquivo_saida = os.path.join(output_folder, f"{instancia}-{algoritmo_constr}.png")
    plt.savefig(arquivo_saida)
    plt.close()


# def plotar_tempo_construcao(instancia: str, algoritmo_constr: int):

#     categorias = ["Sequencial", "1", "2", "3", "4", "5", "6", "7", "8"]
#     valores = []

#     with open(os.path.join(folder_path, f"constr-{instancia}-{algoritmo_constr}-sequencial.txt")) as arq:
#         valores.append(float(arq.read()))

#     for i in range(8):
#         with open(os.path.join(folder_path, f"constr-{instancia}-{algoritmo_constr}-{i+1}.txt")) as arq:
#             valores.append(float(arq.read()))

#     print(categorias)

#     print(valores)

#     plt.bar(categorias, valores)

#     plt.title(f"Tempo gasto na contrução (s): {instancia}")

#     plt.xlabel("Número de threads")
#     plt.ylabel("Tempo gasto (s)")
#     # plt.grid(True)
#     # plt.legend()

#     arquivo_saida = os.path.join(output_folder, f"constr-{instancia}-{algoritmo_constr}.png")
#     plt.savefig(arquivo_saida)
#     plt.close()


if __name__ == "__main__":
    instancia = input("Insira a instancia sem .tsp:\n>> ")
    algo = int(input("Insira o alg. de constr. (0 ou 1):\n>> "))

    plotar_instancia_velocidade(instancia, algo)
    # plotar_tempo_construcao(instancia, algo)
#         plt.xlabel("Iterações")
#         plt.ylabel("Custo")
#         plt.grid(True)
#         plt.legend()

#         # Save the plot
#         output_file = os.path.join(output_folder, filename.replace(".txt", ".png"))
#         plt.savefig(output_file)
#         plt.close()  # Close the figure to free memory

# for filename in os.listdir(folder_path):
#     if filename.endswith(".txt"):
#         arquivo_timestamp = os.path.join(folder_path, filename)

#         labels = []
#         lista_x_iteracoes = []
#         lista_y_custoMinimo = []
#         lista_y_custoMedio = []
#         lista_y_piorPop = []
#         lista_y_melhorPop = []

#         with open(arquivo_timestamp) as valores:
#             for linha in valores:
#                 valores_split = linha.split()
#                 lista_x_iteracoes.append(float(valores_split[0]))
#                 # Valor [1] é o tempo, não será usado
#                 lista_y_custoMinimo.append(float(valores_split[2]))
#                 lista_y_custoMedio.append(float(valores_split[3]))
#                 lista_y_piorPop.append(float(valores_split[4]))
#                 lista_y_melhorPop.append(float(valores_split[5]))

#         plt.figure()
#         plt.plot(lista_x_iteracoes, lista_y_custoMinimo, label="Custo mínimo")
#         plt.plot(lista_x_iteracoes, lista_y_custoMedio, label="Custo médio da população")
#         plt.plot(lista_x_iteracoes, lista_y_piorPop, label="Pior indivíduo da geração")
#         plt.plot(lista_x_iteracoes, lista_y_melhorPop, label="Melhor indivíduo da geração")

#         plt.title(f"Gráfico: {filename}")  # Add the filename as the title


#         plt.xlabel("Iterações")
#         plt.ylabel("Custo")
#         plt.grid(True)
#         plt.legend()

#         # Save the plot
#         output_file = os.path.join(output_folder, filename.replace(".txt", ".png"))
#         plt.savefig(output_file)
#         plt.close()  # Close the figure to free memory
