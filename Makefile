CC=gcc

all: shell

shell: shell.c
	$(CC) -o shell shell.c
	$(CC) -o shell_strict -D STRICT shell.c

clean:
	rm shell shell_strict
