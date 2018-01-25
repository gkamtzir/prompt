CC=gcc

all: shell

shell: shell.c
	$(CC) -O3 -o shell shell.c
	$(CC) -O3 -o shell_strict -D STRICT shell.c

clean:
	rm shell shell_strict
