CBK_BGCHK_DIR := $(SRC_DIR)/cbk/bgchk

CBK_BGCHK_SRCS_NASM := $(wildcard $(CBK_BGCHK_DIR)/*.asm)
CBK_BGCHK_SRCS_CXX  := $(wildcard $(CBK_BGCHK_DIR)/*.cpp)

CBK_BGCHK_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(CBK_BGCHK_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(CBK_BGCHK_SRCS_CXX))

CBK_OBJS += $(CBK_BGCHK_OBJS)