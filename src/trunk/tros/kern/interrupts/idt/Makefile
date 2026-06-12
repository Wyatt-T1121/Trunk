TROS_IDT_DIR := $(SRC_DIR)/trunk/tros/kern/interrupts/idt

TROS_IDT_SRCS_NASM := $(wildcard $(TROS_IDT_DIR)/*.asm)
TROS_IDT_SRCS_CXX  := $(wildcard $(TROS_IDT_DIR)/*.cpp)

TROS_IDT_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(TROS_IDT_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(TROS_IDT_SRCS_CXX))

TRUNK_OBJS += $(TROS_IDT_OBJS)