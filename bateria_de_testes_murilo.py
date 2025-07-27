import os
import shutil
import matplotlib.pyplot as plt

algoritmo_cruzamento = ["1"]

numero_individuos = ["50"]

chances_mutacao = ["0.01"]

criterio_parada = ["10"]

numero_processos = ["2", "3", "4", "5", "6", "7", "8"]

# Aumentar p/ instâncias dps que estiver funcionando
arquivos_de_teste = [
                     "fnl4461.tsp",
                     "d15112.tsp"
]

def ler_timestamp(arquivo: str) -> tuple[list[int],list[float]]:
    # Retorna a lista iteracao, tempo
    lista_tempo = []
    lista_iteracao = []

    with open(arquivo, "r+") as arquivo:
        for linha in arquivo:
            partes = linha.split()
            lista_iteracao.append(int(partes[0]))
            lista_tempo.append(float(partes[1]))
    
    return lista_iteracao, lista_tempo

def grafico(instancia: str):
    # Obs: não leva em consideração algoritmos de construção, assume que é '1'

    if instancia.endswith(".tsp"):
        instancia = instancia.replace(".tsp", "")

    os.makedirs("graficos", exist_ok=True)
    nome_sequencial = f"timestamps/{instancia}-1-sequencial.txt"
    nomes_np = [f"timestamps/{instancia}-1-{i}.txt" for i in numero_processos]

    iter_sequencial, tempo_sequencial = ler_timestamp(nome_sequencial)

    plt.figure()

    plt.plot(tempo_sequencial, iter_sequencial, label=f"{instancia}: sequencial")

    for nome in nomes_np:
        iter_n, tempos_n = ler_timestamp(nome)
        plt.plot(tempos_n, iter_n, label=f"{instancia}: np={nome.split('-')[-1].replace(".txt","")}")

    plt.title(f"Velocidades da instância {instancia}")

    plt.xlabel("Tempo (s)")
    plt.ylabel("Número de iterações")
    plt.grid(True)
    plt.legend()

    arquivo_saida = f"graficos/{instancia}.png"
    plt.savefig(arquivo_saida)
    plt.close()

def tabela(instancia: str):
    if instancia.endswith(".tsp"):
        instancia = instancia.replace(".tsp", "")

    os.makedirs("graficos", exist_ok=True)
    nome_sequencial = f"timestamps/{instancia}-1-sequencial.txt"
    nomes_np = [f"timestamps/{instancia}-1-{i}.txt" for i in numero_processos]

    with open(f"tabelas/tabela-{instancia}.txt", "w+") as arquivo_saida:

        iters_seq, tempo_seq = ler_timestamp(nome_sequencial)
        num_iters_seq = iters_seq[-1]
        tempo_tot_seq = tempo_seq[-1]
        velocidade_seq = num_iters_seq / tempo_tot_seq

        arquivo_saida.write("\\begin{table}[H]\n")
        arquivo_saida.write("\\centering\n")
        arquivo_saida.write("\\begin{tabular}{|c|c|c|c|c|c|}\n")
        arquivo_saida.write("\\hline\n")
        arquivo_saida.write("\\textbf{Instância} & \\textbf{NP} & \\textbf{Tempo (s)} & \\textbf{Iter.} & \\textbf{$dN/dt$} & \\textbf{Vezes Mais Rápido} \\\\ \n")
        arquivo_saida.write("\\hline\n")
        arquivo_saida.write("\\multirow{9}{*}{"+instancia+"} & Sequencial & " + f"{tempo_tot_seq:.2f}" + " & " + f"{num_iters_seq}" + " & " + f"{velocidade_seq:.2f}" + " & - ")
        arquivo_saida.write("\\\\ \n")      

        for nome in nomes_np:
            iters_n, tempo_n = ler_timestamp(nome)
            num_iters_n = iters_n[-1]
            tempo_tot_n = tempo_n[-1]
            velocidade_n = num_iters_n / tempo_tot_n
            
            prop_n = velocidade_n / velocidade_seq

            arquivo_saida.write(f"& {nome.split('-')[-1].replace(".txt","")} & {tempo_tot_n:.2f} & {num_iters_n} & {velocidade_n:.2f} & {prop_n:.2f}x ")
            arquivo_saida.write("\\\\ \n")

        arquivo_saida.write("\\hline\n")
        arquivo_saida.write("\\end{tabular}\n")
        arquivo_saida.write("\\caption{Comparativo de desempenho para a instancia " + instancia + ".}\n")
        arquivo_saida.write("\\label{tab:tempos-" + instancia + "}\n")
        arquivo_saida.write("\\end{table}\n")

                # \begin{table}[H]
                # \centering
                # \begin{tabular}{|c|c|c|c|c|c|}
                # \hline
                # \textbf{Instância} & \textbf{NP} & \textbf{Tempo (s)} & \textbf{Iter.} & \textbf{$dN/dt$} & \textbf{Vezes Mais Rápido} \\ 
                # \hline
                # \multirow{9}{*}{d15112} & Sequencial & 3611.9 & 140 & 0.0387 & - \\ 
                # & 1 & 3621.0 & 143 & 0.0395 & 1.02x \\ 
                # & 2 & 1904.69 & 126 & 0.0662 & 1.71x \\ 
                # & 3 & 1355.96 & 113 & 0.0833 & 2.15x \\ 
                # & 4 & 1041.68 & 100 & 0.096 & 2.48x \\ 
                # & 5 & 909.56 & 92 & 0.1011 & 2.61x \\ 
                # & 6 & 765.67 & 82 & 0.1071 & 2.76x \\ 
                # & 7 & 665.74 & 73 & 0.1097 & 2.83x \\ 
                # & 8 & 582.08 & 67 & 0.1151 & 2.97x \\ 
                # \hline
                # \end{tabular}
                # \caption{Comparativo de desempenho para a instancia d15112.}
                # \label{tab:tempos-d15112}
                # \end{table}

