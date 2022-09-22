LIBS=-lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
# CFLAGS=-Wall -Wextra -std=c11 -Ideps
CFLAGS=-std=c11 -Ideps

all: coxel shadertoy

coxel coxel.c:
	$(CC) $(CFLAGS) deps/glad.c src/coxel.c -o coxel $(LIBS)

shadertoy shadertoy.c:
	$(CC) $(CFLAGS) deps/glad.c src/shadertoy.c -o shadertoy $(LIBS)

.PHONY: coxel shadertoy
