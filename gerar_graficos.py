import os
import matplotlib.pyplot as plt

folder_path = "timestamps"
output_folder = "graficos"

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
            if partes[0] == "real":
                return float(partes[1])
                                  
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


if __name__ == "__main__":
    instancia = input("Insira a instancia sem .tsp:\n>> ")
    algo = int(input("Insira o alg. de constr. (0 ou 1):\n>> "))

    plotar_instancia_velocidade(instancia, algo)