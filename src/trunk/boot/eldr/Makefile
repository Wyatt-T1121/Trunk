BOOT_ELDR_DIR := $(SRC_DIR)/trunk/boot/eldr

BOOT_ELDR_SRCS_CXX := $(wildcard $(BOOT_ELDR_DIR)/*.cpp)

BOOT_ELDR_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(BOOT_ELDR_SRCS_CXX))

TRUNK_OBJS += $(BOOT_ELDR_OBJS)