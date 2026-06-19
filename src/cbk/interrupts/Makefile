include src/cbk/interrupts/idt/Makefile
include src/cbk/interrupts/isr/Makefile

CBK_INTERRUPTS_DIR := $(SRC_DIR)/cbk/interrupts

CBK_INTERRUPTS_SRCS_NASM := $(wildcard $(CBK_INTERRUPTS_DIR)/*.asm)
CBK_INTERRUPTS_SRCS_CXX  := $(wildcard $(CBK_INTERRUPTS_DIR)/*.cpp)

CBK_INTERRUPTS_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(CBK_INTERRUPTS_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(CBK_INTERRUPTS_SRCS_CXX))

CBK_OBJS += $(CBK_INTERRUPTS_OBJS)