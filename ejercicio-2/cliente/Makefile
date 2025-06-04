# Variables
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TARGET = $(BIN_DIR)/ejercicio_2_cliente

# Buscar todos los archivos .cpp en el directorio src
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# Reemplazar la extensión .cpp por .o y cambiar el directorio a obj
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Regla principal
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regla para compilar archivos .cpp a .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para limpiar los archivos generados
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
