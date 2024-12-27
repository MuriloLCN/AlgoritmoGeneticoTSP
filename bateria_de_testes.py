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

representacoes_cromossomos = ["per"]

populacao_inicial = ["vmp", "ivmd"]

selecao_individuos = ["torneio"]

operadores_cruzamento = ["op_teste"]

mutacao = ["mut_teste"]

manutencao_populacao = ["steady"]

numero_individuos = [100, 1000, 10000]

# TODO: Acho que ele quer TODAS as instância do Moodle, e em ordem também :/
arquivos_de_teste =["brd14051.tsp",
                    "d15112.tsp",
                    "fnl4461.tsp",
                    "pcb1173.tsp",
                    "pla33810.tsp",
                    "pla85900.tsp",
                    "pr1002.tsp",
                    "u574.tsp"]

melhores_solucoes_conhecidas = []

timestamps_gerados = []

if __name__ == "__main__":
    
    # Testa todas as possibilidades
    for repr_cromossomos in representacoes_cromossomos:
        for pop_init in populacao_inicial:
            for selec_indiv in selecao_individuos:
                for op_cruz in operadores_cruzamento:
                    for mut in mutacao:
                        for manut_populacao in manutencao_populacao:
                            for num_indiv in numero_individuos:
                                for instancia in arquivos_de_teste:
                                    str_nome_teste = f"{repr_cromossomos};{pop_init};{selec_indiv};{op_cruz};{mut};{manut_populacao};{num_indiv};{instancia.replace(".tsp", "")}"
                                    print(f"Realizando teste com os seguintes paramentros: {str_nome_teste}")

                                    # Verificar se existe já um timestamp para essa execução, se sim, pule este caso

                                    # Montar string de execução para o arquivo em C

                                    # Chamar o sistema com o executável

                                    # Renomear o timestamp gerado para o arquivo com o nome específico

                                    # Guardar o nome no timestamps_gerados (blablabla;instancia.txt)

    for instancia in arquivos_de_teste:
        instancia = instancia.replace(".tsp", "")

        gerar_graficos(instancia, timestamps_gerados)
