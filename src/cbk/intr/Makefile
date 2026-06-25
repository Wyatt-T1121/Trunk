CBK_INTR_DIR := $(SRC_DIR)/cbk/intr

CBK_INTR_SRCS_NASM := $(wildcard $(CBK_INTR_DIR)/*.asm)
CBK_INTR_SRCS_CXX  := $(wildcard $(CBK_INTR_DIR)/*.cpp)

CBK_INTR_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(CBK_INTR_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(CBK_INTR_SRCS_CXX))

CBK_OBJS += $(CBK_INTR_OBJS)