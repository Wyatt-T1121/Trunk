include src/cbk/mem/alloc/Makefile
include src/cbk/mem/virt/Makefile

CBK_MEM_DIR := $(SRC_DIR)/cbk/mem

CBK_MEM_SRCS_NASM := $(wildcard $(CBK_MEM_DIR)/*.asm)
CBK_MEM_SRCS_CXX  := $(wildcard $(CBK_MEM_DIR)/*.cpp)

CBK_MEM_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(CBK_MEM_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(CBK_MEM_SRCS_CXX))

CBK_OBJS += $(CBK_MEM_OBJS)