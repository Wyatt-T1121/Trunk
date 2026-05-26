#  Trunk OS - Root Makefile

# --- Identity ---
KERNEL_NAME    := trunk
VERSION        := 0.0.1

# --- Toolchain ---
CXX            := x86_64-elf-g++
AS             := x86_64-elf-as
NASM           := nasm
LD             := x86_64-elf-ld
AR             := x86_64-elf-ar
OBJCOPY        := x86_64-elf-objcopy
GRUB_MKRESCUE  := grub-mkrescue

# --- Directories ---
SRC_DIR        := src
INCLUDE_DIR    := include
BUILD_DIR      := build
ISO_DIR        := iso
LINKER_DIR     := linker
SCRIPTS_DIR    := scripts

# --- Output Artifacts ---
KERNEL_ELF     := $(BUILD_DIR)/$(KERNEL_NAME).elf
KERNEL_BIN     := $(BUILD_DIR)/$(KERNEL_NAME).bin
ISO_IMAGE      := $(BUILD_DIR)/$(KERNEL_NAME).iso

# --- Linker Script ---
LINKER_SCRIPT  := $(LINKER_DIR)/trunk.ld

# --- Include Paths ---
INCLUDES       := -I$(INCLUDE_DIR)

# --- Compiler Flags ---
CXXFLAGS := \
	-std=c++20              \
	-ffreestanding          \
	-fno-exceptions         \
	-fno-rtti               \
	-fno-stack-protector    \
	-fno-pie                \
	-fno-pic                \
	-mno-red-zone           \
	-mno-mmx                \
	-mno-sse                \
	-mno-sse2               \
	-mcmodel=kernel         \
	-m64                    \
	-Wall                   \
	-Wextra                 \
	-O2                     \
	$(INCLUDES)

ASFLAGS  := $(INCLUDES)
NASMFLAGS := -f elf64

# --- Linker Flags ---
LDFLAGS  := \
	-nostdlib               \
	-static                 \
	-T $(LINKER_SCRIPT)     \
	-z max-page-size=0x1000

# Recursively find all source files under src/
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Gather all .cpp and .S files from both subsystems
TRUNK_SRCS_CXX := $(call rwildcard,$(SRC_DIR)/trunk,*.cpp)
TRUNK_SRCS_ASM := $(call rwildcard,$(SRC_DIR)/trunk,*.S)
TRUNK_SRCS_NASM:= $(call rwildcard,$(SRC_DIR)/trunk,*.asm)

TKLIB_SRCS_CXX := $(call rwildcard,$(SRC_DIR)/tklib,*.cpp)
TKLIB_SRCS_ASM := $(call rwildcard,$(SRC_DIR)/tklib,*.S)

ALL_SRCS_CXX   := $(TRUNK_SRCS_CXX) $(TKLIB_SRCS_CXX)
ALL_SRCS_ASM   := $(TRUNK_SRCS_ASM) $(TKLIB_SRCS_ASM)
ALL_SRCS_NASM  := $(TRUNK_SRCS_NASM)

# Map sources → object files, keeping the mirrored path under build/
OBJS_CXX  := $(patsubst $(SRC_DIR)/%.cpp,  $(BUILD_DIR)/%.o, $(ALL_SRCS_CXX))
OBJS_ASM  := $(patsubst $(SRC_DIR)/%.S,    $(BUILD_DIR)/%.S.o, $(ALL_SRCS_ASM))
OBJS_NASM := $(patsubst $(SRC_DIR)/%.asm,  $(BUILD_DIR)/%.asm.o, $(ALL_SRCS_NASM))

ALL_OBJS  := $(OBJS_CXX) $(OBJS_ASM) $(OBJS_NASM)


.PHONY: all
all: $(ISO_IMAGE)
	@echo " ISO: $(ISO_IMAGE)"


# C++ sources → object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX  $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# GAS assembly sources → object files
$(BUILD_DIR)/%.S.o: $(SRC_DIR)/%.S
	@mkdir -p $(dir $@)
	@echo "  AS   $<"
	@$(CXX) $(CXXFLAGS) -x assembler-with-cpp -c $< -o $@

# NASM assembly sources → object files
$(BUILD_DIR)/%.asm.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	@echo "  NASM $<"
	@$(NASM) $(NASMFLAGS) $< -o $@

$(KERNEL_ELF): $(ALL_OBJS) $(LINKER_SCRIPT)
	@mkdir -p $(BUILD_DIR)
	@echo "  LD   $@"
	@$(LD) $(LDFLAGS) $(ALL_OBJS) -o $@

$(KERNEL_BIN): $(KERNEL_ELF)
	@echo "  OBJCOPY → $(KERNEL_BIN)"
	@$(OBJCOPY) -O binary $< $@

$(ISO_IMAGE): $(KERNEL_ELF)
	@echo "  ISO  Building $(KERNEL_NAME).iso..."
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(KERNEL_ELF) $(ISO_DIR)/boot/$(KERNEL_NAME).elf
	@cp grub.cfg      $(ISO_DIR)/boot/grub/grub.cfg
	@$(GRUB_MKRESCUE) -o $@ $(ISO_DIR) 2>/dev/null
	@echo "  ISO  $(ISO_IMAGE) ready."

.PHONY: kernel
kernel: $(KERNEL_ELF)

.PHONY: iso
iso: $(ISO_IMAGE)

.PHONY: run
run: $(ISO_IMAGE)
	@bash $(SCRIPTS_DIR)/run_qemu.sh

.PHONY: run-debug
run-debug: $(ISO_IMAGE)
	@bash $(SCRIPTS_DIR)/run_qemu_debug.sh

.PHONY: run-kvm
run-kvm: $(ISO_IMAGE)
	@bash $(SCRIPTS_DIR)/run_qemu_kvm.sh

.PHONY: clean
clean:
	@echo "  CLEAN  Removing build/ and iso/ ..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(ISO_DIR)
	@echo "  CLEAN  Done."

.PHONY: mrproper
mrproper: clean
	@echo "  MRPROPER  Full wipe complete."

.PHONY: info
info:
	@echo "Kernel:      $(KERNEL_NAME) v$(VERSION)"
	@echo "CXX sources: $(words $(ALL_SRCS_CXX))"
	@echo "ASM sources: $(words $(ALL_SRCS_ASM)) GAS + $(words $(ALL_SRCS_NASM)) NASM"
	@echo "Objects:     $(words $(ALL_OBJS))"
	@echo "Toolchain:   $(CXX)"

.PHONY: list-srcs
list-srcs:
	@echo "=== C++ Sources ===" && echo "$(ALL_SRCS_CXX)" | tr ' ' '\n'
	@echo "=== ASM Sources ===" && echo "$(ALL_SRCS_ASM) $(ALL_SRCS_NASM)" | tr ' ' '\n'

-include $(ALL_OBJS:.o=.d)