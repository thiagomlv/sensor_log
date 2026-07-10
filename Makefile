# ==============================================================================
# Makefile - SensorLog
# Trabalho Final - Etapa 2 - Linguagens de Programação - 2026.1
# Autor: Thiago Marques de Oliveira
# ==============================================================================

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -Iinclude
TARGET   := sensorlog

SRC_DIR  := src
OBJ_DIR  := build

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# -- Regra principal ------------------------------------------------------------
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "[OK] Binario gerado: ./$(TARGET)"

# -- Compilação dos objetos -----------------------------------------------------
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# -- Instalação de dependências Python -----------------------------------------
deps:
	pip3 install matplotlib numpy --break-system-packages

# -- Limpeza --------------------------------------------------------------------
clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "[OK] Limpeza concluida."

.PHONY: all clean deps
