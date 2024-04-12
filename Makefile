OSNAME=""
ifeq ($(OS),Windows_NT)
# add mingw gcc to path (e.g. set PATH=%PATH%;D:/mingw64/bin/)
  OSNAME=Windows_NT
  DEL=rmdir /s /q
  MKDIR=md
  BOARD=win
  LDFLAGS_EXTRA = -Wl,--gc-sections
  THIRD_INCS=
  THIRD_LIBS=
else 
  OSNAME = $(shell uname -s)
  ifeq ($(OSNAME),Linux)
    DEL=rm -fR
    MKDIR=mkdir -p
    BOARD=linux
    LDFLAGS_EXTRA = -Wl,--gc-sections
    THIRD_INCS=-I/usr/include/ -L/usr/lib/x86_64-linux-gnu
    THIRD_LIBS=-lpthread 
  endif
endif

$(info OS is $(OSNAME))

ifeq ($(OSNAME),"")
  $(info "O.S. not supported")
  $(shell exit)
endif

CC = gcc
BUILD_DIR = build
#MAKEFLAGS += --jobs=8

#-Wextra -Werror
CFLAGS = -g -Wall -std=c11 -I. -I./inc -Iutl/inc -Ihal/inc -Iutl/printf -Iutl/ods $(THIRD_INCS)
LDFLAGS = $(LDFLAGS_EXTRA) -lm  $(THIRD_LIBS)

HAL_SRCS = hal/src/hal.c hal/src/hal_rtc.c hal/src/hal_tmr.c hal/src/hal_cpu.c 
PORT_SRCS = port/$(BOARD)/rtc.c port/$(BOARD)/stdout.c port/$(BOARD)/timer.c port/$(BOARD)/cpu.c
UTL_SRCS = utl/src/utl_dbg.c utl/printf/utl_printf.c  utl/ods/ods.c
EXAMPLE_SRCS = examples/main.c examples/simple.c
THIRD_SRCS = 

SRCS = $(HAL_SRCS) $(UTL_SRCS) $(PORT_SRCS) $(THIRD_SRCS)

OBJS = $(addprefix $(BUILD_DIR)/,$(SRCS:.c=.o))
EXAMPLE_OBJS = $(addprefix $(BUILD_DIR)/,$(EXAMPLE_SRCS:.c=.o))

PATHS = $(dir $(OBJS) $(EXAMPLE_OBJS))

TARGETS = main simple

ifeq ($(OSNAME),Windows_NT)
  PATHS:= $(subst /,\,$(PATHS))
endif

.PHONY: all clean

all: $(BUILD_DIR) $(TARGETS)

main: $(BUILD_DIR)/examples/main.o $(OBJS)
	$(CC) $^ -o $(BUILD_DIR)/$@ $(LDFLAGS)

simple: $(BUILD_DIR)/examples/simple.o $(OBJS)
	$(CC) $^ -o $(BUILD_DIR)/$@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	-$(MKDIR) $(PATHS)

clean:
	-$(DEL) $(BUILD_DIR)

