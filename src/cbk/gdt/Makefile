CBK_GDT_DIR := $(SRC_DIR)/cbk/gdt

CBK_GDT_SRCS_NASM := $(wildcard $(CBK_GDT_DIR)/*.asm)
CBK_GDT_SRCS_CXX  := $(wildcard $(CBK_GDT_DIR)/*.cpp)

CBK_GDT_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(CBK_GDT_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(CBK_GDT_SRCS_CXX))

CBK_OBJS += $(CBK_GDT_OBJS)