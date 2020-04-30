CXX         	:= g++
CXX_FLAGS    	:= -Wall -pedantic -std=c++14
BUILD_DIR   	:= bin
SOURCE_DIR      := src
DEP_DIR			:= dep
SOURCES			:= $(wildcard $(SOURCE_DIR)/*.cpp)
DEPENDENCIES	:= $(patsubst $(SOURCE_DIR)/%.cpp,$(DEP_DIR)/%.d, $(SOURCES))	# searches for pattern defined in first part in text in last part and replaces it with middle part
OBJS	    	:= $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o, $(SOURCES))
COMPILE_TARGET	:= $(BUILD_DIR)/main

# Compile and document the project
.PHONY: all
all: compile doc

# Link object files and compile into binary file
.PHONY: compile
compile: $(DEPENDENCIES) $(COMPILE_TARGET)

# Link object files
$(COMPILE_TARGET): $(OBJS)
	@ $(CXX) $(CXX_FLAGS) $^ -o $@; \
  	rm -r $(DEP_DIR)/ $(DEP_DIR) 2>/dev/null;\
 	echo "Compilation successful...";

# Create object files
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@ mkdir -p $(BUILD_DIR)
	@ $(CXX) $(CXX_FLAGS) $< -c -o $@

# Run the binary file
.PHONY: run
run: $(COMPILE_TARGET)
	@ rm -r $(DEP_DIR)/ $(DEP_DIR) 2>/dev/null; \
	echo "Running game..."; \
	./$(COMPILE_TARGET)


# Clean the build directory with all object files
.PHONY: clean
clean:
	@ rm -r $(BUILD_DIR)/ 2>/dev/null;\
	echo "all build files were removed";


# Translate spaces and \ from gcc output
$(DEP_DIR)/%.d: $(SOURCE_DIR)/%.cpp
	@ mkdir -p $(DEP_DIR)
	@ $(CXX) $(CXX_FLAGS) -MM $< > $@.tmp;\
     	cat $@.tmp | tr -d '\n' | tr -d '\\' > $@;\
     	rm -f $@.tmp;


# Stop and read the dependant files
include $(DEP_DIR)/*.d

# Generate documentation
.PHONY: doc
doc:
	@ doxygen Doxyfile > /dev/null; \
 	echo "Documentation updated..."
