CBK_ISR_DIR := $(SRC_DIR)/cbk/interrupts/isr

CBK_ISR_SRCS_NASM := $(wildcard $(CBK_ISR_DIR)/*.asm)
CBK_ISR_SRCS_CXX  := $(wildcard $(CBK_ISR_DIR)/*.cpp)

CBK_ISR_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(CBK_ISR_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(CBK_ISR_SRCS_CXX))

CBK_OBJS += $(CBK_ISR_OBJS)