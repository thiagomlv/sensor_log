/**
 ******************************************************************************
 * @file           : Leitura.h
 * @brief          : Declaração da classe Leitura
 * @note           : Trabalho Final - Etapa 2 - Linguagens de Programação - 2026.1
 *
 * @author         : Thiago Marques de Oliveira
 * @date           : Julho, 2026
 ******************************************************************************
 */

#ifndef LEITURA_H
#define LEITURA_H

#include <string>

/**
 * @brief Representa uma medição pontual de um sensor.
 *
 * Encapsula todos os atributos de uma leitura importada via CSV:
 * identificador do sensor, tipo, valor numérico, unidade de medida
 * e timestamp da coleta.
 */
class Leitura {
public:
    /**
     * @brief Construtor completo de uma leitura.
     *
     * @param sensorId  ID do sensor que originou a leitura.
     * @param tipo      Tipo do sensor ("temp", "hum" ou "lux").
     * @param valor     Valor numérico medido.
     * @param unidade   Unidade de medida (ex: "C", "%", "lx").
     * @param timestamp String com data/hora da medição (ex: "2026-07-01 10:32:00").
     * @param id        Identificador único da leitura dentro do sistema.
     */
    Leitura(const std::string& sensorId,
            const std::string& tipo,
            double valor,
            const std::string& unidade,
            const std::string& timestamp,
            int id);

    /**
     * @brief Retorna o ID único desta leitura.
     * @return Inteiro identificador da leitura.
     */
    int getId() const;

    /**
     * @brief Retorna o ID do sensor associado.
     * @return String com o ID do sensor.
     */
    const std::string& getSensorId() const;

    /**
     * @brief Retorna o tipo do sensor como string.
     * @return "temp", "hum" ou "lux".
     */
    const std::string& getTipo() const;

    /**
     * @brief Retorna o valor numérico da medição.
     * @return Valor em ponto flutuante.
     */
    double getValor() const;

    /**
     * @brief Retorna a unidade de medida.
     * @return String com a unidade (ex: "C", "%", "lx").
     */
    const std::string& getUnidade() const;

    /**
     * @brief Retorna o timestamp da medição.
     * @return String com data e hora.
     */
    const std::string& getTimestamp() const;

private:
    int         m_id;        ///< ID único da leitura no sistema
    std::string m_sensorId;  ///< ID do sensor de origem
    std::string m_tipo;      ///< Tipo do sensor
    double      m_valor;     ///< Valor medido
    std::string m_unidade;   ///< Unidade de medida
    std::string m_timestamp; ///< Data/hora da medição
};

#endif // LEITURA_H
