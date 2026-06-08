TROS_KERN_LP_DIR := $(SRC_DIR)/trunk/tros/kern/lp

TROS_KERN_LP_SRCS_NASM := $(wildcard $(TROS_KERN_LP_DIR)/*.asm)
TROS_KERN_LP_SRCS_CXX := $(wildcard $(TROS_KERN_LP_DIR)/*.cpp)

TROS_KERN_LP_OBJS := \
	$(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(TROS_KERN_LP_SRCS_NASM))
	$(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(TROS_KERN_LP_SRCS_CXX))

KERN_OBJS += $(TROS_KERN_LP_OBJS)