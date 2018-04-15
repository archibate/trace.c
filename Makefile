SRCS=main.c render.c loader.c
OBJS=$(SRCS:%.c=%.o)

CFLAGS+=-fopenmp -O2
LDFLAGS+=-lm

TARGET=smallpt.tgt
IMGOUT=image.out

run: $(TARGET)
	./$(TARGET) $(SCENE) $(SIZE)
	display $(IMGOUT)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
