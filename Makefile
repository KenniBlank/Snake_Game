cc = gcc
CFLAGS = -Wall -Wextra -Wshadow -O2
LIBS = -lncurses

SOURCE = app.c
APP = "Snake Game"

all: compile run clean

compile:
	$(cc) $(CFLAGS) $(LIBS) $(SOURCE) -o $(APP)

run:
	./$(APP)

clean:
	rm ./$(APP)
