TROS_ISR_DIR := $(SRC_DIR)/cbk/interrupts/isr

TROS_ISR_SRCS_NASM := $(wildcard $(TROS_ISR_DIR)/*.asm)
TROS_ISR_SRCS_CXX  := $(wildcard $(TROS_ISR_DIR)/*.cpp)

TROS_ISR_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(TROS_ISR_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,     $(TROS_ISR_SRCS_CXX))

CBK_OBJS += $(TROS_ISR_OBJS)