CBK_INIT_DIR := $(SRC_DIR)/cbk/init

CBK_INIT_SRCS_NASM := $(wildcard $(CBK_INIT_DIR)/*.asm)
CBK_INIT_SRCS_CXX  := $(wildcard $(CBK_INIT_DIR)/*.cpp)

CBK_INIT_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(CBK_INIT_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(CBK_INIT_SRCS_CXX))

CBK_OBJS += $(CBK_INIT_OBJS)