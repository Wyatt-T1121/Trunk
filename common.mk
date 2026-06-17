REPO_ROOT := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

include $(REPO_ROOT)/builder/config/build.cfg
include $(REPO_ROOT)/builder/config/toolchain.cfg

VERSION         := $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

OBJ_DIR         := $(BUILD_DIR)/obj
ELF_DIR         := $(BUILD_DIR)/elf
LIB_DIR         := $(BUILD_DIR)/lib
ISO_DIR         := $(BUILD_DIR)/iso
IMG_DIR         := $(BUILD_DIR)/img
LOG_DIR         := $(BUILD_DIR)/logs
LOG_QEMU_DIR    := $(LOG_DIR)/qemu
LOG_BUILD_DIR   := $(LOG_DIR)/build

LINKER_DIR      := $(SETUP_DIR)/linker
GRUB_DIR        := $(SETUP_DIR)/grub

TRUNK_ELF       := $(ELF_DIR)/trunk.elf
TKLIB_A         := $(LIB_DIR)/tklib.a
ISO_IMAGE       := $(ISO_DIR)/trunk.iso
DISK_IMAGE      := $(IMG_DIR)/trunk.img
TRUNK_LD        := $(LINKER_DIR)/trunk.ld

_R  := \033[0;31m
_G  := \033[0;32m
_Y  := \033[0;33m
_B  := \033[0;34m
_M  := \033[0;35m
_C  := \033[0;36m
_BD := \033[1m
_RS := \033[0m

_ok       = @printf "  $(_G)[ OK ]$(_RS)  %s\n"    "$1"
_info     = @printf "  $(_C)[ INFO ]$(_RS)  %s\n"  "$1"
_warn     = @printf "  $(_Y)[ WARN ]$(_RS)  %s\n"  "$1"
_step     = @printf "  $(_B)[ .... ]$(_RS)  %s\n"  "$1"
_cxx      = @printf "  $(_M)[ CXX ]$(_RS)  %s\n"   "$1"
_asm      = @printf "  $(_C)[ ASM ]$(_RS)  %s\n"   "$1"
_nasm     = @printf "  $(_C)[ NASM ]$(_RS)  %s\n"  "$1"
_ld       = @printf "  $(_Y)[ LD ]$(_RS)  %s\n"    "$1"
_ar       = @printf "  $(_Y)[ AR ]$(_RS)  %s\n"    "$1"
_iso      = @printf "  $(_B)[ ISO ]$(_RS)  %s\n"   "$1"
_disk     = @printf "  $(_B)[ DISK ]$(_RS)  %s\n"  "$1"
_clean    = @printf "  $(_R)[ CLEAN ]$(_RS)  %s\n" "$1"
_info_cmd = @$1 | xargs -I{} printf "  $(_C)[ INFO ]$(_RS)  %-9s: {}\n" "$2"