/**
 ******************************************************************************
 * @file           : main.cpp
 * @brief          : Ponto de entrada do SensorLog — loop de menu principal
 * @note           : Trabalho Final - Etapa 2 - Linguagens de Programação - 2026.1
 *
 * @author         : Thiago Marques de Oliveira
 * @date           : Julho, 2026
 ******************************************************************************
 */

#include "GerenciadorSensores.h"

#include <iostream>
#include <limits>
#include <string>

/** @brief Exibe o menu principal no terminal. */
static void exibirMenu() {
    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════════════╗\n";
    std::cout << "  ║           SensorLog v1.0             ║\n";
    std::cout << "  ╠══════════════════════════════════════╣\n";
    std::cout << "  ║  1. Cadastrar sensor                 ║\n";
    std::cout << "  ║  2. Importar leituras (CSV)          ║\n";
    std::cout << "  ║  3. Listar leituras                  ║\n";
    std::cout << "  ║  4. Estatisticas de um sensor  [Py]  ║\n";
    std::cout << "  ║  5. Detectar anomalias         [Py]  ║\n";
    std::cout << "  ║  6. Gerar grafico de historico [Py]  ║\n";
    std::cout << "  ║  7. Remover leitura por ID           ║\n";
    std::cout << "  ║  8. Sair                             ║\n";
    std::cout << "  ╚══════════════════════════════════════╝\n";
    std::cout << "  Opcao: ";
}

int main() {
    std::cout << "  Iniciando SensorLog...\n";

    GerenciadorSensores gerenciador("scripts/analise.py");

    int opcao = 0;
    bool rodando = true;

    while (rodando) {
        exibirMenu();

        if (!(std::cin >> opcao)) {
            // Entrada não-numérica: limpa o estado e continua
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  [ERRO] Entrada invalida. Digite um numero de 1 a 8.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::cout << "\n";
        switch (opcao) {
            case 1: gerenciador.cadastrarSensor();    break;
            case 2: gerenciador.importarCSV();        break;
            case 3: gerenciador.listarLeituras();     break;
            case 4: gerenciador.exibirEstatisticas(); break;
            case 5: gerenciador.detectarAnomalias();  break;
            case 6: gerenciador.gerarGrafico();       break;
            case 7: gerenciador.removerLeitura();     break;
            case 8:
                std::cout << "  Encerrando SensorLog. Ate logo!\n\n";
                rodando = false;
                break;
            default:
                std::cout << "  [ERRO] Opcao invalida. Escolha entre 1 e 8.\n";
                break;
        }
    }

    return 0;
}
