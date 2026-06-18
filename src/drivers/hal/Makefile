DRIVERS_HAL_DIR := $(SRC_DIR)/drivers/hal

DRIVERS_HAL_SRCS_CXX := $(wildcard $(DRIVERS_HAL_DIR)/*.cpp)

DRIVERS_HAL_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(DRIVERS_HAL_SRCS_CXX))

CBK_OBJS += $(DRIVERS_HAL_OBJS)