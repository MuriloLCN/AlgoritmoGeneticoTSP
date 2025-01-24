# import os
# import matplotlib.pyplot as plt

# folder_path = "timestamps"
# output_folder = "graficos"

# melhores_solucoes = {
#     "u574": 36905,
#     "pcb1173": 56892,
#     "pr1002": 259045,
#     "brd14051": 469385,
#     "fnl4461": 182566,
#     "d15112": 157308,
#     "pla33810": 66048945,
#     "pla85900": 142382641,
#     "kroA100": 21282,
#     "a280": 2579
# }

# os.makedirs(output_folder, exist_ok=True)

# with open(f"tabelas/resultados_todos.txt", "w+") as arquivo:
#     arquivo.write("\\begin{longtable}{|c|c|c|c|c|c|c|c|}\n")
#     arquivo.write("\\hline\n")
#     arquivo.write(f"Instância & Alg. cruz. & Tam. pop. & Chance mut.\\% & Crit. par. & Resultado & MS & GAP\\% \\\\\n")
#     arquivo.write("\\hline\n")

#     for filename in os.listdir(folder_path):
#         if filename.endswith(".txt"):
#             arquivo_timestamp = os.path.join(folder_path, filename)
            
#             with open(arquivo_timestamp) as valores:
#                 for line in valores:
#                     pass
#                 last_line = line

#                 partes_arquivo = filename.split("-")

#                 algoritmo_cruzamento = "ZX"
#                 if partes_arquivo[0] == "1":
#                     algoritmo_cruzamento = "EXX"

#                 tamanho_pop = partes_arquivo[1]

#                 chance_mutacao = partes_arquivo[2]
#                 chance_mutacao = chance_mutacao[1:]
#                 chance_mutacao = "0." + chance_mutacao
#                 chance_mutacao = float(chance_mutacao)
#                 chance_mutacao *= 100

#                 cond_parada = partes_arquivo[3]

#                 instancia = partes_arquivo[4].replace(".txt", "")

#                 resultado_obtido = float(last_line.split()[2])

#                 ms = melhores_solucoes[instancia]

#                 gap = ((resultado_obtido - ms) / ms) * 100

#                 chance_mutacao = f"{chance_mutacao:.2f}"
#                 resultado_obtido = f"{resultado_obtido:.2f}"
#                 gap = f"{gap:.2f}"

#                 arquivo.write(f"{instancia} & {algoritmo_cruzamento} & {tamanho_pop} & {chance_mutacao} & {cond_parada} & {resultado_obtido} & {ms} & {gap} \\\\\n")

#     arquivo.write("\\hline\n")
#     arquivo.write("\\caption{Comparação entre as melhores soluções conhecidas da literatura e as obtidas pelo algoritmo com base nos parâmetros de execução}\n")
#     arquivo.write("\\label{tab:my_label}\n")
#     arquivo.write("\\end{longtable}\n")

import os
import matplotlib.pyplot as plt

folder_path = "timestamps"
output_folder = "graficos"

melhores_solucoes = {
    "u574": 36905,
    "pcb1173": 56892,
    "pr1002": 259045,
    "brd14051": 469385,
    "fnl4461": 182566,
    "d15112": 157308,
    "pla33810": 66048945,
    "pla85900": 142382641,
    "kroA100": 21282,
    "a280": 2579
}

os.makedirs(output_folder, exist_ok=True)

rows = []  # To store rows for sorting

for filename in os.listdir(folder_path):
    if filename.endswith(".txt"):
        arquivo_timestamp = os.path.join(folder_path, filename)
        
        with open(arquivo_timestamp) as valores:
            for line in valores:
                pass
            last_line = line

            partes_arquivo = filename.split("-")

            algoritmo_cruzamento = "ZX"
            if partes_arquivo[0] == "1":
                algoritmo_cruzamento = "EXX"

            tamanho_pop = partes_arquivo[1]

            chance_mutacao = partes_arquivo[2]
            chance_mutacao = chance_mutacao[1:]
            chance_mutacao = "0." + chance_mutacao
            chance_mutacao = float(chance_mutacao)
            chance_mutacao *= 100

            cond_parada = partes_arquivo[3]

            instancia = partes_arquivo[4].replace(".txt", "")

            resultado_obtido = float(last_line.split()[2])

            ms = melhores_solucoes[instancia]

            gap = ((resultado_obtido - ms) / ms) * 100

            # Format values to two decimal places
            chance_mutacao = f"{chance_mutacao:.2f}"
            resultado_obtido = f"{resultado_obtido:.2f}"
            gap = f"{gap:.2f}"

            # Append a tuple with all necessary data for sorting
            rows.append((float(gap), instancia, algoritmo_cruzamento, tamanho_pop, chance_mutacao, cond_parada, resultado_obtido, ms, gap))

# Sort rows by GAP (first element of the tuple) in descending order
rows.sort(reverse=True, key=lambda x: x[0])

with open(f"tabelas/resultados_todos.txt", "w+") as arquivo:
    arquivo.write("\\begin{longtable}{|c|c|c|c|c|c|c|c|}\n")
    arquivo.write("\\hline\n")
    arquivo.write(f"Instância & Alg. cruz. & Tam. pop. & Chance mut.\\% & Crit. par. & Resultado & MS & GAP\\% \\\\\n")
    arquivo.write("\\hline\n")

    # Write sorted rows
    for row in rows:
        _, instancia, algoritmo_cruzamento, tamanho_pop, chance_mutacao, cond_parada, resultado_obtido, ms, gap = row
        arquivo.write(f"{instancia} & {algoritmo_cruzamento} & {tamanho_pop} & {chance_mutacao} & {cond_parada} & {resultado_obtido} & {ms} & {gap} \\\\\n")

    arquivo.write("\\hline\n")
    arquivo.write("\\caption{Comparação entre as melhores soluções conhecidas da literatura e as obtidas pelo algoritmo com base nos parâmetros de execução}\n")
    arquivo.write("\\label{tab:my_label}\n")
    arquivo.write("\\end{longtable}\n")
