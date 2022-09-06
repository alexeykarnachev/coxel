LIBS=-lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
# CFLAGS=-Wall -Wextra -std=c11 -Ideps
CFLAGS=-std=c11 -Ideps

coxel coxel.c:
	$(CC) $(CFLAGS) deps/glad.c src/coxel.c -o coxel $(LIBS)

.PHONY: coxel
