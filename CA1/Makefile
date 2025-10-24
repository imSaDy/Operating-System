# 1. COMPILER
CXX = g++
# -Wall = show all warnings, -g = add debug info
CXXFLAGS = -Wall -g -std=c++11

# 2. DIRECTORIES
SRC_DIR = src
HEADER_DIR = header
OBJ_DIR = obj
BIN_DIR = bin

# 3. COMPILER FLAGS
# -I$(HEADER_DIR) tells g++: "Also look in the 'header' directory for #include files"
CPPFLAGS = -I$(HEADER_DIR)

# 4. FILES
# Find all .cpp files in the src/ directory
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Create a list of .o (object) files in the obj/ directory
# e.g., src/server.cpp becomes obj/server.o
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Define the final programs (executables) we want to build
TARGET_SERVER = $(BIN_DIR)/server
TARGET_CUSTOMER = $(BIN_DIR)/customer
TARGET_AIRLINE = $(BIN_DIR)/airline

# List of all targets to build
TARGETS = $(TARGET_SERVER) $(TARGET_CUSTOMER) $(TARGET_AIRLINE)

# 5. TARGETS
.PHONY: all clean

# 'all' is the default target (runs when you just type 'make')
# It depends on all our final programs
all: $(TARGETS)

# --- LINKING ---
# Rule to build the 'server' executable from its object file
$(TARGET_SERVER): $(OBJ_DIR)/server.o | $(BIN_DIR)
	@echo "Linking $@..."
	$(CXX) $(CXXFLAGS) -o $@ $<

# Rule to build the 'customer' executable
$(TARGET_CUSTOMER): $(OBJ_DIR)/customer.o | $(BIN_DIR)
	@echo "Linking $@..."
	$(CXX) $(CXXFLAGS) -o $@ $<

# Rule to build the 'airline' executable
$(TARGET_AIRLINE): $(OBJ_DIR)/airline.o | $(BIN_DIR)
	@echo "Linking $@..."
	$(CXX) $(CXXFLAGS) -o $@ $<

# --- COMPILING ---
# This is a "Pattern Rule". It tells 'make' how to build
# any 'obj/%.o' file from its 'src/%.cpp' counterpart.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

# --- DIRECTORY CREATION ---
# These targets create the bin/ and obj/ directories
# The '|' in the rules above makes them "order-only prerequisites"
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# --- CLEANUP ---
# 'make clean' removes all built files
clean:
	@echo "Cleaning up build files..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)