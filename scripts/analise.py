"""
******************************************************************************
@file           : analise.py
@brief          : Módulo externo Python para análise estatística e geração de gráficos
@note           : Trabalho Final - Etapa 2 - Linguagens de Programação - 2026.1

@author         : Thiago Marques de Oliveira
@date           : Julho, 2026
******************************************************************************

Uso:
    python3 analise.py <funcao> '<json_payload>'

Funções disponíveis:
    estatisticas  -- Calcula descritores estatísticos de uma série de valores.
    anomalias     -- Detecta leituras anômalas via z-score.
    grafico       -- Gera gráfico de histórico em PNG com matplotlib.

Toda saída é um objeto JSON impresso no stdout.
Em caso de erro, retorna: {"erro": "<mensagem>"}
"""

import sys
import json
import os
import tempfile
import statistics


def _importar_numpy():
    """Importa numpy, retornando None se não estiver disponível."""
    try:
        import numpy as np
        return np
    except ImportError:
        return None


def calcular_estatisticas(payload: dict) -> dict:
    """
    Calcula descritores estatísticos de uma série de valores numéricos.

    Parâmetros (payload JSON):
        valores (list[float]): Lista com os valores a analisar.

    Retorno (JSON):
        media, mediana, desvio, min, max, cv (coeficiente de variação em %)
    """
    valores = payload.get("valores", [])
    if not valores:
        raise ValueError("Lista de valores vazia.")

    np = _importar_numpy()

    if np is not None:
        arr = np.array(valores, dtype=float)
        media   = float(np.mean(arr))
        mediana = float(np.median(arr))
        desvio  = float(np.std(arr, ddof=1)) if len(arr) > 1 else 0.0
        minimo  = float(np.min(arr))
        maximo  = float(np.max(arr))
    else:
        media   = statistics.mean(valores)
        mediana = statistics.median(valores)
        desvio  = statistics.stdev(valores) if len(valores) > 1 else 0.0
        minimo  = min(valores)
        maximo  = max(valores)

    cv = (desvio / media * 100.0) if media != 0.0 else 0.0

    return {
        "media":   round(media,   6),
        "mediana": round(mediana, 6),
        "desvio":  round(desvio,  6),
        "min":     round(minimo,  6),
        "max":     round(maximo,  6),
        "cv":      round(cv,      6),
    }


def detectar_anomalias(payload: dict) -> dict:
    """
    Detecta anomalias em uma série de valores usando z-score.

    Parâmetros (payload JSON):
        valores (list[float]): Lista com os valores.
        k (float):             Limiar de z-score (padrão: 2.0).

    Retorno (JSON):
        anomalias (list[int]):   Índices das leituras anômalas.
        zscore    (list[float]): Z-scores correspondentes.
    """
    valores = payload.get("valores", [])
    k = float(payload.get("k", 2.0))

    if len(valores) < 2:
        raise ValueError("São necessários pelo menos 2 valores.")

    np = _importar_numpy()

    if np is not None:
        arr    = np.array(valores, dtype=float)
        media  = float(np.mean(arr))
        desvio = float(np.std(arr, ddof=1))
    else:
        media  = statistics.mean(valores)
        desvio = statistics.stdev(valores)

    if desvio == 0.0:
        return {"anomalias": [], "zscore": []}

    indices_anomalos = []
    zscores_anomalos = []

    for i, v in enumerate(valores):
        z = (v - media) / desvio
        if abs(z) > k:
            indices_anomalos.append(i)
            zscores_anomalos.append(round(z, 4))

    return {
        "anomalias": indices_anomalos,
        "zscore":    zscores_anomalos,
    }


def gerar_grafico(payload: dict) -> dict:
    """
    Gera um gráfico de linha do histórico de um sensor e salva em PNG.

    Parâmetros (payload JSON):
        valores  (list[float]):  Valores do eixo Y.
        labels   (list[str]):    Rótulos do eixo X (timestamps).
        titulo   (str):          Título do gráfico.
        unidade  (str):          Unidade do eixo Y.

    Retorno (JSON):
        arquivo (str): Caminho absoluto do arquivo PNG gerado.
    """
    try:
        import matplotlib
        matplotlib.use("Agg")  # backend sem janela (headless)
        import matplotlib.pyplot as plt
        import matplotlib.dates as mdates
    except ImportError:
        raise ImportError("matplotlib nao instalado. Execute: pip3 install matplotlib")

    valores = payload.get("valores", [])
    labels  = payload.get("labels",  [])
    titulo  = payload.get("titulo",  "Historico")
    unidade = payload.get("unidade", "")

    if not valores:
        raise ValueError("Lista de valores vazia.")

    fig, ax = plt.subplots(figsize=(10, 4))

    # Usa índices numéricos no eixo X para evitar problemas de parsing de datas
    x = list(range(len(valores)))

    ax.plot(x, valores, marker="o", markersize=3, linewidth=1.5,
            color="#005AA6", label=titulo)
    ax.fill_between(x, valores, alpha=0.08, color="#005AA6")

    # Exibe apenas alguns labels para não poluir o gráfico
    passo = max(1, len(labels) // 8)
    ax.set_xticks(x[::passo])
    ax.set_xticklabels(labels[::passo], rotation=30, ha="right", fontsize=7)

    ax.set_title(titulo, fontsize=12, fontweight="bold")
    ax.set_ylabel(unidade, fontsize=10)
    ax.set_xlabel("Timestamp", fontsize=10)
    ax.grid(True, alpha=0.3)
    ax.legend(fontsize=8)

    plt.tight_layout()

    # Salva em arquivo temporário
    fd, caminho = tempfile.mkstemp(prefix="sensorlog_", suffix=".png")
    os.close(fd)
    plt.savefig(caminho, dpi=120, bbox_inches="tight")
    plt.close(fig)

    return {"arquivo": caminho}


# ── Dispatcher principal ──────────────────────────────────────────────────────

FUNCOES = {
    "estatisticas": calcular_estatisticas,
    "anomalias":    detectar_anomalias,
    "grafico":      gerar_grafico,
}

def main():
    if len(sys.argv) < 3:
        print(json.dumps({"erro": "Uso: analise.py <funcao> '<json_payload>'"}))
        sys.exit(1)

    funcao_nome = sys.argv[1].strip()
    payload_str = sys.argv[2].strip()

    if funcao_nome not in FUNCOES:
        print(json.dumps({"erro": f"Funcao desconhecida: '{funcao_nome}'. "
                                   f"Disponiveis: {list(FUNCOES.keys())}"}))
        sys.exit(1)

    try:
        payload = json.loads(payload_str)
    except json.JSONDecodeError as e:
        print(json.dumps({"erro": f"JSON invalido: {e}"}))
        sys.exit(1)

    try:
        resultado = FUNCOES[funcao_nome](payload)
        print(json.dumps(resultado))
    except Exception as e:
        print(json.dumps({"erro": str(e)}))
        sys.exit(1)


if __name__ == "__main__":
    main()
