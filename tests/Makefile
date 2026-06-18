include ../common.mk

CXX      := g++
CXXFLAGS := -std=c++23 -Wall -Wextra -O2
LDFLAGS  := -lgtest -lgtest_main -lpthread

BUILD_DIR := build

TEST_ROOT          := $(CURDIR)
UT_DIR             := $(TEST_ROOT)/ut
MOCK_DIR           := $(TEST_ROOT)/mock

GLOBAL_INCLUDE_DIR := $(TEST_ROOT)/../include
TKLIB_INCLUDE_DIR  := $(TEST_ROOT)/../lib/tklib/include

INCLUDES           := -I$(GLOBAL_INCLUDE_DIR) -I$(TKLIB_INCLUDE_DIR) -I$(MOCK_DIR)

TARGET   := $(BUILD_DIR)/TESTEXEC

TKLIB_DIR  := $(TEST_ROOT)/../lib/tklib/src/tklib
MEM_DIR    := $(TEST_ROOT)/../src/cbk/mem

CT_SRCS    := $(wildcard $(UT_DIR)/*.cpp)
MOCK_SRCS  := $(wildcard $(MOCK_DIR)/*.cpp)
TKLIB_SRCS := $(wildcard $(TKLIB_DIR)/*.cpp)
MEM_SRCS   := $(wildcard $(MEM_DIR)/*.cpp)

ALL_SRCS   := $(CT_SRCS) $(MOCK_SRCS) $(TKLIB_SRCS) $(MEM_SRCS)

vpath %.cpp $(UT_DIR):$(MOCK_DIR):$(TKLIB_DIR):$(MEM_DIR)

OBJS       := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(ALL_SRCS)))

.PHONY: all clean run build_dir

all:
	@$(MAKE) --no-print-directory build_dir
	@$(MAKE) --no-print-directory $(TARGET)
	@$(MAKE) --no-print-directory run
	@$(MAKE) --no-print-directory clean

build_dir:
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) -o $(TARGET)

$(BUILD_DIR)/%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run:
	@./$(TARGET)

clean:
	@rm -rf $(BUILD_DIR)