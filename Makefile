# Compiler and linker settings
CXX = g++
CXXFLAGS = -Wall -std=c++11
LDFLAGS = -lGL -lGLU -lglut

# Directories
SRC_DIR = .
BUILD_DIR = build

# Files
SOURCES = $(SRC_DIR)/twin_cars.cpp
OBJECTS = $(BUILD_DIR)/twin_cars.o
EXECUTABLE = $(BUILD_DIR)/twin_cars

# Default target to build the executable
all: $(EXECUTABLE)

# Link the object files to create the executable
$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile the source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up the build directory
clean:
	rm -rf $(BUILD_DIR)

# Run the program
run: $(EXECUTABLE)
	./$(EXECUTABLE)

# Phony targets
.PHONY: all clean run
