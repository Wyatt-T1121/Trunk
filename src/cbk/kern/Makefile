include src/cbk/kern/init/Makefile

TROS_KERN_DIR := $(SRC_DIR)/cbk/kern

TROS_KERN_SRCS_NASM := $(wildcard $(TROS_KERN_DIR)/*.asm)
TROS_KERN_SRCS_CXX  := $(wildcard $(TROS_KERN_DIR)/*.cpp)

TROS_KERN_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(TROS_KERN_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(TROS_KERN_SRCS_CXX))

CBK_OBJS += $(TROS_KERN_OBJS)