/**
 ******************************************************************************
 * @file           : GerenciadorSensores.cpp
 * @brief          : Implementação da classe GerenciadorSensores
 * @note           : Trabalho Final - Etapa 2 - Linguagens de Programação - 2026.1
 *
 * @author         : Thiago Marques de Oliveira
 * @date           : Julho, 2026
 ******************************************************************************
 */

#include "GerenciadorSensores.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <limits>

// -- Utilitários privados -----------------------------------------------------

std::string GerenciadorSensores::trim(const std::string& s) {
    size_t ini = s.find_first_not_of(" \t\r\n");
    if (ini == std::string::npos) return "";
    size_t fim = s.find_last_not_of(" \t\r\n");
    return s.substr(ini, fim - ini + 1);
}

std::string GerenciadorSensores::toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return out;
}

bool GerenciadorSensores::sensorExiste(const std::string& id) const {
    return m_sensores.count(id) > 0;
}

std::vector<double> GerenciadorSensores::coletarValores(const std::string& sensorId) const {
    std::vector<double> vals;
    for (const auto& l : m_leituras)
        if (l.getSensorId() == sensorId)
            vals.push_back(l.getValor());
    return vals;
}

std::vector<std::string> GerenciadorSensores::coletarTimestamps(const std::string& sensorId) const {
    std::vector<std::string> ts;
    for (const auto& l : m_leituras)
        if (l.getSensorId() == sensorId)
            ts.push_back(l.getTimestamp());
    return ts;
}

std::string GerenciadorSensores::solicitarSensorId() const {
    if (m_sensores.empty()) {
        std::cout << "  Nenhum sensor cadastrado. Cadastre um sensor primeiro (opcao 1).\n";
        return "";
    }
    std::cout << "  Sensores disponiveis:\n";
    for (const auto& par : m_sensores)
        std::cout << "    - " << par.first << " [" << par.second.getTipoStr() << "]\n";

    std::string id;
    std::cout << "  Informe o ID do sensor: ";
    std::getline(std::cin, id);
    id = trim(id);

    if (!sensorExiste(id)) {
        std::cout << "  [ERRO] Sensor '" << id << "' nao encontrado.\n";
        return "";
    }
    return id;
}

// -- Construtor ---------------------------------------------------------------

GerenciadorSensores::GerenciadorSensores(const std::string& scriptPath)
    : m_python(scriptPath), m_proxId(1)
{}

// -- Opcao 1: Cadastrar sensor ------------------------------------------------

void GerenciadorSensores::cadastrarSensor() {
    std::string id, tipoStr;

    std::cout << "  ID do sensor: ";
    std::cin >> id;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    id = trim(id);

    if (id.empty()) {
        std::cout << "  [ERRO] ID nao pode ser vazio.\n";
        return;
    }
    if (sensorExiste(id)) {
        std::cout << "  [ERRO] Sensor '" << id << "' ja cadastrado.\n";
        return;
    }

    std::cout << "  Tipo (temp / hum / lux): ";
    std::cin >> tipoStr;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    tipoStr = toLower(trim(tipoStr));

    try {
        TipoSensor tipo = Sensor::parseTipo(tipoStr);
        m_sensores.emplace(id, Sensor(id, tipo));
        std::cout << "  [OK] Sensor '" << id << "' cadastrado com sucesso.\n";
    } catch (const std::invalid_argument& e) {
        std::cout << "  [ERRO] " << e.what() << "\n";
    }
}

// -- Opcao 2: Importar CSV ----------------------------------------------------

