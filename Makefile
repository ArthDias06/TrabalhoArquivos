CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = programaTrab

SRCS = programaTrab.c crud.c crud_sequencial.c crud_arvoreb.c registro.c matriz.c fornecidas.c util.c arvoreb.c join.c
OBJS = $(SRCS:.c=.o)

.PHONY: all run clean rebuild

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all