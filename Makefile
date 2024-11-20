# Makefile for the Raytracer project

# Compiler
CXX := g++

# Compiler flags
CXXFLAGS := -Wall -Wextra -std=c++17 -Iinclude

# Directories
SRCDIR := src
INCDIR := include
# BINDIR := bin
OBJDIR := obj

# Target executable
TARGET := raytracer

# Source files
SRCS := $(wildcard $(SRCDIR)/*.cpp)

# Object files
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

# Default rule
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean
