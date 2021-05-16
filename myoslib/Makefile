PREFIX ?= .
OBJ_DIR ?= $(PREFIX)/obj
LIB_DIR ?= $(PREFIX)/lib

all:
	mkdir -p $(OBJ_DIR) $(LIB_DIR)
	$(CC) -c $(CFLAGS) -MD -Iinclude src/s4433912_os_bt.c -o $(OBJ_DIR)/s4433912_os_bt.o
	$(AR) -rcs $(LIB_DIR)/libmylib.a $(OBJ_DIR)/s4433912_os_bt.o
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR)