/**
 ******************************************************************************
 * @file           : Sensor.cpp
 * @brief          : Implementação da classe Sensor
 * @note           : Trabalho Final - Etapa 2 - Linguagens de Programação - 2026.1
 *
 * @author         : Thiago Marques de Oliveira
 * @date           : Julho, 2026
 ******************************************************************************
 */

#include "Sensor.h"
#include <algorithm>
#include <cctype>

Sensor::Sensor(const std::string& id, TipoSensor tipo)
    : m_id(id), m_tipo(tipo)
{
    if (id.empty()) {
        throw std::invalid_argument("ID do sensor nao pode ser vazio.");
    }
}

const std::string& Sensor::getId() const {
    return m_id;
}

TipoSensor Sensor::getTipo() const {
    return m_tipo;
}

std::string Sensor::getTipoStr() const {
    switch (m_tipo) {
        case TipoSensor::TEMPERATURA:  return "temp";
        case TipoSensor::UMIDADE:      return "hum";
        case TipoSensor::LUMINOSIDADE: return "lux";
    }
    return "desconhecido";
}

TipoSensor Sensor::parseTipo(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    if (lower == "temp" || lower == "temperatura") return TipoSensor::TEMPERATURA;
    if (lower == "hum"  || lower == "umidade")     return TipoSensor::UMIDADE;
    if (lower == "lux"  || lower == "luminosidade") return TipoSensor::LUMINOSIDADE;

    throw std::invalid_argument("Tipo de sensor invalido: '" + s +
                                "'. Use: temp, hum ou lux.");
}

void Sensor::adicionarLeitura(const Leitura& leitura) {
    m_leituras.push_back(leitura);
}

const std::vector<Leitura>& Sensor::getLeituras() const {
    return m_leituras;
}

bool Sensor::removerLeitura(int id) {
    auto it = std::find_if(m_leituras.begin(), m_leituras.end(),
                           [id](const Leitura& l){ return l.getId() == id; });
    if (it != m_leituras.end()) {
        m_leituras.erase(it);
        return true;
    }
    return false;
}
