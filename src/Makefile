E_NAME	= asgard
E_DIR	= ../area/

RM		= rm -f
CP		= cp -f
MV		= mv -f
CC		= gcc
S_FLAGS	= -Wall -O0 -ggdb
C_FLAGS	= $(S_FLAGS) -Dlinux
L_FLAGS	= $(S_FLAGS) -lm -lz -lcrypt

MKDEPS	= $(CC) -M $(C_FLAGS)

C_FILES	= $(wildcard *.c)
O_FILES	= $(C_FILES:.c=.o)
H_FILES	= $(wildcard *.h)
D_FILE	= mk.deps

all: $(O_FILES) $(D_FILE)
	$(RM) $(E_NAME)
	$(CC) -o $(E_NAME) $(O_FILES) $(L_FLAGS)
	$(RM) $(E_DIR)$(E_NAME)
	$(MV) $(E_NAME) $(E_DIR)$(E_NAME)

$(O_FILES): %.o: %.c
	$(CC) -c $(C_FLAGS) $< -o $@

clean:
	$(RM) *.o *.*~ $(E_NAME) $(D_FILE)

# Automatic header dependency generation.
$(D_FILE): $(C_FILES)
	$(RM) $(D_FILE)
	$(MKDEPS) $^ >> $(D_FILE)
	
include $(D_FILE)
