CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic
TARGET = scheduler_app

SOURCES = \
	main.c \
	src/report.c \
	src/algorithms/edf.c \
	src/algorithms/lottery.c \
	src/algorithms/mlfq.c

.PHONY: all run clean rebuild

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean all
