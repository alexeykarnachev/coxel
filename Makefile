LIBS=-lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
CFLAGS=-std=c11 -Ideps
# CFLAGS=-std=c11 -Ideps -DIGNORE_SHADER_LOC_ERRORS
# CFLAGS=-std=c11 -Ideps -fsanitize=address -fno-omit-frame-pointer
SRCS = $(shell find . -type f -name '*.c')

OBJS = $(patsubst %.c, build/%.o, $(SRCS))

all: $(OBJS); \
$(CC) $(CFLAGS) -o demo $^ $(LIBS)

build/%.o: %.c; \
$(shell mkdir -p `dirname $@`) \
$(CC) -c -o $@ $<
# $(CC) -g -c -o $@ $<

