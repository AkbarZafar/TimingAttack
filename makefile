CXX := g++

CXXFLAGS := -std=c++11 -Wall -Wextra

SRCS := time-attack.cpp

# Object files
OBJS := $(SRCS:.cpp=.o)

# Executable name
TARGET := timeattack

# Default target
all: $(TARGET)

# Linking object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compiling source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Cleaning generated files
clean:
	rm -f $(OBJS) $(TARGET)