void GerenciadorSensores::importarCSV() {
    std::string caminho;
    std::cout << "  Caminho do arquivo CSV: ";
    std::getline(std::cin, caminho);
    caminho = trim(caminho);

    std::ifstream arq(caminho);
    if (!arq.is_open()) {
        std::cout << "  [ERRO] Nao foi possivel abrir o arquivo: " << caminho << "\n";
        return;
    }

    int importadas = 0, rejeitadas = 0;
    std::string linha;
    int numLinha = 0;

    // Pula a primeira linha (cabeçalho do CSV)
    if (std::getline(arq, linha)) {
        ++numLinha;
    }

    while (std::getline(arq, linha)) {
        ++numLinha;
        linha = trim(linha);
        if (linha.empty()) continue;

        // Divide pelos delimitadores ';'
        std::vector<std::string> campos;
        std::istringstream ss(linha);
        std::string campo;
        while (std::getline(ss, campo, ';'))
            campos.push_back(trim(campo));

        if (campos.size() < 5) {
            std::cout << "  [AVISO] Linha " << numLinha
                      << " ignorada (campos insuficientes): " << linha << "\n";
            ++rejeitadas;
            continue;
        }

        const std::string& sensorId  = campos[0];
        const std::string& tipoStr   = campos[1];
        const std::string& valorStr  = campos[2];
        const std::string& unidade   = campos[3];
        const std::string& timestamp = campos[4];

        // Valida sensor cadastrado
        if (!sensorExiste(sensorId)) {
            std::cout << "  [AVISO] Linha " << numLinha
                      << ": sensor '" << sensorId << "' nao cadastrado. Linha ignorada.\n";
            ++rejeitadas;
            continue;
        }

        // Valida valor numérico
        double valor = 0.0;
        try {
            valor = std::stod(valorStr);
        } catch (...) {
            std::cout << "  [AVISO] Linha " << numLinha
                      << ": valor '" << valorStr << "' invalido. Linha ignorada.\n";
            ++rejeitadas;
            continue;
        }

        m_leituras.emplace_back(sensorId, toLower(tipoStr), valor, unidade, timestamp, m_proxId++);
        ++importadas;
    }

    std::cout << "  Importacao concluida: " << importadas << " leitura(s) importada(s), "
              << rejeitadas << " linha(s) rejeitada(s).\n";
}

// -- Opcao 3: Listar leituras -------------------------------------------------

void GerenciadorSensores::listarLeituras() const {
    if (m_leituras.empty()) {
        std::cout << "  Nenhuma leitura armazenada.\n";
        return;
    }

    std::cout << "\n";
    std::cout << std::left
              << std::setw(5)  << "ID"
              << std::setw(14) << "Sensor"
              << std::setw(6)  << "Tipo"
              << std::setw(10) << "Valor"
              << std::setw(8)  << "Unidade"
              << "Timestamp\n";
    std::cout << std::string(70, '-') << "\n";

    for (const auto& l : m_leituras) {
        std::cout << std::left
                  << std::setw(5)  << l.getId()
                  << std::setw(14) << l.getSensorId()
                  << std::setw(6)  << l.getTipo()
                  << std::setw(10) << std::fixed << std::setprecision(2) << l.getValor()
                  << std::setw(8)  << l.getUnidade()
                  << l.getTimestamp() << "\n";
    }
    std::cout << "\n";
}

// -- Opcao 4: Estatísticas ----------------------------------------------------

void GerenciadorSensores::exibirEstatisticas() const {
    std::string id = solicitarSensorId();
    if (id.empty()) return;

    std::vector<double> vals = coletarValores(id);
    if (vals.empty()) {
        std::cout << "  Nenhuma leitura para o sensor '" << id << "'.\n";
        return;
    }

    try {
        Estatisticas e = m_python.calcularEstatisticas(vals);
        const auto& s = m_sensores.at(id);
        std::cout << "\n  === Estatisticas: " << id << " [" << s.getTipoStr() << "] ===\n";
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "  Media              : " << e.media   << "\n";
        std::cout << "  Mediana            : " << e.mediana << "\n";
        std::cout << "  Desvio Padrao      : " << e.desvio  << "\n";
        std::cout << "  Minimo             : " << e.minimo  << "\n";
        std::cout << "  Maximo             : " << e.maximo  << "\n";
        std::cout << "  Coef. de Variacao  : " << e.cv      << " %\n\n";
    } catch (const std::exception& e) {
        std::cout << "  [ERRO] " << e.what() << "\n";
    }
}

// -- Opcao 5: Detectar anomalias ----------------------------------------------

void GerenciadorSensores::detectarAnomalias() const {
    std::string id = solicitarSensorId();
    if (id.empty()) return;

    double k = 2.0;
    std::cout << "  Limiar k (z-score, padrao=2.0): ";
    std::string kStr;
    std::getline(std::cin, kStr);
    kStr = trim(kStr);
    if (!kStr.empty()) {
        try { k = std::stod(kStr); }
        catch (...) { std::cout << "  Valor invalido. Usando k=2.0.\n"; }
    }

    std::vector<double> vals = coletarValores(id);
    if (vals.size() < 2) {
        std::cout << "  Leituras insuficientes (minimo 2) para o sensor '" << id << "'.\n";
        return;
    }

    // Coleta as leituras do sensor para exibir junto com os resultados
    std::vector<const Leitura*> leiturasSensor;
    for (const auto& l : m_leituras)
        if (l.getSensorId() == id)
            leiturasSensor.push_back(&l);

    try {
        ResultadoAnomalias r = m_python.detectarAnomalias(vals, k);
        std::cout << "\n  === Anomalias: " << id << " (k=" << k << ") ===\n";
        if (r.indices.empty()) {
            std::cout << "  Nenhuma anomalia detectada.\n\n";
            return;
        }
        std::cout << std::left
                  << std::setw(6)  << "Idx"
                  << std::setw(10) << "Valor"
                  << std::setw(10) << "Z-score"
                  << "Timestamp\n";
        std::cout << std::string(50, '-') << "\n";
        for (size_t i = 0; i < r.indices.size(); ++i) {
            int idx = r.indices[i];
            if (idx >= 0 && idx < static_cast<int>(leiturasSensor.size())) {
                const Leitura* l = leiturasSensor[idx];
                std::cout << std::left
                          << std::setw(6)  << idx
                          << std::setw(10) << std::fixed << std::setprecision(2) << l->getValor()
                          << std::setw(10) << std::setprecision(3) << r.zscores[i]
                          << l->getTimestamp() << "\n";
            }
        }
        std::cout << "\n";
    } catch (const std::exception& e) {
        std::cout << "  [ERRO] " << e.what() << "\n";
    }
}

// -- Opcao 6: Gerar gráfico ---------------------------------------------------

void GerenciadorSensores::gerarGrafico() const {
    std::string id = solicitarSensorId();
    if (id.empty()) return;

    std::vector<double>      vals   = coletarValores(id);
    std::vector<std::string> labels = coletarTimestamps(id);

    if (vals.empty()) {
        std::cout << "  Nenhuma leitura para o sensor '" << id << "'.\n";
        return;
    }

    const auto& sensor = m_sensores.at(id);
    std::string unidade;
    switch (sensor.getTipo()) {
        case TipoSensor::TEMPERATURA:  unidade = "graus C"; break;
        case TipoSensor::UMIDADE:      unidade = "%";       break;
        case TipoSensor::LUMINOSIDADE: unidade = "lx";      break;
    }
    std::string titulo = "Historico - " + id;

    try {
        std::string arquivo = m_python.gerarGrafico(vals, labels, titulo, unidade);
        std::cout << "  [OK] Grafico gerado: " << arquivo << "\n";
    } catch (const std::exception& e) {
        std::cout << "  [ERRO] " << e.what() << "\n";
    }
}

// -- Opcao 7: Remover leitura -------------------------------------------------

void GerenciadorSensores::removerLeitura() {
    if (m_leituras.empty()) {
        std::cout << "  Nenhuma leitura para remover.\n";
        return;
    }

    int idRemover = 0;
    std::cout << "  ID da leitura a remover: ";
    if (!(std::cin >> idRemover)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  [ERRO] Entrada invalida.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    auto it = std::find_if(m_leituras.begin(), m_leituras.end(),
                           [idRemover](const Leitura& l){ return l.getId() == idRemover; });

    if (it == m_leituras.end()) {
        std::cout << "  [ERRO] Leitura com ID " << idRemover << " nao encontrada.\n";
        return;
    }

    m_leituras.erase(it);
    std::cout << "  [OK] Leitura " << idRemover << " removida com sucesso.\n";
}
