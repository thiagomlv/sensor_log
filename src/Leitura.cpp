/**
 ******************************************************************************
 * @file           : Leitura.cpp
 * @brief          : Implementação da classe Leitura
 * @note           : Trabalho Final - Etapa 2 - Linguagens de Programação - 2026.1
 *
 * @author         : Thiago Marques de Oliveira
 * @date           : Julho, 2026
 ******************************************************************************
 */

#include "Leitura.h"

Leitura::Leitura(const std::string& sensorId,
                 const std::string& tipo,
                 double valor,
                 const std::string& unidade,
                 const std::string& timestamp,
                 int id)
    : m_id(id)
    , m_sensorId(sensorId)
    , m_tipo(tipo)
    , m_valor(valor)
    , m_unidade(unidade)
    , m_timestamp(timestamp)
{}

int Leitura::getId() const               { return m_id; }
const std::string& Leitura::getSensorId()  const { return m_sensorId; }
const std::string& Leitura::getTipo()      const { return m_tipo; }
double Leitura::getValor()               const { return m_valor; }
const std::string& Leitura::getUnidade()   const { return m_unidade; }
const std::string& Leitura::getTimestamp() const { return m_timestamp; }
