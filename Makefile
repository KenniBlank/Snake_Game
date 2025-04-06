cc = gcc
CFLAGS = -Wall -Wextra -Wshadow
LIBS = -lncurses
RELEASEFLAGS = -O2
DEBUGFLAGS = -g -DRELEASE

SOURCE = app.c
APP = "Snake Game"

ifeq ($(BUILD),RELEASE)
    CFLAGS += $(RELEASEFLAGS)
else
    CFLAGS += $(DEBUGFLAGS)
endif

all: compile run
compile:
	$(cc) $(CFLAGS) $(LIBS) $(SOURCE) -o $(APP)

run:
	./$(APP)

clean:
	rm ./$(APP)
