byte: byte.c
	$(CC) byte.c -o byte -Wall -Wextra -pedantic -std=c99

# $(CC) is a variable that make expands to cc by default.
# -Wall stands for “all Warnings”, and gets the compiler to warn you when it sees code that might not technically be wrong
# -Wextra and -pedantic turn on even more warnings.
# -std=c99 specifies the exact version of the C language standard we’re using, which is C99.