LIBS=-lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
# CFLAGS=-Wall -Wextra -std=c11 -Ideps
CFLAGS=-std=c11 -Ideps -DIGNORE_SHADER_LOC_ERRORS
# CFLAGS=-std=c11 -Ideps -fsanitize=address -fno-omit-frame-pointer

all: demo

demo demo.c:
	$(CC) $(CFLAGS) deps/glad.c src/sandbox/main.c -o demo $(LIBS)

.PHONY: demo
