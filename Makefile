CC    := gcc
FLAGS := -Wall -Wextra -mwindows -s

viwm.exe: main.c
	$(CC) ${FLAGS} -o $@ $<
