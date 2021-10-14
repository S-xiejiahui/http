# The target file name
TARGET := http
# The compiler
CC := gcc
# The source file
CSRCS := $(wildcard ./src/*.c)
# The header file
INCS := -I./inc
# Link libraries
CFLAG := -lm -lpthread
LIBS := 
# The file object
OBJS := $(patsubst %.c, %.o, $(CSRCS))
# get local ip addr
LOACL_IP_ADDR = $(shell ifconfig | grep inet | grep -v inet6 | grep -v 127 | sed 's/^[ \t]*//g' | cut -d ' ' -f2)
# Use regular expressions to match files's ip addr
JS_FILE_IP_ADDR = $(shell grep -E -o '(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)' js/app.js)
NEW_IP = $(shell echo $(JS_FILE_IP_ADDR)| cut -d ' ' -f 1)
	
# Compiling method
all: $(TARGET)
$(TARGET): $(OBJS) 
	$(CC) $+ $(INCS) $(LIBS) -o $@  $(CFLAG)
%.o:%.c
	$(CC) -c $< $(INCS) -o $@ $(CFLAG)

# Update file to match local
files := js/app.js
update:
	@sed -i 's/$(NEW_IP)/$(LOACL_IP_ADDR)/' $(files)
	@echo "replace [\033[33m$(files)'s\033[0m] old_IP(\033[36m$(NEW_IP)\033[0m) as local_IP(\033[31m$(LOACL_IP_ADDR)\033[0m)"
	@chmod 777 ./ -R
	@git config --add core.filemode false

# Clean up the file
clean:
	rm -r $(OBJS) 
	rm -r $(TARGET)
