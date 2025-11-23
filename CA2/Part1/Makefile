# Compiler settings - Can be customized.
CXX = g++
CXXFLAGS = -Wall -g

# The name of the final executable
TARGET = myshell

# List of source files
SOURCES = main.cpp executor.cpp commands.cpp utils.cpp

# List of object files (automatically generated from SOURCES)
OBJS = $(SOURCES:.cpp=.o)

# Rule to link the object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile source files into object files
# The -c flag tells the compiler to compile into an object file, not an executable
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean up build artifacts
# Run 'make clean' to remove the executable and object files
clean:
	rm -f $(TARGET) $(OBJS) np