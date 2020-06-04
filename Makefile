CXX         	:= g++
CXX_FLAGS    	:= -Wall -pedantic -std=c++14
BUILD_DIR   	:= bin
SOURCE_DIR      := src
DEP_DIR			:= dep
SOURCES			:= $(wildcard $(SOURCE_DIR)/*.cpp)
DEP_TARGET		:= Make.d
OBJS	    	:= $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o, $(SOURCES))
COMPILE_TARGET	:= ryntluka

# Compile and document the project
.PHONY: all
all: compile doc

# Link object files and compile into binary file
.PHONY: compile
compile: depend $(COMPILE_TARGET)

# Link object files
$(COMPILE_TARGET): $(OBJS)
	@ $(CXX) $(CXX_FLAGS) $^ -o $@; \
 	echo "Compilation successful...";

# Create object files
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@ mkdir -p $(BUILD_DIR); \
	$(CXX) $(CXX_FLAGS) $< -c -o $@

# Run the binary file
.PHONY: run
run: depend $(COMPILE_TARGET)
	@ echo "Running game..."; \
	./$(COMPILE_TARGET)

# Clean the build directory with all object files
.PHONY: clean
clean:
	@ rm -r $(DEP_TARGET) $(COMPILE_TARGET) $(BUILD_DIR)/ 2>/dev/null; \
	echo "All build files were removed...";

# Create dependencies that we can read for compilation
.PHONY: depend
depend:
	@ $(CXX) $(CXX_FLAGS) -MM $(SOURCES) > $(DEP_TARGET); \
	echo "Dependencies created...";

.PHONY: memtest
memtest: $(COMPILE_TARGET)
	@ echo "Testing memory..."; \
 	valgrind ./$(COMPILE_TARGET)

# Generate documentation
doc: doc src/*.hpp
	@ doxygen Doxyfile; \
 	echo "Documentation updated...";

# ZIP the directories
zip: 	
	@ mkdir ryntluka; \
	cp -r src testing examples Makefile Doxyfile zadani.txt prohlaseni.txt README.md ryntluka; \
	zip -r ryntluka.zip ryntluka; \
	rm -r ryntluka; \
 	echo "Zip created..."


# Include dependencies
-include $(DEP_TARGET);
