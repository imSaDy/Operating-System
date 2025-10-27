CXX = g++
CXXFLAGS = -Wall -g -std=c++11

SRC_DIR = src
HEADER_DIR = header
OBJ_DIR = obj
BIN_DIR = bin

CPPFLAGS = -I$(HEADER_DIR)

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

TARGET_SERVER = $(BIN_DIR)/server
TARGET_CUSTOMER = $(BIN_DIR)/customer
TARGET_AIRLINE = $(BIN_DIR)/airline

TARGETS = $(TARGET_SERVER) $(TARGET_CUSTOMER) $(TARGET_AIRLINE)

.PHONY: all clean

all: $(TARGETS)

$(TARGET_SERVER): $(OBJ_DIR)/server.o | $(BIN_DIR)
	@echo "Linking $@..."
	$(CXX) $(CXXFLAGS) -o $@ $<

$(TARGET_CUSTOMER): $(OBJ_DIR)/customer.o | $(BIN_DIR)
	@echo "Linking $@..."
	$(CXX) $(CXXFLAGS) -o $@ $<

$(TARGET_AIRLINE): $(OBJ_DIR)/airline.o | $(BIN_DIR)
	@echo "Linking $@..."
	$(CXX) $(CXXFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@echo "Cleaning up build files..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)