if __name__ == "__main__":
    
    # executavel.exe [instancia] [operador_crz] [tamanho_populacao] [chance_mutacao] [criterio_parada]
    
    num_total_testes = len(algoritmo_cruzamento) * len(numero_individuos) * len(chances_mutacao) * len(criterio_parada) * len(arquivos_de_teste) * len(numero_processos)

    num_executado_de_testes = 0

    # Testes com threads
    print("Iniciando testes com processos")
    for operador_crz in algoritmo_cruzamento:
        for tam_pop in numero_individuos:
            for chance_mut in chances_mutacao:
                for qtd_parada in criterio_parada:                    
                    for instancia in arquivos_de_teste:
                        for np in numero_processos:    
                            # str_nome_teste = f"{operador_crz}-{tam_pop}-{chance_mut.replace(".","")}-{qtd_parada}-{instancia.replace(".tsp", "")}"
                            temp = instancia.replace(".tsp", "")
                            str_nome_teste = f"{temp}-{operador_crz}-{np}"

                            str_execucao_linux = f"mpirun -np {np} --oversubscribe tsp instancias/{instancia} {operador_crz} {tam_pop} {chance_mut} {qtd_parada} 0.01"
                            print(f"Realizando teste com os seguintes parametros: {str_execucao_linux}")
                            
                            if os.path.isfile(f"timestamps/{str_nome_teste}.txt"):
                                continue

                            os.system(str_execucao_linux)
                                        
                            shutil.move('timestamp.txt', f'timestamps/{str_nome_teste}.txt')

                            num_executado_de_testes += 1
                            print(f"Teste numero {num_executado_de_testes} de {num_total_testes} feito [{round((num_executado_de_testes*100)/num_total_testes, 2)}%]")

    print("Iniciando testes sequenciais")

    num_total_testes = len(algoritmo_cruzamento) * len(numero_individuos) * len(chances_mutacao) * len(criterio_parada) * len(arquivos_de_teste)

    num_executado_de_testes = 0

    for operador_crz in algoritmo_cruzamento:
        for tam_pop in numero_individuos:
            for chance_mut in chances_mutacao:
                for qtd_parada in criterio_parada:                    
                    for instancia in arquivos_de_teste:
                        # str_nome_teste = f"{operador_crz}-{tam_pop}-{chance_mut.replace(".","")}-{qtd_parada}-{instancia.replace(".tsp", "")}"
                        temp = instancia.replace(".tsp", "")
                        str_nome_teste = f"{temp}-{operador_crz}-sequencial"

                        str_execucao_linux = f"./tsp_sequencial instancias/{instancia} {operador_crz} {tam_pop} {chance_mut} {qtd_parada}"
                        
                        # Alpha
                        if operador_crz == "1":
                            str_execucao_linux = f"./tsp_sequencial instancias/{instancia} {operador_crz} {tam_pop} {chance_mut} {qtd_parada} 0.01"
                        
                        print(f"Realizando teste com os seguintes parametros: {str_execucao_linux}")
                        
                        if os.path.isfile(f"timestamps/{str_nome_teste}.txt"):
                            continue

                        os.system(str_execucao_linux)
                                    
                        shutil.move('timestamp.txt', f'timestamps/{str_nome_teste}.txt')

                        num_executado_de_testes += 1
                        print(f"Teste numero {num_executado_de_testes} de {num_total_testes} feito [{round((num_executado_de_testes*100)/num_total_testes, 2)}%]")

    for instancia in arquivos_de_teste:
        grafico(instancia)
        tabela(instancia)