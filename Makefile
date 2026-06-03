CC     = gcc
CFLAGS = -O2 -Wall -Wextra -pthread
SRCS   = src/main.c src/seq.c src/par.c
TARGET = matmul

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)
