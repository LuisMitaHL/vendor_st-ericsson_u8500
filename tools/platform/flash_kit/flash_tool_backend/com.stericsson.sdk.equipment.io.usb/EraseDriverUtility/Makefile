PRG=erasedrv.exe
SRC_DIR=src
BUILD_DIR=build
CC=cl
CFLAGS=-nologo -Od -Oi
LDFLAGS=-nologo setupapi.lib

#Path to find sources and header files
VPATH=$(SRC_DIR)

#Object files
OBJ=$(BUILD_DIR)/erasedrv.obj

#Sources
SRC=erasedrv.cpp

#Headers
HEAD=erasedrv.h

.PHONY: clean_build
.PHONY: build
.PHONY: clean

#Rule for compiling sources from SRC_DIR and putting related object files into appropriate BUILD_DIR
$(BUILD_DIR)/%.obj : $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -Fo"$@"

#Do clean before build
clean_build: clean $(PRG)

#Do only build
build: $(PRG)

#Make unistall and clean all object files
clean:
	rm -rf $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)

$(PRG): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -Fe"$(BUILD_DIR)/$(PRG)"

$(OBJ): $(HEAD)
