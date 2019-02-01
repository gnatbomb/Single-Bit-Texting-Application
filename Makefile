CFLAGS = -Wall -pedantic -Werror -std=c99
.PHONY: clean
convertsigs.double: convertsigs.c
	gcc -o convertsigs convertsigs.c
convertsigs.single: convertsigs.c
	gcc -DSINGLE -o convertsigs convertsigs.c
clean:
	$(RM) convertsigs
