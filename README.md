# SensorLog

Sistema de gerenciamento e análise de dados de sensores IoT, desenvolvido como trabalho final da disciplina **EEL670 – Linguagens de Programação** (DEL-Poli/UFRJ).

O SensorLog utiliza **C++17** para o gerenciador principal e **Python 3** para análise estatística e geração de gráficos. A comunicação entre as linguagens ocorre via `popen()` com serialização JSON.

## Estrutura do Projeto

```
sensorlog/
├── Makefile              # Compilação e gerenciamento
├── README.md             # README do projeto
├── Relatorio.pdf         # Relatório
├── data/
│   └── exemplo.csv       # Arquivo de teste com leituras de sensores
├── include/
│   ├── GerenciadorSensores.h
│   ├── InterfacePython.h
│   ├── Leitura.h
│   └── Sensor.h
├── scripts/
│   └── analise.py        # Módulo Python (estatísticas, anomalias, gráficos)
└── src/
    ├── main.cpp
    ├── GerenciadorSensores.cpp
    ├── InterfacePython.cpp
    ├── Leitura.cpp
    └── Sensor.cpp
```

## Pré-requisitos

- **Linux** (testado em Ubuntu)
- **g++** com suporte a C++17
- **Python 3** com `pip3`
- **make**

## Instalação

### 1. Instalar dependências Python

```bash
make deps
```

Isso instala as bibliotecas `matplotlib` e `numpy` necessárias para o módulo de análise.

### 2. Compilar o projeto

```bash
make
```

O binário `sensorlog` será gerado na raiz do projeto.

### 3. Limpar os arquivos de compilação

```bash
make clean
```

Remove o diretório `build/` e o binário `sensorlog`.

## Uso

Execute o programa:

```bash
./sensorlog
```

O menu interativo será exibido:

```
  ╔══════════════════════════════════════╗
  ║           SensorLog v1.0             ║
  ╠══════════════════════════════════════╣
  ║  1. Cadastrar sensor                 ║
  ║  2. Importar leituras (CSV)          ║
  ║  3. Listar leituras                  ║
  ║  4. Estatisticas de um sensor  [Py]  ║
  ║  5. Detectar anomalias         [Py]  ║
  ║  6. Gerar grafico de historico [Py]  ║
  ║  7. Remover leitura por ID           ║
  ║  8. Sair                             ║
  ╚══════════════════════════════════════╝
```

### Opções do Menu

| # | Opção | Descrição | Python |
|---|-------|-----------|--------|
| 1 | Cadastrar sensor | Registra um sensor com ID e tipo (`temp`/`hum`/`lux`) | Não |
| 2 | Importar leituras (CSV) | Importa leituras de um arquivo `.csv` | Não |
| 3 | Listar leituras | Exibe todas as leituras em tabela formatada | Não |
| 4 | Estatísticas de um sensor | Calcula média, mediana, desvio padrão, min, max e CV | Sim |
| 5 | Detectar anomalias | Identifica leituras anômalas via z-score | Sim |
| 6 | Gerar gráfico de histórico | Gera gráfico PNG com matplotlib | Sim |
| 7 | Remover leitura por ID | Remove uma leitura específica | Não |
| 8 | Sair | Encerra o programa | Não |

### Exemplo de Uso Rápido

```bash
# 1. Compilar
make

# 2. Executar
./sensorlog

# 3. No menu:
#    - Opção 1: Cadastrar sensores (s1/temp, s2/hum, s3/lux)
#    - Opção 2: Importar leituras com: data/exemplo.csv
#    - Opção 3: Verificar as leituras importadas
#    - Opção 4: Ver estatísticas de um sensor
#    - Opção 5: Detectar anomalias (ex: temperatura 40.0°C em s1)
#    - Opção 6: Gerar gráfico de histórico
```

### Formato do Arquivo CSV

O arquivo CSV deve seguir o formato abaixo, com campos separados por `;`:

```
sensor_id;tipo;valor;unidade;timestamp
s1;temp;22.5;C;2026-07-01 08:00:00
s2;hum;60.2;%;2026-07-01 08:00:00
s3;lux;820.0;lx;2026-07-01 08:00:00
```

> **Nota:** A primeira linha (cabeçalho) é automaticamente ignorada. Apenas leituras de sensores previamente cadastrados são importadas.

## Arquitetura

```
Usuário ──> Menu ──> Gerenciador C++ ──popen()──> analise.py (Python)
               │                           │
               │   std::map<Sensor>        │   numpy / statistics
               │   std::vector<Leitura>    │   matplotlib
               │                           │
               └── Resultado formatado <───┘   JSON via stdout
```

- **C++**: Gerenciamento de sensores e leituras, parsing de CSV, interface com o usuário
- **Python**: Cálculos estatísticos, detecção de anomalias (z-score), geração de gráficos

A comunicação usa `popen()` (não `system()`) com parâmetros em JSON via linha de comando e retorno via `stdout`.

## Autor

**Thiago Marques de Oliveira** – DRE: 122053397

EEL670 – Linguagens de Programação – 2026.1 – DEL-Poli/UFRJ
