TROS_GDT_DIR := $(SRC_DIR)/trunk/tros/gdt

TROS_GDT_SRCS_NASM := $(wildcard $(TROS_GDT_DIR)/*.asm)
TROS_GDT_SRCS_CXX  := $(wildcard $(TROS_GDT_DIR)/*.cpp)

TROS_GDT_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(TROS_GDT_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(TROS_GDT_SRCS_CXX))

TRUNK_OBJS += $(TROS_GDT_OBJS)