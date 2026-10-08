CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -lm

# Separate targets for the main program and the test suite
RUN_TARGET = rsa_main
TEST_TARGET = rsa_test

# Default rule builds both executables without running them
all: $(RUN_TARGET) $(TEST_TARGET)

# Rule to compile the main RSA application
$(RUN_TARGET): rsa.c miller-rabin.c
	$(CC) $(CFLAGS) rsa.c miller-rabin.c -o $(RUN_TARGET) $(LIBS)

# Rule to compile the RSA test suite
$(TEST_TARGET): rsa_test.c miller-rabin.c
	$(CC) $(CFLAGS) rsa_test.c miller-rabin.c -o $(TEST_TARGET) $(LIBS)

# Compiles and runs only the main program
run: $(RUN_TARGET)
	./$(RUN_TARGET)

# Compiles and runs only the test suite
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Cleans up both executables
clean:
	rm -f $(RUN_TARGET) $(TEST_TARGET)

.PHONY: all run test clean
