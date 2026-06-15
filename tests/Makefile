include ../builder/config/build.cfg

CXX      := g++
CXXFLAGS := -std=c++23 -Wall -Wextra -O2

BUILD_DIR := build

INCLUDES := -I$(INCLUDE_DIR)
TARGET   := $(BUILD_DIR)/TESTEXEC

SRCS     := $(wildcard *.cpp)
OBJS     := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))

.PHONY: all clean run build_dir

all:
	@$(MAKE) --no-print-directory build_dir
	@$(MAKE) --no-print-directory $(TARGET)
	@$(MAKE) --no-print-directory run
	@$(MAKE) --no-print-directory clean

build_dir:
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(BUILD_DIR)/%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run:
	@./$(TARGET)

clean:
	@rm -rf $(BUILD_DIR)