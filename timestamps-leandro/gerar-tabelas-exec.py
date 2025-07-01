import os
import re
import glob
from typing import Optional, List, Dict, Any

def parse_time_from_string(time_str: str) -> Optional[float]:
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

def get_total_time_from_file(filepath: str) -> Optional[float]:
    """
    *** ATUALIZADO ***
    Lê um arquivo de resultado e extrai o tempo da linha 'real'.
    """
    if not os.path.exists(filepath):
        return None
    try:
        with open(filepath, 'r') as f:
            for line in f:
                # Procura pela linha que começa com 'real'
                clean_line = line.strip()
                if clean_line.startswith('real'):
                    # A linha é como: 'real\t9m23.449s'
                    # Divide a linha por espaços/tabs e pega a última parte
                    parts = clean_line.split()
                    time_str = parts[-1]
                    return parse_time_from_string(time_str)
        
        # Se o loop terminar sem encontrar a linha 'real'
        print(f"Aviso: Linha 'real' não encontrada em {filepath}")
        return None

    except Exception as e:
        print(f"Erro ao processar o arquivo {filepath}: {e}")
        return None

def generate_latex_table(instance_name: str, sequential_data: Dict, parallel_data: List[Dict]):
    """
    Gera e imprime o código LaTeX para uma única tabela de instância.
    (Esta função não precisou de alterações)
    """
    instance_latex_name = instance_name.replace('_', r'\_')

    print("% Para usar multirow, adicione \\usepackage{multirow} ao seu preâmbulo LaTeX")
    print("\\begin{table}[h!]")
    print("\\centering")
    print(f"\\caption{{Comparativo de desempenho para a instância {instance_latex_name}}}")
    print("\\begin{tabular}{l c r r}")
    print("\\hline")
    print("\\textbf{Instância} & \\textbf{Nº de Threads} & \\textbf{Tempo (s)} & \\textbf{\\% Mais Rápido} \\\\")
    print("\\hline")

    seq_time_str = f"{sequential_data['time_s']:.2f}"
    num_rows = 1 + len(parallel_data)
    print(f"\\multirow{{{num_rows}}}{{*}}{{{instance_latex_name}}} & Sequencial & {seq_time_str} & - \\\\")

    for res in parallel_data:
        time_str = f"{res['time_s']:.2f}" if res['time_s'] is not None else "N/A"
        speedup_str = f"{res['speedup_pct']:.2f}\\%" if res['speedup_pct'] is not None else "N/A"
        print(f" & {res['threads']} & {time_str} & {speedup_str} \\\\")

    print("\\hline")
    print(f"\\end{{tabular}}")
    print(f"\\label{{tab:{instance_name}}}")
    print("\\end{table}")


def main():
    """
    Função principal para encontrar arquivos, processar dados e gerar tabelas.
    (Esta função não precisou de alterações)
    """
    sequential_files = glob.glob('tempo-*-sequencial.txt')
    
    if not sequential_files:
        print("Nenhum arquivo de resultado sequencial encontrado (ex: 'tempo-pla85900-1-sequencial').")
        print("Certifique-se de que o script está no mesmo diretório que os arquivos de log.")
        return

    for seq_file in sequential_files:
        match = re.match(r'tempo-(.+)-(\d+)-sequencial', seq_file)
        if not match:
            continue

        instance_name = match.group(1)
        exp_id = match.group(2)
        
        print(f"% --- Processando instância: {instance_name} (ID do Experimento: {exp_id}) ---")

        sequential_time_s = get_total_time_from_file(seq_file)
        if sequential_time_s is None or sequential_time_s == 0:
            print(f"Erro: Não foi possível obter um tempo válido do arquivo sequencial {seq_file}. Pulando esta instância.")
            continue
            
        sequential_data = {"time_s": sequential_time_s}
        parallel_results = []

        parallel_files = glob.glob(f'tempo-{instance_name}-{exp_id}-*')
        max_threads = 0
        for p_file in parallel_files:
            thread_match = re.match(rf'tempo-{instance_name}-{exp_id}-(\d+)$', p_file)
            if thread_match:
                max_threads = max(max_threads, int(thread_match.group(1)))
        
        if max_threads == 0:
            print(f"Aviso: Nenhum arquivo paralelo encontrado para a instância {instance_name}.")
            continue

        for i in range(1, max_threads + 1):
            par_filename = f"tempo-{instance_name}-{exp_id}-{i}"
            parallel_time_s = get_total_time_from_file(par_filename)
            
            speedup_pct = None
            if parallel_time_s is not None and parallel_time_s > 0:
                speedup_pct = ((sequential_time_s - parallel_time_s) / sequential_time_s) * 100
            
            parallel_results.append({
                "threads": i,
                "time_s": parallel_time_s,
                "speedup_pct": speedup_pct
            })

        generate_latex_table(instance_name, sequential_data, parallel_results)
        print("\n\n")


if __name__ == "__main__":
    main()