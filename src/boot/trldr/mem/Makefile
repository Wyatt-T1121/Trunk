BOOT_MM_DIR := $(SRC_DIR)/boot/trldr/mem

BOOT_MM_SRCS_NASM := $(wildcard $(BOOT_MM_DIR)/*.asm)
BOOT_MM_SRCS_CXX  := $(wildcard $(BOOT_MM_DIR)/*.cpp)

BOOT_MM_OBJS := \
    $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.asm.o, $(BOOT_MM_SRCS_NASM)) \
    $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(BOOT_MM_SRCS_CXX))

CBK_OBJS += $(BOOT_MM_OBJS)