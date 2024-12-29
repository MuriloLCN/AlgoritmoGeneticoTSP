import os
import shutil
import matplotlib.pyplot as plt

def gerar_graficos(instancia: str) -> None:

    labels = []
    listas_x = []
    listas_y = []

    for timestamp in timestamps_gerados:
        if (timestamp.replace(".txt", "")).endswith(instancia):
            with open(timestamp) as valores:
                lx = []
                ly = []
                for linha in valores:
                    x, y = map(float, linha.split())
                    lx.append(x)
                    ly.append(y)
            listas_x.append(lx)
            listas_y.append(ly)
            labels.append(timestamp.replace(".txt", ""))

    for i in range(len(listas_x)):
        plt.plot(listas_x[i], listas_y[i], label=labels[i])

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

populacao_inicial = ["0", "1"]

numero_individuos = ["500", "1000"]

chances_mutacao = ["0.075"] # Acho que mexer muito não faz muita diferença e quadruplica os testes!

criterio_parada = ["1", "256"] # Pra mostrar uma grande diferença!

# TODO: Acho que ele quer TODAS as instância do Moodle, e em ordem também :/
arquivos_de_teste =[
                    "u574.tsp"
                    "pcb1173.tsp",
                    "pr1002.tsp",
                    "brd14051.tsp",
                    "fnl4461.tsp",
                    "d15112.tsp",
                    "pla33810.tsp",
                    "pla85900.tsp",
                    ]

melhores_solucoes_conhecidas = [
                                36905,    # u574
                                56892,    # pcb1173
                                295045,   # pr1002
                                469385,   # brd14051
                                182566,   # fnl4461
                                1573084,  # d15112
                                66048945, # pla33810 
                                142382641,# pla85900
                                ]

timestamps_gerados = []  # Arquivos .txt gerados e salvos

resultados_obtidos = []  # OBS: Guardar tuplas (custo, tempo), na mesma ordem do timestamps_gerados

if __name__ == "__main__":
    
    # executavel.exe [instancia] [construcao_inicial] [tamanho_populacao] [chance_mutacao] [criterio_parada]
    
    num_total_testes = len(populacao_inicial) * len(numero_individuos) * len(chances_mutacao) * len(criterio_parada) * len(arquivos_de_teste)

    num_executado_de_testes = 0

    for construcao_inicial in populacao_inicial:
        for tam_pop in numero_individuos:
            for chance_mut in chances_mutacao:
                for qtd_parada in criterio_parada:                    
                    for instancia in arquivos_de_teste:
                        
                        str_nome_teste = f"{construcao_inicial}-{tam_pop}-{chance_mut.replace(".","")}-{qtd_parada}-{instancia.replace(".tsp", "")}"

                        # Aqui dá pra fazer um igualzinho pra linux e só trocar na hora de executar mesmo
                        str_execucao_windows = f"tsp.exe instancias/{instancia} {construcao_inicial} {tam_pop} {chance_mut} {qtd_parada}"
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

    for instancia in arquivos_de_teste:
        instancia = instancia.replace(".tsp", "")

        gerar_graficos(instancia, timestamps_gerados)

    for variacao in timestamps_gerados:
        variacao = "".join(variacao.split("-")[0:-1])

        if os.path.isfile(f"tabelas/{variacao}.txt"):
            continue

        gerar_tabelas(variacao)
