# Makefile

# Compiler and flags
CXX = gcc
CXXFLAGS = -g -Iinclude

# Sources and objects
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))

# Output binary
TARGET = eval 

# Default rule
all: $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# Compile .cpp to .o
src/%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f *.o $(TARGET)
