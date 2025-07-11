CC = gcc
DEBUG = -g3 -fsanitize=address -Wall
CFLAGS = -std=c99 -lm # $(DEBUG) # remova o asterisco antes da variavel de debug quando for utilizar suas flags

TARGET = compressor-jpeg

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