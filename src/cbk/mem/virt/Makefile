CBK_MEM_VIRT_DIR := $(SRC_DIR)/cbk/mem/virt

CBK_MEM_VIRT_SRCS_NASM := $(wildcard $(CBK_MEM_VIRT_DIR)/*.asm)
CBK_MEM_VIRT_SRCS_CXX  := $(wildcard $(CBK_MEM_VIRT_DIR)/*.cpp)

CBK_MEM_VIRT_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(CBK_MEM_VIRT_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(CBK_MEM_VIRT_SRCS_CXX))

CBK_OBJS += $(CBK_MEM_VIRT_OBJS)