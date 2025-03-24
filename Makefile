# tarman
# Copyright (C) 2024 - 2025 Alessandro Salerno
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

CC=gcc
CFLAGS=-Wall -Wpedantic -Wextra -std=c11 -Iinclude/ -DEXT_TARMAN_BUILD="\"$(shell date +%y.%m.%d)\""
LDFLAGS=

DEBUG_CFLAGS=-O0 -fsanitize=undefined -fsanitize=address -g
DEBUG_LDFLAGS=
RELEASE_CLFAGS=-O3 -Werror
RELEASE_LDFLAGS=-flto

CUSTOM_CFLAGS?=
CUSTOM_LDFLAGS?=

TARMAN_OS?=$(shell uname -s | tr A-Z a-z)
CFLAGS+=-Iinclude/os/$(TARMAN_OS)
CFLAGS+=$(CUSTOM_CFLAGS)
LDFLAGS+=$(CUSTOM_LDFLAGS)

BIN=bin
EXEC=$(BIN)/tarman

PLUGINS=$(wildcard plugins/*)
PLUGIN_MAKEFILES=$(foreach dir,$(PLUGINS),$(wildcard $(dir)/Makefile))

ifeq ($(OS),Windows_NT)
	TARMAN_OS=window
	EXEC+=.exe
endif

CFLAGS+=-DEXT_TARMAN_OS="\"$(TARMAN_OS)\""

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard ,$d, $2) $(filter $(subst *, %, $2),$d))
SRC=$(call rwildcard, src/common, *.c)
SRC+=$(call rwildcard, src/os-specific/$(TARMAN_OS), *.c)

include src/os-specific/$(TARMAN_OS)/Makefile

OBJ=$(patsubst src/%.c,obj/%.o, $(SRC))

SDK_SRC=$(wildcard src/plugin-sdk/*.c)
SDK_OBJ=$(filter-out obj/common/main.o,$(OBJ))
SDK_OBJ+=$(patsubst src/%.c,obj/%.o, $(SDK_SRC))

debug:
	@echo =========== COMPILING IN DEBUG MODE ===========
	@make all CUSTOM_CFLAGS="$(DEBUG_CFLAGS)" "CUSTOM_LDFLAGS=$(DEBUG_LDFLAGS)"

release:
	@echo =========== COMPILING IN DEBUG MODE ===========
	@make CUSTOM_CFLAGS=$(RELEASE_CLFAGS) CUSTOM_LDFLAGS=$(RELEASE_LDFLAGS) all

all: info dirs $(OBJ) $(EXEC) plugins
	@echo
	@echo All done!

plugins: plugin-sdk $(PLUGIN_MAKEFILES)

info:
	@echo Compiling for $(TARMAN_OS)
	@echo

$(EXEC): obj $(OBJ)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJ) -o $(EXEC)
	@echo

obj/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@
	@echo

plugin-sdk: $(SDK_OBJ)
	@echo Compiling Plugin SDK
	@$(CC) -r $(SDK_OBJ) -o $(BIN)/plugin-sdk.o

install: release
	mkdir tarman && \
	cp ./bin/tarman ./tarman/tarman && \
	tar -cvzf ./tarman-package.tar.gz ./tarman && \
	rm -rf ./tarman && \
	./bin/tarman install -f tar.gz -n tarman -x tarman/tarman -p ./tarman-package.tar.gz && \
	rm ./tarman-package.tar.gz

$(PLUGIN_MAKEFILES): force
	@echo Compiling plugin "'$(@D)'"
	@$(MAKE) -C $(@D) DIST="../../$(BIN)/plugins" CC="$(CC)" SDK="../../$(BIN)/plugin-sdk.o" SDK_FLAGS="$(CUSTOM_CFLAGS) $(CUSTOM_LDFLAGS) -I../../include/" > /dev/null

force: ;

dirs:
	@mkdir -p obj/
	@mkdir -p $(BIN)
	@mkdir -p $(BIN)/plugins

clean:
	rm -rf obj/; \
	rm -rf $(BIN); \
	rm -rf lib/
