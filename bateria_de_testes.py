import os
import shutil
import matplotlib.pyplot as plt

def gerar_graficos(instancia: str) -> None:

    labels = []
    listas_x_iteracoes = []
    listas_y_custoMinimo = []
    listas_y_custoMedio = []

    for timestamp in timestamps_gerados:
        if (timestamp.replace(".txt", "")).endswith(instancia):
            with open(timestamp) as valores:
                lx = []
                ly = []
                lz = []
                for linha in valores:
                    valores = linha.split()
                    lx.append(float(valores[0]))
                    # Valor [1] é o tempo, não será usado
                    ly.append(float(valores[2]))
                    lz.append(float(valores[3]))
            listas_x_iteracoes.append(lx)
            listas_y_custoMinimo.append(ly)
            listas_y_custoMedio.append(lz)
            labels.append(timestamp.replace(".txt", ""))

    for i in range(len(listas_x_iteracoes)):
        plt.plot(listas_x_iteracoes[i], listas_y_custoMinimo[i], label=(labels[i] + " (mínimo)"))
        plt.plot(listas_x_iteracoes[i], listas_y_custoMedio[i], label=labels[i] + " (custo médio)")

    plt.xlabel("Tempo (segundos)")
    plt.ylabel("Custo")
    plt.grid(True)

    plt.legend()

    plt.savefig(f"graficos/{instancia}.png")

def gerar_tabelas(variacao: str) -> None:
    with open(f"tabelas/{variacao}.txt", "w+") as arquivo:
        arquivo.write("\\begin{table}[H]\n")
        arquivo.write("\\centering\n")
        arquivo.write("\\begin{tabular}{|c|c|c|c|c|}\n")
        arquivo.write("\\hline\n")
        arquivo.write(f"\\Instância & MS & {variacao} & Tempo (s) & GAP%\n")
        arquivo.write("\\hline\n")

        for i in range(len(timestamps_gerados)):
            if timestamps_gerados[i].startswith(variacao):
                instancia = (timestamps_gerados[i].replace(".txt", "")).split(";")[-1]

                indice_instancia = 0

                for j, con in enumerate(arquivos_de_teste):
                    if con.replace(".tsp", "") == instancia:
                        indice_instancia = j

                ms = melhores_solucoes_conhecidas[indice_instancia]

                custo = resultados_obtidos[i][0]

                tempo = resultados_obtidos[i][1]

                gap = (custo - ms)/ms

                gap *= 100

                arquivo.write(f"{instancia} & {ms} & {custo} & {tempo} & {round(gap, 2)}")

        arquivo.write("\\hline\n")
        arquivo.write("\\end{tabular}[H]\n")
        arquivo.write("\\caption{Comparação entre as melhores soluções conhecidas da literatura e as obtidas pelo algoritmo}[H]\n")
        arquivo.write("\\label{tab:my_label}\n")
        arquivo.write("\\end{table}\n")

# Trocar operador
algoritmo_cruzamento = ["0", "1"]

numero_individuos = ["50", "100"]

chances_mutacao = ["0.005", "0.01", "0.1"]

criterio_parada = ["100", "500", "1000"]

arquivos_de_teste = [
                     "u574.tsp",
                     "kroA100.tsp",
                     "a280.tsp"
]

melhores_solucoes_conhecidas = [
    36905, 21282, 2579
]

# arquivos_de_teste =[
#                     "u574.tsp"
#                     "pcb1173.tsp",
#                     "pr1002.tsp",
#                     "brd14051.tsp",
#                     "fnl4461.tsp",
#                     "d15112.tsp",
#                     "pla33810.tsp",
#                     "pla85900.tsp",
#                     ]

# melhores_solucoes_conhecidas = [
#                                 36905,    # u574
#                                 56892,    # pcb1173
#                                 259045,   # pr1002
#                                 469385,   # brd14051
#                                 182566,   # fnl4461
#                                 1573084,  # d15112
#                                 66048945, # pla33810 
#                                 142382641,# pla85900
#                                 ]

timestamps_gerados = []  # Arquivos .txt gerados e salvos

resultados_obtidos = []  # OBS: Guardar tuplas (custo, tempo), na mesma ordem do timestamps_gerados

if __name__ == "__main__":
    
    # executavel.exe [instancia] [operador_crz] [tamanho_populacao] [chance_mutacao] [criterio_parada]
    
    num_total_testes = len(algoritmo_cruzamento) * len(numero_individuos) * len(chances_mutacao) * len(criterio_parada) * len(arquivos_de_teste)

    num_executado_de_testes = 0

    for operador_crz in algoritmo_cruzamento:
        for tam_pop in numero_individuos:
            for chance_mut in chances_mutacao:
                for qtd_parada in criterio_parada:                    
                    for instancia in arquivos_de_teste:
                        
                        str_nome_teste = f"{operador_crz}-{tam_pop}-{chance_mut.replace(".","")}-{qtd_parada}-{instancia.replace(".tsp", "")}"

                        # Aqui dá pra fazer um igualzinho pra linux e só trocar na hora de executar mesmo
                        str_execucao_windows = f"tsp.exe instancias/{instancia} {operador_crz} {tam_pop} {chance_mut} {qtd_parada}"
                        print(f"Realizando teste com os seguintes parametros: {str_execucao_windows}")
                        
                        # Verifica se existe já um timestamp para essa execução, se sim, pula
                        if os.path.isfile(f"timestamps/{str_nome_teste}.txt"):
                            continue

                        os.system(str_execucao_windows)
                        
                        # Abrir o timestamp e pegar a última linha (tempo e custo)
                                      
                        shutil.move('timestamp.txt', f'timestamps/{str_nome_teste}.txt')

                        timestamps_gerados.append(f"{str_nome_teste}.txt")  

                        # valores = (tempo, custo)
                        # resultados_obtidos.append(valores)

                        num_executado_de_testes += 1
                        print(f"Teste numero {num_executado_de_testes} de {num_total_testes} feito [{round((num_executado_de_testes*100)/num_total_testes, 2)}%]")

    # for instancia in arquivos_de_teste:
        # instancia = instancia.replace(".tsp", "")

        # gerar_graficos(instancia)

    # for variacao in timestamps_gerados:
    #     variacao = "".join(variacao.split("-")[0:-1])

    #     print(f"Gerando tabela para variacao: {variacao}")

    #     if os.path.isfile(f"tabelas/{variacao}.txt"):
    #         continue

    #     gerar_tabelas(variacao)
