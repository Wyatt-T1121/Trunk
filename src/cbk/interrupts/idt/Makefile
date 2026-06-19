CBK_IDT_DIR := $(SRC_DIR)/cbk/interrupts/idt

CBK_IDT_SRCS_NASM := $(wildcard $(CBK_IDT_DIR)/*.asm)
CBK_IDT_SRCS_CXX  := $(wildcard $(CBK_IDT_DIR)/*.cpp)

CBK_IDT_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(CBK_IDT_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(CBK_IDT_SRCS_CXX))

CBK_OBJS += $(CBK_IDT_OBJS)