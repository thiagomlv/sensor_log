/**
 ******************************************************************************
 * @file           : InterfacePython.h
 * @brief          : Declaração da classe InterfacePython
 * @note           : Trabalho Final - Etapa 2 - Linguagens de Programação - 2026.1
 *
 * @author         : Thiago Marques de Oliveira
 * @date           : Julho, 2026
 ******************************************************************************
 */

#ifndef INTERFACEPYTHON_H
#define INTERFACEPYTHON_H

#include <string>
#include <vector>
#include <stdexcept>

/**
 * @brief Estrutura com os descritores estatísticos retornados pelo módulo Python.
 */
struct Estatisticas {
    double media;    ///< Média aritmética
    double mediana;  ///< Mediana
    double desvio;   ///< Desvio padrão
    double minimo;   ///< Valor mínimo
    double maximo;   ///< Valor máximo
    double cv;       ///< Coeficiente de variação (%)
};

/**
 * @brief Estrutura com os resultados da detecção de anomalias.
 */
struct ResultadoAnomalias {
    std::vector<int>    indices;  ///< Índices das leituras anômalas no vetor original
    std::vector<double> zscores;  ///< Z-scores correspondentes
};

/**
 * @brief Encapsula toda a comunicação entre o gerenciador C++ e o módulo Python.
 *
 * Utiliza popen()/pclose() para abrir um pipe com o script analise.py,
 * serializa os parâmetros como JSON na linha de comando e desserializa
 * o JSON retornado via stdout. Nenhum arquivo intermediário é usado.
 *
 * @note A variável de ambiente ANALISE_PY_PATH pode ser definida para
 *       sobrescrever o caminho padrão do script (scripts/analise.py).
 */
class InterfacePython {
public:
    /**
     * @brief Constrói a interface com o caminho do script Python.
     *
     * @param scriptPath Caminho para o arquivo analise.py.
     *                   Padrão: "scripts/analise.py".
     */
    explicit InterfacePython(const std::string& scriptPath = "scripts/analise.py");

    /**
     * @brief Calcula estatísticas descritivas de uma série de valores.
     *
     * Invoca a função "estatisticas" do módulo Python, passando o vetor
     * de valores serializado em JSON.
     *
     * @param valores Vetor com os valores numéricos das leituras.
     * @return Estrutura Estatisticas preenchida.
     * @throws std::runtime_error se o pipe falhar ou o Python retornar erro.
     * @throws std::invalid_argument se o vetor estiver vazio.
     */
    Estatisticas calcularEstatisticas(const std::vector<double>& valores) const;

    /**
     * @brief Detecta anomalias em uma série de valores via z-score.
     *
     * Invoca a função "anomalias" do módulo Python. Leituras cujo z-score
     * (em módulo) exceder o limiar @p k são consideradas anômalas.
     *
     * @param valores Vetor com os valores numéricos das leituras.
     * @param k       Limiar de z-score (padrão: 2.0).
     * @return Estrutura ResultadoAnomalias com índices e z-scores.
     * @throws std::runtime_error se o pipe falhar ou o Python retornar erro.
     * @throws std::invalid_argument se o vetor tiver menos de 2 elementos.
     */
    ResultadoAnomalias detectarAnomalias(const std::vector<double>& valores,
                                          double k = 2.0) const;

    /**
     * @brief Gera um gráfico de histórico em PNG via matplotlib.
     *
     * Invoca a função "grafico" do módulo Python, que salva um arquivo PNG
     * em /tmp e retorna o caminho.
     *
     * @param valores  Vetor com os valores a plotar.
     * @param labels   Vetor com os timestamps/rótulos do eixo X.
     * @param titulo   Título do gráfico.
     * @param unidade  Unidade do eixo Y (ex: "°C", "%", "lx").
     * @return Caminho absoluto do arquivo PNG gerado.
     * @throws std::runtime_error se o pipe falhar ou o Python retornar erro.
     * @throws std::invalid_argument se valores e labels tiverem tamanhos diferentes.
     */
    std::string gerarGrafico(const std::vector<double>& valores,
                              const std::vector<std::string>& labels,
                              const std::string& titulo,
                              const std::string& unidade) const;

private:
    std::string m_scriptPath; ///< Caminho para o script analise.py

    /**
     * @brief Executa o script Python com os argumentos fornecidos e retorna o stdout.
     *
     * @param funcao  Nome da função Python a invocar.
     * @param payload JSON serializado com os parâmetros.
     * @return String com o JSON retornado pelo stdout do Python.
     * @throws std::runtime_error se popen() falhar ou o processo retornar erro.
     */
    std::string executar(const std::string& funcao,
                          const std::string& payload) const;

    /**
     * @brief Escapa aspas simples em uma string para uso seguro no shell.
     *
     * @param s String a escapar.
     * @return String com aspas simples escapadas.
     */
    static std::string escaparAspas(const std::string& s);
};

#endif // INTERFACEPYTHON_H
