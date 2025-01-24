import os
import matplotlib.pyplot as plt

folder_path = "timestamps"
output_folder = "graficos"

os.makedirs(output_folder, exist_ok=True)

for filename in os.listdir(folder_path):
    if filename.endswith(".txt"):
        arquivo_timestamp = os.path.join(folder_path, filename)

        labels = []
        lista_x_iteracoes = []
        lista_y_custoMinimo = []
        lista_y_custoMedio = []
        lista_y_piorPop = []
        lista_y_melhorPop = []

        with open(arquivo_timestamp) as valores:
            for linha in valores:
                valores_split = linha.split()
                lista_x_iteracoes.append(float(valores_split[0]))
                # Valor [1] é o tempo, não será usado
                lista_y_custoMinimo.append(float(valores_split[2]))
                lista_y_custoMedio.append(float(valores_split[3]))
                lista_y_piorPop.append(float(valores_split[4]))
                lista_y_melhorPop.append(float(valores_split[5]))

        plt.figure()
        plt.plot(lista_x_iteracoes, lista_y_custoMinimo, label="Custo mínimo")
        plt.plot(lista_x_iteracoes, lista_y_custoMedio, label="Custo médio da população")
        plt.plot(lista_x_iteracoes, lista_y_piorPop, label="Pior indivíduo da geração")
        plt.plot(lista_x_iteracoes, lista_y_melhorPop, label="Melhor indivíduo da geração")

        plt.title(f"Gráfico: {filename}")  # Add the filename as the title


        plt.xlabel("Iterações")
        plt.ylabel("Custo")
        plt.grid(True)
        plt.legend()

        # Save the plot
        output_file = os.path.join(output_folder, filename.replace(".txt", ".png"))
        plt.savefig(output_file)
        plt.close()  # Close the figure to free memory