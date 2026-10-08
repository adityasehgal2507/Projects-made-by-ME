CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -lm
TARGET = rsa_test

# The default rule now builds AND runs the program
all: $(TARGET)
	./$(TARGET)

# Rule to compile the executable directly
$(TARGET): rsa.c miller-rabin.c
	$(CC) $(CFLAGS) rsa.c miller-rabin.c -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
