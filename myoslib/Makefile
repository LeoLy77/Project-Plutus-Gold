PREFIX ?= .
OBJ_DIR ?= $(PREFIX)/obj
LIB_DIR ?= $(PREFIX)/lib

all:
	mkdir -p $(OBJ_DIR) $(LIB_DIR)
	$(CC) -c $(CFLAGS) -MD -Iinclude src/s4433912_os_led.c -o $(OBJ_DIR)/s4433912_os_led.o
	$(AR) -rcs $(LIB_DIR)/libmylib.a $(OBJ_DIR)/s4433912_os_led.o

	$(CC) -c $(CFLAGS) -MD -Iinclude src/os_scu_bt.c -o $(OBJ_DIR)/os_scu_bt.o
	$(AR) -rcs $(LIB_DIR)/libmylib.a $(OBJ_DIR)/os_scu_bt.o

	$(CC) -c $(CFLAGS) -MD -Iinclude src/ahu_bt.c -o $(OBJ_DIR)/ahu_bt.o
	$(AR) -rcs $(LIB_DIR)/libmylib.a $(OBJ_DIR)/ahu_bt.o
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR)