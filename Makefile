LIBS=-lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
# CFLAGS=-Wall -Wextra -std=c11 -Ideps
CFLAGS=-std=c11 -Ideps

all: demo shadertoy

demo demo.c:
	$(CC) $(CFLAGS) deps/glad.c src/demo/main.c -o demo $(LIBS)

shadertoy shadertoy.c:
	$(CC) $(CFLAGS) deps/glad.c src/shadertoy.c -o shadertoy $(LIBS)

.PHONY: demo shadertoy
