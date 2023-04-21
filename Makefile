SUB = concord
CC = gcc

CORE_DIR      = $(SUB)/core
INCLUDE_DIR   = $(SUB)/include
GENCODECS_DIR = $(SUB)/gencodecs
SRC           = src
INC           = inc

BOT   = dc_bot
BOTS += $(SRC)/$(BOT)

CFLAGS  += -O0 -g -pthread -Wall \
           -I$(INCLUDE_DIR) -I$(CORE_DIR) -I$(GENCODECS_DIR) -I$(INC)
LDFLAGS  = -L$(SUB)/lib
LDLIBS   = -ldiscord -lcurl

all: build_sub $(BOTS) target run

build_sub:
	$(MAKE) -C $(SUB)

echo:
	@ echo -e 'CC: $(CC)\n'
	@ echo -e 'BOT: $(SRC)/$(BOT)\n'

target:
	@ mkdir -p build
	@ mv $(SRC)/$(BOT) build

clean:
	@ rm -f $(SRC)/$(BOT)

run: all
	@ echo -e 'Running $(BOT)...\n'
	@ ./build/$(BOT)

.PHONY: build_sub all echo clean
