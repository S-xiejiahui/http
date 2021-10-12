TARGET := http

CC := gcc

CSRCS := $(wildcard ./src/*.c)

INCS := -I./inc

CFLAG := -lm -lpthread

LIBS := 

OBJS := $(patsubst %.c, %.o, $(CSRCS))

$(TARGET): $(OBJS) 
	$(CC) $+ $(INCS) $(LIBS) -o $@  $(CFLAG)

all:
	@git config --add core.filemode false

%.o:%.c
	$(CC) -c $< $(INCS) -o $@ $(CFLAG)

clean:
	rm -r $(OBJS) 
	rm -r $(TARGET)
