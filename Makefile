OBJS=main.c wad.c
CC=gcc

OBJ_NAME=wadloader.exe

all:$(OBJS)
	$(CC) $(OBJS) -o $(OBJ_NAME)