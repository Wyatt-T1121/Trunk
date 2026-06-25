CBK_MM_DIR := $(SRC_DIR)/cbk/mm

CBK_MM_SRCS_NASM := $(wildcard $(CBK_MM_DIR)/*.asm)
CBK_MM_SRCS_CXX  := $(wildcard $(CBK_MM_DIR)/*.cpp)

CBK_MM_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(CBK_MM_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(CBK_MM_SRCS_CXX))

CBK_OBJS += $(CBK_MM_OBJS)