import os
import shutil
import matplotlib.pyplot as plt

algoritmo_cruzamento = ["0", "1"]

numero_individuos = ["50", "100"]

chances_mutacao = ["0.005", "0.01", "0.1"]

criterio_parada = ["100", "500", "1000"]

arquivos_de_teste = [
                     "u574.tsp",
                     "kroA100.tsp",
                     "a280.tsp"
]

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
