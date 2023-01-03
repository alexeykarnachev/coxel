LIBS=-lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
# CFLAGS=-std=c11 -Ideps
CFLAGS=-std=c11 -Ideps -fsanitize=address -fno-omit-frame-pointer
# CFLAGS=-std=c11 -Ideps -DIGNORE_SHADER_LOC_ERRORS
SRCS = $(shell find . -type f -name '*.c')

OBJS = $(patsubst %.c, build/%.o, $(SRCS))

all: $(OBJS); \
$(CC) $(CFLAGS) -o demo $^ $(LIBS)

build/%.o: %.c; \
$(shell mkdir -p `dirname $@`) \
$(CC) -g -c -o $@ $<
# $(CC) -c -o $@ $<

