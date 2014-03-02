EXE=comp rmsdiff byterev diffbmax
UNAME=$(shell uname)
ifneq ($(UNAME),Linux)
UNAME=Win32
endif

BIN_DIR=./bin/$(UNAME)

EXECUTABLE= $(addprefix $(BIN_DIR)/, $(EXE))
VPATH=src

all:$(BIN_DIR) $(EXECUTABLE)

clean realclean:
	rm -f $(EXECUTABLE)
	rm -fr $(BIN_DIR)
	rm -fr ./bin

$(BIN_DIR):
	mkdir -p $@

$(EXECUTABLE): 
	gcc -g -o $@ src/$(notdir $@).c -lm
