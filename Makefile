# Thomas Daley
# September 13, 2021

# A generic build template for C/C++ programs

# LIBcutable name
LIB = libhttpc.so

# C compiler
CC = gcc
# linker
LD = gcc

# C flags
CFLAGS = -fPIC -g -std=gnu99
# dependency-generation flags
DEPFLAGS =
# linker flags
LDFLAGS = -shared -g
# library flags
LDLIBS = 

# build directories
BIN = .
OBJ = out
SRC = src

SOURCES := $(wildcard $(SRC)/*.c)

OBJECTS := \
	$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(wildcard $(SRC)/*.c)) \

# include compiler-generated dependency rules
DEPENDS := $(OBJECTS:.o=.d)

# compile C source
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -c -o $@
# link objects
LINK.o = $(LD) $(LDFLAGS) $(LDLIBS) $(OBJECTS) -o $@

.DEFAULT_GOAL = all

.PHONY: all
all: $(BIN)/$(LIB)
	cp $(BIN)/$(LIB) test_app/
	make -C test_app

$(BIN)/$(LIB): $(SRC) $(OBJ) $(BIN) $(OBJECTS)
	$(LINK.o)

$(SRC):
	mkdir -p $(SRC)

$(OBJ):
	mkdir -p $(OBJ)

$(BIN):
	mkdir -p $(BIN)

$(OBJ)/%.o:	$(SRC)/%.c
	$(COMPILE.c) $<

# force rebuild
.PHONY: remake
remake:	clean $(BIN)/$(LIB)

# LIBcute the program
.PHONY: run
run: $(BIN)/$(LIB)
	./$(BIN)/$(LIB)

# remove previous build and objects
.PHONY: clean
clean:
	$(RM) $(OBJECTS)
	$(RM) $(DEPENDS)
	$(RM) $(BIN)/$(LIB)

# remove everything except source
.PHONY: reset
reset:
	$(RM) -r $(OBJ)
	$(RM) -r $(BIN)

-include $(DEPENDS)
