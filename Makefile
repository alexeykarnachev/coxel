LIBS=-lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
CFLAGS=-std=c11 -Ideps # -DIGNORE_SHADER_LOC_ERRORS
# CFLAGS=-std=c11 -Ideps -fsanitize=address -fno-omit-frame-pointer
SRCS = $(shell find . -type f -name '*.c')

# Use the patsubst function to generate the names of the corresponding object files.
OBJS = $(patsubst %.c,%.o,$(SRCS))

# Compile the source files using gcc.
all: $(OBJS); \
$(CC) $(CFLAGS) -o demo $^ $(LIBS)

# Compile each source file into an object file.
%.o: %.c; \
$(CC) -c -o $@ $<

