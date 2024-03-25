prog1: prog1.c
	gcc -g -Wall -Wextra -Wwrite-strings -oprog1 prog1.c

clean:
	rm -f prog1
	rm -f tests.out