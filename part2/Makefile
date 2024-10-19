# Compiler and flags
CXX = g++
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++11

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Source and object files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Output executable
TARGET = $(BIN_DIR)/my_project

# Libraries to link
LIBS = -lpigpio -lpthread

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CXX) -o $@ $^ $(LIBS)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)

lsp:
	bear -- make

# Phony targets
.PHONY: all clean
