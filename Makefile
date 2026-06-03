CC = gcc
CFLAGS = -Wall
SRC = src/main.c src/helpers.c
OUT = Packify

all: $(OUT)

$(OUT):
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)

.PHONY: all clean
