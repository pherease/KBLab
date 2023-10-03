# Compiler and flags
CC = clang++
CFLAGS = -std=c++17 -Wall -Wextra -fcolor-diagnostics -fansi-escape-codes -g -w -lCore -lRIO -lHist -lGraf -lGpad -lTree

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = .

# Libraries and includes
ROOT_LIBS = `root-config --libs`
ROOT_CFLAGS = `root-config --cflags`
HDF5_LIBS = -L/opt/homebrew/Cellar/hdf5/1.14.2/lib -lhdf5_cpp -lhdf5
INCLUDES = -I$(SRC_DIR) $(ROOT_CFLAGS)
INCLUDES += -I/opt/homebrew/Cellar/hdf5/1.14.2/include

# Source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Executable name
EXEC = $(BIN_DIR)/main

# Build rule
$(EXEC): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJS) $(ROOT_LIBS) $(HDF5_LIBS) -o $(EXEC)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Phony target to ensure 'clean' is always executed
.PHONY: clean
