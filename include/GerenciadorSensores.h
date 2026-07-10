/**
 ******************************************************************************
 * @file           : GerenciadorSensores.h
 * @brief          : Declaração da classe GerenciadorSensores
 * @note           : Trabalho Final - Etapa 2 - Linguagens de Programação - 2026.1
 *
 * @author         : Thiago Marques de Oliveira
 * @date           : Julho, 2026
 ******************************************************************************
 */

#ifndef GERENCIADORSENSORES_H
#define GERENCIADORSENSORES_H

#include "Sensor.h"
#include "Leitura.h"
#include "InterfacePython.h"

#include <map>
#include <vector>
#include <string>

/**
 * @brief Núcleo do sistema SensorLog: gerencia sensores e leituras.
 *
 * Mantém um catálogo de sensores cadastrados (std::map), onde cada
 * sensor encapsula seu próprio histórico de leituras. Delega análises
 * estatísticas e geração de gráficos ao módulo Python via InterfacePython.
 *
 * Todas as operações de entrada/saída com o usuário são realizadas nos
 * métodos públicos desta classe, que são invocados pelo menu em main.cpp.
 */
class GerenciadorSensores {
public:
    /**
     * @brief Constrói o gerenciador inicializando a interface com o Python.
     *
     * @param scriptPath Caminho para o script analise.py.
     *                   Padrão: "scripts/analise.py".
     */
    explicit GerenciadorSensores(const std::string& scriptPath = "scripts/analise.py");

    // -- Opções do menu ----------------------------------------------------

    /**
     * @brief [Opção 1] Cadastra um novo sensor interativamente.
     *
     * Solicita ao usuário o ID e o tipo do sensor. Rejeita duplicatas
     * e tipos inválidos com mensagens de erro sem encerrar o programa.
     */
    void cadastrarSensor();

    /**
     * @brief [Opção 2] Importa leituras de um arquivo CSV.
     *
     * Solicita o caminho do arquivo. Cada linha deve seguir o formato:
     * sensor_id;tipo;valor;unidade;timestamp
     * Linhas inválidas são rejeitadas individualmente com aviso.
     * Apenas leituras de sensores previamente cadastrados são aceitas.
     */
    void importarCSV();

    /**
     * @brief [Opção 3] Lista todas as leituras em tabela formatada.
     *
     * Exibe id, sensor_id, tipo, valor, unidade e timestamp de cada
     * leitura armazenada. Informa se não houver leituras.
     */
    void listarLeituras() const;

    /**
     * @brief [Opção 4] Exibe estatísticas de um sensor via Python.
     *
     * Solicita o ID do sensor, coleta os valores de suas leituras e
     * invoca InterfacePython::calcularEstatisticas(). Exibe os
     * descritores retornados formatados.
     */
    void exibirEstatisticas() const;

    /**
     * @brief [Opção 5] Detecta anomalias nas leituras de um sensor via Python.
     *
     * Solicita o ID do sensor e o limiar k. Invoca
     * InterfacePython::detectarAnomalias() e exibe as leituras anômalas
     * com seus respectivos z-scores.
     */
    void detectarAnomalias() const;

    /**
     * @brief [Opção 6] Gera gráfico de histórico de um sensor via Python.
     *
     * Solicita o ID do sensor. Invoca InterfacePython::gerarGrafico()
     * e exibe o caminho do arquivo PNG gerado.
     */
    void gerarGrafico() const;

    /**
     * @brief [Opção 7] Remove uma leitura pelo seu ID.
     *
     * Solicita o ID da leitura ao usuário. Procura em todos os sensores e
     * remove a leitura se encontrada; exibe erro caso contrário.
     */
    void removerLeitura();

private:
    std::map<std::string, Sensor> m_sensores; ///< Sensores cadastrados (chave: ID)
    InterfacePython               m_python;   ///< Interface com o módulo Python
    int                           m_proxId;   ///< Próximo ID disponível para leituras

    /**
     * @brief Coleta os valores numéricos de um sensor específico.
     *
     * @param sensorId ID do sensor a filtrar.
     * @return Vetor com os valores (double) das leituras desse sensor,
     *         na ordem em que foram importadas.
     */
    std::vector<double> coletarValores(const std::string& sensorId) const;

    /**
     * @brief Coleta os timestamps das leituras de um sensor específico.
     *
     * @param sensorId ID do sensor a filtrar.
     * @return Vetor com os timestamps (string) das leituras desse sensor.
     */
    std::vector<std::string> coletarTimestamps(const std::string& sensorId) const;

    /**
     * @brief Verifica se um sensor com o ID dado está cadastrado.
     *
     * @param id ID a verificar.
     * @return true se o sensor existir, false caso contrário.
     */
    bool sensorExiste(const std::string& id) const;

    /**
     * @brief Solicita e valida o ID de um sensor cadastrado via stdin.
     *
     * Exibe a lista de sensores disponíveis e aguarda entrada válida.
     *
     * @return ID do sensor escolhido pelo usuário.
     */
    std::string solicitarSensorId() const;

    /**
     * @brief Remove espaços em branco no início e no fim de uma string.
     *
     * @param s String a processar.
     * @return String sem espaços nas bordas.
     */
    static std::string trim(const std::string& s);

    /**
     * @brief Converte uma string para letras minúsculas.
     *
     * @param s String a converter.
     * @return Cópia da string em minúsculas.
     */
    static std::string toLower(const std::string& s);
};

#endif // GERENCIADORSENSORES_H
