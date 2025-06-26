import os
import shutil
import matplotlib.pyplot as plt

algoritmo_cruzamento = ["1"]

numero_individuos = ["50"]

chances_mutacao = ["0.01"]

criterio_parada = ["500"]

numero_threads = ["1", "2", "3", "4", "5", "6", "7", "8"]

# Aumentar p/ instâncias dps que estiver funcionando
arquivos_de_teste = [
                     "pcb1173.tsp",
                     "pla33810.tsp"
                     ]
# timestamps_gerados = []  # Arquivos .txt gerados e salvos

# resultados_obtidos = []  # OBS: Guardar tuplas (custo, tempo), na mesma ordem do timestamps_gerados

if __name__ == "__main__":
    
    # executavel.exe [instancia] [operador_crz] [tamanho_populacao] [chance_mutacao] [criterio_parada]
    
    num_total_testes = len(algoritmo_cruzamento) * len(numero_individuos) * len(chances_mutacao) * len(criterio_parada) * len(arquivos_de_teste) * len(numero_threads)

    num_executado_de_testes = 0

    # Testes com threads
    print("Iniciando testes com threads")
    for operador_crz in algoritmo_cruzamento:
        for tam_pop in numero_individuos:
            for chance_mut in chances_mutacao:
                for qtd_parada in criterio_parada:                    
                    for instancia in arquivos_de_teste:
                        for num_threads in numero_threads:    
                            # str_nome_teste = f"{operador_crz}-{tam_pop}-{chance_mut.replace(".","")}-{qtd_parada}-{instancia.replace(".tsp", "")}"
                            temp = instancia.replace(".tsp", "")
                            str_nome_teste = f"{temp}-{operador_crz}-{num_threads}"

                            str_execucao_linux = f"/usr/bin/time -p ./tsp instancias/{instancia} {operador_crz} {tam_pop} {chance_mut} {qtd_parada} {num_threads} 2> tempo.txt"
                            print(f"Realizando teste com os seguintes parametros: {str_execucao_linux}")
                            
                            if os.path.isfile(f"timestamps/{str_nome_teste}.txt"):
                                continue

                            os.system(str_execucao_linux)
                                        
                            shutil.move('timestamp.txt', f'timestamps/{str_nome_teste}.txt')
                            shutil.move('tempo_construcao.txt', f'timestamps/constr-{str_nome_teste}.txt')
                            shutil.move('tempo.txt', f'timestamps/tempo-{str_nome_teste}.txt')

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

                        str_execucao_linux = f"/usr/bin/time -p ./tsp_sequencial instancias/{instancia} {operador_crz} {tam_pop} {chance_mut} {qtd_parada} 2> tempo.txt"
                        
                        # Alpha
                        if operador_crz == "1":
                            str_execucao_linux = f"/usr/bin/time -p ./tsp_sequencial instancias/{instancia} {operador_crz} {tam_pop} {chance_mut} {qtd_parada} 0.01 2> tempo.txt"
                        
                        print(f"Realizando teste com os seguintes parametros: {str_execucao_linux}")
                        
                        if os.path.isfile(f"timestamps/{str_nome_teste}.txt"):
                            continue

                        os.system(str_execucao_linux)
                                    
                        shutil.move('timestamp.txt', f'timestamps/{str_nome_teste}.txt')
                        shutil.move('tempo_construcao.txt', f'timestamps/constr-{str_nome_teste}.txt')
                        shutil.move('tempo.txt', f'timestamps/tempo-{str_nome_teste}.txt')

                        num_executado_de_testes += 1
                        print(f"Teste numero {num_executado_de_testes} de {num_total_testes} feito [{round((num_executado_de_testes*100)/num_total_testes, 2)}%]")

    
