CXX = g++
CXXFLAGS = -Wall -Wextra -Wpedantic -Wshadow -g3 -O0 -std=c++17

# Output executable
TARGET = build/Debug/outDebug

# Source files
SRCS = main.cpp \
       src/admin/admin.cpp \
       src/clients/client.cpp \
	   src/property/property.cpp \
       src/menu/menu.cpp \
       src/utils/screen/screen.cpp \
       src/utils/user/user.cpp


# Object files
OBJS = $(patsubst src/%.cpp, build/Debug/%.o, $(SRCS))

# Default target: build and run
all: $(TARGET)
	@echo "===== Running Program ====="
	./$(TARGET)

# Link
$(TARGET): $(OBJS)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile each .cpp -> .o
build/Debug/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@


# Clean
clean:
	rm -rf build/Debug/*
