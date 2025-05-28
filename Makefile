CC = gcc
DEBUG = -g3 -fsanitize=address -Wall
CFLAGS = -std=c99 $(DEBUG) # remova o debug quando nao for utilizar suas flags

TARGET = jpegtools

SRCS = ${wildcard src/*.c} 
HDRS = ${wildcard include/*.h}
OBJS = $(SRCS:src/%.c=obj/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

obj/%.o: src/%.c $(HDRS)
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

.PHONY: clean
clean:
	rm -rf obj $(TARGET)