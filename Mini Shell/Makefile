myshell: myshell.o argparse.o builtin.o
	gcc -Werror -Wall -g -o myshell myshell.o argparse.o builtin.o

myshell.o: myshell.c argparse.h builtin.h
	gcc -Werror -Wall -g -c myshell.c

argparse.o: argparse.c argparse.h
	gcc -Werror -Wall -g -c argparse.c

builtin.o: builtin.c builtin.h
	gcc -Werror -Wall -g -c builtin.c

clean:
	rm -f myshell myshell.exe
	rm -f *.o *.bak *~*
