# The target file name
TARGET := http
# The compiler
CC    := gcc
# The source file
SRC   := ./c-web-server/src
CSRCS := $(wildcard $(SRC)/*.c)
# The header file
INCS  := -I./c-web-server/inc
INCS  += -I./c-web-server/lib-h
# Link libraries
CFLAG := -lm -lpthread
LIBS  := -L./c-web-server/lib
# The file object
OBJ   := ./c-web-server/obj
OBJS  := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(CSRCS))

# get local ip addr
LOACL_IP_ADDR = $(shell ifconfig | grep inet | grep -v inet6 | grep -v 127 | sed 's/^[ \t]*//g' | cut -d ' ' -f2)
# Use regular expressions to match files's ip addr
JS_FILE_IP_ADDR = $(shell grep -E -o '(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)' js/app.js)
OLD_IP = $(shell echo $(JS_FILE_IP_ADDR)| cut -d ' ' -f 1)
	
# Compiling method
all: $(TARGET)
$(TARGET): $(OBJS) 
	$(CC) $+ $(INCS) $(LIBS) -o $@  $(CFLAG)
$(OBJ)/%.o:$(SRC)/%.c
	$(CC) -c $< $(INCS) -o $@ $(CFLAG)

# Update IP to match local
files := js/app.js
update:
	@sed -i 's/$(OLD_IP)/$(LOACL_IP_ADDR)/' $(files)
	@echo "\033[33mUpdate success:\033[0m"
	@echo "replace \033[36mold_IP($(OLD_IP)\033[0m) as \033[31mlocal_IP($(LOACL_IP_ADDR)\033[0m)"
	@mkdir $(OBJ)
	@chmod 777 ./ -R
	@git config --add core.filemode false

# Clean up the file
clean:
	rm -r $(OBJS) 
	rm -r $(TARGET)
