/**
 ******************************************************************************
 * @file           : Sensor.h
 * @brief          : Declaração da classe Sensor
 * @note           : Trabalho Final - Etapa 2 - Linguagens de Programação - 2026.1
 *
 * @author         : Thiago Marques de Oliveira
 * @date           : Julho, 2026
 ******************************************************************************
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <string>
#include <stdexcept>

/**
 * @brief Tipos de sensores suportados pelo sistema.
 */
enum class TipoSensor {
    TEMPERATURA, ///< Sensor de temperatura (°C, °F, K)
    UMIDADE,     ///< Sensor de umidade relativa (%)
    LUMINOSIDADE ///< Sensor de luminosidade (lx)
};

/**
 * @brief Representa um sensor cadastrado no sistema.
 *
 * Encapsula o identificador único e o tipo de um sensor IoT.
 * Um sensor deve ser cadastrado antes que leituras associadas a ele
 * possam ser importadas.
 */
class Sensor {
public:
    /**
     * @brief Construtor padrão. Cria um sensor inválido (usado por containers STL).
     */
    Sensor() = default;

    /**
     * @brief Constrói um sensor com ID e tipo definidos.
     *
     * @param id   Identificador único do sensor (não pode ser vazio).
     * @param tipo Tipo do sensor (TEMPERATURA, UMIDADE ou LUMINOSIDADE).
     * @throws std::invalid_argument se o ID for uma string vazia.
     */
    Sensor(const std::string& id, TipoSensor tipo);

    /**
     * @brief Retorna o identificador do sensor.
     * @return String com o ID do sensor.
     */
    const std::string& getId() const;

    /**
     * @brief Retorna o tipo do sensor.
     * @return Valor do enum TipoSensor.
     */
    TipoSensor getTipo() const;

    /**
     * @brief Retorna o tipo do sensor como string legível.
     * @return "temp", "hum" ou "lux".
     */
    std::string getTipoStr() const;

    /**
     * @brief Converte uma string para o enum TipoSensor.
     *
     * @param s String a converter: "temp", "hum" ou "lux" (case-insensitive).
     * @return TipoSensor correspondente.
     * @throws std::invalid_argument se a string não corresponder a nenhum tipo.
     */
    static TipoSensor parseTipo(const std::string& s);

private:
    std::string m_id;   ///< Identificador único do sensor
    TipoSensor  m_tipo; ///< Tipo do sensor
};

#endif // SENSOR_H
