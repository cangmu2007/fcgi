debug:
	gcc -O2 -o test.cgi cgi_task.c CThread.c main.c -lpthread -lfcgi
all:
	gcc -g -o test.cgi cgi_task.c CThread.c main.c -lpthread -lfcgi
clean:
	rm test.cgi
