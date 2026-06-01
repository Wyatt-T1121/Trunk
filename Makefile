# =============================================================================
#  Trunk OS - Root Makefile
# =============================================================================

# --- Identity ---
KERNEL_NAME    := trunk
VERSION        := 0.3.3

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

# --- Build Mode ---
ifdef DEBUG
    BUILD_MODE  := DEBUG
    OPT_FLAGS   := -O0 -g
    MODE_FLAGS  := -DTRUNK_DEBUG
else
    BUILD_MODE  := RELEASE
    OPT_FLAGS   := -O2
    MODE_FLAGS  :=
endif

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
	-MMD -MP                \
	$(OPT_FLAGS)            \
	$(MODE_FLAGS)           \
	$(INCLUDES)

ASFLAGS   := $(INCLUDES)
NASMFLAGS := -f elf64

# --- Linker Flags ---
LDFLAGS  := \
	-nostdlib               \
	-static                 \
	-T $(LINKER_SCRIPT)     \
	-z max-page-size=0x1000

# =============================================================================
#  Source Discovery
# =============================================================================

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

TRUNK_SRCS_CXX  := $(call rwildcard,$(SRC_DIR)/trunk,*.cpp)
TRUNK_SRCS_ASM  := $(call rwildcard,$(SRC_DIR)/trunk,*.S)
TRUNK_SRCS_NASM := $(call rwildcard,$(SRC_DIR)/trunk,*.asm)

TKLIB_SRCS_CXX  := $(call rwildcard,$(SRC_DIR)/tklib,*.cpp)
TKLIB_SRCS_ASM  := $(call rwildcard,$(SRC_DIR)/tklib,*.S)

ALL_SRCS_CXX    := $(TRUNK_SRCS_CXX) $(TKLIB_SRCS_CXX)
ALL_SRCS_ASM    := $(TRUNK_SRCS_ASM) $(TKLIB_SRCS_ASM)
ALL_SRCS_NASM   := $(TRUNK_SRCS_NASM)

OBJS_CXX  := $(patsubst $(SRC_DIR)/%.cpp,   $(BUILD_DIR)/%.o,     $(ALL_SRCS_CXX))
OBJS_ASM  := $(patsubst $(SRC_DIR)/%.S,     $(BUILD_DIR)/%.S.o,   $(ALL_SRCS_ASM))
OBJS_NASM := $(patsubst $(SRC_DIR)/%.asm,   $(BUILD_DIR)/%.asm.o, $(ALL_SRCS_NASM))

ALL_OBJS  := $(OBJS_CXX) $(OBJS_ASM) $(OBJS_NASM)

# =============================================================================
#  Targets
# =============================================================================

.PHONY: all
all: $(ISO_IMAGE)
	@echo ""
	@echo "  Build : $(BUILD_MODE)"
	@echo "  ISO   : $(ISO_IMAGE)"
	@echo ""

# --- Compilation ---

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX   $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.S.o: $(SRC_DIR)/%.S
	@mkdir -p $(dir $@)
	@echo "  AS    $<"
	@$(CXX) $(CXXFLAGS) -x assembler-with-cpp -c $< -o $@

$(BUILD_DIR)/%.asm.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	@echo "  NASM  $<"
	@$(NASM) $(NASMFLAGS) $< -o $@

# --- Linking ---

$(KERNEL_ELF): $(ALL_OBJS) $(LINKER_SCRIPT)
	@mkdir -p $(BUILD_DIR)
	@echo "  LD    $@"
	@$(LD) $(LDFLAGS) $(ALL_OBJS) -o $@

$(KERNEL_BIN): $(KERNEL_ELF)
	@echo "  OBJCOPY → $(KERNEL_BIN)"
	@$(OBJCOPY) -O binary $< $@

# --- ISO ---

$(ISO_IMAGE): $(KERNEL_ELF)
	@echo "  ISO   Building $(KERNEL_NAME).iso..."
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(KERNEL_ELF) $(ISO_DIR)/boot/$(KERNEL_NAME).elf
	@cp grub.cfg      $(ISO_DIR)/boot/grub/grub.cfg
	@$(GRUB_MKRESCUE) -o $@ $(ISO_DIR) 2>/dev/null
	@echo "  ISO   $(ISO_IMAGE) ready."

# --- Convenience ---

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

# --- Clean ---

.PHONY: clean
clean:
	@echo "  CLEAN   Removing build/ and iso/ ..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(ISO_DIR)
	@echo "  CLEAN   Done."

.PHONY: mrproper
mrproper: clean
	@echo "  MRPROPER  Full wipe complete."

# --- Info ---

.PHONY: info
info:
	@echo "Kernel  : $(KERNEL_NAME) v$(VERSION)"
	@echo "Mode    : $(BUILD_MODE)"
	@echo "CXX     : $(words $(ALL_SRCS_CXX)) sources"
	@echo "ASM     : $(words $(ALL_SRCS_ASM)) GAS + $(words $(ALL_SRCS_NASM)) NASM"
	@echo "Objects : $(words $(ALL_OBJS))"
	@echo "Chain   : $(CXX)"

.PHONY: list-srcs
list-srcs:
	@echo "=== C++ ===" && echo "$(ALL_SRCS_CXX)" | tr ' ' '\n'
	@echo "=== ASM ===" && echo "$(ALL_SRCS_ASM) $(ALL_SRCS_NASM)" | tr ' ' '\n'

-include $(ALL_OBJS:.o=.d)