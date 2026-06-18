include src/cbk/interrupts/idt/Makefile
include src/cbk/interrupts/isr/Makefile

TROS_INTERRUPTS_DIR := $(SRC_DIR)/cbk/interrupts

TROS_INTERRUPTS_SRCS_NASM := $(wildcard $(TROS_INTERRUPTS_DIR)/*.asm)
TROS_INTERRUPTS_SRCS_CXX  := $(wildcard $(TROS_INTERRUPTS_DIR)/*.cpp)

TROS_INTERRUPTS_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(TROS_INTERRUPTS_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(TROS_INTERRUPTS_SRCS_CXX))

CBK_OBJS += $(TROS_INTERRUPTS_OBJS)