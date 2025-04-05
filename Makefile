cc = gcc
CFLAGS = -Wall

all: compile run
SOURCE = app.c

compile:
	$(cc) $(CFLAGS) $(SOURCE) -o app

run:
	./app
