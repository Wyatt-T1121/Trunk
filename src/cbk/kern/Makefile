include src/cbk/kern/init/Makefile

CBK_KERN_DIR := $(SRC_DIR)/cbk/kern

CBK_KERN_SRCS_NASM := $(wildcard $(CBK_KERN_DIR)/*.asm)
CBK_KERN_SRCS_CXX  := $(wildcard $(CBK_KERN_DIR)/*.cpp)

CBK_KERN_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(CBK_KERN_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(CBK_KERN_SRCS_CXX))

CBK_OBJS += $(CBK_KERN_OBJS)