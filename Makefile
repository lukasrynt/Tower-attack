CXX         	:= g++
CXXFLAGS    	:= -Wall -pedantic -std=c++14
BUILD_DIR   	:= bin
SOURCE_DIR      := src
SOURCES		:= $(wildcard $(SOURCE_DIR)/*.cpp)
OBJS	    	:= $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o, $(SOURCES))
COMPILE_TARGET	:= $(BUILD_DIR)/main

# Create all object files
.PHONY: all
all: $(OBJS)
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(FLAGS) -c $< -o $@

# Link object files and compile into binary file
.PHONY: compile
compile: $(COMPILE_TARGET)
$(BUILD_DIR)/main: $(BUILD_DIR)/*.o
	mkdir -p $(BUILD_DIR)
	$(CXX) $(DEPFLAGS) $(FLAGS) $^ -o $@

# Run the binary file
.PHONY: run
run: bin/main
	bin/main

# Clean the build directory with all object files
.PHONY: clean
clean:
	@ rm -r $(BUILD_DIR)/ 2>/dev/null;\
	echo "all build files were removed";

# Generate documentation
.PHONY: doc
doc:
	doxygen -g doxy-config
	doxygen doxy-config

CFrames.o: CFrames.hpp CFrames.cpp
CTile.o: CTile.cpp CTile.hpp CPosition.hpp
CPath.o: CPath.cpp CPath.hpp
CWaves.o: CWaves.hpp CWaves.cpp CTrooper.hpp CFrames.hpp
CTrooper.o: CTrooper.cpp CTrooper.hpp CTile.hpp CFrames.hpp
CTower.o: CTower.cpp CTower.hpp CTile.hpp
CMap.o: CMap.cpp CMap.hpp CTower.hpp CTrooper.hpp CPath.hpp
CPosition.o: CPosition.cpp CPosition.hpp
CMageTower.o: CMageTower.cpp CMageTower.hpp CTower.hpp
CArmoredTrooper.o: CArmoredTrooper.cpp CArmoredTrooper.hpp CTrooper.hpp
CApplication.o: CApplication.cpp CApplication.hpp CMap.hpp CWaves.hpp
main.o: main.cpp CApplication.hpp CMap.hpp
