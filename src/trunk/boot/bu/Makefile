BOOT_BU_DIR := $(SRC_DIR)/trunk/boot/bu

BOOT_BU_SRCS_CXX := $(wildcard $(BOOT_BU_DIR)/*.cpp)

BOOT_BU_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(BOOT_BU_SRCS_CXX))

BOOT_OBJS += $(BOOT_BU_OBJS)