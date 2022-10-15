LIBS=-lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
# CFLAGS=-Wall -Wextra -std=c11 -Ideps
CFLAGS=-std=c11 -Ideps

all: demo

demo demo.c:
	$(CC) $(CFLAGS) deps/glad.c src/sandbox/main.c -o demo $(LIBS)

.PHONY: demo
