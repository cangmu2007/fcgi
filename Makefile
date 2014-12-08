debug:
	gcc  -g -o test.fcg cgi_task.c CThread.c main.c -lpthread -lfcgi
all:
	gcc -g -o test.fcg cgi_task.c CThread.c main.c -lpthread -lfcgi
clean:
	rm test.fcg
