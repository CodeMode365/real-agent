CXX = g++
CXXFLAGS = -Wall -Wextra -Wpedantic -Wshadow -g3 -O0 -std=c++17

# Output executable
TARGET = build/Debug/outDebug

# Source files
SRCS = main.cpp \
       src/admin/admin.cpp \
       src/clients/client.cpp \
       src/seller/seller.cpp \
       src/property/property.cpp \
       src/menu/menu.cpp \
       src/utils/input/input.cpp \
       src/utils/screen/screen.cpp \
       src/utils/user/user.cpp


# Object files
OBJS = build/Debug/main.o \
       build/Debug/admin/admin.o \
       build/Debug/clients/client.o \
       build/Debug/seller/seller.o \
       build/Debug/property/property.o \
       build/Debug/menu/menu.o \
       build/Debug/utils/input/input.o \
       build/Debug/utils/screen/screen.o \
       build/Debug/utils/user/user.o

.PHONY: all test clean

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

build/Debug/main.o: main.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@


# Run the full test suite (per-module scripts + fuzzer + analytics).
test: $(TARGET)
	@bash tests/run_all.sh

# Run a single module's tests, e.g. `make test-seller`.
test-%: $(TARGET)
	@bash tests/test_$*.sh

# Clean
clean:
	rm -rf build/Debug/*
