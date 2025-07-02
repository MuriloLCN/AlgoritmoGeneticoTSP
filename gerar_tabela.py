# \begin{table}[h!]
#     \centering
#     \begin{tabular}{|c|c|c|c|c|c|}
#     \hline
#     \textbf{Instância} & \textbf{Threads} & \textbf{Tempo (s)} & \textbf{Iter.} & \textbf{$dN/dt$} & \textbf{Vezes Mais Rápido} \\
#     \hline
#     \multirow{12}{*}{pla85900} & Sequencial & 4051.02 & 8 & 0.0019 & - \\
#      & 1 & 3747.51 & 8 & 0.0021 & 1.10x \\
#      & 2 & 2001.63 & 8 & 0.0039 & 2.05x \\
#      & 3 & 1407.94 & 8 & 0.0056 & 2.94x \\
#      & 4 & 1081.33 & 8 & 0.0074 & 3.89x \\
#      & 5 & 912.45  & 7 & 0.0076 & 4.00x \\
#      & 6 & 743.46  & 6 & 0.0080 & 4.21x \\
#      & 7 & 635.78  & 5 & 0.0078 & 4.10x \\
#      & 8 & 563.45  & 5 & 0.0088 & 4.63x \\
#      & 9 & 611.36  & 6 & 0.0098 & 5.15x \\
#      & 10 & 632.33 & 5 & 0.0079 & 4.15x \\
#      & 11 & 592.60 & 5 & 0.0084 & 4.42x \\
#     \hline
#     \end{tabular}
#     \caption{Comparativo de desempenho para a instância pla85900.}
#     \label{tab:tempos-pla85900}
# \end{table}

import os

folder_path = "timestamps"
output_folder = "tabelas"

#instancias = ["pla33810", "d15112", "fnl4461", "pcb1173", "pr1002"]
instancias = ["pr1002"]

finais = ["sequencial", "1", "2", "3", "4", "5", "6", "7", "8"]

def parse_time_from_string(time_str: str) -> float | None:
    """
    Converte uma string de tempo (como '9m23.449s', '1:07:31.02' ou '45.123s')
    para um valor numérico em segundos.
    """
    # NOVO: Trata o formato XmY.YYYs (ex: 9m23.449s)
    if 'm' in time_str:
        try:
            parts = time_str.lower().replace('s', '').split('m')
            minutes = float(parts[0])
            seconds = float(parts[1])
            return minutes * 60 + seconds
        except (ValueError, IndexError):
            return None

    # Mantém a lógica para formatos antigos por robustez
    elif ':' in time_str:
        try:
            parts = time_str.split(':')
            if len(parts) == 3:  # H:M:S
                return int(parts[0]) * 3600 + int(parts[1]) * 60 + float(parts[2])
            elif len(parts) == 2:  # M:S
                return int(parts[0]) * 60 + float(parts[1])
            else:
                return None
        except (ValueError, IndexError):
            return None
            
    else:
        try:
            return float(time_str.rstrip('s'))
        except ValueError:
            return None
        
os.makedirs(output_folder, exist_ok=True)

def ler_numero_iteracoes(arquivo: str) -> int:
    # Retorna a lista iteracao, tempo
    # lista_tempo = []
    try:
        lista_iteracao = []

        with open(os.path.join(folder_path, arquivo), "r+") as arquivo:
            for linha in arquivo:
                partes = linha.split()
                lista_iteracao.append(int(partes[0]))
                # lista_tempo.append(float(partes[1]))

        return lista_iteracao[-1]
    except Exception as e:
        print(f"Erro: {e}")
        return -1
    
def ler_tempo_real(arquivo: str) -> float:
    try:
        with open(os.path.join(folder_path, arquivo), "r+") as arquivo:
            for linha in arquivo:
                partes = linha.split()
                if partes[0] == "real":
                    try:
                        return float(partes[1])
                    except Exception as e:
                        return parse_time_from_string(partes[1])
    except Exception as e:
        print(f"Erro: {e}")
        return -1
    
for instancia in instancias:
    with open(f"{output_folder}/tabela-{instancia}.txt", "w+") as arquivo_saida:
        tempos_execucao = []
        numero_iteracoes = []

        for final in finais:
            numero_iteracoes.append(ler_numero_iteracoes(f"{instancia}-1-{final}.txt"))
            tempos_execucao.append(ler_tempo_real(f"tempo-{instancia}-1-{final}.txt"))

        print(instancia)
        print(numero_iteracoes)
        print(tempos_execucao)

        velocidade_seq = numero_iteracoes[0] / tempos_execucao[0]

        arquivo_saida.write("\\begin{table}[h!]\n")
        arquivo_saida.write("\\centering\n")
        arquivo_saida.write("\\begin{tabular}{|c|c|c|c|c|c|}\n")
        arquivo_saida.write("\\hline\n")
        arquivo_saida.write("\\textbf{Instância} & \\textbf{Threads} & \\textbf{Tempo (s)} & \\textbf{Iter.} & \\textbf{$dN/dt$} & \\textbf{Vezes Mais Rápido} \\\\ \n")
        arquivo_saida.write("\\hline\n")
        arquivo_saida.write("\\multirow{9}{*}{"+instancia+"} & Sequencial & " + str(tempos_execucao[0]) + " & " + str(numero_iteracoes[0]) + " & " + str(velocidade_seq) + " & - ")
        arquivo_saida.write("\\\\ \n")        
        
        for i, final in enumerate(finais):
            if final == "sequencial":
                continue
            velocidade_i = numero_iteracoes[i] / tempos_execucao[i]
            prop_i = velocidade_i / velocidade_seq

            velocidade_i = str(round(velocidade_i,4))
            prop_i = str(round(prop_i, 2))
            arquivo_saida.write(f"& {i} & {tempos_execucao[i]} & {numero_iteracoes[i]} & {velocidade_i} & {prop_i}x ")
            arquivo_saida.write("\\\\ \n")

        arquivo_saida.write("\\hline\n")
        arquivo_saida.write("\\end{tabular}\n")
        arquivo_saida.write("\\caption{Comparativo de desempenho para a instancia " + instancia + ".}\n")
        arquivo_saida.write("\\label{tab:tempos-" + instancia + "}\n")
        arquivo_saida.write("\\end{table}\n")
        
