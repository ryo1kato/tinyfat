CC     = gcc
LD     = gcc

CFLAGS  = -Wall -O2 -Wfatal-errors -Wcast-align -Werror -I.
LDFLAGS =

TARGET = tinyfat
SRCS = tinyfat.c main.c
OBJS = $(SRCS:.c=.o)


all: $(TARGET)


$(TARGET): $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS)


%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<


tinyfat.c: tinyfat.h debug.h

main.c: tinyfat.c tinyfat.h


clean:
	rm -f $(TARGET) $(OBJS)


.PHONY: clean
